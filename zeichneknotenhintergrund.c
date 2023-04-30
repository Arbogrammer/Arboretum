static gboolean zeichneknotenhintergrund(GtkWidget *widget, cairo_t *cr, gpointer data)
{
  cairo_set_source_rgba(cr, 0, 0, 1, 0);
  cairo_paint(cr);
  return FALSE;
}
