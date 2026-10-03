int anzahlnachfolger(int i) {
  int knotenzaehler = 0;
  const char *basisname = gtk_widget_get_name(textfeld[i]);
  g_autofree gchar *tempname =
      g_strdup_printf("%s-%i", basisname, knotenzaehler);
  while (knotenexistiert(tempname) > -1) {
    knotenzaehler += 1;
    g_free(g_steal_pointer(&tempname));
    tempname = g_strdup_printf("%s-%i", basisname, knotenzaehler);
  }
  return knotenzaehler;
}
