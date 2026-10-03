/* Generator für Urnenmodelle.  Er schreibt zunächst eine normale .bdg-Datei
 * mit den aktuellen Darstellungseinstellungen und lädt diese anschließend.
 * Dadurch verwendet der erzeugte Baum denselben, gut getesteten Ladeweg wie
 * eine von Hand gespeicherte Datei. */
typedef struct {
  GPtrArray *namen;
  GArray *anzahlen;
  int ziehungen;
  int modus; /* 0: ohne, 1: mit Zurücklegen, 2: dazulegen */
  GString *knoten, *ergebnisse, *wsk, *ergebniswsk;
  GPtrArray *knoten_zeilen, *wsk_zeilen;
  int knotenindex, ergebnisindex;
  gboolean zu_gross;
  gpointer layout;
} Urnenbaum;

static void urnen_fehler(const char *text) {
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", text);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static void urnen_zeile(GString *ziel, const char *format, ...) {
  va_list args;
  va_start(args, format);
  g_string_append_vprintf(ziel, format, args);
  va_end(args);
}

static void urnen_knoten_zeile(Urnenbaum *baum, int index, int ypos,
                               const char *name, const char *text) {
  while (baum->knoten_zeilen->len <= (guint)index)
    g_ptr_array_add(baum->knoten_zeilen, NULL);
  g_ptr_array_index(baum->knoten_zeilen, index) = g_strdup_printf(
      "%d%c%d%c%s%c%s%c\n", index, 31, ypos, 31, name, 31, text, 31);
}

static void urnen_wsk_zeile(Urnenbaum *baum, int index, const char *name,
                            const char *wert) {
  while (baum->wsk_zeilen->len <= (guint)index)
    g_ptr_array_add(baum->wsk_zeilen, NULL);
  g_ptr_array_index(baum->wsk_zeilen, index) =
      g_strdup_printf("%d%c%sW%c%s%c\n", index, 31, name, 31, wert, 31);
}

static void urnen_baum_bauen(Urnenbaum *baum, const char *name,
                             const int *bestand, int tiefe, long double pfadwsk,
                             const char *beschriftung, int *y) {
  if (baum->zu_gross)
    return;
  (void)pfadwsk;
  int eigener_index = baum->knotenindex++;
  if (baum->knotenindex > MAX_KNOTEN) {
    baum->zu_gross = TRUE;
    return;
  }

  int summe = 0;
  for (guint i = 0; i < baum->anzahlen->len; i++)
    summe += bestand[i];

  if (tiefe == baum->ziehungen || summe == 0) {
    int ypos = (*y)++ * (KnotenHoehe + KnotenAbstand);
    urnen_knoten_zeile(baum, eigener_index, ypos, name, beschriftung);
    urnen_wsk_zeile(baum, eigener_index, name, tiefe ? "1" : "");
    if (tiefe) {
      const char *pfad = name + 1; /* ohne den führenden Bindestrich */
      GString *ergebnis = g_string_new(NULL);
      gchar **teile = g_strsplit(pfad, "-", -1);
      for (int i = 0; teile[i]; i++) {
        if (i > 0 && ErgebnisTrenner)
          g_string_append_c(ergebnis, ErgebnisTrenner);
        g_string_append(ergebnis, teile[i]);
      }
      urnen_zeile(baum->ergebnisse, "%d%c%d%c%s-E%c%s%c\n", baum->ergebnisindex,
                  31, ypos, 31, name, 31, ergebnis->str, 31);
      urnen_zeile(baum->ergebniswsk, "%d%c%s-EW%c%s%c\n", baum->ergebnisindex++,
                  31, name, 31, "0", 31);
      g_string_free(ergebnis, TRUE);
      g_strfreev(teile);
    }
    return;
  }

  int erstes_y = -1, letztes_y = -1;
  int kindnummer = 0;
  for (guint i = 0; i < baum->namen->len; i++) {
    if (bestand[i] <= 0)
      continue;
    int *neu = g_new(int, baum->anzahlen->len);
    memcpy(neu, bestand, sizeof(int) * baum->anzahlen->len);
    if (baum->modus == 0)
      neu[i]--;
    else if (baum->modus == 2)
      neu[i]++;
    g_autofree gchar *kindname = g_strdup_printf("%s-%d", name, kindnummer++);
    int vorher = *y;
    urnen_baum_bauen(baum, kindname, neu, tiefe + 1,
                     pfadwsk * (long double)bestand[i] / summe,
                     g_ptr_array_index(baum->namen, i), y);
    g_free(neu);
    if (baum->zu_gross)
      return;
    if (erstes_y < 0)
      erstes_y = vorher * (KnotenHoehe + KnotenAbstand);
    letztes_y = (*y - 1) * (KnotenHoehe + KnotenAbstand);
  }
  int ypos = erstes_y < 0 ? 0 : (erstes_y + letztes_y) / 2;
  urnen_knoten_zeile(baum, eigener_index, ypos, name, beschriftung);
  urnen_wsk_zeile(baum, eigener_index, name, tiefe ? "0" : "");
}

static void urnen_wsk_eintragen(Urnenbaum *baum, const char *name,
                                const int *bestand, int tiefe) {
  if (tiefe == baum->ziehungen)
    return;
  int summe = 0;
  for (guint i = 0; i < baum->anzahlen->len; i++)
    summe += bestand[i];
  int kindnummer = 0;
  for (guint i = 0; i < baum->namen->len; i++)
    if (bestand[i] > 0) {
      g_autofree gchar *kind = g_strdup_printf("%s-%d", name, kindnummer++);
      int index = knotenexistiert(kind);
      if (index >= 0) {
        g_autofree gchar *bruchtext =
            g_strdup_printf("%d/%d", bestand[i], summe);
        g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[index],
                                        G_CALLBACK(wskeingabe), baum->layout);
        gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[index]),
                           bruchtext);
        g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[index],
                                          G_CALLBACK(wskeingabe), baum->layout);
        int *neu = g_new(int, baum->anzahlen->len);
        memcpy(neu, bestand, sizeof(int) * baum->anzahlen->len);
        if (baum->modus == 0)
          neu[i]--;
        else if (baum->modus == 2)
          neu[i]++;
        urnen_wsk_eintragen(baum, kind, neu, tiefe + 1);
        g_free(neu);
      }
    }
}

/* Die oberste sichtbare Stufe ist bereits die erste Ziehung; sie braucht
 * daher keinen künstlichen, leeren Wurzelknoten. */
static void urnen_erste_wsk_eintragen(Urnenbaum *baum, const int *bestand) {
  int summe = 0;
  for (guint i = 0; i < baum->anzahlen->len; i++)
    summe += bestand[i];
  int kindnummer = 0;
  for (guint i = 0; i < baum->namen->len; i++)
    if (bestand[i] > 0) {
      g_autofree gchar *name = g_strdup_printf("-%d", kindnummer++);
      int index = knotenexistiert(name);
      if (index < 0)
        continue;
      g_autofree gchar *bruchtext = g_strdup_printf("%d/%d", bestand[i], summe);
      g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[index],
                                      G_CALLBACK(wskeingabe), baum->layout);
      gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[index]),
                         bruchtext);
      g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[index],
                                        G_CALLBACK(wskeingabe), baum->layout);
      int *neu = g_new(int, baum->anzahlen->len);
      memcpy(neu, bestand, sizeof(int) * baum->anzahlen->len);
      if (baum->modus == 0)
        neu[i]--;
      else if (baum->modus == 2)
        neu[i]++;
      urnen_wsk_eintragen(baum, name, neu, 1);
      g_free(neu);
    }
}

static gboolean urnen_generieren(GtkWidget *dialog, gpointer daten) {
  GtkWidget **felder = daten;
  const char *namen_text = gtk_editable_get_text(GTK_EDITABLE(felder[0]));
  const char *anzahlen_text = gtk_editable_get_text(GTK_EDITABLE(felder[1]));
  int ziehungen = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(felder[2]));
  int modus = gtk_drop_down_get_selected(GTK_DROP_DOWN(felder[3]));
  gchar **namen = g_strsplit(namen_text, ",", -1);
  gchar **anzahlen = g_strsplit(anzahlen_text, ",", -1);
  int anzahl = g_strv_length(namen);
  if (anzahl < 1 || (guint)anzahl != g_strv_length(anzahlen)) {
    urnen_fehler("Bitte geben Sie gleich viele, nicht leere Bezeichnungen und "
                 "Anzahlen ein.");
    goto ende;
  }
  Urnenbaum baum = {0};
  baum.namen = g_ptr_array_new_with_free_func(g_free);
  baum.anzahlen = g_array_new(FALSE, FALSE, sizeof(int));
  baum.ziehungen = ziehungen;
  baum.modus = modus;
  baum.layout = felder[4];
  for (int i = 0; i < anzahl; i++) {
    gchar *n = g_strstrip(namen[i]);
    char *endezahl = NULL;
    long wert = strtol(g_strstrip(anzahlen[i]), &endezahl, 10);
    if (!*n || !endezahl || *endezahl || wert < 0 || wert > 100000) {
      urnen_fehler("Bezeichnungen dürfen nicht leer sein; Anzahlen müssen "
                   "nichtnegative ganze Zahlen sein.");
      g_ptr_array_free(baum.namen, TRUE);
      g_array_free(baum.anzahlen, TRUE);
      goto ende;
    }
    for (guint j = 0; j < baum.namen->len; j++)
      if (!strcmp(n, g_ptr_array_index(baum.namen, j))) {
        urnen_fehler("Jede Bezeichnung darf nur einmal vorkommen.");
        g_ptr_array_free(baum.namen, TRUE);
        g_array_free(baum.anzahlen, TRUE);
        goto ende;
      }
    g_ptr_array_add(baum.namen, g_strdup(n));
    int w = (int)wert;
    g_array_append_val(baum.anzahlen, w);
  }
  int gesamt = 0;
  for (guint i = 0; i < baum.anzahlen->len; i++)
    gesamt += g_array_index(baum.anzahlen, int, i);
  if (!gesamt) {
    urnen_fehler("Die Urne muss mindestens eine Kugel enthalten.");
    g_ptr_array_free(baum.namen, TRUE);
    g_array_free(baum.anzahlen, TRUE);
    goto ende;
  }
  baum.knoten = g_string_new(NULL);
  baum.ergebnisse = g_string_new(NULL);
  baum.wsk = g_string_new(NULL);
  baum.ergebniswsk = g_string_new(NULL);
  baum.knoten_zeilen = g_ptr_array_new_with_free_func(g_free);
  baum.wsk_zeilen = g_ptr_array_new_with_free_func(g_free);
  int y = 0;
  int kindnummer = 0;
  for (guint i = 0; i < baum.namen->len; i++)
    if (g_array_index(baum.anzahlen, int, i) > 0) {
      int *neu = g_new(int, baum.anzahlen->len);
      memcpy(neu, baum.anzahlen->data, sizeof(int) * baum.anzahlen->len);
      if (baum.modus == 0)
        neu[i]--;
      else if (baum.modus == 2)
        neu[i]++;
      g_autofree gchar *name = g_strdup_printf("-%d", kindnummer++);
      urnen_baum_bauen(&baum, name, neu, 1, 1, g_ptr_array_index(baum.namen, i),
                       &y);
      g_free(neu);
      if (baum.zu_gross)
        break;
    }
  if (baum.zu_gross) {
    urnen_fehler("Dieses Urnenmodell würde mehr als 10.000 Knoten erzeugen. "
                 "Verringern Sie Ziehungen oder Anzahl der möglichen Farben.");
  } else {
    for (guint i = 0; i < baum.knoten_zeilen->len; i++)
      g_string_append(baum.knoten, g_ptr_array_index(baum.knoten_zeilen, i));
    for (guint i = 0; i < baum.wsk_zeilen->len; i++)
      g_string_append(baum.wsk, g_ptr_array_index(baum.wsk_zeilen, i));
    gchar *tmpname = NULL;
    int fd = g_file_open_tmp("arboretum-urne-XXXXXX.bdg", &tmpname, NULL);
    if (fd < 0 || !speichern(tmpname))
      urnen_fehler("Der Urnenbaum konnte nicht vorbereitet werden.");
    else {
      gchar *alt = NULL;
      gsize laenge = 0;
      if (g_file_get_contents(tmpname, &alt, &laenge, NULL)) {
        char *trennung = strchr(alt, 30);
        if (trennung) {
          GString *datei = g_string_new_len(alt, trennung - alt + 2);
          g_string_append(datei, baum.knoten->str);
          g_string_append_c(datei, 30);
          g_string_append_c(datei, '\n');
          g_string_append(datei, baum.ergebnisse->str);
          g_string_append_c(datei, 30);
          g_string_append_c(datei, '\n');
          g_string_append(datei, baum.wsk->str);
          g_string_append_c(datei, 30);
          g_string_append_c(datei, '\n');
          g_string_append(datei, baum.ergebniswsk->str);
          if (!g_file_set_contents(tmpname, datei->str, datei->len, NULL))
            urnen_fehler("Der Urnenbaum konnte nicht gespeichert werden.");
          else {
            /* Das Laden ersetzt den Baum vollständig. Deshalb muss der
             * bisherige Zustand vorher als genau ein Undo-Schritt gesichert
             * werden, wie bei jeder anderen strukturellen Änderung. */
            tempspeichern();
            laden(felder[4], tmpname);
            urnen_erste_wsk_eintragen(&baum, (int *)baum.anzahlen->data);
            for (int i = 0; i <= maxzaehlererg; i++) {
              ergebnistextneuschreiben(textfeldErgebnis[i]);
              wskergebnisneuschreiben(textfeldErgebnisWahrscheinlichkeit[i]);
            }
            dateiveraendert++;
            gtk_window_destroy(GTK_WINDOW(dialog));
          }
          g_string_free(datei, TRUE);
        }
        g_free(alt);
      }
    }
    if (fd >= 0) {
      close(fd);
      g_unlink(tmpname);
    }
    g_free(tmpname);
  }
  g_string_free(baum.knoten, TRUE);
  g_string_free(baum.ergebnisse, TRUE);
  g_string_free(baum.wsk, TRUE);
  g_string_free(baum.ergebniswsk, TRUE);
  g_ptr_array_free(baum.knoten_zeilen, TRUE);
  g_ptr_array_free(baum.wsk_zeilen, TRUE);
  g_ptr_array_free(baum.namen, TRUE);
  g_array_free(baum.anzahlen, TRUE);
ende:
  g_strfreev(namen);
  g_strfreev(anzahlen);
  return TRUE;
}

void urnenmodell_dialog(GtkWidget *widget, gpointer data) {
  (void)widget;
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Urnenmodell", GTK_WINDOW(window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Abbrechen",
      GTK_RESPONSE_CANCEL, "Baum erzeugen", GTK_RESPONSE_OK, NULL);
  GtkWidget *grid = gtk_grid_new();
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
  gtk_widget_set_margin_start(grid, 12);
  gtk_widget_set_margin_end(grid, 12);
  gtk_widget_set_margin_top(grid, 12);
  gtk_widget_set_margin_bottom(grid, 12);
  gtk_box_append(GTK_BOX(content), grid);
  GtkWidget *namen = gtk_entry_new();
  gtk_editable_set_text(GTK_EDITABLE(namen), "B, G, W");
  GtkWidget *anzahlen = gtk_entry_new();
  gtk_editable_set_text(GTK_EDITABLE(anzahlen), "2, 3, 1");
  GtkWidget *ziehungen = gtk_spin_button_new_with_range(1, 100, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ziehungen), 2);
  const char *modi[] = {"ohne Zurücklegen", "mit Zurücklegen",
                        "mit Dazulegen (gezogene Farbe)", NULL};
  GtkWidget *modus = gtk_drop_down_new_from_strings(modi);
  const char *labels[] = {
      "Bezeichnungen (mit Komma trennen):", "Anzahlen (mit Komma trennen):",
      "Anzahl Ziehungen:", "Modus:"};
  GtkWidget *werte[] = {namen, anzahlen, ziehungen, modus};
  for (int i = 0; i < 4; i++) {
    GtkWidget *label = gtk_label_new(labels[i]);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, i, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), werte[i], 1, i, 1, 1);
  }
  GtkWidget **felder = g_new(GtkWidget *, 5);
  felder[0] = namen;
  felder[1] = anzahlen;
  felder[2] = ziehungen;
  felder[3] = modus;
  felder[4] = data;
  gint antwort = gtk_dialog_run(GTK_DIALOG(dialog));
  if (antwort == GTK_RESPONSE_OK)
    urnen_generieren(dialog, felder);
  else
    gtk_window_destroy(GTK_WINDOW(dialog));
  g_free(felder);
}
