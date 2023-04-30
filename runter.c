gboolean runter(GtkWidget *widget, gpointer data)
{
  if(labelein==1)
  {
    return FALSE;
  }
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  if(aktuelleswidget == NULL)
  {
    aktuelleswidget = textfeld[0];
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[0])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[0]),-1);
    return FALSE;
  }
  if(strrchr(gtk_widget_get_name(aktuelleswidget),'W'))
  {
    runterwsk(widget, data);
    return FALSE;
  }

  // Einschub: Folgende drei Zeilen sind nur notwendig, wenn man mit der Maus die Felder auswählen will!
  zaehler = knotenexistiert(gtk_widget_get_name(aktuelleswidget));
  if(zaehler==-1)
  {
    return FALSE;
  }
  Stufe = zeichenzaehlen(gtk_widget_get_name(aktuelleswidget),'-')-1;
  Knoten[Stufe] = atoi(gtk_widget_get_name(aktuelleswidget)+(int)(strrchr(gtk_widget_get_name(aktuelleswidget),'-')-&(gtk_widget_get_name(aktuelleswidget)[0])+1));
  // Einschub Ende


  char tempname[10000];
  snprintf(tempname,(int)(strrchr(gtk_widget_get_name(aktuelleswidget),'-')-&(gtk_widget_get_name(aktuelleswidget)[0])+1),"%s",gtk_widget_get_name(aktuelleswidget));
  char temp[30];
  sprintf(temp,"-%i",Knoten[Stufe]+1);
  strcat(tempname,temp);

  zaehler += 1;
  Knoten[Stufe] += 1;
  int i;
  int weiter=1;
  for(i=0 ; i<=maxzaehler ; i++)
  {
    if(strcmp(tempname,gtk_widget_get_name(textfeld[i])) == 0)
    {
      weiter=0;
      zaehler=i;
      gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
      gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]),-1);
      break;
    }
  }
  if(weiter)
  {
    dateiveraendert++;
    tempspeichern();
    int tempzaehler = zaehler-1;
    zaehler = maxzaehler + 1;
    maxzaehler += 1;
    zaehlererg = maxzaehlererg + 1;
    maxzaehlererg += 1;
    vorgaenger[zaehler] = vorgaenger[tempzaehler];
    textfeld[zaehler] = gtk_entry_new();
      gchar name[10000] = "";
      snprintf(name,(int)(strrchr(gtk_widget_get_name(aktuelleswidget),'-')-&(gtk_widget_get_name(aktuelleswidget)[0])+1),"%s",gtk_widget_get_name(aktuelleswidget));
      gchar temp[30];
      sprintf(temp,"-%i",Knoten[Stufe]);
      strcat(name,temp);
      gtk_widget_set_name(textfeld[zaehler],name);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeld[zaehler]),KnotenTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeld[zaehler]),0.5);
    g_signal_connect (textfeld[zaehler], "changed", G_CALLBACK (buchstabeneingabe), data);

    textfeldErgebnis[zaehlererg] = gtk_entry_new();
    textfeldErgebnisWahrscheinlichkeit[zaehlererg] = gtk_entry_new();
      gchar nameerg[10002] = "";
      gchar nameergw[10004] = "";
      sprintf(nameerg,"%s-E",name);
      sprintf(nameergw,"%sW",nameerg);
      gtk_widget_set_name(textfeldErgebnis[zaehlererg],nameerg);
      gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[zaehlererg],nameergw);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[zaehlererg]),3*(maxStufe+1));
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[zaehlererg]),WahrscheinlichkeitErgebnisTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnis[zaehlererg]),0.5);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[zaehlererg]),0.5);

    textfeldWahrscheinlichkeit[zaehler] = gtk_entry_new();
      gchar namew[10002] = "";
      sprintf(namew,"%sW",name);
      gtk_widget_set_name(textfeldWahrscheinlichkeit[zaehler],namew);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldWahrscheinlichkeit[zaehler]),WahrscheinlichkeitTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldWahrscheinlichkeit[zaehler]),0.5);
    g_signal_connect (textfeldWahrscheinlichkeit[zaehler], "changed", G_CALLBACK (wskeingabe), data);

    
    y[zaehler] = y[zaehleruntenrechts(tempzaehler)] + KnotenAbstand + KnotenHoehe;
    ymax = (ymax < y[zaehler])?y[zaehler]:ymax;
    yerg[zaehlererg] = y[zaehler];
    int ywsktemp=(y[knotenexistiert(gtk_widget_get_name(*vorgaenger[zaehler]))]+y[zaehler])/2;
    gtk_layout_put (GTK_LAYOUT (data), textfeld[zaehler], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufe,FensterRandOben+RandOben+y[zaehler]);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnis[zaehlererg], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[zaehlererg]);
    gtk_layout_put (GTK_LAYOUT (data), textfeldErgebnisWahrscheinlichkeit[zaehlererg], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[zaehlererg]);
    
    gtk_layout_put (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[zaehler], ((FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufe-0))+(FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufe-1)+KnotenBreite))/2-WahrscheinlichkeitBreite/2,(Stufe==0)?(((zaehler*KnotenHoehe+(zaehler-1)*KnotenAbstand)/2+RandOben+FensterRandOben)+y[zaehler])/2:ywsktemp+RandOben+FensterRandOben);


    
    gtk_widget_show_all(textfeld[zaehler]);
    if(ergebnisseanzeigen)
    {
      gtk_widget_show_all(textfeldErgebnis[zaehlererg]);
    }
    if(ergebnissewskanzeigen)
    {
      gtk_widget_show_all(textfeldErgebnisWahrscheinlichkeit[zaehlererg]);
    }
    gtk_widget_show_all(textfeldWahrscheinlichkeit[zaehler]);
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]),-1);
    if(Stufe > 0)
    {
      // Zunächst werden alle Knoten unterhalb des Pfades um Knotenhoehe + KnotenAbstand nach unten verschoben.
      for(i=0;i<=maxzaehler;i++)
      {
        if(weiterunten(name,gtk_widget_get_name(textfeld[i])))
        {
          y[i] += KnotenAbstand+KnotenHoehe;
          ymax = (ymax < y[i])?y[i]:ymax; 
          int tempstufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-')-1;
          gtk_layout_move(GTK_LAYOUT(data),textfeld[i],FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*tempstufe,FensterRandOben+RandOben+y[i]);
        }
      }
      for(i=0;i<=maxzaehlererg;i++)
      {
        if(weiterunten(name,gtk_widget_get_name(textfeldErgebnis[i])))
        {
          yerg[i] += KnotenAbstand+KnotenHoehe;
          gtk_layout_move(GTK_LAYOUT(data),textfeldErgebnis[i],FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[i]);
          gtk_layout_move(GTK_LAYOUT(data),textfeldErgebnisWahrscheinlichkeit[i],FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[i]);
        }
      }

      // Jetzt werden alle Vorgänger nach unten verschoben. Von rechts nach links wird immer die Mitte der nachfolgenden Knoten gewählt.
      int zaehleri = zaehler;
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

    // Jetzt werden die Wahrscheinlichkeiten verschoben.
    positionsanpassungwsk(data);
    if(knotenhintergrundfarbewurdegeaendert)
    {
      knotenhintergrundfarbeaendern();
    }
    if(knotenrahmenfarbewurdegeaendert)
    {
      knotenrandfarbeaendern();
    }
    if(knotenrahmendickegeaendert)
    {
      knotenranddickeaendern();
    }
    if(schriftfarbewurdegeaendert)
    {
      schriftfarbeaendern();
    }
    if(schriftartwurdegewechselt)
    {
      schriftartanpassen(NULL,NULL,NULL);
    }

//        buchstabeneingabe(NULL);
    scrv=1;
    gtk_widget_queue_draw (da);

  }
  return TRUE;
}
