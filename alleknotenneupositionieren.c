static int compare_widgets(const void *a, const void *b) {
  GtkWidget *widgetA = *(GtkWidget **)a;
  GtkWidget *widgetB = *(GtkWidget **)b;

  const char *nameA = gtk_widget_get_name(widgetA);
  const char *nameB = gtk_widget_get_name(widgetB);

  return strcmp(nameA, nameB);
}

static int compare_widgets_wsk(const void *a, const void *b) {
  GtkWidget *widgetA = *(GtkWidget **)a;
  GtkWidget *widgetB = *(GtkWidget **)b;

  const char *nameA = gtk_widget_get_name(widgetA);
  const char *nameB = gtk_widget_get_name(widgetB);

  g_autofree gchar *nameA2 = g_strndup(nameA, strlen(nameA) - 1);
  g_autofree gchar *nameB2 = g_strndup(nameB, strlen(nameB) - 1);

  printf("Name: A: %s vs. %s und B: %s vs. %s\n", gtk_widget_get_name(widgetA),
         nameA2, gtk_widget_get_name(widgetB), nameB2);

  return strcmp(nameA2, nameB2);
}

void alleknotenneupositionieren(gpointer data) {
  gboolean vorher_intern = undo_intern;
  undo_intern = TRUE;
  qsort(textfeldErgebnis, maxzaehlererg + 1, sizeof(GtkWidget *),
        compare_widgets);
  qsort(textfeldErgebnisWahrscheinlichkeit, maxzaehlererg + 1,
        sizeof(GtkWidget *), compare_widgets);

  for (int i = 0; i <= maxzaehlererg; i++) {
    yerg[i] = 0 + i * KnotenAbstand + i * KnotenHoehe;
  }
  tempspeichern();
  templaden(data);

  qsort(textfeld, maxzaehler + 1, sizeof(GtkWidget *), compare_widgets);
  qsort(textfeldWahrscheinlichkeit, maxzaehler + 1, sizeof(GtkWidget *),
        compare_widgets_wsk);

  for (int i = 0; i <= maxzaehler; i++) {
    printf("textfeldWahrscheinlichkeit[%i] hat den Namen %s\n", i,
           gtk_widget_get_name(textfeldWahrscheinlichkeit[i]));
  }

  for (int i = 0; i <= maxzaehlererg; i++) {
    const char *ergebnisname = gtk_widget_get_name(textfeldErgebnis[i]);
    g_autofree gchar *tempname =
        g_strndup(ergebnisname, strlen(ergebnisname) - 2);
    y[knotenexistiert(tempname)] = yerg[i];
  }

  tempspeichern();
  templaden(data);

  int stufenanzahl = 0;
  for (int i = 0; i <= maxzaehlererg; i++) {
    int stufenanzahlaktuell =
        zeichenzaehlen(gtk_widget_get_name(textfeldErgebnis[i]), '-') - 1;
    stufenanzahl = (stufenanzahl < stufenanzahlaktuell) ? stufenanzahlaktuell
                                                        : stufenanzahl;
  }
  for (int j = stufenanzahl; j > 1; j--) {
    for (int i = 0; i <= maxzaehler; i++) {
      if (zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') == j) {
        positionsanpassung(*vorgaenger[i], data);
      }
    }
  }

  tempspeichern();
  templaden(data);
  undo_intern = vorher_intern;
}
