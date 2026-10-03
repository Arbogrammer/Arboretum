void groesseneu(GtkWidget *widget, GdkRectangle *ap, gpointer data) {
  /* Recompute from every displayed value, including results on later paths.
   * Measure with the field's font and leave room for the text cursor. This
   * also repairs stale widths restored from a file or an undo snapshot. */
  WahrscheinlichkeitTextBreite = 3;
  WahrscheinlichkeitErgebnisTextBreite = 3;
  ErgebnisTextBreite = 3;
  for (int i = 0; i <= maxzaehlererg; i++)
    ErgebnisTextBreite = MAX(ErgebnisTextBreite,
        textbreite_in_zeichen(textfeldErgebnis[i],
            gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i]))) + 1);
  /* Apply before measuring: the result probability needs the new width
   * during this layout pass, before GTK allocates the resized entries. */
  for (int i = 0; i <= maxzaehlererg; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[i]),
                              ErgebnisTextBreite);
  for (int i = 0; i <= maxzaehler; i++)
    WahrscheinlichkeitTextBreite = MAX(WahrscheinlichkeitTextBreite,
        textbreite_in_zeichen(textfeldWahrscheinlichkeit[i],
            gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i]))) + 1);
  for (int i = 0; i <= maxzaehlererg; i++)
    WahrscheinlichkeitErgebnisTextBreite = MAX(WahrscheinlichkeitErgebnisTextBreite,
        textbreite_in_zeichen(textfeldErgebnisWahrscheinlichkeit[i],
            gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]))) + 1);
  for (int i = 0; i <= maxzaehler; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldWahrscheinlichkeit[i]),
                              WahrscheinlichkeitTextBreite);
  for (int i = 0; i <= maxzaehlererg; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]),
                              WahrscheinlichkeitErgebnisTextBreite);

  int mh = 0, nh = 0, mw = 0, nw = 0;
  gtk_widget_get_preferred_height(textfeld[0], &mh, &nh);
  KnotenHoehe = nh;
  KnotenBreite = 0;
  for (int i = 0; i <= maxzaehler; i++) {
    gtk_widget_get_preferred_width(textfeld[i], &mw, &nw);
    KnotenBreite = MAX(KnotenBreite, nw);
  }
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
    gtk_widget_get_preferred_width(textfeld[i], &mw, &nw);
    int versatz = (KnotenBreite - nw) / 2;
    int x = baum_vertikal ? FensterRandLinks + RandLinks + y[i] + versatz
                          : FensterRandLinks + RandLinks + StufenBreite +
                                (StufenBreite + KnotenBreite) * Stufetemp +
                                versatz;
    int yposition = baum_vertikal ? vertikale_stufe_y(Stufetemp)
                                  : FensterRandOben + RandOben + y[i];
    gtk_layout_move(GTK_LAYOUT(data), textfeld[i], x, yposition);
  }
  for (i = 0; i <= maxzaehlererg; i++) {
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
