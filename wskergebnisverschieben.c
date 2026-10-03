void wskergebnisverschieben(GtkWidget *widget, GtkAllocation *allocation,
                            gpointer data) {
  int i;
  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
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
