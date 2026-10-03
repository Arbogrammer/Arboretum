gboolean hochwsk(GtkWidget *widget, gpointer data) {
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  int aktuell_index = wskexistiert(gtk_widget_get_name(aktuelleswidget));
  int vorheriger_index = wsk_nachbar_auf_stufe(aktuell_index, -1);
  if (vorheriger_index == -1) {
    return FALSE; // man ist schon ganz oben, also mache nichts
  }
  gtk_entry_grab_focus_without_selecting(
      GTK_ENTRY(textfeldWahrscheinlichkeit[vorheriger_index]));
  gtk_editable_set_position(
      GTK_EDITABLE(textfeldWahrscheinlichkeit[vorheriger_index]), -1);
  return TRUE;
}
