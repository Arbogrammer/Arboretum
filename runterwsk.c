/* Liefert das nächste Wahrscheinlichkeitsfeld auf derselben Baumstufe.
 * Die Knoten einer Stufe können zu unterschiedlichen Eltern gehören. Ihre
 * vertikale Position ist daher die natürliche Eingabereihenfolge: Nach dem
 * letzten Kind eines Elternknotens folgt das erste Kind des nächsten. */
static int wsk_nachbar_auf_stufe(int aktueller_index, int richtung) {
  if (aktueller_index < 0 || aktueller_index > maxzaehler)
    return -1;

  const char *aktueller_name = gtk_widget_get_name(textfeld[aktueller_index]);
  int stufe = zeichenzaehlen(aktueller_name, '-') - 1;
  int bester_index = -1;

  for (int i = 0; i <= maxzaehler; i++) {
    if (i == aktueller_index || !textfeld[i] || !textfeldWahrscheinlichkeit[i])
      continue;
    if (zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1 != stufe)
      continue;

    gboolean liegt_in_richtung =
        richtung > 0 ? y[i] > y[aktueller_index] : y[i] < y[aktueller_index];
    if (!liegt_in_richtung)
      continue;

    if (bester_index == -1 ||
        (richtung > 0 ? y[i] < y[bester_index] : y[i] > y[bester_index]))
      bester_index = i;
  }

  return bester_index;
}

gboolean runterwsk(GtkWidget *widget, gpointer data) {
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  int aktuell_index = wskexistiert(gtk_widget_get_name(aktuelleswidget));
  int naechster_index = wsk_nachbar_auf_stufe(aktuell_index, 1);
  if (naechster_index != -1) {
    gtk_entry_grab_focus_without_selecting(
        GTK_ENTRY(textfeldWahrscheinlichkeit[naechster_index]));
    gtk_editable_set_position(
        GTK_EDITABLE(textfeldWahrscheinlichkeit[naechster_index]), -1);
  } else {
    return FALSE;
  }
  return TRUE;
}
