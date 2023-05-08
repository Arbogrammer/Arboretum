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

    // Ignoriere das letzte Zeichen der Namen
    size_t lengthA = strlen(nameA);
    size_t lengthB = strlen(nameB);

    if (lengthA > 0)
    {
        lengthA--;
    }

    if (lengthB > 0)
    {
        lengthB--;
    }

    return strncmp(nameA, nameB, lengthA > lengthB ? lengthB : lengthA);
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

  for (int i = 0; i <= maxzaehler; i++)
  {
    printf("%s: y[%i]=%i\n",gtk_widget_get_name(textfeld[i]),i,y[i]);
    if(*vorgaenger[i] != NULL && zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-') > 1)
    {
      positionsanpassung(*vorgaenger[i],data);
    }
  }
  
  tempspeichern();
  templaden(data);










  /*
  int stufenanzahl=1
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
