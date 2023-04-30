void positionsanpassungwsk(gpointer data)
{
  int i=0;
  char unten[22] = "";
  sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
  int yunten = y[knotenexistiert(unten)];
  while(i<=maxzaehler)
  {
    if(zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),'-') == 1)
    {
      gtk_layout_move (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[i], FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
      i++;
   }
    else
    {
      int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),'-')-1;
      int ywsktemp=(y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]+y[i])/2;
      gtk_layout_move (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[i], ((FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufetemp)+(FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufetemp-1)+KnotenBreite))/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+ywsktemp);
      i++;
    }
  }
}
