void speichern(char *dateiname)
{
//  if(dateiname[strlen(dateiname)-1] != 'g' || dateiname[strlen(dateiname)-2] != 'd' || dateiname[strlen(dateiname)-3] != 'b' || dateiname[strlen(dateiname)-4] != '.')
//  {
//    strcat(dateiname,".bdg");
//  } 
  int i=0;
  FILE *datei;
  datei = fopen(dateiname,"w+");

  fprintf(datei,"%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%c%c%i%c%i%c%i%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%i%c%s%c%f%c%f%c%i%c%i%c%i%c%i%c%i%c%i%c\n", RandLinks,31,RandRechts,31,RandOben,31,RandUnten,31,FensterRandLinks,31,FensterRandRechts,31,FensterRandOben,31,FensterRandUnten,31,StufenBreite,31,KnotenAbstand,31,KnotenHoehe,31,KnotenBreite,31,ErgebnisAbstand,31,ErgebnisBreite,31,ErgebnisTextBreite,31,ErgebnisTrenner,31,LinienDicke,31,KnotenTextBreite,31,WahrscheinlichkeitTextBreite,31,hintergrundfarbe.red,31,hintergrundfarbe.green,31,hintergrundfarbe.blue,31,hintergrundfarbe.alpha,31,zweigfarbe.red,31,zweigfarbe.green,31,zweigfarbe.blue,31,zweigfarbe.alpha,31,knotenhintergrundfarbe.red,31,knotenhintergrundfarbe.green,31,knotenhintergrundfarbe.blue,31,knotenhintergrundfarbe.alpha,31,knotenrandfarbe.red,31,knotenrandfarbe.green,31,knotenrandfarbe.blue,31,knotenrandfarbe.alpha,31,schriftfarbe.red,31,schriftfarbe.green,31,schriftfarbe.blue,31,schriftfarbe.alpha,31,ergebnisseanzeigen,31,wskanzeigen,31,ergebnissewskanzeigen,31,labelein,31,KnotenLabelBreite,31,klbmax,31,KnotenLabelHoehe,31,ErgebnisLabelBreite,31,WahrscheinlichkeitErgebnisTextBreite,31,WahrscheinlichkeitErgebnisLabelBreite,31,ZaehlerErgebnisLabelBreite,31,NennerErgebnisLabelBreite,31,ZaehlerErgebnisLabelHoehe,31,NennerErgebnisLabelHoehe,31,schriftart,31,padding,31,paddingk,31,genauigkeit,31,kuerzen,31,bruchou,31,knotenrahmenabstand,31,wskverschiebung,31,knotenrahmendicke,31);
  fprintf(datei,"%c\n",30);

  for(i=0 ; i<=maxzaehler ; i++)
  {
    fprintf(datei,"%i%c%i%c%s%c%s%c\n",i,31,y[i],31,gtk_widget_get_name(textfeld[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),31);
  }

  fprintf(datei,"%c\n",30);

  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    fprintf(datei,"%i%c%i%c%s%c%s%c\n",i,31,yerg[i],31,gtk_widget_get_name(textfeldErgebnis[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),31);
  }

  fprintf(datei,"%c\n",30);

  for(i=0 ; i<=maxzaehler ; i++)
  {
    fprintf(datei,"%i%c%s%c%s%c\n",i,31,gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),31);
  }

  fprintf(datei,"%c\n",30);

  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    fprintf(datei,"%i%c%s%c%s%c\n",i,31,gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),31);
  }

  fclose(datei);
  dateiveraendert = 0;
}
