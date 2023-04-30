void xy(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  GtkWidget *freifeld = gtk_entry_new();
  gtk_layout_put (GTK_LAYOUT (data), freifeld, event->x,event->y);
  gtk_widget_show(freifeld);
}
