gboolean positionsanpassung(GtkWidget *widget, gpointer data)
{
  int tempzaehler = knotenexistiert(gtk_widget_get_name(widget));
  int knotenzaehler = 0;
  char tempname[10000] = "", knotennummer[22]="";
  sprintf(knotennummer,"-%i",knotenzaehler);
  strcpy(tempname,gtk_widget_get_name(widget));
  strcat(tempname,knotennummer);
  int ytempo = (knotenexistiert(tempname))?y[knotenexistiert(tempname)]:y[tempzaehler];
  int ytempu;
  while(knotenexistiert(tempname) > -1)
  {
    ytempu = (knotenexistiert(tempname))?y[knotenexistiert(tempname)]:y[tempzaehler];
    knotenzaehler += 1;
    memset(tempname,0,10000);
    memset(knotennummer,0,22);
    strcpy(tempname,gtk_widget_get_name(widget));
    sprintf(knotennummer,"-%i",knotenzaehler);
    strcat(tempname,knotennummer);
  }
  int tempstufe=zeichenzaehlen(gtk_widget_get_name(widget),'-')-1;
  y[tempzaehler] = (ytempo + ytempu) / 2;
  printf("Neuer y-Wert: %i\n",y[tempzaehler]);
  gtk_layout_move(GTK_LAYOUT(data),widget,FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*tempstufe,FensterRandOben+RandOben+y[tempzaehler]);
  return TRUE;
//      printf("ErgebnisBreite = %i\nHALLOPOS: DABREITE = %i\n",ErgebnisBreite,RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0));
//      GROESSEDRAWINGAREA
//      GROESSELAYOUTD
}
