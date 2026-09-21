void xy(GtkWidget *widget, double x, double y, gpointer data)
{
  GtkWidget *freifeld = gtk_entry_new();
  gtk_layout_put (GTK_LAYOUT (data), freifeld, x, y);
  gtk_widget_show(freifeld);
}
