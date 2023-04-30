int zaehleruntenrechts(int zaehlerlinks)
{
  int knotenzaehler = 0;
  char tempname[10000] = "", knotennummer[22]="", ergebnis[10000] = "";
  sprintf(knotennummer,"-%i",knotenzaehler);
  strcpy(tempname,gtk_widget_get_name(textfeld[zaehlerlinks]));
  strcat(tempname,knotennummer);
  char tempnamelang[10000] = "";
  strcpy(tempnamelang,gtk_widget_get_name(textfeld[zaehlerlinks]));
  if(knotenexistiert(tempname) == -1)
  {
    return zaehlerlinks;
  }
//  printf("tempname: %s\n",tempname);
  while(knotenexistiert(tempnamelang) > -1)
  {
    while(knotenexistiert(tempname) > -1)
    {
      strcpy(ergebnis,tempname);
      knotenzaehler += 1;
      memset(tempname,0,10000);
      memset(knotennummer,0,22);
      strcpy(tempname,tempnamelang);
      sprintf(knotennummer,"-%i",knotenzaehler);
      strcat(tempname,knotennummer);
//      printf("tempname: %s\ntempnamelang: %s\n\n",tempname,tempnamelang);
    }
    sprintf(knotennummer,"-%i",knotenzaehler-1);
    strcat(tempnamelang,knotennummer);
//    printf("tempname: %s\ntempnamelang: %s\n\n",tempname,tempnamelang);
    knotenzaehler = 0;
    memset(tempname,0,10000);
    memset(knotennummer,0,22);
    strcpy(tempname,tempnamelang);
  }
//  printf("Letzter Knoten unten rechts: %s\n",ergebnis);
  return knotenexistiert(ergebnis);
}
