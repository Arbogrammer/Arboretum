gboolean rechts(GtkWidget *widget, gpointer data)
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
    rechtswsk(widget, data);
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

  char neuname[10000] = "";
  strcpy(neuname,gtk_widget_get_name(aktuelleswidget));
  strcat(neuname,"-0");
  char nameerg[10002] = "";
  strcpy(nameerg,gtk_widget_get_name(aktuelleswidget));
  strcat(nameerg,"-E");
  char neunamew[10002] = "";
  sprintf(neunamew,"%sW",neuname);
  char nameergw[10004] = "";
  zaehlererg = ergebniszaehlernummer(nameerg);
  int ex = 0;
  Knoten[Stufe+1] = 0;
  int i=0;
  for(i=0;i<=maxzaehler;i++)
  {
    if(strcmp(neuname,gtk_widget_get_name(textfeld[i])) == 0)
    {
      ex = 1;
    }
  }
  if(ex == 0)
  {
    dateiveraendert++;
    tempspeichern();
    int tempzaehler = zaehler;
    zaehler = maxzaehler + 1;
    maxzaehler += 1;
    vorgaenger[zaehler] = &textfeld[tempzaehler];
    Stufe += 1;
    if(Stufe > maxStufe)
    {
      maxStufe += 1; // alternativ: maxStufe = Stufe;
    }
    textfeld[zaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeld[zaehler],neuname);
    textfeldWahrscheinlichkeit[zaehler] = gtk_entry_new();
    gtk_widget_set_name(textfeldWahrscheinlichkeit[zaehler],neunamew);
      char nameerg[10000] = "";
      strcat(nameerg,neuname);
      strcat(nameerg,"-E");
      sprintf(nameergw,"%sW",nameerg);
    gtk_widget_set_name(textfeldErgebnis[zaehlererg],nameerg);
    gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[zaehlererg],nameergw);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeld[zaehler]),KnotenTextBreite);
    gtk_entry_set_width_chars (GTK_ENTRY(textfeldWahrscheinlichkeit[zaehler]),WahrscheinlichkeitTextBreite);
    gtk_entry_set_alignment (GTK_ENTRY(textfeld[zaehler]),0.5);
    gtk_entry_set_alignment (GTK_ENTRY(textfeldWahrscheinlichkeit[zaehler]),0.5);
    g_signal_connect (textfeld[zaehler], "changed", G_CALLBACK (buchstabeneingabe), data);
    g_signal_connect (textfeldWahrscheinlichkeit[zaehler], "changed", G_CALLBACK (wskeingabe), data);
    y[zaehler] = y[tempzaehler];
    gtk_layout_put (GTK_LAYOUT (data), textfeld[zaehler], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufe,FensterRandOben+RandOben+y[zaehler]);
    gtk_layout_put (GTK_LAYOUT (data), textfeldWahrscheinlichkeit[zaehler], ((FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufe)+(FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufe-1)+KnotenBreite))/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+y[zaehler]);
    gtk_widget_show_all(textfeld[zaehler]);
    gtk_widget_show_all(textfeldWahrscheinlichkeit[zaehler]);
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]),-1);

    // Die Ergebnisspalten werden nach rechts geschoben.
    int i=0;
    for(i=0 ; i <= maxzaehlererg ; i++)
    {
      gtk_layout_move(GTK_LAYOUT(data),textfeldErgebnis[i],FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[i]);
      gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[i]),ErgebnisTextBreite);
      gtk_layout_move(GTK_LAYOUT(data),textfeldErgebnisWahrscheinlichkeit[i],FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisBreite:0),FensterRandOben+RandOben+yerg[i]);
    }

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
  }
  else
  {
    Stufe += 1;
    int i=0;
    while(strcmp(neuname,gtk_widget_get_name(textfeld[i])))
    {
      i++;
    }
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[i])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[i]),-1);
    zaehler=i;
  }
  scrh=1;
  gtk_widget_queue_draw (da);
  return TRUE;
}
