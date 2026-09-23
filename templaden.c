static double tempzahl_laden(const char *text)
{
  char *kopie = g_strdup(text);
  char *komma = strchr(kopie, ',');
  if(komma) *komma = '.';
  double wert = g_ascii_strtod(kopie, NULL);
  g_free(kopie);
  return wert;
}

static void tempdarstellung_laden(char *zeile)
{
  gchar **werte = g_strsplit(zeile, "\x1f", 45);
  int n = g_strv_length(werte);
  if(n >= 45)
  {
    int v=0;
    RandLinks=atoi(werte[v++]); RandRechts=atoi(werte[v++]);
    RandOben=atoi(werte[v++]); RandUnten=atoi(werte[v++]);
    StufenBreite=atoi(werte[v++]); KnotenAbstand=atoi(werte[v++]);
    ErgebnisAbstand=atoi(werte[v++]); LinienDicke=atoi(werte[v++]);
    KnotenTextBreite=atoi(werte[v++]); WahrscheinlichkeitTextBreite=atoi(werte[v++]);
    ErgebnisTrenner=(char)atoi(werte[v++]); genauigkeit=atoi(werte[v++]);
    padding=tempzahl_laden(werte[v++]); paddingk=tempzahl_laden(werte[v++]);
    bruchou=atoi(werte[v++]); kuerzen=atoi(werte[v++]);
    knotenrahmenabstand=atoi(werte[v++]); wskverschiebung=atoi(werte[v++]);
    knotenrahmendicke=atoi(werte[v++]);
    hintergrundfarbe.red=tempzahl_laden(werte[v++]); hintergrundfarbe.green=tempzahl_laden(werte[v++]);
    hintergrundfarbe.blue=tempzahl_laden(werte[v++]); hintergrundfarbe.alpha=tempzahl_laden(werte[v++]);
    zweigfarbe.red=tempzahl_laden(werte[v++]); zweigfarbe.green=tempzahl_laden(werte[v++]);
    zweigfarbe.blue=tempzahl_laden(werte[v++]); zweigfarbe.alpha=tempzahl_laden(werte[v++]);
    knotenhintergrundfarbe.red=tempzahl_laden(werte[v++]); knotenhintergrundfarbe.green=tempzahl_laden(werte[v++]);
    knotenhintergrundfarbe.blue=tempzahl_laden(werte[v++]); knotenhintergrundfarbe.alpha=tempzahl_laden(werte[v++]);
    knotenrandfarbe.red=tempzahl_laden(werte[v++]); knotenrandfarbe.green=tempzahl_laden(werte[v++]);
    knotenrandfarbe.blue=tempzahl_laden(werte[v++]); knotenrandfarbe.alpha=tempzahl_laden(werte[v++]);
    schriftfarbe.red=tempzahl_laden(werte[v++]); schriftfarbe.green=tempzahl_laden(werte[v++]);
    schriftfarbe.blue=tempzahl_laden(werte[v++]); schriftfarbe.alpha=tempzahl_laden(werte[v++]);
    knotenhintergrundfarbewurdegeaendert=atoi(werte[v++]);
    knotenrahmenfarbewurdegeaendert=atoi(werte[v++]);
    knotenrahmendickegeaendert=atoi(werte[v++]);
    schriftfarbewurdegeaendert=atoi(werte[v++]);
    schriftartwurdegewechselt=atoi(werte[v++]);
    g_strlcpy(schriftart,g_strchomp(werte[v]),sizeof(schriftart));
  }
  g_strfreev(werte);
}

void templaden(gpointer data)
{

  if(labelein==1)
  {
    labelein=2;
  }
  if(dateinummerierung <= 1)
  {
    return;
  }

  /* Erst vollständig einlesen, bevor der sichtbare Baum verändert wird. */
  int naechste_dateinummer = dateinummerierung - 1;
  g_autofree gchar *dateiname = arboretum_temp_path(naechste_dateinummer);
  FILE *datei = g_fopen(dateiname,"rb");
  if(!datei)
  {
    g_warning("Undo-Zustand konnte nicht geöffnet werden: %s", dateiname);
    zurueck = 0;
    return;
  }
  if(fseek(datei,0L,SEEK_END) != 0)
  {
    g_warning("Undo-Zustand konnte nicht gelesen werden: %s", dateiname);
    fclose(datei);
    zurueck = 0;
    return;
  }
  long dateigroesse_lang = ftell(datei);
  if(dateigroesse_lang <= 0 || fseek(datei,0L,SEEK_SET) != 0)
  {
    g_warning("Undo-Zustand ist leer oder unlesbar: %s", dateiname);
    fclose(datei);
    zurueck = 0;
    return;
  }
  size_t dateigroesse = (size_t)dateigroesse_lang;
  char *dateiinhalt=calloc(dateigroesse+2,sizeof(char));
  if(!dateiinhalt)
  {
    g_warning("Nicht genügend Speicher zum Laden des Undo-Zustands");
    fclose(datei);
    zurueck = 0;
    return;
  }
  size_t gelesen = fread(dateiinhalt,1,dateigroesse,datei);
  int schliessfehler = fclose(datei);
  if(gelesen != dateigroesse || schliessfehler != 0)
  {
    g_warning("Undo-Zustand konnte nicht vollständig gelesen werden: %s", dateiname);
    free(dateiinhalt);
    zurueck = 0;
    return;
  }
  dateiinhalt[gelesen]=0;

  int abschnitte = 0;
  for(size_t p=0; p<gelesen; p++)
    if((unsigned char)dateiinhalt[p] == 30) abschnitte++;
  if(abschnitte < 4)
  {
    g_warning("Undo-Zustand ist beschädigt: %s", dateiname);
    free(dateiinhalt);
    zurueck = 0;
    return;
  }

  /* Geometrie muss bereits vor dem Neuaufbau der Widgets feststehen. */
  int gefundene_abschnitte = 0;
  for(size_t p=0; p<gelesen; p++)
  {
    if((unsigned char)dateiinhalt[p] == 30 && ++gefundene_abschnitte == 4)
    {
      if(p + 2 < gelesen)
        tempdarstellung_laden(dateiinhalt+p+2);
      break;
    }
  }

  int focuszaehler = -1;
  int focuspos = -1;
  int focuszaehlerw = -1;
  int focusposw = -1;
  if(zurueck == 0)
  {
    if(gtk_window_get_focus(GTK_WINDOW(window)))
    {
      focuszaehler = knotenexistiert(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))));
      focuspos = ((focuszaehler<0)?-1:gtk_editable_get_position (GTK_EDITABLE(textfeld[focuszaehler]))+1);
      focuszaehlerw = wskexistiert(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))));
      focusposw = ((focuszaehlerw<0)?-1:gtk_editable_get_position (GTK_EDITABLE(textfeldWahrscheinlichkeit[focuszaehlerw]))+1);
    }
  }
  zurueck = 0;

  int i=0;
  zaehler=0;
  Stufe=0;
  maxStufe=0;
  g_signal_handlers_disconnect_by_data(textfeldErgebnis[0], data);
  for(i=0 ; i <= maxzaehler ; i++)
  {
    gtk_widget_destroy(textfeld[i]);
    textfeld[i] = NULL;
    vorgaenger[i] = NULL;
    gtk_widget_destroy(textfeldWahrscheinlichkeit[i]);
    textfeldWahrscheinlichkeit[i] = NULL;
    if(knotenlabel[i])
    {
      gtk_widget_destroy(knotenlabel[i]);
      knotenlabel[i] = NULL;
    }
    if(wahrscheinlichkeitlabel[i])
    {
      gtk_widget_destroy(wahrscheinlichkeitlabel[i]);
      wahrscheinlichkeitlabel[i] = NULL;
    }
    if(zaehlerlabel[i])
    {
      gtk_widget_destroy(zaehlerlabel[i]);
      zaehlerlabel[i] = NULL;
    }
    if(nennerlabel[i])
    {
      gtk_widget_destroy(nennerlabel[i]);
      nennerlabel[i] = NULL;
    }
  }
  zaehlererg = 0;
  for(i=0 ; i <= maxzaehlererg ; i++)
  {
    gtk_widget_destroy(textfeldErgebnis[i]);
    textfeldErgebnis[i] = NULL;
    gtk_widget_destroy(textfeldErgebnisWahrscheinlichkeit[i]);
    textfeldErgebnisWahrscheinlichkeit[i] = NULL;
    if(ergebnislabel[i])
    {
      gtk_widget_destroy(ergebnislabel[i]);
      ergebnislabel[i] = NULL;
    }
    if(ergebniswsklabel[i])
    {
      gtk_widget_destroy(ergebniswsklabel[i]);
      ergebniswsklabel[i] = NULL;
    }
    if(ergebniszaehlerlabel[i])
    {
      gtk_widget_destroy(ergebniszaehlerlabel[i]);
      ergebniszaehlerlabel[i] = NULL;
    }
    if(ergebnisnennerlabel[i])
    {
      gtk_widget_destroy(ergebnisnennerlabel[i]);
      ergebnisnennerlabel[i] = NULL;
    }
  }
  maxzaehler=0;
  maxzaehlererg=0;
  dateinummerierung = naechste_dateinummer;

  gtk_widget_set_size_request (da, RandLinks+RandRechts+StufenBreite+KnotenBreite,RandOben+KnotenHoehe+RandUnten);
  gtk_layout_set_size(GTK_LAYOUT (data),RandLinks+RandRechts+StufenBreite+KnotenBreite,RandOben+KnotenHoehe+RandUnten);

  int j = 0;
//  printf("dateiinhalt: %s\n",dateiinhalt);
  ymax = 0;
  j=0;
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
    Stufe=zeichenzaehlen(name,'-')-1;
    maxStufe=((Stufe>maxStufe)?Stufe:maxStufe);
    tempzaehler = atoi(tempzaehlerstring);
    if(!knotenindex_gueltig(tempzaehler))
    {
      free(dateiinhalt);
      return;
    }
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
//      printf("tempname: %s\n",tempname);
      vorgaenger[tempzaehler] = &textfeld[knotenexistiert(tempname)];
    }
    gtk_widget_set_name(textfeld[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeld[tempzaehler]),KnotenTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeld[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeld[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeld[tempzaehler], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufe,FensterRandOben+RandOben+y[tempzaehler]);
//    gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten);
//    gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten+FensterRandOben+FensterRandUnten);
    g_signal_connect (textfeld[tempzaehler], "changed", G_CALLBACK (buchstabeneingabe), data);
    gtk_widget_show_all(textfeld[tempzaehler]);
    zaehler=tempzaehler;
    maxzaehler=tempzaehler;
    j++;
//    gtk_widget_queue_draw (da);
  }
  if(textfeld[maxzaehler])
  {
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeld[maxzaehler]));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[maxzaehler]),-1);
  }
  else
  {
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeld[0]));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[0]),-1);
  }
  /* Beim Löschen existiert der zuvor fokussierte Index unter Umständen
   * nicht mehr. Dann den nächsten, beziehungsweise den letzten verbliebenen
   * Knoten fokussieren. */
  if(focuszaehler > maxzaehler)
    focuszaehler = maxzaehler;
  if(focuszaehler > -1 && textfeld[focuszaehler])
  {
//    printf("Focus: %i\n",focuszaehler);
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeld[focuszaehler]));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[focuszaehler]),focuspos);
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
    if(!knotenindex_gueltig(tempzaehler))
    {
      free(dateiinhalt);
      return;
    }
    yerg[tempzaehler] = atoi(ystring);
    textfeldErgebnis[tempzaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldErgebnis[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[tempzaehler]),3*(maxStufe+1));
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnis[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldErgebnis[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnis[tempzaehler], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[tempzaehler]);


//    gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten);
//    gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten+FensterRandOben+FensterRandUnten);
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
    if(!knotenindex_gueltig(tempzaehler))
    {
      free(dateiinhalt);
      return;
    }
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
  if(focuszaehlerw > maxzaehler)
    focuszaehlerw = maxzaehler;
  if(focuszaehlerw > -1 && textfeldWahrscheinlichkeit[focuszaehlerw])
  {
//    printf("Focusw: %i\n",focuszaehlerw);
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeldWahrscheinlichkeit[focuszaehlerw]));
    gtk_editable_set_position(GTK_EDITABLE(textfeldWahrscheinlichkeit[focuszaehlerw]),focusposw);
  }

  j += 2;
  while(dateiinhalt[j] != 30 && dateiinhalt[j] != 0)
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
    if(!knotenindex_gueltig(tempzaehler))
    {
      free(dateiinhalt);
      return;
    }
    textfeldErgebnisWahrscheinlichkeit[tempzaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[tempzaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),WahrscheinlichkeitErgebnisTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),text);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnisWahrscheinlichkeit[tempzaehler], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[tempzaehler]);


//    gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten);
//    gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite,ymax+KnotenHoehe+RandUnten+FensterRandOben+FensterRandUnten);
    gtk_widget_show_all(textfeldErgebnisWahrscheinlichkeit[tempzaehler]);
    zaehlererg=tempzaehler;
    maxzaehlererg=tempzaehler;
    j++;
  }

  /* Neue Snapshots besitzen nach den vier Baumabschnitten die Darstellung. */
  arboretum_refresh_entry_overlines();
  positionsanpassungwsk(data);

  knotenhintergrundfarbeaendern();
  knotenrandfarbeaendern();
  knotenranddickeaendern();
  schriftfarbeaendern();
  schriftartanpassen(NULL,NULL,NULL);

  if(labelein == 2)
  {
    labelein = 0;
    umwandeln(NULL,data);
  }

  gtk_widget_queue_draw (da);
  free(dateiinhalt);
}
