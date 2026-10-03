gboolean hoch(GtkWidget *widget, gpointer data) {
  if (labelein == 1) {
    return FALSE;
  }
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  if (aktuelleswidget == NULL) {
    aktuelleswidget = textfeld[0];
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[0])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[0]), -1);
  }

  if (strrchr(gtk_widget_get_name(aktuelleswidget), 'W')) {
    hochwsk(widget, data);
    return FALSE;
  }

  zaehler = knotenexistiert(gtk_widget_get_name(aktuelleswidget));
  if (zaehler == -1) {
    return FALSE;
  }
  Stufe = zeichenzaehlen(gtk_widget_get_name(aktuelleswidget), '-') - 1;
  Knoten[Stufe] =
      atoi(gtk_widget_get_name(aktuelleswidget) +
           (int)(strrchr(gtk_widget_get_name(aktuelleswidget), '-') -
                 &(gtk_widget_get_name(aktuelleswidget)[0]) + 1));

  const char *aktueller_name = gtk_widget_get_name(aktuelleswidget);
  g_autofree gchar *namensanfang = g_strndup(
      aktueller_name, (gsize)(strrchr(aktueller_name, '-') - aktueller_name));
  g_autofree gchar *name = g_strdup_printf(
      "%s-%i", namensanfang, (Knoten[Stufe] > 0) ? Knoten[Stufe] - 1 : 0);

  int i = 0;
  while (strcmp(name, gtk_widget_get_name(textfeld[i]))) {
    i++;
  }
  zaehler = i;
  if (Knoten[Stufe] > 0) {
    Knoten[Stufe] -= 1;
  }
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
  gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]), -1);
  gtk_widget_queue_draw(da);
  return TRUE;
}
