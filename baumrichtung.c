void baumrichtung_umschalten(GtkCheckButton *schalter, gpointer data) {
  baum_vertikal = gtk_check_button_get_active(schalter);
  baumrichtung_aktualisieren(data);
}

void baumrichtung_aktualisieren(gpointer data) {
  if (labelein) {
    labelverschieben(data);
    arboretum_layout_dirty = FALSE;
    arboretum_widget_queue_draw_clean(da);
    return;
  }
  groesseneu(NULL, NULL, data);
  wskergebnisverschieben(NULL, NULL, data);
  positionsanpassungwsk(data);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  arboretum_layout_dirty = FALSE;
  baumfokus_wiederherstellen();
  arboretum_widget_queue_draw_clean(da);
}
