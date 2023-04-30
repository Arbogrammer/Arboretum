void laden(gpointer data,char *dateiname)
{
  int i=0;
  zaehler=0;
  Stufe=0;
  maxStufe=0;
  for(i=0 ; i <= maxzaehler ; i++)
  {
    gtk_widget_destroy(textfeld[i]);
    textfeld[i] = NULL;
    vorgaenger[i] = NULL;
    gtk_widget_destroy(textfeldWahrscheinlichkeit[i]);
    textfeldWahrscheinlichkeit[i] = NULL;
  }
  zaehlererg = 0;
  for(i=0 ; i <= maxzaehlererg ; i++)
  {
    gtk_widget_destroy(textfeldErgebnis[i]);
    textfeldErgebnis[i] = NULL;
    gtk_widget_destroy(textfeldErgebnisWahrscheinlichkeit[i]);
    textfeldErgebnisWahrscheinlichkeit[i] = NULL;
  }
  maxzaehler=0;
  maxzaehlererg=0;

  gtk_widget_set_size_request (da, RandLinks+RandRechts+StufenBreite+KnotenBreite,RandOben+KnotenHoehe+RandUnten);
  gtk_layout_set_size(GTK_LAYOUT (data),RandLinks+RandRechts+StufenBreite+KnotenBreite,RandOben+KnotenHoehe+RandUnten);

  FILE *datei;
  datei = fopen(dateiname,"r");
  fseek(datei,0L,SEEK_END);
  int dateigroesse = ftell(datei);
  rewind(datei);
  char *dateiinhalt=calloc(dateigroesse+2,sizeof(char));
  int temp=0, j = 0;
  while((temp = fgetc(datei))!=EOF)
  {
    dateiinhalt[j]=temp;
    j++;
  }
  fclose(datei);
  dateiinhalt[j]=0;


  j=0;
  while(dateiinhalt[j] != 30)
  {
    int l=0;
    char randlinksstring[22] = "", randrechtsstring[22] = "", randobenstring[22] = "", randuntenstring[22] = "", fensterrandlinksstring[22] = "", fensterrandrechtsstring[22] = "", fensterrandobenstring[22] = "", fensterranduntenstring[22] = "", stufenbreitestring[22] = "", knotenabstandstring[22] = "", knotenhoehestring[22] = "", knotenbreitestring[22] = "", ergebnisabstandstring[22] = "", ergebnisbreitestring[22] = "", ergebnistextbreitestring[22] = "", liniendickestring[22] = "", knotentextbreitestring[22] = "", wahrscheinlichkeittextbreitestring[22] = "", hintergrundfarberedstring[100] = "", hintergrundfarbegreenstring[100] = "", hintergrundfarbebluestring[100] = "", hintergrundfarbealphastring[100] = "", zweigfarberedstring[100] = "", zweigfarbegreenstring[100] = "", zweigfarbebluestring[100] = "", zweigfarbealphastring[100] = "", knotenhintergrundfarberedstring[100] = "", knotenhintergrundfarbegreenstring[100] = "", knotenhintergrundfarbebluestring[100] = "", knotenhintergrundfarbealphastring[100] = "", knotenrandfarberedstring[100] = "", knotenrandfarbegreenstring[100] = "", knotenrandfarbebluestring[100] = "", knotenrandfarbealphastring[100] = "", schriftfarberedstring[100] = "", schriftfarbegreenstring[100] = "", schriftfarbebluestring[100] = "", schriftfarbealphastring[100] = "", ergebnisseanzeigenstring[22] = "", wskanzeigenstring[22] = "", ergebnissewskanzeigenstring[22] = "", labeleinstring[22] = "", KnotenLabelBreitestring[22] = "", klbmaxstring[22] = "", KnotenLabelHoehestring[22] = "", ErgebnisLabelBreitestring[22] = "", WahrscheinlichkeitErgebnisTextBreitestring[22] = "", WahrscheinlichkeitErgebnisLabelBreitestring[22] = "", ZaehlerErgebnisLabelBreitestring[22] = "", NennerErgebnisLabelBreitestring[22] = "", ZaehlerErgebnisLabelHoehestring[22] = "", NennerErgebnisLabelHoehestring[22] = "", schriftartstring[1000] = "", paddingstring[100] = "", paddingkstring[100] = "", genauigkeitstring[22] = "", kuerzenstring[22] = "", bruchoustring[22] = "", knotenrahmenabstandstring[22] = "", wskverschiebungstring[22] = "", knotenrahmendickestring[22] = "";
    while(dateiinhalt[j] != 10)
    {
      int k=0;
      while(dateiinhalt[j] != 31)
      {
        if(l==0)
        {
          randlinksstring[k] = dateiinhalt[j];
        }
        if(l==1)
        {
          randrechtsstring[k] = dateiinhalt[j];
        }
        if(l==2)
        {
          randobenstring[k] = dateiinhalt[j];
        }
        if(l==3)
        {
          randuntenstring[k] = dateiinhalt[j];
        }
        if(l==4)
        {
          fensterrandlinksstring[k] = dateiinhalt[j];
        }
        if(l==5)
        {
          fensterrandrechtsstring[k] = dateiinhalt[j];
        }
        if(l==6)
        {
          fensterrandobenstring[k] = dateiinhalt[j];
        }
        if(l==7)
        {
          fensterranduntenstring[k] = dateiinhalt[j];
        }
        if(l==8)
        {
          stufenbreitestring[k] = dateiinhalt[j];
        }
        if(l==9)
        {
          knotenabstandstring[k] = dateiinhalt[j];
        }
        if(l==10)
        {
          knotenhoehestring[k] = dateiinhalt[j];
        }
        if(l==11)
        {
          knotenbreitestring[k] = dateiinhalt[j];
        }
        if(l==12)
        {
          ergebnisabstandstring[k] = dateiinhalt[j];
        }
        if(l==13)
        {
          ergebnisbreitestring[k] = dateiinhalt[j];
        }
        if(l==14)
        {
          ergebnistextbreitestring[k] = dateiinhalt[j];
        }
        if(l==15)
        {
          ErgebnisTrenner = dateiinhalt[j];
        }
        if(l==16)
        {
          liniendickestring[k] = dateiinhalt[j];
        }
        if(l==17)
        {
          knotentextbreitestring[k] = dateiinhalt[j];
        }
        if(l==18)
        {
          wahrscheinlichkeittextbreitestring[k] = dateiinhalt[j];
        }
        if(l==19)
        {
          hintergrundfarberedstring[k] = dateiinhalt[j];
        }
        if(l==20)
        {
          hintergrundfarbegreenstring[k] = dateiinhalt[j];
        }
        if(l==21)
        {
          hintergrundfarbebluestring[k] = dateiinhalt[j];
        }
        if(l==22)
        {
          hintergrundfarbealphastring[k] = dateiinhalt[j];
        }
        if(l==23)
        {
          zweigfarberedstring[k] = dateiinhalt[j];
        }
        if(l==24)
        {
          zweigfarbegreenstring[k] = dateiinhalt[j];
        }
        if(l==25)
        {
          zweigfarbebluestring[k] = dateiinhalt[j];
        }
        if(l==26)
        {
          zweigfarbealphastring[k] = dateiinhalt[j];
        }
        if(l==27)
        {
          knotenhintergrundfarberedstring[k] = dateiinhalt[j];
        }
        if(l==28)
        {
          knotenhintergrundfarbegreenstring[k] = dateiinhalt[j];
        }
        if(l==29)
        {
          knotenhintergrundfarbebluestring[k] = dateiinhalt[j];
        }
        if(l==30)
        {
          knotenhintergrundfarbealphastring[k] = dateiinhalt[j];
        }
        if(l==31)
        {
          knotenrandfarberedstring[k] = dateiinhalt[j];
        }
        if(l==32)
        {
          knotenrandfarbegreenstring[k] = dateiinhalt[j];
        }
        if(l==33)
        {
          knotenrandfarbebluestring[k] = dateiinhalt[j];
        }
        if(l==34)
        {
          knotenrandfarbealphastring[k] = dateiinhalt[j];
        }
        if(l==35)
        {
          schriftfarberedstring[k] = dateiinhalt[j];
        }
        if(l==36)
        {
          schriftfarbegreenstring[k] = dateiinhalt[j];
        }
        if(l==37)
        {
          schriftfarbebluestring[k] = dateiinhalt[j];
        }
        if(l==38)
        {
          schriftfarbealphastring[k] = dateiinhalt[j];
        }
        if(l==39)
        {
          ergebnisseanzeigenstring[k] = dateiinhalt[j];
        }
        if(l==40)
        {
          wskanzeigenstring[k] = dateiinhalt[j];
        }
        if(l==41)
        {
          ergebnissewskanzeigenstring[k] = dateiinhalt[j];
        }
        if(l==42)
        {
          labeleinstring[k] = dateiinhalt[j];
        }
        if(l==43)
        {
          KnotenLabelBreitestring[k] = dateiinhalt[j];
        }
        if(l==44)
        {
          klbmaxstring[k] = dateiinhalt[j];
        }
        if(l==45)
        {
          KnotenLabelHoehestring[k] = dateiinhalt[j];
        }
        if(l==46)
        {
          ErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if(l==47)
        {
          WahrscheinlichkeitErgebnisTextBreitestring[k] = dateiinhalt[j];
        }
        if(l==48)
        {
          WahrscheinlichkeitErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if(l==49)
        {
          ZaehlerErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if(l==50)
        {
          NennerErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if(l==51)
        {
          ZaehlerErgebnisLabelHoehestring[k] = dateiinhalt[j];
        }
        if(l==52)
        {
          NennerErgebnisLabelHoehestring[k] = dateiinhalt[j];
        }
        if(l==53)
        {
          schriftartstring[k] = dateiinhalt[j];
        }
        if(l==54)
        {
          paddingstring[k] = dateiinhalt[j];
        }
        if(l==55)
        {
          paddingkstring[k] = dateiinhalt[j];
        }
        if(l==56)
        {
          genauigkeitstring[k] = dateiinhalt[j];
        }
        if(l==57)
        {
          kuerzenstring[k] = dateiinhalt[j];
        }
        if(l==58)
        {
          bruchoustring[k] = dateiinhalt[j];
        }
        if(l==59)
        {
          knotenrahmenabstandstring[k] = dateiinhalt[j];
        }
        if(l==60)
        {
          wskverschiebungstring[k] = dateiinhalt[j];
        }
        if(l==61)
        {
          knotenrahmendickestring[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    RandLinks=atoi(randlinksstring);
    RandRechts=atoi(randrechtsstring);
    RandOben=atoi(randobenstring);
    RandUnten=atoi(randuntenstring);
    FensterRandLinks=atoi(fensterrandlinksstring);
    FensterRandRechts=atoi(fensterrandrechtsstring);
    FensterRandOben=atoi(fensterrandobenstring);
    FensterRandUnten=atoi(fensterranduntenstring);
    StufenBreite=atoi(stufenbreitestring);
    KnotenAbstand=atoi(knotenabstandstring);
    KnotenHoehe=atoi(knotenhoehestring);
    KnotenBreite=atoi(knotenbreitestring);
    ErgebnisAbstand=atoi(ergebnisabstandstring);
    ErgebnisBreite=atoi(ergebnisbreitestring);
    ErgebnisTextBreite=atoi(ergebnistextbreitestring);
    LinienDicke=atoi(liniendickestring);
    KnotenTextBreite=atoi(knotentextbreitestring);
    WahrscheinlichkeitTextBreite=atoi(wahrscheinlichkeittextbreitestring);
    hintergrundfarbe.red = atof(hintergrundfarberedstring);
    hintergrundfarbe.green = atof(hintergrundfarbegreenstring);
    hintergrundfarbe.blue = atof(hintergrundfarbebluestring);
    hintergrundfarbe.alpha = atof(hintergrundfarbealphastring);
    zweigfarbe.red = atof(zweigfarberedstring);
    zweigfarbe.green = atof(zweigfarbegreenstring);
    zweigfarbe.blue = atof(zweigfarbebluestring);
    zweigfarbe.alpha = atof(zweigfarbealphastring);
    knotenhintergrundfarbe.red = atof(knotenhintergrundfarberedstring);
    knotenhintergrundfarbe.green = atof(knotenhintergrundfarbegreenstring);
    knotenhintergrundfarbe.blue = atof(knotenhintergrundfarbebluestring);
    knotenhintergrundfarbe.alpha = atof(knotenhintergrundfarbealphastring);
    knotenrandfarbe.red = atof(knotenrandfarberedstring);
    knotenrandfarbe.green = atof(knotenrandfarbegreenstring);
    knotenrandfarbe.blue = atof(knotenrandfarbebluestring);
    knotenrandfarbe.alpha = atof(knotenrandfarbealphastring);
    schriftfarbe.red = atof(schriftfarberedstring);
    schriftfarbe.green = atof(schriftfarbegreenstring);
    schriftfarbe.blue = atof(schriftfarbebluestring);
    schriftfarbe.alpha = atof(schriftfarbealphastring);
    ergebnisseanzeigen = atoi(ergebnisseanzeigenstring);
    wskanzeigen = atoi(wskanzeigenstring);
    ergebnissewskanzeigen = atoi(ergebnissewskanzeigenstring);
    labelein = atoi(labeleinstring);
    KnotenLabelBreite = atoi(KnotenLabelBreitestring);
    klbmax = atoi(klbmaxstring);
    KnotenLabelHoehe = atoi(KnotenLabelHoehestring);
    ErgebnisLabelBreite = atoi(ErgebnisLabelBreitestring);
    WahrscheinlichkeitErgebnisTextBreite = atoi(WahrscheinlichkeitErgebnisTextBreitestring);
    WahrscheinlichkeitErgebnisLabelBreite = atoi(WahrscheinlichkeitErgebnisLabelBreitestring);
    ZaehlerErgebnisLabelBreite = atoi(ZaehlerErgebnisLabelBreitestring);
    NennerErgebnisLabelBreite = atoi(NennerErgebnisLabelBreitestring);
    ZaehlerErgebnisLabelHoehe = atoi(ZaehlerErgebnisLabelHoehestring);
    NennerErgebnisLabelHoehe = atoi(NennerErgebnisLabelHoehestring);
    strcpy(schriftart,schriftartstring);
    padding = atof(paddingstring);
    paddingk = atof(paddingkstring);
    genauigkeit = atoi(genauigkeitstring);
    kuerzen = atoi(kuerzenstring);
    bruchou = atoi(bruchoustring);
    knotenrahmenabstand = atoi(knotenrahmenabstandstring);
    wskverschiebung = atoi(wskverschiebungstring);
    knotenrahmendicke = atoi(knotenrahmendickestring);
    j++;
  }


  ymax = 0;
  j += 2;
  while(dateiinhalt[j] != 30)
  {
    int l=0, tempzaehler=0;
    char tempzaehlerstring[22] = "", ystring[22] = "", name[10000] = "", text[10000] = "";
    while(dateiinhalt[j] != 10)
    {
      int k=0;
      while(dateiinhalt[j] != 31)
      {
        if(l==0)
        {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if(l==1)
        {
          ystring[k] = dateiinhalt[j];
        }
        if(l==2)
        {
          name[k] = dateiinhalt[j];
        }
        if(l==3)
        {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    Stufe=zeichenzaehlen(name,'-')-1;
    maxStufe=((Stufe>maxStufe)?Stufe:maxStufe);
    tempzaehler = atoi(tempzaehlerstring);
    y[tempzaehler] = atoi(ystring);
    ymax=((ymax<y[tempzaehler])?y[tempzaehler]:ymax);
    textfeld[tempzaehler] = gtk_entry_new();
    if(tempzaehler == 0 || zeichenzaehlen(name,'-') == 1)
    {
      vorgaenger[tempzaehler] = &textfeld[0];
    }
    else
    {
      char tempname[10000] = "";
      strncpy(tempname,name,strrchr(name,'-')-name);
      vorgaenger[tempzaehler] = &textfeld[knotenexistiert(tempname)];
    }
    gtk_widget_set_name(textfeld[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeld[tempzaehler]),KnotenTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeld[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeld[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeld[tempzaehler], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufe,FensterRandOben+RandOben+y[tempzaehler]);
    gtk_widget_show_all(textfeld[tempzaehler]);
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
    g_signal_connect (textfeld[tempzaehler], "changed", G_CALLBACK (buchstabeneingabe), data);
    zaehler=tempzaehler;
    maxzaehler=tempzaehler;
    j++;
  }


  j += 2;
  while(dateiinhalt[j] != 30)
  {
    int l=0, tempzaehler=0;
    char tempzaehlerstring[22] = "", ystring[22] = "", name[10000] = "", text[10000] = "";
    while(dateiinhalt[j] != 10)
    {
      int k=0;
      while(dateiinhalt[j] != 31)
      {
        if(l==0)
        {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if(l==1)
        {
          ystring[k] = dateiinhalt[j];
        }
        if(l==2)
        {
          name[k] = dateiinhalt[j];
        }
        if(l==3)
        {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
//    printf("tempzaelerstring: %s\nystring: %s\nname: %s\n",tempzaehlerstring,ystring,name);
    tempzaehler = atoi(tempzaehlerstring);
    yerg[tempzaehler] = atoi(ystring);
    textfeldErgebnis[tempzaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldErgebnis[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[tempzaehler]),3*(maxStufe+1));
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnis[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldErgebnis[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnis[tempzaehler], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[tempzaehler]);
    gtk_widget_show_all(textfeldErgebnis[tempzaehler]);
    zaehlererg=tempzaehler;
    maxzaehlererg=tempzaehler;
    j++;
  }



  j += 2;
  while(dateiinhalt[j] != 30)
  {
    int l=0, tempzaehler=0;
    char tempzaehlerstring[22] = "", name[10000] = "", text[10000] = "";
    while(dateiinhalt[j] != 10)
    {
      int k=0;
      while(dateiinhalt[j] != 31)
      {
        if(l==0)
        {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if(l==1)
        {
          name[k] = dateiinhalt[j];
        }
        if(l==2)
        {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    tempzaehler = atoi(tempzaehlerstring);
    textfeldWahrscheinlichkeit[tempzaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldWahrscheinlichkeit[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),WahrscheinlichkeitTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[tempzaehler],0,0);
    gtk_widget_show_all(textfeldWahrscheinlichkeit[tempzaehler]);
    g_signal_connect (textfeldWahrscheinlichkeit[tempzaehler], "changed", G_CALLBACK (wskeingabe), data);
    j++;
  }


  j += 2;
  while(dateiinhalt[j] != 0)
  {
    int l=0, tempzaehler=0;
    char tempzaehlerstring[22] = "", name[10000] = "", text[10000] = "";
    while(dateiinhalt[j] != 10)
    {
      int k=0;
      while(dateiinhalt[j] != 31)
      {
        if(l==0)
        {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if(l==1)
        {
          name[k] = dateiinhalt[j];
        }
        if(l==2)
        {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    tempzaehler = atoi(tempzaehlerstring);
    textfeldErgebnisWahrscheinlichkeit[tempzaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),WahrscheinlichkeitErgebnisTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnisWahrscheinlichkeit[tempzaehler], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[tempzaehler]);
    gtk_widget_show_all(textfeldErgebnisWahrscheinlichkeit[tempzaehler]);
    j++;
  }

  bruch=0;
  for(i=0;i<=maxzaehler;i++)
  {
    if(strchr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),'/'))
    {
      bruch=1;
    }
  }


  positionsanpassungwsk(data);
  g_signal_connect(textfeldErgebnis[0], "size-allocate", G_CALLBACK(wskergebnisverschieben), data);

  if(labelein)
  {
    labelein=0;
    umwandeln(NULL,data);
  }


  gtk_widget_queue_draw (da);
  free(dateiinhalt);
  strcpy(aktuelledatei,dateiname);
}
