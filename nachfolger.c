int nachfolger(int i, int nachfolgernummer)
{
  char tempname[10000] = "";
  strcpy(tempname, gtk_widget_get_name(textfeld[i]));
  char nachfolgerstring[22] = "";
  sprintf(nachfolgerstring,"-%i",nachfolgernummer);
  strcat(tempname,nachfolgerstring);
  if(knotenexistiert(tempname) > -1)
  {
    return knotenexistiert(tempname);
  }
  else
  {
    fprintf(stderr,"Fehler: Knoten existiert nicht\n");
    return 0;
  }
}
