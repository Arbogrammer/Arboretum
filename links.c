gboolean links(GtkWidget *widget, gpointer data) {
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
    linkswsk(widget, data);
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

  if (Stufe != 0) {
    Stufe -= 1;
  } else {
    return FALSE;
  }

  int i = 0;
  while (vorgaenger[zaehler] != &textfeld[i]) {
    i++;
  }
  zaehler = i;
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
  gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]), -1);
  int aktuellerKnoten =
      atoi(gtk_widget_get_name(textfeld[zaehler]) +
           (int)(strrchr(gtk_widget_get_name(textfeld[zaehler]), '-') -
                 &(gtk_widget_get_name(textfeld[zaehler])[0]) + 1));

  Knoten[Stufe] = aktuellerKnoten;
  return TRUE;
}
