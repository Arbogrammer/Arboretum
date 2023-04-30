void wsklabelgroesse(GtkWidget *widget, GdkRectangle *ap, gpointer data)
{
  if(gtk_label_get_angle(GTK_LABEL(widget)) == 0)
  {
    breitevordrehung[wskexistiert(gtk_widget_get_name(widget))]=ap->width;
    hoehevordrehung[wskexistiert(gtk_widget_get_name(widget))]=ap->height;
  }
}
