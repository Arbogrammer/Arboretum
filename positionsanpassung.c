gboolean positionsanpassung(GtkWidget *widget, gpointer data) {
  int tempzaehler = knotenexistiert(gtk_widget_get_name(widget));
  int knotenzaehler = 0;
  const char *basisname = gtk_widget_get_name(widget);
  g_autofree gchar *tempname =
      g_strdup_printf("%s-%i", basisname, knotenzaehler);
  int ytempo = (knotenexistiert(tempname)) ? y[knotenexistiert(tempname)]
                                           : y[tempzaehler];
  int ytempu = ytempo;
  while (knotenexistiert(tempname) > -1) {
    ytempu = (knotenexistiert(tempname)) ? y[knotenexistiert(tempname)]
                                         : y[tempzaehler];
    knotenzaehler += 1;
    g_free(g_steal_pointer(&tempname));
    tempname = g_strdup_printf("%s-%i", basisname, knotenzaehler);
  }
  int tempstufe = zeichenzaehlen(gtk_widget_get_name(widget), '-') - 1;
  y[tempzaehler] = (ytempo + ytempu) / 2;
  printf("Neuer y-Wert: %i\n", y[tempzaehler]);
  int mw = 0, nw = 0;
  gtk_widget_get_preferred_width(widget, &mw, &nw);
  int versatz = (KnotenBreite - nw) / 2;
  int x = baum_vertikal ? FensterRandLinks + RandLinks + y[tempzaehler] + versatz
                        : FensterRandLinks + RandLinks + StufenBreite +
                              (StufenBreite + KnotenBreite) * tempstufe + versatz;
  int yposition = baum_vertikal ? vertikale_stufe_y(tempstufe)
                                : FensterRandOben + RandOben + y[tempzaehler];
  gtk_layout_move(GTK_LAYOUT(data), widget, x, yposition);
  return TRUE;
}
