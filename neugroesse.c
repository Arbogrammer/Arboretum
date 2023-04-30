void neugroesse(GtkWidget *widget, GtkAllocation *allocation, void *data)
{
  if(allocation)
  {
    ErgebnisBreite = allocation->width;
    GROESSEDRAWINGAREA
    GROESSELAYOUTD
  }
  else
  {
    ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
    GROESSEDRAWINGAREA
    GROESSELAYOUTD
  }
}

/*
void neugroesse(gpointer data)
{
  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
}
*/
