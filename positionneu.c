void positionneu(int KnotenAbstandVorher, gpointer data) {
  for (int i = 0; i <= maxzaehlererg; i++) {
    yerg[i] = (yerg[i] / (KnotenAbstandVorher + KnotenHoehe)) *
              (KnotenAbstand + KnotenHoehe);
    const char *ergebnisname = gtk_widget_get_name(textfeldErgebnis[i]);
    g_autofree gchar *tempname = g_strndup(
        ergebnisname, (gsize)(strrchr(ergebnisname, '-') - ergebnisname));
    int knotenindex = knotenexistiert(tempname);
    y[knotenindex] = (y[knotenindex] / (KnotenAbstandVorher + KnotenHoehe)) *
                     (KnotenAbstand + KnotenHoehe);
    int zaehleri = knotenindex;
    while (zaehleri > 0) {
      positionsanpassung(*vorgaenger[zaehleri], data);
      int abbruch =
          zeichenzaehlen(gtk_widget_get_name(*vorgaenger[zaehleri]), '-');
      zaehleri = knotenexistiert(gtk_widget_get_name(*vorgaenger[zaehleri]));
      int abbruch2 =
          zeichenzaehlen(gtk_widget_get_name(*vorgaenger[zaehleri]), '-');
      if (abbruch == abbruch2) {
        break;
      }
    }
  }
}
