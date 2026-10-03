static gchar *knotenname_nach_loeschen(const char *altername,
                                       const char *namensanfang,
                                       int geloeschte_nummer) {
  gsize anfangslaenge = strlen(namensanfang);
  if (altername[anfangslaenge] != '-')
    return g_strdup(altername);
  const char *nummer = altername + anfangslaenge + 1;
  const char *ende = nummer;
  while (*ende && *ende != '-' && *ende != 'E' && *ende != 'W')
    ende++;
  int alte_nummer = atoi(nummer);
  if (alte_nummer <= geloeschte_nummer)
    return g_strdup(altername);
  return g_strdup_printf("%s-%i%s", namensanfang, alte_nummer - 1, ende);
}

void loeschen(gpointer data) {
  if (maxzaehler == 0) {
    return;
  }
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  g_autofree gchar *name = g_strdup(gtk_widget_get_name(aktuelleswidget));
  g_autofree gchar *nameBeginn = g_strdup(name);
  int k = 0;
  while (name[strlen(name) - k - 1] != '-') {
    nameBeginn[strlen(name) - k - 1] = 0;
    k++;
  }
  nameBeginn[strlen(name) - k - 1] = 0;
  gchar nameEnde[10] = "";
  k = 0;
  while (name[strlen(name) - k - 1] != '-') {
    nameEnde[strlen(name) - strlen(nameBeginn) - k - 2] =
        name[strlen(name) - k - 1];
    k++;
  }

  tempspeichern();

  int i = 0;
  FILE *datei;
  g_autofree gchar *dateiname = arboretum_temp_path(dateinummerierung);
  if (!dateiname)
    return;
  dateinummerierung += 1;
  datei = g_fopen(dateiname, "w+b");
  if (!datei) {
    g_warning("Temporäre Datei konnte nicht geöffnet werden: %s", dateiname);
    dateinummerierung -= 1;
    return;
  }
  int j = 0;
  for (i = 0; i <= maxzaehler; i++) {
    const gchar *altername = gtk_widget_get_name(textfeld[i]);
    if (strncmp(name, altername, strlen(name)) != 0) {
      if (strncmp(nameBeginn, altername, strlen(nameBeginn)) != 0) {
        fprintf(datei, "%i%c%i%c%s%c%s%c\n", i - j, 31, y[i], 31, altername, 31,
                gtk_entry_get_text(GTK_ENTRY(textfeld[i])), 31);
      } else {
        g_autofree gchar *neuername =
            knotenname_nach_loeschen(altername, nameBeginn, atoi(nameEnde));
        fprintf(datei, "%i%c%i%c%s%c%s%c\n", i - j, 31, y[i], 31, neuername, 31,
                gtk_entry_get_text(GTK_ENTRY(textfeld[i])), 31);
      }
    } else {
      j++;
    }
  }
  fprintf(datei, "%c\n", 30);
  j = 0;
  for (i = 0; i <= maxzaehlererg; i++) {
    const gchar *altername = gtk_widget_get_name(textfeldErgebnis[i]);
    if (nameEnde[0] == '0' &&
        strncmp(nameBeginn, altername, strlen(nameBeginn)) == 0 &&
        anzahlnachfolger(knotenexistiert(
            gtk_widget_get_name(*vorgaenger[knotenexistiert(name)]))) ==
            1) // Die letzte Bedingung bedeutet, dass die Anzahl der Nachfolger
               // des Vorgängers gleich 1 ist, dass also der Knoten keine Knoten
               // mehr unter sich hat.
    {
      printf("altername: %s, nameBeginn: %s, nameEnde: %s, maxzaehlererg: %i\n",
             altername, nameBeginn, nameEnde, maxzaehlererg);
      g_autofree gchar *neuername = g_strdup_printf("%s-E", nameBeginn);
      printf("neuername: %s\n", neuername);
      fprintf(datei, "%i%c%i%c%s%c%s%c\n", i - j, 31, yerg[i], 31, neuername,
              31, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])), 31);
      continue;
    }
    if (strncmp(name, altername, strlen(name)) != 0) {
      if (strncmp(nameBeginn, altername, strlen(nameBeginn)) != 0) {
        fprintf(datei, "%i%c%i%c%s%c%s%c\n", i - j, 31, yerg[i], 31, altername,
                31, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])), 31);
      } else {
        g_autofree gchar *neuername =
            knotenname_nach_loeschen(altername, nameBeginn, atoi(nameEnde));
        fprintf(datei, "%i%c%i%c%s%c%s%c\n", i - j, 31, yerg[i], 31, neuername,
                31, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])), 31);
      }
    } else {
      j++;
    }
  }
  fprintf(datei, "%c\n", 30);
  j = 0;
  for (i = 0; i <= maxzaehler; i++) {
    const gchar *altername = gtk_widget_get_name(textfeldWahrscheinlichkeit[i]);
    if (strncmp(name, altername, strlen(name)) != 0) {
      if (strncmp(nameBeginn, altername, strlen(nameBeginn)) != 0) {
        fprintf(datei, "%i%c%s%c%s%c\n", i - j, 31, altername, 31,
                gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),
                31);
      } else {
        g_autofree gchar *neuername =
            knotenname_nach_loeschen(altername, nameBeginn, atoi(nameEnde));
        fprintf(datei, "%i%c%s%c%s%c\n", i - j, 31, neuername, 31,
                gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),
                31);
      }
    } else {
      j++;
    }
  }

  fprintf(datei, "%c\n", 30);
  j = 0;
  for (i = 0; i <= maxzaehlererg; i++) {
    const gchar *altername =
        gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]);
    if (nameEnde[0] == '0' &&
        strncmp(nameBeginn, altername, strlen(nameBeginn)) == 0 &&
        anzahlnachfolger(knotenexistiert(
            gtk_widget_get_name(*vorgaenger[knotenexistiert(name)]))) == 1) {
      g_autofree gchar *neuername = g_strdup_printf("%s-E", nameBeginn);
      fprintf(
          datei, "%i%c%s%c%s%c\n", i - j, 31, neuername, 31,
          gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),
          31);
      continue;
    }
    if (strncmp(name, altername, strlen(name)) != 0) {
      if (strncmp(nameBeginn, altername, strlen(nameBeginn)) != 0) {
        fprintf(datei, "%i%c%s%c%s%c\n", i - j, 31, altername, 31,
                gtk_entry_get_text(
                    GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),
                31);
      } else {
        g_autofree gchar *neuername =
            knotenname_nach_loeschen(altername, nameBeginn, atoi(nameEnde));
        fprintf(datei, "%i%c%s%c%s%c\n", i - j, 31, neuername, 31,
                gtk_entry_get_text(
                    GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),
                31);
      }
    } else {
      j++;
    }
  }

  /* templaden() erwartet auch bei Zuständen ohne eigenen
   * Darstellungsdatensatz den Abschluss des vierten Baumabschnitts. */
  fprintf(datei, "%c\n", 30);

  fclose(datei);

  templaden(data);
  for (i = 0; i <= maxzaehlererg; i++) {
    ergebnistextneuschreiben(textfeldErgebnis[i]);
  }
  alleknotenneupositionieren(data);
}
