typedef struct {
  GPtrArray *names, *wahrscheinlichkeitstexte, *nodes, *ws;
  GArray *ps;
  GString *es, *ews;
  int depth, ni, ei;
  gboolean bruchdarstellung, bruch_ueberlauf;
} Modell;

static gboolean modell_brueche_multiplizieren(const char *erstes,
                                              const char *zweites,
                                              char *ergebnis,
                                              gsize ergebnisgroesse);

static void modell_row(GPtrArray *a, int i, const char *f, ...) {
  while (a->len <= (guint)i)
    g_ptr_array_add(a, NULL);
  va_list v;
  va_start(v, f);
  g_ptr_array_index(a, i) = g_strdup_vprintf(f, v);
  va_end(v);
}
static void modell_build(Modell *m, const char *n, const char *t, const char *e,
                         int d, double path, double kantewert,
                         const char *pfadtext,
                         const char *kantentext, int *y) {
  int x = m->ni++;
  if (x >= MAX_KNOTEN)
    return;
  char q[48] = "";
  if (m->bruchdarstellung)
    g_strlcpy(q, kantentext, sizeof q);
  else
    g_ascii_formatd(q, sizeof q, "%.12g", kantewert);
  if (d == m->depth) {
    int yy = ((*y)++) * (KnotenHoehe + KnotenAbstand);
    modell_row(m->nodes, x, "%d%c%d%c%s%c%s%c\n", x, 31, yy, 31, n, 31, t, 31);
    modell_row(m->ws, x, "%d%c%sW%c%s%c\n", x, 31, n, 31, q, 31);
    if (m->bruchdarstellung)
      g_strlcpy(q, pfadtext, sizeof q);
    g_string_append_printf(m->es, "%d%c%d%c%s-E%c%s%c\n", m->ei, 31, yy, 31, n,
                           31, e, 31);
    g_string_append_printf(m->ews, "%d%c%s-EW%c%s%c\n", m->ei++, 31, n, 31, q,
                           31);
    return;
  }
  int first = -1, last = -1;
  for (guint i = 0; i < m->names->len; i++) {
    double p = g_array_index(m->ps, double, i);
    const char *kante = g_ptr_array_index(m->wahrscheinlichkeitstexte, i);
    g_autofree char *k = g_strdup_printf("%s-%u", n, i);
    const char *name = g_ptr_array_index(m->names, i);
    g_autofree char *r =
        ErgebnisTrenner && ErgebnisTrenner != 127
            ? g_strdup_printf("%s%c%s", e, ErgebnisTrenner, name)
            : g_strdup_printf("%s%s", e, name);
    int old = *y;
    char neuer_pfad[100] = "";
    if (m->bruchdarstellung) {
      if (!modell_brueche_multiplizieren(pfadtext, kante, neuer_pfad,
                                         sizeof neuer_pfad)) {
        m->bruch_ueberlauf = TRUE;
        return;
      }
    }
    modell_build(m, k, name, r, d + 1, path * p, p,
                 m->bruchdarstellung ? neuer_pfad : "", kante, y);
    if (first < 0)
      first = old * (KnotenHoehe + KnotenAbstand);
    last = (*y - 1) * (KnotenHoehe + KnotenAbstand);
  }
  modell_row(m->nodes, x, "%d%c%d%c%s%c%s%c\n", x, 31, (first + last) / 2, 31,
             n, 31, t, 31);
  modell_row(m->ws, x, "%d%c%sW%c%s%c\n", x, 31, n, 31, q, 31);
}

static int modell_textbreite(GtkWidget *feld) {
  /* Zeichen sind in proportionalen Schriften unterschiedlich breit.
   * Mit der tatsächlichen Schrift messen und Platz für den Cursor lassen. */
  return MAX(2, textbreite_in_zeichen(
                    feld, gtk_entry_get_text(GTK_ENTRY(feld))) + 1);
}
static void modell_wahrscheinlichkeitsfelder_anpassen(gpointer data);
/* Das Laden erzeugt die Felder mit der gespeicherten Standardbreite. Die
 * Breiten des erzeugten Modells werden daher erst danach aus dem Inhalt
 * bestimmt, damit auch lange Namen und vollständige Ergebnisse sichtbar sind.
 */
static void modell_feldgroessen_anpassen(gpointer data) {
  KnotenTextBreite = 2;
  for (int i = 0; i <= maxzaehler; i++)
    KnotenTextBreite =
        MAX(KnotenTextBreite,
            modell_textbreite(textfeld[i]));
  for (int i = 0; i <= maxzaehler; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeld[i]), KnotenTextBreite);
  groesseneu(NULL, NULL, data);
  positionsanpassungwsk(data);
  wskergebnisverschieben(NULL, NULL, data);
  modell_wahrscheinlichkeitsfelder_anpassen(data);
  GROESSEDRAWINGAREA GROESSELAYOUTD gtk_widget_queue_draw(da);
}
static gboolean modell_name_gueltig(const char *name) {
  return *name && !strpbrk(name, "\n\r\036\037");
}

/* Die allgemeine Eingabe akzeptiert Dezimalzahlen und Brüche. Die
 * Pfadvorlage muss dieselbe Schreibweise verstehen, damit z. B. 1/3 nicht
 * stillschweigend als 1 eingelesen wird. */
static gboolean modell_wahrscheinlichkeit_lesen(const char *text,
                                                double *wert) {
  g_autofree char *kopie = g_strdup(text);
  char *ende = NULL;
  g_strstrip(kopie);
  char *schraegstrich = strchr(kopie, '/');
  if (schraegstrich) {
    if (strchr(schraegstrich + 1, '/'))
      return FALSE;
    *schraegstrich = '\0';
    char *zaehlertext = g_strstrip(kopie);
    char *nennertext = g_strstrip(schraegstrich + 1);
    long long zaehler = g_ascii_strtoll(zaehlertext, &ende, 10);
    if (!*zaehlertext || ende == zaehlertext || *ende || zaehler < 0)
      return FALSE;
    long long nenner = g_ascii_strtoll(nennertext, &ende, 10);
    if (!*nennertext || ende == nennertext || *ende || nenner <= 0 ||
        zaehler > nenner)
      return FALSE;
    *wert = (double)zaehler / (double)nenner;
    return TRUE;
  }

  char *komma = strchr(kopie, ',');
  if (komma)
    *komma = '.';
  *wert = g_ascii_strtod(kopie, &ende);
  while (ende && g_ascii_isspace(*ende))
    ende++;
  return *kopie && ende && !*ende && isfinite(*wert) && *wert >= 0.0 &&
         *wert <= 1.0;
}

static gboolean modell_bruch_lesen(const char *text, long long *zaehler,
                                   long long *nenner) {
  g_autofree char *kopie = g_strdup(text);
  char *ende = NULL;
  char *trenner = strchr(kopie, '/');
  if (!trenner || strchr(trenner + 1, '/'))
    return FALSE;
  *trenner = '\0';
  char *zaehlertext = g_strstrip(kopie);
  char *nennertext = g_strstrip(trenner + 1);
  *zaehler = g_ascii_strtoll(zaehlertext, &ende, 10);
  if (!*zaehlertext || ende == zaehlertext || *ende || *zaehler < 0)
    return FALSE;
  *nenner = g_ascii_strtoll(nennertext, &ende, 10);
  return *nennertext && ende != nennertext && !*ende && *nenner > 0 &&
         *zaehler <= *nenner;
}

static gboolean modell_brueche_multiplizieren(const char *erstes,
                                              const char *zweites,
                                              char *ergebnis,
                                              gsize ergebnisgroesse) {
  long long az, an, bz, bn;
  if (!modell_bruch_lesen(erstes, &az, &an) ||
      !modell_bruch_lesen(zweites, &bz, &bn))
    return FALSE;
  long long teiler = ggt(az, bn);
  az /= teiler;
  bn /= teiler;
  teiler = ggt(bz, an);
  bz /= teiler;
  an /= teiler;
  if ((az && bz > G_MAXINT64 / az) || (an && bn > G_MAXINT64 / an))
    return FALSE;
  az *= bz;
  an *= bn;
  teiler = ggt(az, an);
  if (teiler) {
    az /= teiler;
    an /= teiler;
  }
  g_snprintf(ergebnis, ergebnisgroesse, "%lld/%lld", az, an);
  return TRUE;
}

static void modell_gleichverteilung_geaendert(GObject *schalter,
                                              GParamSpec *eigenschaft,
                                              gpointer feld) {
  (void)eigenschaft;
  gtk_widget_set_sensitive(
      GTK_WIDGET(feld),
      !gtk_check_button_get_active(GTK_CHECK_BUTTON(schalter)));
}

static void modell_gleichverteilung_bruchoption_geaendert(
    GObject *schalter, GParamSpec *eigenschaft, gpointer option) {
  (void)eigenschaft;
  gtk_widget_set_sensitive(
      GTK_WIDGET(option),
      gtk_check_button_get_active(GTK_CHECK_BUTTON(schalter)));
}
static void modell_wahrscheinlichkeitsfelder_anpassen(gpointer data) {
  for (int i = 0; i <= maxzaehler; i++) {
    g_autofree char *text =
        g_strdup(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
    for (char *p = text; *p; p++)
      if (*p == '.')
        *p = ',';
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i]), text);
  }
  WahrscheinlichkeitTextBreite = 2;
  WahrscheinlichkeitErgebnisTextBreite = 2;
  for (int i = 0; i <= maxzaehler; i++)
    WahrscheinlichkeitTextBreite =
        MAX(WahrscheinlichkeitTextBreite,
            modell_textbreite(textfeldWahrscheinlichkeit[i]));
  for (int i = 0; i <= maxzaehlererg; i++)
    WahrscheinlichkeitErgebnisTextBreite =
        MAX(WahrscheinlichkeitErgebnisTextBreite,
            modell_textbreite(textfeldErgebnisWahrscheinlichkeit[i]));
  for (int i = 0; i <= maxzaehler; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldWahrscheinlichkeit[i]),
                              WahrscheinlichkeitTextBreite);
  for (int i = 0; i <= maxzaehlererg; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]),
                              WahrscheinlichkeitErgebnisTextBreite);
  positionsanpassungwsk(data);
  wskergebnisverschieben(NULL, NULL, data);
  GROESSEDRAWINGAREA GROESSELAYOUTD gtk_widget_queue_draw(da);
}

static void modell_dialog(GtkWidget *w, gpointer data, gboolean bin) {
  (void)w;
  GtkWidget *d = gtk_dialog_new_with_buttons(
                bin ? "Binomialmodell" : "Pfadvorlage", GTK_WINDOW(window),
                GTK_DIALOG_MODAL, "Abbrechen", GTK_RESPONSE_CANCEL,
                "Baum erzeugen", GTK_RESPONSE_OK, NULL),
            *g = gtk_grid_new(),
            *c = gtk_dialog_get_content_area(GTK_DIALOG(d));
  gtk_grid_set_row_spacing(GTK_GRID(g), 8);
  gtk_grid_set_column_spacing(GTK_GRID(g), 8);
  gtk_widget_set_margin_start(g, 12);
  gtk_widget_set_margin_end(g, 12);
  gtk_widget_set_margin_top(g, 12);
  gtk_widget_set_margin_bottom(g, 12);
  gtk_box_append(GTK_BOX(c), g);
  GtkWidget *a = bin ? gtk_spin_button_new_with_range(0, 1, .01)
                     : gtk_entry_new(),
            *b = bin ? NULL : gtk_entry_new(),
            *s = gtk_spin_button_new_with_range(1, 20, 1), *name1 = NULL,
            *name2 = NULL, *gleichverteilung = NULL,
            *gleichverteilung_bruch = NULL;
  if (bin) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(a), .5);
    name1 = gtk_entry_new();
    name2 = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(name1), "T");
    gtk_editable_set_text(GTK_EDITABLE(name2), "N");
  } else {
    gtk_editable_set_text(GTK_EDITABLE(a), "A, B");
    gtk_editable_set_text(GTK_EDITABLE(b), "0.5, 0.5");
    gleichverteilung =
        gtk_check_button_new_with_label("Gleichverteilung verwenden");
    gleichverteilung_bruch =
        gtk_check_button_new_with_label("Gleichverteilung als Brüche anzeigen");
    gtk_widget_set_sensitive(gleichverteilung_bruch, FALSE);
    /* active ist die GTK4-Eigenschaft des GtkCheckButton. Das
     * Property-Signal funktioniert auch bei einer programmgesteuerten
     * Änderung zuverlässig und hält das Eingabefeld entsprechend aktuell. */
    g_signal_connect(gleichverteilung, "notify::active",
                     G_CALLBACK(modell_gleichverteilung_geaendert), b);
    g_signal_connect(gleichverteilung, "notify::active",
                     G_CALLBACK(modell_gleichverteilung_bruchoption_geaendert),
                     gleichverteilung_bruch);
  }
  int row = 0;
  gtk_grid_attach(
      GTK_GRID(g),
      gtk_label_new(bin ? "Wahrscheinlichkeit für das erste Ergebnis:"
                        : "Ausgänge:"),
      0, row, 1, 1);
  gtk_grid_attach(GTK_GRID(g), a, 1, row++, 1, 1);
  if (bin) {
    gtk_grid_attach(GTK_GRID(g), gtk_label_new("Erstes Ergebnis:"), 0, row, 1,
                    1);
    gtk_grid_attach(GTK_GRID(g), name1, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(g), gtk_label_new("Zweites Ergebnis:"), 0, row, 1,
                    1);
    gtk_grid_attach(GTK_GRID(g), name2, 1, row++, 1, 1);
  } else {
    gtk_grid_attach(GTK_GRID(g), gtk_label_new("Wahrscheinlichkeiten:"), 0, row,
                    1, 1);
    gtk_grid_attach(GTK_GRID(g), b, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(g), gleichverteilung, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(g), gleichverteilung_bruch, 1, row++, 1, 1);
  }
  gtk_grid_attach(GTK_GRID(g), gtk_label_new("Anzahl Stufen:"), 0, row, 1, 1);
  gtk_grid_attach(GTK_GRID(g), s, 1, row, 1, 1);
  if (gtk_dialog_run(GTK_DIALOG(d)) == GTK_RESPONSE_OK) {
    gchar **ns = NULL;
    if (bin) {
      g_autofree gchar *erstes =
          g_strdup(gtk_editable_get_text(GTK_EDITABLE(name1)));
      g_autofree gchar *zweites =
          g_strdup(gtk_editable_get_text(GTK_EDITABLE(name2)));
      g_strstrip(erstes);
      g_strstrip(zweites);
      if (!modell_name_gueltig(erstes) || !modell_name_gueltig(zweites)) {
        urnen_fehler("Bitte zwei nichtleere Ergebnisnamen ohne Zeilen- oder "
                     "Steuerzeichen eingeben.");
        gtk_window_destroy(GTK_WINDOW(d));
        return;
      }
      ns = g_new0(gchar *, 3);
      ns[0] = g_strdup(erstes);
      ns[1] = g_strdup(zweites);
    } else
      ns = g_strsplit(gtk_editable_get_text(GTK_EDITABLE(a)), ",", -1);
    gchar **vs = bin ? NULL : g_strsplit(gtk_editable_get_text(GTK_EDITABLE(b)),
                                         ",", -1);
    guint z = g_strv_length(ns);
    Modell m = {0};
    m.names = g_ptr_array_new_with_free_func(g_free);
    m.wahrscheinlichkeitstexte = g_ptr_array_new_with_free_func(g_free);
    m.ps = g_array_new(FALSE, FALSE, sizeof(double));
    m.depth = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s));
    double sum = 0;
    gboolean gleich = !bin &&
                      gtk_check_button_get_active(
                          GTK_CHECK_BUTTON(gleichverteilung));
    m.bruchdarstellung = gleich && gtk_check_button_get_active(
                                        GTK_CHECK_BUTTON(gleichverteilung_bruch));
    if (!bin && !gleich && z != g_strv_length(vs))
      sum = NAN;
    for (guint i = 0; i < z && isfinite(sum); i++) {
      double p = 0.0;
      gboolean gueltig = TRUE;
      if (bin)
        p = i ? 1 - gtk_spin_button_get_value(GTK_SPIN_BUTTON(a))
              : gtk_spin_button_get_value(GTK_SPIN_BUTTON(a));
      else if (gleich)
        p = 1.0 / z;
      else
        gueltig = modell_wahrscheinlichkeit_lesen(vs[i], &p);
      gboolean ist_bruch = !bin &&
                            (gleich ? m.bruchdarstellung
                                    : strchr(g_strstrip(vs[i]), '/') != NULL);
      if (!gleich && i == 0)
        m.bruchdarstellung = ist_bruch;
      else if (!gleich && m.bruchdarstellung != ist_bruch)
        gueltig = FALSE;
      if (!gueltig) {
        sum = NAN;
        break;
      }
      g_ptr_array_add(m.names, g_strdup(g_strstrip(ns[i])));
      if (m.bruchdarstellung)
        g_ptr_array_add(m.wahrscheinlichkeitstexte,
                        gleich ? g_strdup_printf("1/%u", z)
                               : g_strdup(g_strstrip(vs[i])));
      else {
        char text[G_ASCII_DTOSTR_BUF_SIZE] = "";
        g_ascii_formatd(text, sizeof text, "%.12g", p);
        g_ptr_array_add(m.wahrscheinlichkeitstexte, g_strdup(text));
      }
      g_array_append_val(m.ps, p);
      sum += p;
    }
    if (!isfinite(sum) || fabs(sum - 1) > 1e-9)
      urnen_fehler("Bitte passende Ausgänge und Wahrscheinlichkeiten mit Summe "
                   "1 eingeben. Brüche wie 1/3 sind möglich.");
    else {
      m.nodes = g_ptr_array_new_with_free_func(g_free);
      m.ws = g_ptr_array_new_with_free_func(g_free);
      m.es = g_string_new(NULL);
      m.ews = g_string_new(NULL);
      int y = 0;
      for (guint i = 0; i < z; i++) {
        double p = g_array_index(m.ps, double, i);
        const char *text = g_ptr_array_index(m.wahrscheinlichkeitstexte, i);
        g_autofree char *n = g_strdup_printf("-%u", i);
        modell_build(&m, n, g_ptr_array_index(m.names, i),
                     g_ptr_array_index(m.names, i), 1, p, p, text, text, &y);
      }
      if (m.bruch_ueberlauf)
        urnen_fehler("Die Bruchwerte werden über die gewählte Anzahl Stufen "
                     "zu groß.");
      else if (m.ni >= MAX_KNOTEN)
        urnen_fehler("Das Modell ist zu groß.");
      else {
        gchar *tmp = NULL;
        int fd = g_file_open_tmp("arboretum-modell-XXXXXX.bdg", &tmp, NULL);
        if (fd >= 0) {
          speichern(tmp);
          gchar *old = NULL;
          gsize l;
          if (g_file_get_contents(tmp, &old, &l, NULL)) {
            char *cut = strchr(old, 30);
            GString *out = g_string_new_len(old, cut - old + 2);
            for (guint i = 0; i < m.nodes->len; i++)
              g_string_append(out, g_ptr_array_index(m.nodes, i));
            g_string_append_printf(out, "%c\n%s%c\n", 30, m.es->str, 30);
            for (guint i = 0; i < m.ws->len; i++)
              g_string_append(out, g_ptr_array_index(m.ws, i));
            g_string_append_printf(out, "%c\n%s", 30, m.ews->str);
            g_file_set_contents(tmp, out->str, out->len, NULL);
            tempspeichern();
            laden(data, tmp);
            modell_feldgroessen_anpassen(data);
            g_string_free(out, TRUE);
            g_free(old);
          }
          close(fd);
          g_unlink(tmp);
          g_free(tmp);
        }
      }
      g_ptr_array_free(m.nodes, TRUE);
      g_ptr_array_free(m.ws, TRUE);
      g_string_free(m.es, TRUE);
      g_string_free(m.ews, TRUE);
    }
    g_ptr_array_free(m.names, TRUE);
    g_ptr_array_free(m.wahrscheinlichkeitstexte, TRUE);
    g_array_free(m.ps, TRUE);
    g_strfreev(ns);
    g_strfreev(vs);
  }
  gtk_window_destroy(GTK_WINDOW(d));
}
void binomialmodell_dialog(GtkWidget *w, gpointer d) {
  modell_dialog(w, d, TRUE);
}
void pfadvorlage_dialog(GtkWidget *w, gpointer d) {
  modell_dialog(w, d, FALSE);
}
