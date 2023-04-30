void wskergebnisspalteanzeigen(gpointer data)
{
  if(labelein)
  {
    int i=0;
    if(gtk_widget_get_visible(ergebniswsklabel[0]))
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_hide(ergebniswsklabel[i]);
      }
      ergebnissewskanzeigen = 0;
    }
    else
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_show(ergebniswsklabel[i]);
      }
      ergebnissewskanzeigen = 1;
    }
  }
  else
  {
    int i=0;
    if(gtk_widget_get_visible(textfeldErgebnisWahrscheinlichkeit[0]))
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_hide(textfeldErgebnisWahrscheinlichkeit[i]);
      }
      ergebnissewskanzeigen = 0;
    }
    else
    {
      for(i=0; i <= maxzaehlererg ; i++)
      {
        gtk_widget_show(textfeldErgebnisWahrscheinlichkeit[i]);
      }
      ergebnissewskanzeigen = 1;
    }
  }
  gtk_widget_queue_draw (da);
}
