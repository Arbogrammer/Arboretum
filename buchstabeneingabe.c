/* Keep the entries and their native input contexts alive while editing.
 * Defer geometry updates until GTK has finished processing the edit. */
static guint eingabe_neuaufbau_id = 0;

/* GtkEntry legt seine Breite in durchschnittlichen Zeichen fest. Die reine
 * Zeichenanzahl reicht daher bei breiten Buchstaben wie „G“ nicht aus. */
static int textbreite_in_zeichen(GtkWidget *widget, const char *text) {
  PangoLayout *layout = gtk_widget_create_pango_layout(widget, text);
  PangoContext *context = gtk_widget_get_pango_context(widget);
  PangoFontMetrics *metrics =
      pango_context_get_metrics(context, NULL, pango_context_get_language(context));
  int textbreite = 0;
  /* GtkEntry bemisst width-chars anhand einer Ziffernbreite. Das entspricht
   * seinem tatsächlichen Layout besser als die allgemeinere Pango-Näherung. */
  int zeichenbreite = pango_font_metrics_get_approximate_digit_width(metrics);
  int laenge = g_utf8_strlen(text, -1);

  pango_layout_get_size(layout, &textbreite, NULL);

  g_object_unref(layout);
  pango_font_metrics_unref(metrics);

  if (zeichenbreite <= 0)
    return MAX(2, laenge);
  return MAX(2, (textbreite + zeichenbreite - 1) / zeichenbreite);
}

static int knoten_textbreite_fuer_stufe(int stufe) {
  return MAX(2, KnotenTextBreiteStufe[stufe]);
}

static gboolean eingabe_neuaufbauen(gpointer data) {
  eingabe_neuaufbau_id = 0;
  for (int i = 0; i <= maxzaehler; i++) {
    int stufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    gtk_entry_set_width_chars(GTK_ENTRY(textfeld[i]),
                              knoten_textbreite_fuer_stufe(stufe));
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldWahrscheinlichkeit[i]),
                              WahrscheinlichkeitTextBreite);
  }
  for (int i = 0; i <= maxzaehlererg; i++) {
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[i]),
                              ErgebnisTextBreite);
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]),
                              WahrscheinlichkeitErgebnisTextBreite);
  }
  groesseneu(NULL, NULL, data);
  /* Ergebniswahrscheinlichkeiten folgen derselben Geometrie wie die
   * Ergebnisfelder. Eine eigene horizontale Zwischenposition würde beim
   * Tippen in der vertikalen Ansicht sichtbar aufflackern. */
  wskergebnisverschieben(NULL, NULL, data);
  positionsanpassungwsk(data);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  gtk_widget_queue_draw(da);
  return G_SOURCE_REMOVE;
}

static void eingabe_neuaufbau_planen(gpointer data) {
  if (!eingabe_neuaufbau_id)
    eingabe_neuaufbau_id =
        g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, eingabe_neuaufbauen,
                        g_object_ref(data), g_object_unref);
}

void buchstabeneingabe(GtkEditable *editable, gpointer data) {

  dateiveraendert++;

  g_autofree gchar *tempname = NULL;

  int i = 0;
  KnotenTextBreite = 2;
  for (i = 0; i <= maxStufe; i++)
    KnotenTextBreiteStufe[i] = 2;

  for (i = 0; i <= maxzaehler; i++) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(textfeld[i]));
    int breite = textbreite_in_zeichen(textfeld[i], text);
    int stufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    KnotenTextBreiteStufe[stufe] = MAX(KnotenTextBreiteStufe[stufe], breite);
    if (KnotenTextBreite < breite) {
      KnotenTextBreite = breite;
    }
  }

  tempname = g_strdup(gtk_widget_get_name(GTK_WIDGET(editable)));
  i = 0;
  for (i = 0; i <= maxzaehlererg; i++) {
    if (strlen(tempname) < strlen(gtk_widget_get_name(textfeldErgebnis[i]))) {
      if (strncmp(gtk_widget_get_name(textfeldErgebnis[i]), tempname,
                  strlen(tempname)) == 0) {
        ergebnistextneuschreiben(textfeldErgebnis[i]);
      }
    }
  }
  arboretum_refresh_entry_overlines();
  tempspeichern();
  eingabe_neuaufbau_planen(data);
}
