gboolean linkswsk(GtkWidget *widget, gpointer data) {
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  const char *aktuell = gtk_widget_get_name(aktuelleswidget);
  g_autofree gchar *ohne_w = g_strndup(aktuell, strlen(aktuell) - 1);
  char *letzter = strrchr(ohne_w, '-');
  g_autofree gchar *name =
      letzter ? g_strdup_printf("%.*sW", (int)(letzter - ohne_w), ohne_w)
              : g_strdup("W");
  if (wskexistiert(name) != -1) {
    gtk_entry_grab_focus_without_selecting(
        GTK_ENTRY((textfeldWahrscheinlichkeit[wskexistiert(name)])));
    gtk_editable_set_position(
        GTK_EDITABLE(textfeldWahrscheinlichkeit[wskexistiert(name)]), -1);
  } else {
    return FALSE;
  }
  return TRUE;
}
