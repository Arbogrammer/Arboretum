void reset(gpointer data) {
  tempspeichern();
  int nummer = dateinummerierung;
  dateinummerierung = 2;
  zurueck = 1;
  templaden(data);
  dateinummerierung = nummer;
  dateiveraendert++;
}
