int nachfolger(int i, int nachfolgernummer) {
  g_autofree gchar *tempname = g_strdup_printf(
      "%s-%i", gtk_widget_get_name(textfeld[i]), nachfolgernummer);
  if (knotenexistiert(tempname) > -1) {
    return knotenexistiert(tempname);
  } else {
    fprintf(stderr, "Fehler: Knoten existiert nicht\n");
    return 0;
  }
}
