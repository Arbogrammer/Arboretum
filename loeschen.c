void loeschen()
{
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  int tempzaehler = knotenexistiert(gtk_widget_get_name(aktuelleswidget));
  gtk_widget_grab_focus(textfeld[0]);
//  printf("%i\n",tempzaehler);
  maxzaehler -= 1;
  gtk_widget_destroy(textfeld[tempzaehler]);
}
