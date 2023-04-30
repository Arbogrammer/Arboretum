void positionneu(int KnotenAbstandVorher,gpointer data)
{
  int i=0;
  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    yerg[i] = (yerg[i]/(KnotenAbstandVorher+KnotenHoehe))*(KnotenAbstand+KnotenHoehe);
    char tempname[10000] = "";
    strcpy(tempname,gtk_widget_get_name(textfeldErgebnis[i]));
    tempname[strrchr(tempname,'-')-tempname] = 0;
    y[knotenexistiert(tempname)] = (y[knotenexistiert(tempname)]/(KnotenAbstandVorher+KnotenHoehe))*(KnotenAbstand+KnotenHoehe);
      int zaehleri = knotenexistiert(tempname);
      while(zaehleri > 0) // vorgaenger[zaehleri] != &textfeld[zaehleri])
      {
        positionsanpassung(*vorgaenger[zaehleri],data);
        int abbruch = zeichenzaehlen(gtk_widget_get_name(*vorgaenger[zaehleri]),'-');
        zaehleri = knotenexistiert(gtk_widget_get_name(*vorgaenger[zaehleri]));
        int abbruch2 = zeichenzaehlen(gtk_widget_get_name(*vorgaenger[zaehleri]),'-');
        if(abbruch == abbruch2)
        {
          break;
        }
      }

  }
/*
  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    char tempname[10000] = "";
    strcpy(tempname,gtk_widget_get_name(textfeldErgebnis[i]));
    tempname[strrchr(tempname,'-')-tempname] = 0;
    y[knotenexistiert(tempname)] = ((y[knotenexistiert(tempname)]-RandOben)/(KnotenAbstandVorher+KnotenHoehe))*(KnotenAbstand+KnotenHoehe);
  }
*/

}
