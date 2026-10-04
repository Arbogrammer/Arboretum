typedef struct { int stage, height; double center; gboolean fraction; } EingabeAbstand;
static int eingabe_abstand_vergleichen(const void *a, const void *b) {
  const EingabeAbstand *x = a, *z = b;
  if (x->stage != z->stage) return x->stage < z->stage ? -1 : 1;
  return (x->center > z->center) - (x->center < z->center);
}

static void bruchfeld_abstaende(void) {
  eingabe_y_faktor = 1.0;
  eingabe_y_rand = 0;
  if (baum_vertikal || labelein || !bruchou) return;
  g_autofree EingabeAbstand *items = g_new0(EingabeAbstand, maxzaehler + maxzaehlererg + 2);
  int count = 0;
  char name[32];
  g_snprintf(name, sizeof(name), "-%d", anzahlknoteninstufe(0) - 1);
  int last = knotenexistiert(name);
  double root = (y[0] + y[last >= 0 ? last : 0]) / 2.0;
  for (int i = 0; wskanzeigen && i <= maxzaehler; i++) {
    int stage = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    int parent = stage ? knotenexistiert(gtk_widget_get_name(*vorgaenger[i])) : -1;
    int height;
    gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[i], NULL, &height);
    gboolean fraction = bruchfeld(textfeldWahrscheinlichkeit[i])->vertical;
    items[count++] = (EingabeAbstand){stage, height,
        ((parent >= 0 ? y[parent] : root) + y[i]) / 2.0, fraction};
    if (fraction) eingabe_y_rand = MAX(eingabe_y_rand, (height - KnotenHoehe + 1) / 2);
  }
  for (int i = 0; ergebnissewskanzeigen && i <= maxzaehlererg; i++) {
    int height;
    gtk_widget_get_preferred_height(textfeldErgebnisWahrscheinlichkeit[i], NULL, &height);
    gboolean fraction = bruchfeld(textfeldErgebnisWahrscheinlichkeit[i])->vertical;
    items[count++] = (EingabeAbstand){maxStufe + 1, height, yerg[i], fraction};
    if (fraction) eingabe_y_rand = MAX(eingabe_y_rand, (height - KnotenHoehe + 1) / 2);
  }
  eingabe_y_rand = MAX(0, eingabe_y_rand - FensterRandOben - LayoutRandOben);
  qsort(items, count, sizeof(*items), eingabe_abstand_vergleichen);
  for (int i = 1; i < count; i++) {
    EingabeAbstand *a = &items[i - 1], *b = &items[i];
    double gap = b->center - a->center;
    if (a->stage == b->stage && gap > 0 && (a->fraction || b->fraction))
      eingabe_y_faktor = MAX(eingabe_y_faktor, ((a->height + b->height) / 2.0 + 8) / gap);
  }
}

static int wahrscheinlichkeit_textbreite(GtkWidget *widget) {
  const char *text = gtk_entry_get_text(GTK_ENTRY(widget));
  const char *slash = strchr(text, '/');
  if (bruchou && slash && !strchr(slash + 1, '/')) {
    g_autofree char *numerator = g_strndup(text, slash - text);
    return MAX(textbreite_in_zeichen(widget, numerator),
               textbreite_in_zeichen(widget, slash + 1));
  }
  return textbreite_in_zeichen(widget, text);
}

void groesseneu(GtkWidget *widget, GdkRectangle *ap, gpointer data) {
  ueberschrift_vorbereiten(data);
  for (int i = 0; i <= maxzaehler; i++)
    bruchfeld_aktualisieren(bruchfeld(textfeldWahrscheinlichkeit[i]));
  for (int i = 0; i <= maxzaehlererg; i++)
    bruchfeld_aktualisieren(bruchfeld(textfeldErgebnisWahrscheinlichkeit[i]));
  /* Recompute from every displayed value, including results on later paths.
   * Measure with the field's font and leave room for the text cursor. This
   * also repairs stale widths restored from a file or an undo snapshot. */
  WahrscheinlichkeitTextBreite = 3;
  WahrscheinlichkeitErgebnisTextBreite = 3;
  ErgebnisTextBreite = 3;
  for (int i = 0; i <= maxzaehlererg; i++)
    ErgebnisTextBreite = MAX(ErgebnisTextBreite,
        textbreite_in_zeichen(textfeldErgebnis[i],
            gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i]))) + 1);
  if (!baum_vertikal && ueberschrift_modus) {
    ErgebnisTextBreite = MAX(ErgebnisTextBreite,
        textbreite_in_zeichen(textfeldErgebnis[0], gtk_label_get_text(GTK_LABEL(ueberschrift_label[0]))) + 1);
  }
  /* Apply before measuring: the result probability needs the new width
   * during this layout pass, before GTK allocates the resized entries. */
  for (int i = 0; i <= maxzaehlererg; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[i]),
                              ErgebnisTextBreite);
  for (int i = 0; i <= maxzaehler; i++)
    WahrscheinlichkeitTextBreite = MAX(WahrscheinlichkeitTextBreite,
        wahrscheinlichkeit_textbreite(textfeldWahrscheinlichkeit[i]) + 1);
  for (int i = 0; i <= maxzaehlererg; i++)
    WahrscheinlichkeitErgebnisTextBreite = MAX(WahrscheinlichkeitErgebnisTextBreite,
        wahrscheinlichkeit_textbreite(textfeldErgebnisWahrscheinlichkeit[i]) + 1);
  if (!baum_vertikal && ueberschrift_modus)
    WahrscheinlichkeitErgebnisTextBreite = MAX(WahrscheinlichkeitErgebnisTextBreite,
        textbreite_in_zeichen(textfeldErgebnisWahrscheinlichkeit[0], gtk_label_get_text(GTK_LABEL(ueberschrift_label[1]))) + 1);
  for (int i = 0; i <= maxzaehler; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldWahrscheinlichkeit[i]),
                              WahrscheinlichkeitTextBreite);
  for (int i = 0; i <= maxzaehlererg; i++)
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]),
                              WahrscheinlichkeitErgebnisTextBreite);

  for (int i = 0; i <= maxzaehler; i++)
    bruchfeld_aktualisieren(bruchfeld(textfeldWahrscheinlichkeit[i]));
  for (int i = 0; i <= maxzaehlererg; i++)
    bruchfeld_aktualisieren(bruchfeld(textfeldErgebnisWahrscheinlichkeit[i]));
  int mh = 0, nh = 0, mw = 0, nw = 0;
  gtk_widget_get_preferred_height(textfeld[0], &mh, &nh);
  KnotenHoehe = nh;
  KnotenBreite = 0;
  for (int i = 0; i <= maxzaehler; i++) {
    gtk_widget_get_preferred_width(textfeld[i], &mw, &nw);
    KnotenBreite = MAX(KnotenBreite, nw);
  }
  gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[0], &mh, &nh);
  gtk_widget_get_preferred_width(textfeldWahrscheinlichkeit[0], &mw, &nw);
  WahrscheinlichkeitHoehe = nh;
  WahrscheinlichkeitBreite = nw;
  for (int j = 0; j <= maxzaehler; j++) {
    gtk_widget_get_preferred_width(textfeldWahrscheinlichkeit[j], NULL, &nw);
    WahrscheinlichkeitBreite = MAX(WahrscheinlichkeitBreite, nw);
    gtk_widget_get_preferred_height(textfeldWahrscheinlichkeit[j], NULL, &nh);
    WahrscheinlichkeitHoehe = MAX(WahrscheinlichkeitHoehe, nh);
  }
  gtk_widget_get_preferred_height(textfeldErgebnis[0], &mh, &nh);
  gtk_widget_get_preferred_width(textfeldErgebnis[0], &mw, &nw);
  ErgebnisBreite = nw;
  gtk_widget_get_preferred_height(textfeldErgebnisWahrscheinlichkeit[0], &mh,
                                  &nh);
  gtk_widget_get_preferred_width(textfeldErgebnisWahrscheinlichkeit[0], &mw,
                                 &nw);
  WahrscheinlichkeitErgebnisHoehe = nh;
  WahrscheinlichkeitErgebnisBreite = nw;
  for (int j = 0; j <= maxzaehlererg; j++) {
    gtk_widget_get_preferred_width(textfeldErgebnisWahrscheinlichkeit[j], NULL, &nw);
    WahrscheinlichkeitErgebnisBreite = MAX(WahrscheinlichkeitErgebnisBreite, nw);
    gtk_widget_get_preferred_height(textfeldErgebnisWahrscheinlichkeit[j], NULL, &nh);
    WahrscheinlichkeitErgebnisHoehe = MAX(WahrscheinlichkeitErgebnisHoehe, nh);
  }
  bruchfeld_abstaende();
  int i = 0;
  for (i = 0; i <= maxzaehler; i++) {
    int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    gtk_widget_get_preferred_width(textfeld[i], &mw, &nw);
    int versatz = (KnotenBreite - nw) / 2;
    int x = baum_vertikal ? FensterRandLinks + LayoutRandLinks + y[i] + versatz
                          : FensterRandLinks + LayoutRandLinks + StufenBreite +
                                (StufenBreite + KnotenBreite) * Stufetemp +
                                versatz;
    int yposition = baum_vertikal ? vertikale_stufe_y(Stufetemp)
                                  : FensterRandOben + LayoutRandOben + eingabe_y_position(y[i]);
    gtk_layout_move(GTK_LAYOUT(data), textfeld[i], x, yposition);
  }
  for (i = 0; i <= maxzaehlererg; i++) {
    int x = baum_vertikal
                ? FensterRandLinks + LayoutRandLinks + yerg[i]
                : FensterRandLinks + LayoutRandLinks + (maxStufe + 1) * StufenBreite +
                      (maxStufe + 1) * KnotenBreite + ErgebnisAbstand;
    int yposition = baum_vertikal ? vertikale_stufe_y(maxStufe) + KnotenHoehe +
                                        ErgebnisAbstand
                                  : FensterRandOben + LayoutRandOben + eingabe_y_position(yerg[i]);
    gtk_layout_move(GTK_LAYOUT(data), textfeldErgebnis[i], x, yposition);
  }
}
