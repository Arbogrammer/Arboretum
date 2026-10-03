void wskergebnisverschieben(GtkWidget *widget, GtkAllocation *allocation,
                            gpointer data) {
  int i;
  /* Allocation may still describe the previous text (or be zero directly
   * after loading). Use the requested width of the newly sized entries. */
  gtk_widget_get_preferred_width(textfeldErgebnis[0], NULL, &ErgebnisBreite);
  gtk_widget_get_preferred_width(textfeldErgebnisWahrscheinlichkeit[0], NULL,
                                 &WahrscheinlichkeitErgebnisBreite);
  for (i = 0; i <= maxzaehlererg; i++) {
    int x =
        baum_vertikal
            ? FensterRandLinks + RandLinks + yerg[i] +
                  (ErgebnisBreite - WahrscheinlichkeitErgebnisBreite) / 2
            : FensterRandLinks + RandLinks + (maxStufe + 1) * StufenBreite +
                  (maxStufe + 1) * KnotenBreite + ErgebnisAbstand +
                  ((ergebnisseanzeigen) ? ErgebnisAbstand + ErgebnisBreite : 0);
    int y = baum_vertikal ? vertikale_stufe_y(maxStufe) + 2 * KnotenHoehe +
                                2 * ErgebnisAbstand
                          : FensterRandOben + RandOben + yerg[i];
    gtk_layout_move(GTK_LAYOUT(data), textfeldErgebnisWahrscheinlichkeit[i], x,
                    y);
  }
}
