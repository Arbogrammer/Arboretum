void baumrichtung_umschalten(GtkCheckButton *schalter, gpointer data) {
  tempspeichern();
  dateiveraendert++;
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

static void baumrichtung_synchronisieren(void) {
  if (!baumrichtungsschalter) return;
  g_signal_handlers_block_by_func(baumrichtungsschalter,
      G_CALLBACK(baumrichtung_umschalten), gtk_widget_get_parent(textfeld[0]));
  gtk_check_button_set_active(baumrichtungsschalter, baum_vertikal);
  g_signal_handlers_unblock_by_func(baumrichtungsschalter,
      G_CALLBACK(baumrichtung_umschalten), gtk_widget_get_parent(textfeld[0]));
}
