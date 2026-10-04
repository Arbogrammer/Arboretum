void wskergebnisverschieben(GtkWidget *widget, GtkAllocation *allocation,
                            gpointer data) {
  int i;
  /* Allocation may still describe the previous text (or be zero directly
   * after loading). Use the requested width of the newly sized entries. */
  gtk_widget_get_preferred_width(textfeldErgebnis[0], NULL, &ErgebnisBreite);

  for (i = 0; i <= maxzaehlererg; i++) {
    int hoehe, breite;
    gtk_widget_get_preferred_height(textfeldErgebnisWahrscheinlichkeit[i], NULL, &hoehe);
    gtk_widget_get_preferred_width(textfeldErgebnisWahrscheinlichkeit[i], NULL, &breite);
    int x =
        baum_vertikal
            ? FensterRandLinks + LayoutRandLinks + yerg[i] +
                  (ErgebnisBreite - breite) / 2
            : FensterRandLinks + LayoutRandLinks + (maxStufe + 1) * StufenBreite +
                  (maxStufe + 1) * KnotenBreite + ErgebnisAbstand +
                  ((ergebnisseanzeigen) ? ErgebnisAbstand + ErgebnisBreite : 0);
    int y = baum_vertikal ? vertikale_stufe_y(maxStufe) + KnotenHoehe + ErgebnisAbstand +
                                (ergebnisseanzeigen ? KnotenHoehe + ErgebnisAbstand : 0)
                          : FensterRandOben + LayoutRandOben + eingabe_y_position(yerg[i]) + (KnotenHoehe - hoehe) / 2;
    gtk_layout_move(GTK_LAYOUT(data), textfeldErgebnisWahrscheinlichkeit[i], x,
                    y);
  }
  ueberschrift_positionieren(data);
}
