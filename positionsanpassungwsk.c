void positionsanpassungwsk(gpointer data) {
  if (baum_vertikal) {
    for (int i = 0; i <= maxzaehler; i++) {
      int stufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
      char unterster_name[22] = "";
      sprintf(unterster_name, "-%i", anzahlknoteninstufe(0) - 1);
      int unten_index = knotenexistiert(unterster_name);
      int virtuelle_wurzel_x =
          (y[0] + (unten_index >= 0 ? y[unten_index] : y[0]) + KnotenBreite) /
          2;
      int x = FensterRandLinks + RandLinks + y[i] + KnotenBreite / 2 -
              WahrscheinlichkeitBreite / 2;
      int yposition = vertikale_stufe_y(stufe) - WahrscheinlichkeitHoehe / 2;
      if (stufe == 0) {
        x = FensterRandLinks + RandLinks +
            (virtuelle_wurzel_x + y[i] + KnotenBreite / 2) / 2 -
            WahrscheinlichkeitBreite / 2;
        yposition = FensterRandOben + RandOben + StufenBreite / 2 -
                    WahrscheinlichkeitHoehe / 2;
      }
      if (stufe > 0) {
        int parent = knotenexistiert(gtk_widget_get_name(*vorgaenger[i]));
        if (parent >= 0) {
          x = FensterRandLinks + RandLinks + (y[parent] + y[i]) / 2 +
              KnotenBreite / 2 - WahrscheinlichkeitBreite / 2;
          yposition = vertikale_stufe_y(stufe - 1) + KnotenHoehe +
                      StufenBreite / 2 - WahrscheinlichkeitHoehe / 2;
        }
      }
      gtk_layout_move(GTK_LAYOUT(data), textfeldWahrscheinlichkeit[i], x,
                      yposition);
    }
    return;
  }
  /* Setzt jedes Wahrscheinlichkeitsfeld in die Mitte des zugehörigen Zweigs.
   * Bei Knoten der ersten Stufe beginnt der Zweig am gemeinsamen Startpunkt;
   * bei tieferen Stufen liegt er zwischen Vorgänger und aktuellem Knoten. */
  int i = 0;
  char unten[22] = "";
  sprintf(unten, "-%i", anzahlknoteninstufe(0) - 1);
  int yunten = y[knotenexistiert(unten)];
  while (i <= maxzaehler) {
    if (zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),
                       '-') == 1) {
      gtk_layout_move(
          GTK_LAYOUT(data), textfeldWahrscheinlichkeit[i],
          FensterRandLinks + RandLinks + StufenBreite / 2 -
              WahrscheinlichkeitBreite / 2,
          FensterRandOben + RandOben +
              (((y[0] + yunten + KnotenHoehe) / 2) + (y[i] + KnotenHoehe / 2)) /
                  2 -
              WahrscheinlichkeitHoehe / 2);
      i++;
    } else {
      int Stufetemp =
          zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),
                         '-') -
          1;
      int ywsktemp =
          (y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))] + y[i]) / 2;
      gtk_layout_move(
          GTK_LAYOUT(data), textfeldWahrscheinlichkeit[i],
          ((FensterRandLinks + RandLinks + StufenBreite +
            (StufenBreite + KnotenBreite) * Stufetemp) +
           (FensterRandLinks + RandLinks + StufenBreite +
            (StufenBreite + KnotenBreite) * (Stufetemp - 1) + KnotenBreite)) /
                  2 -
              WahrscheinlichkeitBreite / 2,
          FensterRandOben + RandOben + ywsktemp);
      i++;
    }
  }
}
