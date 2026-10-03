/* Only values on this path determine its numeric representation. */
void wskergebnisneuschreiben(GtkWidget *widget) {
  const char *name = gtk_widget_get_name(widget);
  g_autofree char *pfad = g_strdup(name);
  char *ende = strstr(pfad, "-EW");
  if (!ende) return;
  *ende = '\0';
  double produkt = 1.0;
  long long z = 1, n = 1;
  gboolean exakt = TRUE, hat_bruch = FALSE, komma = FALSE;
  for (char *p = pfad + 1;; p++) {
    if (*p && *p != '-') continue;
    char zeichen = *p;
    *p = '\0';
    g_autofree char *kante = g_strconcat(pfad, "W", NULL);
    int index = wskexistiert(kante);
    *p = zeichen;
    double wert;
    if (index < 0 || !wahrscheinlichkeit_lesen(
          gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[index])), &wert)) {
      gtk_entry_set_text(GTK_ENTRY(widget), "");
      return;
    }
    const char *text = gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[index]));
    produkt *= wert;
    komma |= strchr(text, ',') != NULL;
    long long faktor_z, faktor_n;
    if (strchr(text, '/')) {
      hat_bruch = TRUE;
      wahrscheinlichkeit_als_bruch(text, &faktor_z, &faktor_n);
    } else if (wert == 0.0 || wert == 1.0) {
      faktor_z = (long long)wert;
      faktor_n = 1;
    } else {
      exakt = FALSE;
      faktor_z = 1;
      faktor_n = 1;
    }
    if (exakt) {
      if (kuerzen) {
        long long teiler = ggt(z, faktor_n);
        z /= teiler; faktor_n /= teiler;
        teiler = ggt(faktor_z, n);
        faktor_z /= teiler; n /= teiler;
      }
      if (__builtin_mul_overflow(z, faktor_z, &z) ||
          __builtin_mul_overflow(n, faktor_n, &n))
        exakt = FALSE; /* Fall back to the finite decimal product. */
    }
    if (!zeichen) break;
  }
  char ergebnis[100];
  if (exakt && hat_bruch) {
    long long teiler = kuerzen ? ggt(z, n) : 1;
    g_snprintf(ergebnis, sizeof ergebnis, "%lld/%lld", z / teiler, n / teiler);
  } else {
    char format[20];
    g_snprintf(format, sizeof format, "%%.%df", genauigkeit);
    g_ascii_formatd(ergebnis, sizeof ergebnis, format, produkt);
    if (komma) {
      char *punkt = strchr(ergebnis, '.');
      if (punkt) *punkt = ',';
    }
  }
  gtk_entry_set_text(GTK_ENTRY(widget), ergebnis);
}
