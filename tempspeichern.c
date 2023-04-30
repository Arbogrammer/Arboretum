void tempspeichern()
{
  int i=0;
  FILE *datei;
  TEMPDATEI dateinummerierung);
  dateinummerierung += 1;
  datei = fopen(dateiname,"w+");
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
//    printf("%i,%i,%s,%s\n",i,yerg[i],gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
  }

  fclose(datei);
}
