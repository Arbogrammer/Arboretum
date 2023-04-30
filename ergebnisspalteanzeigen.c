void ergebnisspalteanzeigen(gpointer data)
{
  if(labelein)
  {
    int i=0;
    if(gtk_widget_get_visible(ergebnislabel[0]))
    {
    for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_hide(ergebnislabel[i]);
      }
      ergebnisseanzeigen = 0;
    }
    else
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_show(ergebnislabel[i]);
      }
      ergebnisseanzeigen = 1;
    }
  }
  else
  {
    int i=0;
    if(gtk_widget_get_visible(textfeldErgebnis[0]))
    {
    for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_hide(textfeldErgebnis[i]);
      }
      ergebnisseanzeigen = 0;
    }
    else
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_show(textfeldErgebnis[i]);
      }
      ergebnisseanzeigen = 1;
    }
  }
  gtk_widget_queue_draw (da);
}
