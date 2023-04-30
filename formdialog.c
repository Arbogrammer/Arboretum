static void formdialog(GtkWidget *button, gpointer data)
{
  GtkWidget *content_area;
  GtkWidget *dialog;
  GtkWidget *hbox;
  GtkWidget *table;
  GtkWidget *label;
  gint response;

  dialog = gtk_dialog_new_with_buttons ("Größeneinstellungen",GTK_WINDOW (window),GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,"Übernehme die Änderungen und gehe zurück",GTK_RESPONSE_OK,"Verwerfe die Änderungen und gehe zurück",GTK_RESPONSE_CANCEL,NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
  gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

  table = gtk_grid_new ();
  gtk_grid_set_row_spacing (GTK_GRID (table), 6);
  gtk_grid_set_column_spacing (GTK_GRID (table), 4);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0);
  GtkWidget *randlinks = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(randlinks),RandLinks);
  GtkWidget *randrechts = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(randrechts),RandRechts);
  GtkWidget *randoben = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(randoben),RandOben);
  GtkWidget *randunten = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(randunten),RandUnten);
  GtkWidget *liniendicke = gtk_spin_button_new_with_range (1, 15, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(liniendicke),LinienDicke);
  GtkWidget *stufenbreite = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(stufenbreite),StufenBreite);
  GtkWidget *ergebnisabstand = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ergebnisabstand),ErgebnisAbstand);
  GtkWidget *knotenabstand = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(knotenabstand),KnotenAbstand);
  GtkWidget *ergebnistrenner = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ergebnistrenner),1);
  char ErgebnisTrennerString[2] = "";
  ErgebnisTrennerString[0] = ErgebnisTrenner;
  gtk_entry_set_text(GTK_ENTRY(ergebnistrenner),ErgebnisTrennerString);
  GtkWidget *genau = gtk_spin_button_new_with_range (1, 20, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(genau),genauigkeit);
  GtkWidget *pad = gtk_spin_button_new_with_range (0, 20, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(pad),padding);
  GtkWidget *padk = gtk_spin_button_new_with_range (0, 100, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(padk),paddingk);
  GtkWidget *knotentextbreite = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(knotentextbreite),KnotenTextBreite);
  GtkWidget *wahrscheinlichkeittextbreite = gtk_spin_button_new_with_range (0, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(wahrscheinlichkeittextbreite),WahrscheinlichkeitTextBreite);
  GtkWidget *bruchdarstellungvertikal = gtk_check_button_new();
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(bruchdarstellungvertikal), bruchou);
  GtkWidget *kuerzenauswahl = gtk_check_button_new();
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(kuerzenauswahl), kuerzen);
  GtkWidget *knotenrahmenabstandauswahl = gtk_spin_button_new_with_range (0, 100, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(knotenrahmenabstandauswahl),knotenrahmenabstand);
  GtkWidget *wskversch = gtk_spin_button_new_with_range (-500, 500, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(wskversch),wskverschiebung);
  GtkWidget *knotenrahmendickeauswahl = gtk_spin_button_new_with_range (1, 20, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(knotenrahmendickeauswahl),knotenrahmendicke);
  
  label = gtk_label_new ("Linker Seitenrand:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (table), randlinks, 1, 0, 1, 1);
  label = gtk_label_new ("Rechter Seitenrand:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (table), randrechts, 1, 1, 1, 1);
  label = gtk_label_new ("Oberer Seitenrand:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 2, 1, 1);
  gtk_grid_attach (GTK_GRID (table), randoben, 1, 2, 1, 1);
  label = gtk_label_new ("Unterer Seitenrand:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 3, 1, 1);
  gtk_grid_attach (GTK_GRID (table), randunten, 1, 3, 1, 1);
  label = gtk_label_new ("Linienstärke:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (table), liniendicke, 3, 0, 1, 1);
  label = gtk_label_new ("Abstand der Stufen:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (table), stufenbreite, 3, 1, 1, 1);
  label = gtk_label_new ("Abstand zur Ergebnisspalte:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 2, 1, 1);
  gtk_grid_attach (GTK_GRID (table), ergebnisabstand, 3, 2, 1, 1);
  label = gtk_label_new ("Vertikaler Abstand der Knoten:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 3, 1, 1);
  gtk_grid_attach (GTK_GRID (table), knotenabstand, 3, 3, 1, 1);
  label = gtk_label_new ("Anzahl Nachkommastellen:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 5, 1, 1);
  gtk_grid_attach (GTK_GRID (table), genau, 1, 5, 1, 1);
  label = gtk_label_new ("Ergebnis-Trennzeichen:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 4, 1, 1);
  gtk_grid_attach (GTK_GRID (table), ergebnistrenner, 1, 4, 1, 1);
  label = gtk_label_new ("Breite der Knotenpunkte:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 4, 1, 1);
  gtk_grid_attach (GTK_GRID (table), knotentextbreite, 3, 4, 1, 1);
  label = gtk_label_new ("Breite der Wahrscheinlichkeiten:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 5, 1, 1);
  gtk_grid_attach (GTK_GRID (table), wahrscheinlichkeittextbreite, 3, 5, 1, 1);
  label = gtk_label_new ("Abstand Zweig zu Wahrscheinlichkeit:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 6, 1, 1);
  gtk_grid_attach (GTK_GRID (table), pad, 3, 6, 1, 1);
  label = gtk_label_new ("Abstand Knoten zu Zweig:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 7, 1, 1);
  gtk_grid_attach (GTK_GRID (table), padk, 3, 7, 1, 1);
  label = gtk_label_new ("Bruchdarstellung vertikal:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 6, 1, 1);
  gtk_grid_attach (GTK_GRID (table), bruchdarstellungvertikal, 1, 6, 1, 1);
  label = gtk_label_new ("Brüche kürzen:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 0, 7, 1, 1);
  gtk_grid_attach (GTK_GRID (table), kuerzenauswahl, 1, 7, 1, 1);
  label = gtk_label_new ("Abstand Knoten zu Rahmen:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 8, 1, 1);
  gtk_grid_attach (GTK_GRID (table), knotenrahmenabstandauswahl, 3, 8, 1, 1);
  label = gtk_label_new ("Verschiebung der Wahrscheinlichkeiten nach rechts:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 10, 1, 1);
  gtk_grid_attach (GTK_GRID (table), wskversch, 3, 10, 1, 1);
  label = gtk_label_new ("Knotenrahmendicke:");
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (table), label, 2, 9, 1, 1);
  gtk_grid_attach (GTK_GRID (table), knotenrahmendickeauswahl, 3, 9, 1, 1);

  gtk_widget_show_all (hbox);

  response = gtk_dialog_run (GTK_DIALOG (dialog));

  if (response == GTK_RESPONSE_OK)
    {
      RandLinks = gtk_spin_button_get_value(GTK_SPIN_BUTTON(randlinks));
      RandRechts = gtk_spin_button_get_value(GTK_SPIN_BUTTON(randrechts));
      RandOben = gtk_spin_button_get_value(GTK_SPIN_BUTTON(randoben));
      RandUnten = gtk_spin_button_get_value(GTK_SPIN_BUTTON(randunten));
      LinienDicke = gtk_spin_button_get_value(GTK_SPIN_BUTTON(liniendicke));
      StufenBreite = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stufenbreite));
      ErgebnisAbstand = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ergebnisabstand));
      int KnotenAbstandTemp = KnotenAbstand;
      KnotenAbstand = gtk_spin_button_get_value(GTK_SPIN_BUTTON(knotenabstand));
      genauigkeit = gtk_spin_button_get_value(GTK_SPIN_BUTTON(genau));
      padding = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pad));
      paddingk = gtk_spin_button_get_value(GTK_SPIN_BUTTON(padk));
      bruchou = (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(bruchdarstellungvertikal));
      kuerzen = (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(kuerzenauswahl));
      knotenrahmenabstand = gtk_spin_button_get_value(GTK_SPIN_BUTTON(knotenrahmenabstandauswahl));
      wskverschiebung = gtk_spin_button_get_value(GTK_SPIN_BUTTON(wskversch));
      int knotenrahmendickevorher = knotenrahmendicke;
      knotenrahmendicke = gtk_spin_button_get_value(GTK_SPIN_BUTTON(knotenrahmendickeauswahl));
      if(knotenrahmendickevorher != knotenrahmendicke)
      {
        knotenrahmendickegeaendert = 1;
      }
      if(KnotenAbstandTemp != KnotenAbstand)
      {
        positionneu(KnotenAbstandTemp,data);
      }
      ErgebnisTrenner = gtk_entry_get_text(GTK_ENTRY(ergebnistrenner))[0];
      int i=0;
      for(i=0 ; i<= maxzaehlererg ; i++)
      {
        ergebnistextneuschreiben(textfeldErgebnis[i]);
      }
      KnotenTextBreite = gtk_spin_button_get_value(GTK_SPIN_BUTTON(knotentextbreite));
      i=0;
      for(i=0 ; i<= maxzaehler ; i++)
      {
        gtk_entry_set_width_chars (GTK_ENTRY(textfeld[i]),KnotenTextBreite);
        gtk_widget_queue_resize(textfeld[i]);
      }
      KnotenBreite = gtk_widget_get_allocated_width(textfeld[0]);
      WahrscheinlichkeitTextBreite = gtk_spin_button_get_value(GTK_SPIN_BUTTON(wahrscheinlichkeittextbreite));
      i=0;
      for(i=0 ; i<= maxzaehler ; i++)
      {
        gtk_entry_set_width_chars (GTK_ENTRY(textfeldWahrscheinlichkeit[i]),WahrscheinlichkeitTextBreite);
        gtk_widget_queue_resize(textfeldWahrscheinlichkeit[i]);
      }
      for(i=0 ; i<= maxzaehlererg ; i++)
      {
        wskergebnisneuschreiben(textfeldErgebnisWahrscheinlichkeit[i]);
      }
      WahrscheinlichkeitBreite = gtk_widget_get_allocated_width(textfeldWahrscheinlichkeit[0]);
      positionsanpassungwsk(data);
      if(knotenrahmendickegeaendert)
      {
        knotenranddickeaendern();
      }
      tempspeichern();
      templaden(data);
      gtk_widget_queue_draw (da);
    }
  gtk_widget_destroy (dialog);

}
