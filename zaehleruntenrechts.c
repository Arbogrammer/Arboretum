int zaehleruntenrechts(int zaehlerlinks) {
  int knotenzaehler = 0;
  g_autoptr(GString) tempnamelang =
      g_string_new(gtk_widget_get_name(textfeld[zaehlerlinks]));
  g_autofree gchar *tempname =
      g_strdup_printf("%s-%i", tempnamelang->str, knotenzaehler);
  g_autofree gchar *ergebnis = NULL;
  if (knotenexistiert(tempname) == -1) {
    return zaehlerlinks;
  }
  while (knotenexistiert(tempnamelang->str) > -1) {
    while (knotenexistiert(tempname) > -1) {
      g_free(g_steal_pointer(&ergebnis));
      ergebnis = g_strdup(tempname);
      knotenzaehler += 1;
      g_free(g_steal_pointer(&tempname));
      tempname = g_strdup_printf("%s-%i", tempnamelang->str, knotenzaehler);
    }
    g_string_append_printf(tempnamelang, "-%i", knotenzaehler - 1);
    knotenzaehler = 0;
    g_free(g_steal_pointer(&tempname));
    tempname = g_strdup(tempnamelang->str);
  }
  return knotenexistiert(ergebnis);
}
