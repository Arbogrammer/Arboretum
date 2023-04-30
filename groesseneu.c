void groesseneu(GtkWidget *widget, GdkRectangle *ap, gpointer data)
{
    int mh = 0, nh = 0, mw = 0, nw = 0;
    gtk_widget_get_preferred_height(textfeld[0], &mh, &nh);
    gtk_widget_get_preferred_width(textfeld[0], &mw, &nw);
    KnotenHoehe = nh;
    KnotenBreite = nw;
    gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[0], &mh, &nh);
    gtk_widget_get_preferred_width(textfeldWahrscheinlichkeit[0], &mw, &nw);
    WahrscheinlichkeitHoehe = nh;
    WahrscheinlichkeitBreite = nw;
    gtk_widget_get_preferred_height(textfeldErgebnis[0], &mh, &nh);
    gtk_widget_get_preferred_width(textfeldErgebnis[0], &mw, &nw);
    ErgebnisBreite = nw;
    int i=0;
    for(i=0;i<=maxzaehler;i++)
    {
      int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-')-1;
      gtk_layout_move (GTK_LAYOUT (data), textfeld[i], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufetemp,FensterRandOben+RandOben+y[i]);
    }
    for(i=0;i<=maxzaehlererg;i++)
    {
      gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[i]),ErgebnisTextBreite);
      gtk_layout_move (GTK_LAYOUT (data), textfeldErgebnis[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[i]);
    }
    
//      printf("KH: %i, KB: %i, WH: %i, WB: %i\n",KnotenHoehe,KnotenBreite,WahrscheinlichkeitHoehe, WahrscheinlichkeitBreite);
}
