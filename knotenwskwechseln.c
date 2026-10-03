void knotenwskwechseln() {
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  if (aktuelleswidget) {
    g_autoptr(GString) name =
        g_string_new(gtk_widget_get_name(aktuelleswidget));
    if (name->str[name->len - 1] == 'W') {
      g_string_truncate(name, name->len - 1);
      gtk_entry_grab_focus_without_selecting(
          GTK_ENTRY(textfeld[knotenexistiert(name->str)]));
    } else {
      g_string_append_c(name, 'W');
      gtk_entry_grab_focus_without_selecting(
          GTK_ENTRY(textfeldWahrscheinlichkeit[wskexistiert(name->str)]));
    }
  }
}
