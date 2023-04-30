int anzahlnachfolger(int i)
{
  char tempname[10000] = "", temptempname[10000] = "";
  strcpy(tempname, gtk_widget_get_name(textfeld[i]));
  strcpy(temptempname, tempname);
  char nachfolgerstring[22] = "";
  int knotenzaehler = 0;
  sprintf(nachfolgerstring,"-%i",knotenzaehler);
  strcat(tempname,nachfolgerstring);
  while(knotenexistiert(tempname) > -1)
  {
    knotenzaehler += 1;
    memset(nachfolgerstring,0,22);
    sprintf(nachfolgerstring,"-%i",knotenzaehler);
    memset(tempname,0,10000);
    strcat(tempname,temptempname);
    strcat(tempname,nachfolgerstring);
  }
  return knotenzaehler;
}
