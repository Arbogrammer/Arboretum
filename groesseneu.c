void groesseneu(GtkWidget *widget, GdkRectangle *ap, gpointer data) {
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
  gtk_widget_get_preferred_height(textfeldErgebnisWahrscheinlichkeit[0], &mh,
                                  &nh);
  gtk_widget_get_preferred_width(textfeldErgebnisWahrscheinlichkeit[0], &mw,
                                 &nw);
  WahrscheinlichkeitErgebnisHoehe = nh;
  WahrscheinlichkeitErgebnisBreite = nw;
  int i = 0;
  for (i = 0; i <= maxzaehler; i++) {
    int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    int x = baum_vertikal ? FensterRandLinks + RandLinks + y[i]
                          : FensterRandLinks + RandLinks + StufenBreite +
                                (StufenBreite + KnotenBreite) * Stufetemp;
    int yposition = baum_vertikal ? vertikale_stufe_y(Stufetemp)
                                  : FensterRandOben + RandOben + y[i];
    gtk_layout_move(GTK_LAYOUT(data), textfeld[i], x, yposition);
  }
  for (i = 0; i <= maxzaehlererg; i++) {
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[i]),
                              ErgebnisTextBreite);
    int x = baum_vertikal
                ? FensterRandLinks + RandLinks + yerg[i]
                : FensterRandLinks + RandLinks + (maxStufe + 1) * StufenBreite +
                      (maxStufe + 1) * KnotenBreite + ErgebnisAbstand;
    int yposition = baum_vertikal ? vertikale_stufe_y(maxStufe) + KnotenHoehe +
                                        ErgebnisAbstand
                                  : FensterRandOben + RandOben + yerg[i];
    gtk_layout_move(GTK_LAYOUT(data), textfeldErgebnis[i], x, yposition);
  }
}
