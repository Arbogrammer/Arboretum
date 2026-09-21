void tempspeichern()
{
  int i=0;
  FILE *datei;
  TEMPDATEI dateinummerierung);
  datei = fopen(dateiname,"w+");
  if(!datei)
  {
    g_warning("Undo-Zustand konnte nicht gespeichert werden: %s", dateiname);
    return;
  }

  int schreibfehler = 0;
  for(i=0 ; i<=maxzaehler ; i++)
  {
    if(fprintf(datei,"%i%c%i%c%s%c%s%c\n",i,31,y[i],31,gtk_widget_get_name(textfeld[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),31) < 0)
      schreibfehler = 1;
  }
  if(fprintf(datei,"%c\n",30) < 0) schreibfehler = 1;
  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    if(fprintf(datei,"%i%c%i%c%s%c%s%c\n",i,31,yerg[i],31,gtk_widget_get_name(textfeldErgebnis[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),31) < 0)
      schreibfehler = 1;
  }
  if(fprintf(datei,"%c\n",30) < 0) schreibfehler = 1;

  for(i=0 ; i<=maxzaehler ; i++)
  {
    if(fprintf(datei,"%i%c%s%c%s%c\n",i,31,gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),31) < 0)
      schreibfehler = 1;
  }

  if(fprintf(datei,"%c\n",30) < 0) schreibfehler = 1;

  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    if(fprintf(datei,"%i%c%s%c%s%c\n",i,31,gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),31) < 0)
      schreibfehler = 1;
//    printf("%i,%i,%s,%s\n",i,yerg[i],gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
  }

  if(fprintf(datei,"%c\n",30) < 0) schreibfehler = 1;

  /* Darstellungszustand: Er gehört genauso zum Undo wie Texte und Knoten. */
  double darstellungszahlen[] = {
    padding, paddingk,
    hintergrundfarbe.red, hintergrundfarbe.green, hintergrundfarbe.blue, hintergrundfarbe.alpha,
    zweigfarbe.red, zweigfarbe.green, zweigfarbe.blue, zweigfarbe.alpha,
    knotenhintergrundfarbe.red, knotenhintergrundfarbe.green,
    knotenhintergrundfarbe.blue, knotenhintergrundfarbe.alpha,
    knotenrandfarbe.red, knotenrandfarbe.green, knotenrandfarbe.blue, knotenrandfarbe.alpha,
    schriftfarbe.red, schriftfarbe.green, schriftfarbe.blue, schriftfarbe.alpha
  };
  char zahlstrings[G_N_ELEMENTS(darstellungszahlen)][G_ASCII_DTOSTR_BUF_SIZE];
  for(i=0; i<(int)G_N_ELEMENTS(darstellungszahlen); i++)
    g_ascii_dtostr(zahlstrings[i], sizeof(zahlstrings[i]), darstellungszahlen[i]);

  if(fprintf(datei,
      "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%d%c%d%c%d%c%d%c%d%c"
      "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c"
      "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c"
      "%d%c%d%c%d%c%d%c%d%c%s\n",
      RandLinks,31,RandRechts,31,RandOben,31,RandUnten,31,StufenBreite,31,
      KnotenAbstand,31,ErgebnisAbstand,31,LinienDicke,31,KnotenTextBreite,31,
      WahrscheinlichkeitTextBreite,31,(int)(unsigned char)ErgebnisTrenner,31,
      genauigkeit,31,zahlstrings[0],31,zahlstrings[1],31,bruchou,31,kuerzen,31,
      knotenrahmenabstand,31,wskverschiebung,31,knotenrahmendicke,31,
      zahlstrings[2],31,zahlstrings[3],31,zahlstrings[4],31,zahlstrings[5],31,
      zahlstrings[6],31,zahlstrings[7],31,zahlstrings[8],31,zahlstrings[9],31,
      zahlstrings[10],31,zahlstrings[11],31,zahlstrings[12],31,zahlstrings[13],31,
      zahlstrings[14],31,zahlstrings[15],31,zahlstrings[16],31,zahlstrings[17],31,
      zahlstrings[18],31,zahlstrings[19],31,zahlstrings[20],31,zahlstrings[21],31,
      knotenhintergrundfarbewurdegeaendert,31,knotenrahmenfarbewurdegeaendert,31,
      knotenrahmendickegeaendert,31,schriftfarbewurdegeaendert,31,
      schriftartwurdegewechselt,31,
      schriftart) < 0)
    schreibfehler = 1;

  if(fflush(datei) != 0)
    schreibfehler = 1;
  if(fclose(datei) != 0)
    schreibfehler = 1;

  if(schreibfehler)
  {
    g_warning("Undo-Zustand konnte nicht vollständig gespeichert werden: %s", dateiname);
    remove(dateiname);
    return;
  }

  dateinummerierung += 1;
}
