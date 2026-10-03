void baumrichtung_umschalten(GtkCheckButton *schalter, gpointer data) {
  baum_vertikal = gtk_check_button_get_active(schalter);
  baumrichtung_aktualisieren(data);
}

void baumrichtung_aktualisieren(gpointer data) {
  groesseneu(NULL, NULL, data);
  wskergebnisverschieben(NULL, NULL, data);
  positionsanpassungwsk(data);
  if (baum_vertikal && labelein) {
    /* Der Export arbeitet mit Labels statt Eingabefeldern. Auch diese Widgets
     * müssen beim Umschalten dieselbe vertikale Geometrie erhalten. */
    for (int i = 0; i <= maxzaehler; i++) {
      int mw = 0, w = 0, mh = 0, h = 0;
      int stufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
      gtk_widget_get_preferred_width(knotenlabel[i], &mw, &w);
      gtk_widget_get_preferred_height(knotenlabel[i], &mh, &h);
      gtk_layout_move(GTK_LAYOUT(data), knotenlabel[i],
                      FensterRandLinks + RandLinks + y[i] + KnotenBreite / 2 -
                          w / 2,
                      vertikale_stufe_y(stufe) + KnotenHoehe / 2 - h / 2);
      int parent = knotenexistiert(gtk_widget_get_name(*vorgaenger[i]));
      if (stufe > 0 && parent >= 0) {
        gtk_widget_get_preferred_width(wahrscheinlichkeitlabel[i], &mw, &w);
        gtk_widget_get_preferred_height(wahrscheinlichkeitlabel[i], &mh, &h);
        gtk_label_set_angle(GTK_LABEL(wahrscheinlichkeitlabel[i]), 0);
        gtk_layout_move(GTK_LAYOUT(data), wahrscheinlichkeitlabel[i],
                        FensterRandLinks + RandLinks + (y[parent] + y[i]) / 2 +
                            KnotenBreite / 2 - w / 2,
                        vertikale_stufe_y(stufe - 1) + KnotenHoehe +
                            StufenBreite / 2 - h / 2);
      } else if (stufe == 0 && wahrscheinlichkeitlabel[i]) {
        char unterster_name[22] = "";
        sprintf(unterster_name, "-%i", anzahlknoteninstufe(0) - 1);
        int unten = knotenexistiert(unterster_name);
        int wurzel_x =
            (y[0] + (unten >= 0 ? y[unten] : y[0]) + KnotenBreite) / 2;
        gtk_widget_get_preferred_width(wahrscheinlichkeitlabel[i], &mw, &w);
        gtk_widget_get_preferred_height(wahrscheinlichkeitlabel[i], &mh, &h);
        gtk_label_set_angle(GTK_LABEL(wahrscheinlichkeitlabel[i]), 0);
        gtk_layout_move(GTK_LAYOUT(data), wahrscheinlichkeitlabel[i],
                        FensterRandLinks + RandLinks +
                            (wurzel_x + y[i] + KnotenBreite / 2) / 2 - w / 2,
                        FensterRandOben + RandOben + StufenBreite / 2 - h / 2);
      }
    }
    for (int i = 0; i <= maxzaehlererg; i++) {
      int mw = 0, w = 0, mh = 0, h = 0;
      gtk_widget_get_preferred_width(ergebnislabel[i], &mw, &w);
      gtk_widget_get_preferred_height(ergebnislabel[i], &mh, &h);
      gtk_layout_move(
          GTK_LAYOUT(data), ergebnislabel[i],
          FensterRandLinks + RandLinks + yerg[i] + KnotenBreite / 2 - w / 2,
          vertikale_stufe_y(maxStufe) + KnotenHoehe + ErgebnisAbstand);
      if (!bruchou || !bruch) {
        gtk_widget_get_preferred_width(ergebniswsklabel[i], &mw, &w);
        gtk_widget_get_preferred_height(ergebniswsklabel[i], &mh, &h);
        gtk_layout_move(GTK_LAYOUT(data), ergebniswsklabel[i],
                        FensterRandLinks + RandLinks + yerg[i] +
                            KnotenBreite / 2 - w / 2,
                        vertikale_stufe_y(maxStufe) + 2 * KnotenHoehe +
                            2 * ErgebnisAbstand);
      }
    }
  }
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  arboretum_layout_dirty = FALSE;
  baumfokus_wiederherstellen();
  arboretum_widget_queue_draw_clean(da);
}
