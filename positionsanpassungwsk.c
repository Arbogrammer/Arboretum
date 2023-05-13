void positionsanpassungwsk(gpointer data)
{
  int i=0;
  char unten[22] = "";
//  printf("anzahlknoteninstufe(0)-1: %i\n",anzahlknoteninstufe(0)-1);
  sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
  int yunten = y[knotenexistiert(unten)];
//  printf("yunten: %i, KnotenHoehe: %i, WahrscheinlichkeitsHoehe: %i, StufenBreite: %i, WahrscheinlichkeitsBreite: %i, FensterRandLinks: %i, RandLinks: %i, FensterRandOben: %i, RandOben: %i\n",yunten,KnotenHoehe,WahrscheinlichkeitHoehe,StufenBreite,WahrscheinlichkeitBreite,FensterRandLinks,RandLinks,FensterRandOben,RandOben);
  while(i<=maxzaehler)
  {
    if(zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),'-') == 1)
    {
//      printf("Erste Stufe:\n");
//      printf("Der Wsk-Knoten %s liegt bei y = %i\n",gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
      gtk_layout_move (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[i], FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
      i++;
   }
    else
    {
      int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),'-')-1;
      int ywsktemp=(y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]+y[i])/2;
//      printf("Zweite Stufe:\n");
//      printf("Der Wsk-Knoten %s liegt bei y = %i\n",gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),ywsktemp);
//      printf("Der Vorgänger hat den Namen %s\n",gtk_widget_get_name(*vorgaenger[i]));
//      printf("knotenexistiert(gtk_widget_get_name(*vorgaenger[%i])) = %i\n",i,knotenexistiert(gtk_widget_get_name(*vorgaenger[i])));
//      printf("y[knotenexistiert(gtk_widget_get_name(*vorgaenger[%i]))] = %i, y[%i] = %i\n",i, y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))],i,y[i]);
      gtk_layout_move (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[i], ((FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufetemp)+(FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufetemp-1)+KnotenBreite))/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+ywsktemp);
      i++;
    }
  }
}
