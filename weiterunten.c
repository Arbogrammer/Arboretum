int weiterunten(char *stringoben, const gchar *stringunten) {
  g_autofree gchar *stringobentemp = g_strdup(stringoben);
  g_autofree gchar *stringuntentemp = g_strdup(stringunten);
  char *zeigeroben, *zeigerunten;
  zeigeroben = strtok(stringobentemp, "-");
  zeigerunten = strtok(stringuntentemp, "-");
  int laengeobenkumulativ = 0, laengeuntenkumulativ = 0;
  while (zeigeroben != NULL && zeigerunten != NULL) {
    int laengeoben = strlen(zeigeroben);
    int laengeunten = strlen(zeigerunten);
    laengeobenkumulativ += laengeoben + 1;
    laengeuntenkumulativ += laengeunten + 1;
    if (atoi(zeigeroben) < atoi(zeigerunten)) {
      return 1;
    }
    if (atoi(zeigeroben) > atoi(zeigerunten)) {
      return 0;
    }
    zeigeroben = strtok(stringobentemp + laengeobenkumulativ + 1, "-");
    zeigerunten = strtok(stringuntentemp + laengeuntenkumulativ + 1, "-");
  }
  return 0;
}
