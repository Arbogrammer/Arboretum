void positionsanpassungwsk(gpointer data) {
  if (baum_vertikal) {
    for (int i = 0; i <= maxzaehler; i++) {
      int hoehe, breite;
      gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[i], NULL, &hoehe);
      gtk_widget_get_preferred_width(textfeldWahrscheinlichkeit[i], NULL, &breite);
      int stufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
      char unterster_name[22] = "";
      sprintf(unterster_name, "-%i", anzahlknoteninstufe(0) - 1);
      int unten_index = knotenexistiert(unterster_name);
      int virtuelle_wurzel_x =
          (y[0] + (unten_index >= 0 ? y[unten_index] : y[0]) + KnotenBreite) /
          2;
      int x = FensterRandLinks + LayoutRandLinks + y[i] + KnotenBreite / 2 -
              breite / 2;
      int yposition = vertikale_stufe_y(stufe) - hoehe / 2;
      if (stufe == 0) {
        x = FensterRandLinks + LayoutRandLinks +
            (virtuelle_wurzel_x + y[i] + KnotenBreite / 2) / 2 -
            breite / 2;
        yposition = FensterRandOben + LayoutRandOben + StufenBreite / 2 -
                    hoehe / 2;
      }
      if (stufe > 0) {
        int parent = knotenexistiert(gtk_widget_get_name(*vorgaenger[i]));
        if (parent >= 0) {
          x = FensterRandLinks + LayoutRandLinks + (y[parent] + y[i]) / 2 +
              KnotenBreite / 2 - breite / 2;
          yposition = vertikale_stufe_y(stufe - 1) + KnotenHoehe +
                      StufenBreite / 2 - hoehe / 2;
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
  int yunten = eingabe_y_position(y[knotenexistiert(unten)]);
  while (i <= maxzaehler) {
    int hoehe, breite;
    gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[i], NULL, &hoehe);
    gtk_widget_get_preferred_width(textfeldWahrscheinlichkeit[i], NULL, &breite);
    if (zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),
                       '-') == 1) {
      gtk_layout_move(
          GTK_LAYOUT(data), textfeldWahrscheinlichkeit[i],
          FensterRandLinks + LayoutRandLinks + StufenBreite / 2 -
              breite / 2,
          FensterRandOben + LayoutRandOben +
              (((eingabe_y_position(y[0]) + yunten + KnotenHoehe) / 2) + (eingabe_y_position(y[i]) + KnotenHoehe / 2)) /
                  2 -
              hoehe / 2);
      i++;
    } else {
      int Stufetemp =
          zeichenzaehlen(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),
                         '-') -
          1;
      int ywsktemp =
          (eingabe_y_position(y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]) + eingabe_y_position(y[i])) / 2;
      gtk_layout_move(
          GTK_LAYOUT(data), textfeldWahrscheinlichkeit[i],
          ((FensterRandLinks + LayoutRandLinks + StufenBreite +
            (StufenBreite + KnotenBreite) * Stufetemp) +
           (FensterRandLinks + LayoutRandLinks + StufenBreite +
            (StufenBreite + KnotenBreite) * (Stufetemp - 1) + KnotenBreite)) /
                  2 -
              breite / 2,
          FensterRandOben + LayoutRandOben + ywsktemp + (KnotenHoehe - hoehe) / 2);
      i++;
    }
  }
}
