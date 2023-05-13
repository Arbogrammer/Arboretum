void alleknotenneupositionieren(gpointer data)
{
  int compare_widgets(const void* a, const void* b)
  {
    GtkWidget* widgetA = *(GtkWidget**)a;
    GtkWidget* widgetB = *(GtkWidget**)b;

    const char* nameA = gtk_widget_get_name(widgetA);
    const char* nameB = gtk_widget_get_name(widgetB);

    return strcmp(nameA, nameB);
  }
  int compare_widgets_wsk(const void* a, const void* b)
  {
    GtkWidget* widgetA = *(GtkWidget**)a;
    GtkWidget* widgetB = *(GtkWidget**)b;

    const char* nameA = gtk_widget_get_name(widgetA);
    const char* nameB = gtk_widget_get_name(widgetB);

    char nameA2[10000] = "";
    char nameB2[10000] = "";
    strcpy(nameA2,nameA);
    strcpy(nameB2,nameB);

    nameA2[strlen(nameA2)-1] = 0;
    nameB2[strlen(nameB2)-1] = 0;

    printf("Name: A: %s vs. %s und B: %s vs. %s\n",gtk_widget_get_name(widgetA),nameA2, gtk_widget_get_name(widgetB),nameB2);

    return strcmp(nameA2, nameB2);
  }
  qsort(textfeldErgebnis, maxzaehlererg+1, sizeof(GtkWidget*), compare_widgets);
  qsort(textfeldErgebnisWahrscheinlichkeit, maxzaehlererg+1, sizeof(GtkWidget*), compare_widgets);
  
  
  for (int i = 0; i <= maxzaehlererg; i++)
  {
    yerg[i] = 0+i*KnotenAbstand+i*KnotenHoehe;
  }
  tempspeichern();
  templaden(data);
  
  qsort(textfeld, maxzaehler+1, sizeof(GtkWidget*), compare_widgets);
  qsort(textfeldWahrscheinlichkeit, maxzaehler+1, sizeof(GtkWidget*), compare_widgets_wsk);

  for (int i = 0; i <= maxzaehler; i++)
  {
    printf("textfeldWahrscheinlichkeit[%i] hat den Namen %s\n",i,gtk_widget_get_name(textfeldWahrscheinlichkeit[i]));
  }
  
  for (int i = 0; i <= maxzaehlererg; i++)
  {
    gchar tempname[10000] = "";
    strcpy(tempname, gtk_widget_get_name(textfeldErgebnis[i]));
    tempname[strlen(tempname)-1] = 0;
    tempname[strlen(tempname)-1] = 0;
    y[knotenexistiert(tempname)] = yerg[i];
  }
  
  tempspeichern();
  templaden(data);

  int stufenanzahl=0;
  for(int i=0 ; i <= maxzaehlererg ; i++)
  {
    int stufenanzahlaktuell=zeichenzaehlen(gtk_widget_get_name(textfeldErgebnis[i]),'-')-1;
    stufenanzahl=(stufenanzahl<stufenanzahlaktuell)?stufenanzahlaktuell:stufenanzahl;    
  }
//  printf("Stufenanzahl: %i\n",stufenanzahl);
  for (int j = stufenanzahl; j > 1; j--)
  {
    for (int i = 0 ; i <= maxzaehler ; i++)
    {
      if(zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-') == j)
      {
//        printf("Knotenname: %s\nVorgängername: %s\nAlter y-Wert: %i\n",gtk_widget_get_name(textfeld[i]),gtk_widget_get_name(*vorgaenger[i]),y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]);
        positionsanpassung(*vorgaenger[i],data);
      }
    }
  }
  
  tempspeichern();
  templaden(data);










  /*
  int stufenanzahl=0;
  int i;
  for(i=0 ; i <= maxzaehlererg ; i++)
  {
    int stufenanzahlaktuell=zeichenzaehlen(gtk_widget_get_name(textfeldErgebnis[i]),'-')-1;
    stufenanzahl=(stufenanzahl<stufenanzahlaktuell)?stufenanzahlaktuell:stufenanzahl;    
  }
  */
  
  /*
  //Suche den obersten hintersten Knoten und weise dem zugehörigen Ergebnisknoten den y-Wert 0 zu:
  
  gchar tempname[10000] = "-0";
  gchar tempname2[10000] = "-0";
  while(knotenexistiert(strcat(tempname2,"-0")) != -1)
  {
    strcat(tempname,"-0");
  }
  printf("%s\n",tempname);
  yerg[knotenexistiert(strcat(tempname,-E))] = 0;
  
  memset(tempname2,0,10000);
  strcpy(tempname2,tempname);
  
  
  //Suche jeweils den nächsten Knoten und weise ihm den nächsten y-Wert zu:
  
  tempname2[strlen(tempname2)-1] = 0;
  int i=0;
  gchar istr[10]="";
  sprintf(istr,"%i",i);
  if(knotenexistiert(strcat(tempname2,istr)) != -1)
  {
    strcat(tempname,istr);
  }
  
  */
}
