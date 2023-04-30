void wskergebnisverschieben(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
  int i;
  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
  for(i=0 ; i <= maxzaehlererg ; i++)
  {
    gtk_layout_move(GTK_LAYOUT(data),textfeldErgebnisWahrscheinlichkeit[i],FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisBreite:0),FensterRandOben+RandOben+yerg[i]);
  }
}
