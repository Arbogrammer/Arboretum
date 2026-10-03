/* History owns complete snapshots, independent of scratch files used to rebuild
 * widgets. Text edits take one snapshot before a burst, including paste/cut. */
static GPtrArray *undo_schritte, *redo_schritte;
static gboolean undo_intern = FALSE;
static GtkWidget *undo_feld;
static gint64 undo_zeit;

static void undo_gruppe_beenden(void) {
  g_clear_object(&undo_feld);
  undo_zeit = 0;
}

static void undo_leeren(void) {
  undo_gruppe_beenden();
  g_clear_pointer(&undo_schritte, g_ptr_array_unref);
  g_clear_pointer(&redo_schritte, g_ptr_array_unref);
}

static void undo_ablegen(GPtrArray **liste, const char *inhalt) {
  if (!*liste) *liste = g_ptr_array_new_with_free_func(g_free);
  if ((*liste)->len && !strcmp(g_ptr_array_index(*liste, (*liste)->len - 1), inhalt))
    return;
  g_ptr_array_add(*liste, g_strdup(inhalt));
  if ((*liste)->len > 100) g_ptr_array_remove_index(*liste, 0);
}

static void undo_snapshot_merken(const char *pfad) {
  if (undo_intern) return;
  undo_gruppe_beenden();
  g_autofree char *inhalt = NULL;
  if (g_file_get_contents(pfad, &inhalt, NULL, NULL)) {
    undo_ablegen(&undo_schritte, inhalt);
    g_clear_pointer(&redo_schritte, g_ptr_array_unref);
  }
}

static void undo_vor_eingabe(GtkEditable *editable) {
  GtkWidget *feld = GTK_WIDGET(editable);
  if (undo_intern || letzte_wahrscheinlichkeit_wird_gesetzt ||
      !gtk_widget_get_parent(feld)) return;
  gboolean baumfeld = FALSE;
  for (int i = 0; i <= maxzaehler; i++)
    baumfeld |= feld == textfeld[i] || feld == textfeldWahrscheinlichkeit[i];
  if (!baumfeld) return;
  gint64 jetzt = g_get_monotonic_time();
  if (undo_feld != feld || jetzt - undo_zeit > G_USEC_PER_SEC) {
    tempspeichern();
    undo_feld = g_object_ref(feld);
  }
  undo_zeit = jetzt;
}

static void undo_vor_einfuegen(GtkEditable *editable, const char *text,
                                int laenge, int *position, gpointer data) {
  undo_vor_eingabe(editable);
}
static void undo_vor_loeschen(GtkEditable *editable, int start, int ende,
                               gpointer data) {
  undo_vor_eingabe(editable);
}

static void undo_wechseln(gpointer data, gboolean wiederholen) {
  undo_gruppe_beenden();
  GPtrArray **quelle = wiederholen ? &redo_schritte : &undo_schritte;
  GPtrArray **ziel = wiederholen ? &undo_schritte : &redo_schritte;
  if (!*quelle || !(*quelle)->len) return;
  undo_intern = TRUE;
  int vorher = dateinummerierung;
  tempspeichern();
  if (dateinummerierung == vorher) {
    undo_intern = FALSE;
    return;
  }
  g_autofree char *pfad = arboretum_temp_path(vorher);
  g_autofree char *aktuell = NULL;
  if (!g_file_get_contents(pfad, &aktuell, NULL, NULL)) {
    undo_intern = FALSE;
    return;
  }
  while ((*quelle)->len && !strcmp(aktuell,
      g_ptr_array_index(*quelle, (*quelle)->len - 1)))
    g_ptr_array_remove_index(*quelle, (*quelle)->len - 1);
  if ((*quelle)->len) {
    const char *inhalt = g_ptr_array_index(*quelle, (*quelle)->len - 1);
    if (g_file_set_contents(pfad, inhalt, -1, NULL)) {
      undo_ablegen(ziel, aktuell);
      zurueck = 1;
      templaden(data);
      g_ptr_array_remove_index(*quelle, (*quelle)->len - 1);
      dateiveraendert++;
    }
  }
  undo_intern = FALSE;
}

void wiederherstellen(GtkWidget *widget, gpointer data) {
  undo_wechseln(data, TRUE);
}
