static gboolean letzte_wahrscheinlichkeit_wird_gesetzt = FALSE;
static gboolean wahrscheinlichkeit_als_dezimalzahl(const char *text,
                                                   double *wert) {
  char normalisiert[1000] = "";
  char *ende = NULL;
  g_strlcpy(normalisiert, text, sizeof(normalisiert));
  char *komma = strchr(normalisiert, ',');
  if (komma)
    *komma = '.';
  errno = 0;
  *wert = g_ascii_strtod(normalisiert, &ende);
  if (ende == normalisiert) return FALSE;
  while (ende && g_ascii_isspace(*ende))
    ende++;
  return errno != ERANGE && *ende == '\0' && isfinite(*wert) &&
         *wert >= 0.0 && *wert <= 1.0;
}

static gboolean wahrscheinlichkeit_als_bruch(const char *text,
                                             long long *zaehler,
                                             long long *nenner) {
  char *ende = NULL;
  const char *trenner = strchr(text, '/');
  if (!trenner || strchr(trenner + 1, '/'))
    return FALSE;
  errno = 0;
  *zaehler = g_ascii_strtoll(text, &ende, 10);
  if (errno == ERANGE || ende == text || ende != trenner || *zaehler < 0)
    return FALSE;
  errno = 0;
  *nenner = g_ascii_strtoll(trenner + 1, &ende, 10);
  while (ende && g_ascii_isspace(*ende))
    ende++;
  return errno != ERANGE && ende != trenner + 1 && *nenner > 0 &&
         *ende == '\0' && *zaehler <= *nenner;
}

static gboolean wahrscheinlichkeit_lesen(const char *text, double *wert) {
  if (!strchr(text, '/'))
    return wahrscheinlichkeit_als_dezimalzahl(text, wert);
  long long z, n;
  if (!wahrscheinlichkeit_als_bruch(text, &z, &n))
    return FALSE;
  *wert = (double)z / n;
  return TRUE;
}

static double wahrscheinlichkeit_einlesen(const char *text) {
  double wert;
  return wahrscheinlichkeit_lesen(text, &wert) ? wert : NAN;
}

static void wahrscheinlichkeit_alle_pruefen(void) {
  bruch = 0;
  for (int i = 0; i <= maxzaehler; i++) {
    GtkWidget *feld = textfeldWahrscheinlichkeit[i];
    const char *text = gtk_entry_get_text(GTK_ENTRY(feld));
    double wert;
    gboolean ungueltig = *text && !wahrscheinlichkeit_lesen(text, &wert);
    if (ungueltig)
      gtk_widget_add_css_class(feld, "error");
    else
      gtk_widget_remove_css_class(feld, "error");
    /* Keep the text area symmetric: an entry icon shifts centered text.
     * The error style and tooltip indicate invalid input without an icon. */
    gtk_widget_set_tooltip_text(feld, ungueltig
        ? "Wahrscheinlichkeit zwischen 0 und 1 eingeben, z. B. 0,5 oder 1/2."
        : NULL);
    bruch |= strchr(text, '/') != NULL;
  }
}

static void unnoetige_nachkommastellen_entfernen(char *text) {
  char *trenner = strchr(text, '.');
  if (!trenner)
    trenner = strchr(text, ',');
  if (!trenner)
    return;
  char *ende = text + strlen(text);
  while (ende > trenner + 1 && ende[-1] == '0')
    *--ende = '\0';
  if (ende == trenner + 1)
    *trenner = '\0';
}

static void letzte_wahrscheinlichkeit_erganzen(GtkEditable *editable) {
  if (!letzte_wahrscheinlichkeit_automatisch ||
      letzte_wahrscheinlichkeit_wird_gesetzt)
    return;

  GtkWidget *aktuelles_feld = GTK_WIDGET(editable);
  const char *name = gtk_widget_get_name(aktuelles_feld);
  if (!name || !g_str_has_suffix(name, "W"))
    return;

  g_autofree gchar *knotenname = g_strndup(name, strlen(name) - 1);
  char *letzter_trenner = strrchr(knotenname, '-');
  if (!letzter_trenner)
    return;
  int kindnummer = atoi(letzter_trenner + 1);
  *letzter_trenner = '\0';
  gboolean virtuelle_wurzel = knotenname[0] == '\0';
  int elternindex = virtuelle_wurzel ? -1 : knotenexistiert(knotenname);
  if (!virtuelle_wurzel && elternindex < 0)
    return;

  int anzahl = 0;
  if (virtuelle_wurzel) {
    g_autofree gchar *geschwistername = g_strdup_printf("-%d", anzahl);
    while (knotenexistiert(geschwistername) >= 0) {
      anzahl++;
      g_free(g_steal_pointer(&geschwistername));
      geschwistername = g_strdup_printf("-%d", anzahl);
    }
  } else
    anzahl = anzahlnachfolger(elternindex);
  if (anzahl < 2 || kindnummer != anzahl - 2)
    return;

  double summe = 0.0;
  gboolean komma = FALSE;
  int bruchmodus = -1;
  long long bruchzaehler = 0, bruchnenner = 1;
  for (int i = 0; i < anzahl - 1; i++) {
    int kindindex;
    if (virtuelle_wurzel) {
      g_autofree gchar *geschwistername = g_strdup_printf("-%d", i);
      kindindex = knotenexistiert(geschwistername);
    } else
      kindindex = nachfolger(elternindex, i);
    GtkWidget *feld = textfeldWahrscheinlichkeit[kindindex];
    const char *text = gtk_entry_get_text(GTK_ENTRY(feld));
    gboolean ist_bruch = strchr(text, '/') != NULL;
    if (bruchmodus < 0)
      bruchmodus = ist_bruch;
    if (!ist_bruch)
      bruchmodus = 0;
    double wert;
    if (!wahrscheinlichkeit_lesen(text, &wert))
      return;
    summe += wert;
    komma |= strchr(text, ',') != NULL;
    if (ist_bruch && bruchmodus) {
      long long zaehler = 0, nenner = 1;
      if (!wahrscheinlichkeit_als_bruch(text, &zaehler, &nenner))
        return;
      long long teiler = ggt(bruchnenner, nenner);
      long long faktor = nenner / teiler;
      if (bruchnenner > G_MAXINT64 / faktor)
        return;
      long long gemeinsamer_nenner = bruchnenner * faktor;
      long long alter_faktor = gemeinsamer_nenner / bruchnenner;
      long long neuer_faktor = gemeinsamer_nenner / nenner;
      if ((bruchzaehler > G_MAXINT64 / alter_faktor) ||
          (zaehler > G_MAXINT64 / neuer_faktor))
        return;
      long long alter_anteil = bruchzaehler * alter_faktor;
      long long neuer_anteil = zaehler * neuer_faktor;
      if (alter_anteil > G_MAXINT64 - neuer_anteil)
        return;
      bruchzaehler = alter_anteil + neuer_anteil;
      bruchnenner = gemeinsamer_nenner;
    }
  }
  if ((bruchmodus && bruchzaehler > bruchnenner) ||
      (!bruchmodus && summe > 1.0 + 1e-12))
    return;

  int letzter_index;
  if (virtuelle_wurzel) {
    g_autofree gchar *letzter_name = g_strdup_printf("-%d", anzahl - 1);
    letzter_index = knotenexistiert(letzter_name);
  } else
    letzter_index = nachfolger(elternindex, anzahl - 1);
  GtkWidget *letztes_feld = textfeldWahrscheinlichkeit[letzter_index];
  char resttext[128] = "";
  if (bruchmodus) {
    long long rest = bruchnenner - bruchzaehler;
    long long teiler = ggt(rest, bruchnenner);
    if (kuerzen && teiler) {
      rest /= teiler;
      bruchnenner /= teiler;
    }
    g_snprintf(resttext, sizeof(resttext), "%lld/%lld", rest, bruchnenner);
  } else {
    char format[20] = "";
    snprintf(format, sizeof(format), "%%.%df", genauigkeit);
    g_ascii_formatd(resttext, sizeof(resttext), format, MAX(0.0, 1.0 - summe));
    if (komma) {
      char *punkt = strchr(resttext, '.');
      if (punkt)
        *punkt = ',';
    }
    unnoetige_nachkommastellen_entfernen(resttext);
  }

  letzte_wahrscheinlichkeit_wird_gesetzt = TRUE;
  gtk_entry_set_text(GTK_ENTRY(letztes_feld), resttext);
  letzte_wahrscheinlichkeit_wird_gesetzt = FALSE;
}

void wskeingabe(GtkEditable *editable, gpointer data) {
  dateiveraendert++;
  wahrscheinlichkeit_alle_pruefen();
  /* Bei programmgesteuerten Änderungen (z. B. Restwahrscheinlichkeit) kann
   * der Fokus noch auf einem anderen Feld liegen. Maßgeblich ist daher das
   * Feld, das das Signal ausgelöst hat. */
  g_autofree gchar *tempname =
      g_strdup(gtk_widget_get_name(GTK_WIDGET(editable)));

  int i = 0;

  for (i = 0; i <= maxzaehlererg; i++) {
    if (strlen(tempname) <
        strlen(gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]))) {
      if (strncmp(gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),
                  tempname, strlen(tempname) - 1) == 0) {
        wskergebnisneuschreiben(textfeldErgebnisWahrscheinlichkeit[i]);
      }
    }
  }

  letzte_wahrscheinlichkeit_erganzen(editable);

  arboretum_refresh_entry_overlines();

  eingabe_neuaufbau_planen(data);
}
