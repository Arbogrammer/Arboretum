static gsize bdg_header_feldgrenze(guint feld) {
  if (feld == 15)
    return 1;
  if ((feld >= 19 && feld <= 38) || feld == 54 || feld == 55)
    return 99;
  if (feld == 53)
    return 999;
  return 21;
}

static gboolean bdg_ganzzahl_pruefen(const char *start, gsize laenge,
                                     gint64 minimum, gint64 maximum,
                                     gint64 *wert) {
  if (!laenge || laenge > 21)
    return FALSE;
  char puffer[22];
  memcpy(puffer, start, laenge);
  puffer[laenge] = 0;
  char *ende = NULL;
  errno = 0;
  gint64 ergebnis = g_ascii_strtoll(puffer, &ende, 10);
  if (errno == ERANGE || ende == puffer || *ende || ergebnis < minimum ||
      ergebnis > maximum)
    return FALSE;
  if (wert)
    *wert = ergebnis;
  return TRUE;
}

static gboolean bdg_kommazahl_pruefen(const char *start, gsize laenge,
                                      double minimum, double maximum) {
  if (!laenge || laenge > 99)
    return FALSE;
  char puffer[100];
  memcpy(puffer, start, laenge);
  puffer[laenge] = 0;
  /* Older files use the current locale when writing colors and padding, so
   * accept both the canonical dot and a decimal comma. */
  char *komma = strchr(puffer, ',');
  if (komma) {
    if (strchr(komma + 1, ','))
      return FALSE;
    *komma = '.';
  }
  char *ende = NULL;
  errno = 0;
  double wert = g_ascii_strtod(puffer, &ende);
  return errno != ERANGE && ende != puffer && !*ende && isfinite(wert) &&
         wert >= minimum && wert <= maximum;
}

/* Header values ultimately become widget dimensions and coordinates.  Keep
 * them within a range in which the later integer layout arithmetic remains
 * safe, including for the maximum supported tree depth. */
static gboolean bdg_headerwert_pruefen(guint feld, const char *start,
                                       gsize laenge) {
  if (feld == 15)
    return laenge == 1;
  if (feld >= 19 && feld <= 38)
    return bdg_kommazahl_pruefen(start, laenge, 0., 1.);
  if (feld == 54)
    return bdg_kommazahl_pruefen(start, laenge, 0., 10000.);
  if (feld == 55)
    return bdg_kommazahl_pruefen(start, laenge, 0., 10000.);
  if (feld == 60)
    return bdg_ganzzahl_pruefen(start, laenge, -10000, 10000, NULL);
  if (feld == 16 || feld == 56 || feld == 61)
    return bdg_ganzzahl_pruefen(start, laenge, 0, 10000, NULL);
  if ((feld >= 39 && feld <= 42) || feld == 57 || feld == 58)
    return bdg_ganzzahl_pruefen(start, laenge, 0, 1, NULL);
  if (feld == 53)
    return TRUE; /* UTF-8 is checked separately below. */
  return bdg_ganzzahl_pruefen(start, laenge, 0, 10000, NULL);
}

static gboolean bdg_knotenname_pruefen(const char *start, gsize laenge,
                                       guint art) {
  if (art == 2) /* Ergebnisname endet auf -E. */
  {
    if (laenge < 3 || memcmp(start + laenge - 2, "-E", 2))
      return FALSE;
    laenge -= 2;
  } else if (art == 3) /* Wahrscheinlichkeit endet auf W. */
  {
    if (laenge < 2 || start[laenge - 1] != 'W')
      return FALSE;
    laenge--;
  } else if (art == 4) /* Ergebniswahrscheinlichkeit endet auf -EW. */
  {
    if (laenge < 4 || memcmp(start + laenge - 3, "-EW", 3))
      return FALSE;
    laenge -= 3;
  }
  if (laenge < 2 || start[0] != '-')
    return FALSE;
  for (gsize i = 1; i < laenge;) {
    if (!g_ascii_isdigit(start[i]))
      return FALSE;
    while (i < laenge && g_ascii_isdigit(start[i]))
      i++;
    if (i < laenge) {
      if (start[i++] != '-')
        return FALSE;
      if (i == laenge)
        return FALSE;
    }
  }
  return TRUE;
}

static gboolean bdg_zeile_pruefen(const char *daten, gsize laenge, gsize *pos,
                                  guint felder, gboolean header, guint art,
                                  guint datensatz, char **name_ausgabe,
                                  const char **grund) {
  for (guint feld = 0; feld < felder; feld++) {
    gsize start = *pos;
    while (*pos < laenge && (unsigned char)daten[*pos] != 31 &&
           daten[*pos] != '\n' && (unsigned char)daten[*pos] != 30)
      (*pos)++;
    if (*pos >= laenge || (unsigned char)daten[*pos] != 31) {
      *grund = "Ein Feld besitzt kein gültiges Trennzeichen.";
      return FALSE;
    }
    gsize maximum;
    if (header)
      maximum = bdg_header_feldgrenze(feld);
    else if (feld == 0 || (felder == 4 && feld == 1))
      maximum = 21;
    else
      maximum = (feld == (felder == 4 ? 2u : 1u)) ? MAX_KNOTENNAME_BYTES
                                                  : MAX_EINGABE_BYTES;
    if (*pos - start > maximum) {
      *grund = "Ein Feld überschreitet die zulässige Länge.";
      return FALSE;
    }
    gsize feldlaenge = *pos - start;
    if (header && !bdg_headerwert_pruefen(feld, daten + start, feldlaenge)) {
      *grund = "Eine Darstellungseinstellung liegt außerhalb des zulässigen Bereichs.";
      return FALSE;
    }
    if (header && feld >= 62 &&
        !bdg_ganzzahl_pruefen(daten + start, feldlaenge, 0,
                              feld == 63 ? 2 : 1, NULL)) {
      *grund = "Ungültige Einstellung für die Wahrscheinlichkeitsdarstellung.";
      return FALSE;
    }
    if (memchr(daten + start, '\0', feldlaenge)) {
      *grund = "Ein Feld enthält ein unzulässiges Nullbyte.";
      return FALSE;
    }
    if (!header && feld == 0) {
      gint64 index;
      if (!bdg_ganzzahl_pruefen(daten + start, feldlaenge, 0, MAX_KNOTEN - 1,
                                &index) ||
          index != (gint64)datensatz) {
        *grund = "Die Knotenindizes sind ungültig oder nicht fortlaufend.";
        return FALSE;
      }
    }
    if (!header && felder == 4 && feld == 1 &&
        !bdg_ganzzahl_pruefen(daten + start, feldlaenge, G_MININT, G_MAXINT,
                              NULL)) {
      *grund = "Eine Knotenposition ist keine gültige Ganzzahl.";
      return FALSE;
    }
    guint namensfeld = felder == 4 ? 2 : 1;
    gboolean ist_text = !header && (feld == namensfeld || feld == felder - 1);
    if ((ist_text || (header && feld == 53)) &&
        !g_utf8_validate(daten + start, (gssize)feldlaenge, NULL)) {
      *grund = "Ein Textfeld enthält ungültiges UTF-8.";
      return FALSE;
    }
    if (!header && feld == namensfeld &&
        !bdg_knotenname_pruefen(daten + start, feldlaenge, art)) {
      *grund = "Ein Knotenname besitzt ein ungültiges Format.";
      return FALSE;
    }
    if (!header && feld == namensfeld && name_ausgabe)
      *name_ausgabe = g_strndup(daten + start, feldlaenge);
    (*pos)++;
  }
  if (*pos >= laenge || daten[*pos] != '\n') {
    *grund = "Ein Datensatz besitzt zu viele oder zu wenige Felder.";
    return FALSE;
  }
  (*pos)++;
  return TRUE;
}

static gboolean bdg_abschnitt_pruefen(const char *daten, gsize laenge,
                                      gsize *pos, guint felder,
                                      gboolean letzter, gboolean header,
                                      guint art, const char **grund) {
  guint datensaetze = 0;
  GHashTable *knotennamen =
      art == 1 ? g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL)
               : NULL;
  while (*pos < laenge && (unsigned char)daten[*pos] != 30) {
    char *name = NULL;
    if (!bdg_zeile_pruefen(daten, laenge, pos, felder, header, art, datensaetze,
                           &name, grund)) {
      if (knotennamen)
        g_hash_table_destroy(knotennamen);
      return FALSE;
    }
    if (knotennamen) {
      if (datensaetze == 0 && strcmp(name, "-0")) {
        *grund = "Der erste Knoten muss den Namen -0 besitzen.";
        g_free(name);
        g_hash_table_destroy(knotennamen);
        return FALSE;
      }
      char *letzter_trenner = strrchr(name, '-');
      if (letzter_trenner != name) {
        char *vorgaengername = g_strndup(name, (gsize)(letzter_trenner - name));
        gboolean vorhanden = g_hash_table_contains(knotennamen, vorgaengername);
        g_free(vorgaengername);
        if (!vorhanden) {
          *grund = "Ein Knoten verweist auf einen fehlenden Vorgänger.";
          g_free(name);
          g_hash_table_destroy(knotennamen);
          return FALSE;
        }
      }
      if (g_hash_table_contains(knotennamen, name)) {
        *grund = "Ein Knotenname kommt mehrfach vor.";
        g_free(name);
        g_hash_table_destroy(knotennamen);
        return FALSE;
      }
      g_hash_table_add(knotennamen, name);
    } else
      g_free(name);
    if (++datensaetze > (header ? 1u : (guint)MAX_KNOTEN)) {
      *grund = "Ein Abschnitt enthält zu viele Datensätze.";
      if (knotennamen)
        g_hash_table_destroy(knotennamen);
      return FALSE;
    }
    if (letzter && *pos == laenge) {
      if (knotennamen)
        g_hash_table_destroy(knotennamen);
      return TRUE;
    }
  }
  if (knotennamen)
    g_hash_table_destroy(knotennamen);
  if (letzter) {
    if (*pos != laenge) {
      *grund = "Unerwarteter Abschnittstrenner am Dateiende.";
      return FALSE;
    }
    return datensaetze > 0;
  }
  if (*pos >= laenge || (unsigned char)daten[*pos] != 30) {
    *grund = "Ein benötigter Dateiabschnitt fehlt.";
    return FALSE;
  }
  (*pos)++;
  if (*pos >= laenge || daten[*pos] != '\n') {
    *grund = "Ungültiger Abschnittstrenner.";
    return FALSE;
  }
  (*pos)++;
  return datensaetze > 0;
}

static gboolean bdg_struktur_pruefen(const char *daten, gsize laenge,
                                     const char **grund) {
  gsize pos = 0;
  guint kopffelder = 0;
  for (gsize i = 0; i < laenge && daten[i] != '\n'; i++)
    if ((unsigned char)daten[i] == 31) kopffelder++;
  if (kopffelder != 62 && kopffelder != 63 && kopffelder != 66) {
    *grund = "Unbekannte Anzahl von Darstellungseinstellungen.";
    return FALSE;
  }
  return bdg_abschnitt_pruefen(daten, laenge, &pos, kopffelder, FALSE, TRUE, 0,
                               grund) &&
         bdg_abschnitt_pruefen(daten, laenge, &pos, 4, FALSE, FALSE, 1,
                               grund) &&
         bdg_abschnitt_pruefen(daten, laenge, &pos, 4, FALSE, FALSE, 2,
                               grund) &&
         bdg_abschnitt_pruefen(daten, laenge, &pos, 3, FALSE, FALSE, 3,
                               grund) &&
         bdg_abschnitt_pruefen(daten, laenge, &pos, 3, TRUE, FALSE, 4, grund) &&
         pos == laenge;
}

void laden(gpointer data, char *dateiname) {
  if (dateiname && strlen(dateiname) >= sizeof(aktuelledatei)) {
    dateifehler("Öffnen", dateiname, "Der Dateipfad ist zu lang.");
    return;
  }
  GStatBuf status;
  if (!dateiname || g_stat(dateiname, &status) != 0 || status.st_size < 0) {
    dateifehler("Öffnen", dateiname, "Die Datei konnte nicht gelesen werden.");
    return;
  }
  if ((guint64)status.st_size > MAX_BDG_DATEIGROESSE) {
    dateifehler("Öffnen", dateiname, "Die Datei ist größer als 1 GiB.");
    return;
  }
  char *dateiinhalt = NULL;
  gsize gelesen = 0;
  g_autoptr(GError) error = NULL;
  if (!dateiname ||
      !g_file_get_contents(dateiname, &dateiinhalt, &gelesen, &error)) {
    dateifehler("Öffnen", dateiname,
                error ? error->message : "Kein Dateipfad ausgewählt.");
    return;
  }
  if ((guint64)gelesen > MAX_BDG_DATEIGROESSE) {
    dateifehler("Öffnen", dateiname, "Die Datei ist größer als 1 GiB.");
    g_free(dateiinhalt);
    return;
  }
  /* Accept older Windows files using CRLF as well as the canonical LF format.
   */
  gsize ziel = 0;
  for (gsize quelle = 0; quelle < gelesen; quelle++)
    if (!(dateiinhalt[quelle] == '\r' && quelle + 1 < gelesen &&
          dateiinhalt[quelle + 1] == '\n'))
      dateiinhalt[ziel++] = dateiinhalt[quelle];
  dateiinhalt[ziel] = 0;
  const char *pruefgrund = NULL;
  if (!ziel || !bdg_struktur_pruefen(dateiinhalt, ziel, &pruefgrund)) {
    dateifehler("Öffnen", dateiname,
                pruefgrund ? pruefgrund : "Keine lesbare Arboretum-Datei.");
    g_free(dateiinhalt);
    return;
  }
  if (labelein == 1) umwandeln(NULL, data);
  int i = 0;
  zaehler = 0;
  Stufe = 0;
  maxStufe = 0;
  for (i = 0; i <= maxzaehler; i++) {
    gtk_widget_destroy(textfeld[i]);
    textfeld[i] = NULL;
    vorgaenger[i] = NULL;
    gtk_widget_destroy(textfeldWahrscheinlichkeit[i]);
    textfeldWahrscheinlichkeit[i] = NULL;
  }
  zaehlererg = 0;
  for (i = 0; i <= maxzaehlererg; i++) {
    gtk_widget_destroy(textfeldErgebnis[i]);
    textfeldErgebnis[i] = NULL;
    gtk_widget_destroy(textfeldErgebnisWahrscheinlichkeit[i]);
    textfeldErgebnisWahrscheinlichkeit[i] = NULL;
  }
  maxzaehler = 0;
  maxzaehlererg = 0;

  gtk_widget_set_size_request(
      da, RandLinks + RandRechts + StufenBreite + KnotenBreite,
      RandOben + KnotenHoehe + RandUnten);
  gtk_layout_set_size(GTK_LAYOUT(data),
                      RandLinks + RandRechts + StufenBreite + KnotenBreite,
                      RandOben + KnotenHoehe + RandUnten);

  int j = 0;
  while (dateiinhalt[j] != 30) {
    int l = 0;
    char randlinksstring[22] = "", randrechtsstring[22] = "",
         randobenstring[22] = "", randuntenstring[22] = "",
         fensterrandlinksstring[22] = "", fensterrandrechtsstring[22] = "",
         fensterrandobenstring[22] = "", fensterranduntenstring[22] = "",
         stufenbreitestring[22] = "", knotenabstandstring[22] = "",
         knotenhoehestring[22] = "", knotenbreitestring[22] = "",
         ergebnisabstandstring[22] = "", ergebnisbreitestring[22] = "",
         ergebnistextbreitestring[22] = "", liniendickestring[22] = "",
         knotentextbreitestring[22] = "",
         wahrscheinlichkeittextbreitestring[22] = "",
         hintergrundfarberedstring[100] = "",
         hintergrundfarbegreenstring[100] = "",
         hintergrundfarbebluestring[100] = "",
         hintergrundfarbealphastring[100] = "", zweigfarberedstring[100] = "",
         zweigfarbegreenstring[100] = "", zweigfarbebluestring[100] = "",
         zweigfarbealphastring[100] = "",
         knotenhintergrundfarberedstring[100] = "",
         knotenhintergrundfarbegreenstring[100] = "",
         knotenhintergrundfarbebluestring[100] = "",
         knotenhintergrundfarbealphastring[100] = "",
         knotenrandfarberedstring[100] = "",
         knotenrandfarbegreenstring[100] = "",
         knotenrandfarbebluestring[100] = "",
         knotenrandfarbealphastring[100] = "", schriftfarberedstring[100] = "",
         schriftfarbegreenstring[100] = "", schriftfarbebluestring[100] = "",
         schriftfarbealphastring[100] = "", ergebnisseanzeigenstring[22] = "",
         wskanzeigenstring[22] = "", ergebnissewskanzeigenstring[22] = "",
         labeleinstring[22] = "", KnotenLabelBreitestring[22] = "",
         klbmaxstring[22] = "", KnotenLabelHoehestring[22] = "",
         ErgebnisLabelBreitestring[22] = "",
         WahrscheinlichkeitErgebnisTextBreitestring[22] = "",
         WahrscheinlichkeitErgebnisLabelBreitestring[22] = "",
         ZaehlerErgebnisLabelBreitestring[22] = "",
         NennerErgebnisLabelBreitestring[22] = "",
         ZaehlerErgebnisLabelHoehestring[22] = "",
         NennerErgebnisLabelHoehestring[22] = "", schriftartstring[1000] = "",
         paddingstring[100] = "", paddingkstring[100] = "",
         genauigkeitstring[22] = "", kuerzenstring[22] = "",
         bruchoustring[22] = "", knotenrahmenabstandstring[22] = "",
         wskverschiebungstring[22] = "", knotenrahmendickestring[22] = "",
         letztewskautomatischstring[22] = "", seitens[22] = "",
         mittens[22] = "", automatiks[22] = "";
    while (dateiinhalt[j] != 10) {
      int k = 0;
      while (dateiinhalt[j] != 31) {
        if (l == 0) {
          randlinksstring[k] = dateiinhalt[j];
        }
        if (l == 1) {
          randrechtsstring[k] = dateiinhalt[j];
        }
        if (l == 2) {
          randobenstring[k] = dateiinhalt[j];
        }
        if (l == 3) {
          randuntenstring[k] = dateiinhalt[j];
        }
        if (l == 4) {
          fensterrandlinksstring[k] = dateiinhalt[j];
        }
        if (l == 5) {
          fensterrandrechtsstring[k] = dateiinhalt[j];
        }
        if (l == 6) {
          fensterrandobenstring[k] = dateiinhalt[j];
        }
        if (l == 7) {
          fensterranduntenstring[k] = dateiinhalt[j];
        }
        if (l == 8) {
          stufenbreitestring[k] = dateiinhalt[j];
        }
        if (l == 9) {
          knotenabstandstring[k] = dateiinhalt[j];
        }
        if (l == 10) {
          knotenhoehestring[k] = dateiinhalt[j];
        }
        if (l == 11) {
          knotenbreitestring[k] = dateiinhalt[j];
        }
        if (l == 12) {
          ergebnisabstandstring[k] = dateiinhalt[j];
        }
        if (l == 13) {
          ergebnisbreitestring[k] = dateiinhalt[j];
        }
        if (l == 14) {
          ergebnistextbreitestring[k] = dateiinhalt[j];
        }
        if (l == 15) {
          ErgebnisTrenner = dateiinhalt[j];
        }
        if (l == 16) {
          liniendickestring[k] = dateiinhalt[j];
        }
        if (l == 17) {
          knotentextbreitestring[k] = dateiinhalt[j];
        }
        if (l == 18) {
          wahrscheinlichkeittextbreitestring[k] = dateiinhalt[j];
        }
        if (l == 19) {
          hintergrundfarberedstring[k] = dateiinhalt[j];
        }
        if (l == 20) {
          hintergrundfarbegreenstring[k] = dateiinhalt[j];
        }
        if (l == 21) {
          hintergrundfarbebluestring[k] = dateiinhalt[j];
        }
        if (l == 22) {
          hintergrundfarbealphastring[k] = dateiinhalt[j];
        }
        if (l == 23) {
          zweigfarberedstring[k] = dateiinhalt[j];
        }
        if (l == 24) {
          zweigfarbegreenstring[k] = dateiinhalt[j];
        }
        if (l == 25) {
          zweigfarbebluestring[k] = dateiinhalt[j];
        }
        if (l == 26) {
          zweigfarbealphastring[k] = dateiinhalt[j];
        }
        if (l == 27) {
          knotenhintergrundfarberedstring[k] = dateiinhalt[j];
        }
        if (l == 28) {
          knotenhintergrundfarbegreenstring[k] = dateiinhalt[j];
        }
        if (l == 29) {
          knotenhintergrundfarbebluestring[k] = dateiinhalt[j];
        }
        if (l == 30) {
          knotenhintergrundfarbealphastring[k] = dateiinhalt[j];
        }
        if (l == 31) {
          knotenrandfarberedstring[k] = dateiinhalt[j];
        }
        if (l == 32) {
          knotenrandfarbegreenstring[k] = dateiinhalt[j];
        }
        if (l == 33) {
          knotenrandfarbebluestring[k] = dateiinhalt[j];
        }
        if (l == 34) {
          knotenrandfarbealphastring[k] = dateiinhalt[j];
        }
        if (l == 35) {
          schriftfarberedstring[k] = dateiinhalt[j];
        }
        if (l == 36) {
          schriftfarbegreenstring[k] = dateiinhalt[j];
        }
        if (l == 37) {
          schriftfarbebluestring[k] = dateiinhalt[j];
        }
        if (l == 38) {
          schriftfarbealphastring[k] = dateiinhalt[j];
        }
        if (l == 39) {
          ergebnisseanzeigenstring[k] = dateiinhalt[j];
        }
        if (l == 40) {
          wskanzeigenstring[k] = dateiinhalt[j];
        }
        if (l == 41) {
          ergebnissewskanzeigenstring[k] = dateiinhalt[j];
        }
        if (l == 42) {
          labeleinstring[k] = dateiinhalt[j];
        }
        if (l == 43) {
          KnotenLabelBreitestring[k] = dateiinhalt[j];
        }
        if (l == 44) {
          klbmaxstring[k] = dateiinhalt[j];
        }
        if (l == 45) {
          KnotenLabelHoehestring[k] = dateiinhalt[j];
        }
        if (l == 46) {
          ErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if (l == 47) {
          WahrscheinlichkeitErgebnisTextBreitestring[k] = dateiinhalt[j];
        }
        if (l == 48) {
          WahrscheinlichkeitErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if (l == 49) {
          ZaehlerErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if (l == 50) {
          NennerErgebnisLabelBreitestring[k] = dateiinhalt[j];
        }
        if (l == 51) {
          ZaehlerErgebnisLabelHoehestring[k] = dateiinhalt[j];
        }
        if (l == 52) {
          NennerErgebnisLabelHoehestring[k] = dateiinhalt[j];
        }
        if (l == 53) {
          schriftartstring[k] = dateiinhalt[j];
        }
        if (l == 54) {
          paddingstring[k] = dateiinhalt[j];
        }
        if (l == 55) {
          paddingkstring[k] = dateiinhalt[j];
        }
        if (l == 56) {
          genauigkeitstring[k] = dateiinhalt[j];
        }
        if (l == 57) {
          kuerzenstring[k] = dateiinhalt[j];
        }
        if (l == 58) {
          bruchoustring[k] = dateiinhalt[j];
        }
        if (l == 59) {
          knotenrahmenabstandstring[k] = dateiinhalt[j];
        }
        if (l == 60) {
          wskverschiebungstring[k] = dateiinhalt[j];
        }
        if (l == 61) {
          knotenrahmendickestring[k] = dateiinhalt[j];
        }
        if (l == 62) {
          letztewskautomatischstring[k] = dateiinhalt[j];
        }
        if (l == 63) seitens[k] = dateiinhalt[j];
        if (l == 64) mittens[k] = dateiinhalt[j];
        if (l == 65) automatiks[k] = dateiinhalt[j];
        k++;
        j++;
      }
      l++;
      j++;
    }
    RandLinks = atoi(randlinksstring);
    RandRechts = atoi(randrechtsstring);
    RandOben = atoi(randobenstring);
    RandUnten = atoi(randuntenstring);
    FensterRandLinks = atoi(fensterrandlinksstring);
    FensterRandRechts = atoi(fensterrandrechtsstring);
    FensterRandOben = atoi(fensterrandobenstring);
    FensterRandUnten = atoi(fensterranduntenstring);
    StufenBreite = atoi(stufenbreitestring);
    KnotenAbstand = atoi(knotenabstandstring);
    KnotenHoehe = atoi(knotenhoehestring);
    KnotenBreite = atoi(knotenbreitestring);
    ErgebnisAbstand = atoi(ergebnisabstandstring);
    ErgebnisBreite = atoi(ergebnisbreitestring);
    ErgebnisTextBreite = atoi(ergebnistextbreitestring);
    LinienDicke = atoi(liniendickestring);
    KnotenTextBreite = atoi(knotentextbreitestring);
    WahrscheinlichkeitTextBreite = atoi(wahrscheinlichkeittextbreitestring);
    hintergrundfarbe.red = atof(hintergrundfarberedstring);
    hintergrundfarbe.green = atof(hintergrundfarbegreenstring);
    hintergrundfarbe.blue = atof(hintergrundfarbebluestring);
    hintergrundfarbe.alpha = atof(hintergrundfarbealphastring);
    zweigfarbe.red = atof(zweigfarberedstring);
    zweigfarbe.green = atof(zweigfarbegreenstring);
    zweigfarbe.blue = atof(zweigfarbebluestring);
    zweigfarbe.alpha = atof(zweigfarbealphastring);
    knotenhintergrundfarbe.red = atof(knotenhintergrundfarberedstring);
    knotenhintergrundfarbe.green = atof(knotenhintergrundfarbegreenstring);
    knotenhintergrundfarbe.blue = atof(knotenhintergrundfarbebluestring);
    knotenhintergrundfarbe.alpha = atof(knotenhintergrundfarbealphastring);
    knotenrandfarbe.red = atof(knotenrandfarberedstring);
    knotenrandfarbe.green = atof(knotenrandfarbegreenstring);
    knotenrandfarbe.blue = atof(knotenrandfarbebluestring);
    knotenrandfarbe.alpha = atof(knotenrandfarbealphastring);
    schriftfarbe.red = atof(schriftfarberedstring);
    schriftfarbe.green = atof(schriftfarbegreenstring);
    schriftfarbe.blue = atof(schriftfarbebluestring);
    schriftfarbe.alpha = atof(schriftfarbealphastring);
    ergebnisseanzeigen = atoi(ergebnisseanzeigenstring);
    wskanzeigen = atoi(wskanzeigenstring);
    ergebnissewskanzeigen = atoi(ergebnissewskanzeigenstring);
    labelein = atoi(labeleinstring);
    KnotenLabelBreite = atoi(KnotenLabelBreitestring);
    klbmax = atoi(klbmaxstring);
    KnotenLabelHoehe = atoi(KnotenLabelHoehestring);
    ErgebnisLabelBreite = atoi(ErgebnisLabelBreitestring);
    WahrscheinlichkeitErgebnisTextBreite =
        atoi(WahrscheinlichkeitErgebnisTextBreitestring);
    WahrscheinlichkeitErgebnisLabelBreite =
        atoi(WahrscheinlichkeitErgebnisLabelBreitestring);
    ZaehlerErgebnisLabelBreite = atoi(ZaehlerErgebnisLabelBreitestring);
    NennerErgebnisLabelBreite = atoi(NennerErgebnisLabelBreitestring);
    ZaehlerErgebnisLabelHoehe = atoi(ZaehlerErgebnisLabelHoehestring);
    NennerErgebnisLabelHoehe = atoi(NennerErgebnisLabelHoehestring);
    strcpy(schriftart, schriftartstring);
    padding = atof(paddingstring);
    paddingk = atof(paddingkstring);
    genauigkeit = atoi(genauigkeitstring);
    kuerzen = atoi(kuerzenstring);
    bruchou = atoi(bruchoustring);
    knotenrahmenabstand = atoi(knotenrahmenabstandstring);
    wskverschiebung = atoi(wskverschiebungstring);
    knotenrahmendicke = atoi(knotenrahmendickestring);
    if (letztewskautomatischstring[0])
      letzte_wahrscheinlichkeit_automatisch = atoi(letztewskautomatischstring);
    wskseite = seitens[0] ? atoi(seitens) : 2;
    wskmitteunten = mittens[0] ? atoi(mittens) : FALSE;
    wskautomatik = automatiks[0] ? atoi(automatiks) : FALSE;
    j++;
  }

  ymax = 0;
  j += 2;
  while (dateiinhalt[j] != 30) {
    int l = 0, tempzaehler = 0;
    char tempzaehlerstring[22] = "", ystring[22] = "", text[10000] = "";
    g_autofree char *name = g_malloc0(MAX_KNOTENNAME_BYTES + 1);
    while (dateiinhalt[j] != 10) {
      int k = 0;
      while (dateiinhalt[j] != 31) {
        if (l == 0) {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if (l == 1) {
          ystring[k] = dateiinhalt[j];
        }
        if (l == 2) {
          name[k] = dateiinhalt[j];
        }
        if (l == 3) {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    Stufe = zeichenzaehlen(name, '-') - 1;
    maxStufe = ((Stufe > maxStufe) ? Stufe : maxStufe);
    tempzaehler = atoi(tempzaehlerstring);
    if (!knotenindex_gueltig(tempzaehler)) {
      free(dateiinhalt);
      return;
    }
    y[tempzaehler] = atoi(ystring);
    ymax = ((ymax < y[tempzaehler]) ? y[tempzaehler] : ymax);
    textfeld[tempzaehler] = gtk_entry_new();
    eingabefeld_absichern(textfeld[tempzaehler]);
    if (tempzaehler == 0 || zeichenzaehlen(name, '-') == 1) {
      vorgaenger[tempzaehler] = &textfeld[0];
    } else {
      g_autofree char *tempname = g_strndup(name, strrchr(name, '-') - name);
      vorgaenger[tempzaehler] = &textfeld[knotenexistiert(tempname)];
    }
    gtk_widget_set_name(textfeld[tempzaehler], name);
    gtk_entry_set_width_chars(GTK_ENTRY(textfeld[tempzaehler]),
                              KnotenTextBreite);
    gtk_entry_set_alignment(GTK_ENTRY(textfeld[tempzaehler]), 0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeld[tempzaehler]), text);
    gtk_layout_put(GTK_LAYOUT(data), textfeld[tempzaehler],
                   FensterRandLinks + RandLinks + StufenBreite +
                       (StufenBreite + KnotenBreite) * Stufe,
                   FensterRandOben + RandOben + y[tempzaehler]);
    gtk_widget_show_all(textfeld[tempzaehler]);
    g_signal_connect(textfeld[tempzaehler], "changed",
                     G_CALLBACK(buchstabeneingabe), data);
    zaehler = tempzaehler;
    maxzaehler = tempzaehler;
    j++;
  }

  j += 2;
  while (dateiinhalt[j] != 30) {
    int l = 0, tempzaehler = 0;
    char tempzaehlerstring[22] = "", ystring[22] = "", text[10000] = "";
    g_autofree char *name = g_malloc0(MAX_KNOTENNAME_BYTES + 1);
    while (dateiinhalt[j] != 10) {
      int k = 0;
      while (dateiinhalt[j] != 31) {
        if (l == 0) {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if (l == 1) {
          ystring[k] = dateiinhalt[j];
        }
        if (l == 2) {
          name[k] = dateiinhalt[j];
        }
        if (l == 3) {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    tempzaehler = atoi(tempzaehlerstring);
    if (!knotenindex_gueltig(tempzaehler)) {
      free(dateiinhalt);
      return;
    }
    yerg[tempzaehler] = atoi(ystring);
    textfeldErgebnis[tempzaehler] = gtk_entry_new();
    eingabefeld_absichern(textfeldErgebnis[tempzaehler]);
    gtk_widget_set_name(textfeldErgebnis[tempzaehler], name);
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[tempzaehler]),
                              3 * (maxStufe + 1));
    gtk_entry_set_alignment(GTK_ENTRY(textfeldErgebnis[tempzaehler]), 0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldErgebnis[tempzaehler]), text);
    gtk_layout_put(GTK_LAYOUT(data), textfeldErgebnis[tempzaehler],
                   FensterRandLinks + RandLinks +
                       (maxStufe + 1) * StufenBreite +
                       (maxStufe + 1) * KnotenBreite + ErgebnisAbstand,
                   FensterRandOben + RandOben + yerg[tempzaehler]);
    gtk_widget_show_all(textfeldErgebnis[tempzaehler]);
    zaehlererg = tempzaehler;
    maxzaehlererg = tempzaehler;
    j++;
  }

  j += 2;
  while (dateiinhalt[j] != 30) {
    int l = 0, tempzaehler = 0;
    char tempzaehlerstring[22] = "", text[10000] = "";
    g_autofree char *name = g_malloc0(MAX_KNOTENNAME_BYTES + 1);
    while (dateiinhalt[j] != 10) {
      int k = 0;
      while (dateiinhalt[j] != 31) {
        if (l == 0) {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if (l == 1) {
          name[k] = dateiinhalt[j];
        }
        if (l == 2) {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    tempzaehler = atoi(tempzaehlerstring);
    if (!knotenindex_gueltig(tempzaehler)) {
      free(dateiinhalt);
      return;
    }
    textfeldWahrscheinlichkeit[tempzaehler] = gtk_entry_new();
    eingabefeld_absichern(textfeldWahrscheinlichkeit[tempzaehler]);
    gtk_widget_set_name(textfeldWahrscheinlichkeit[tempzaehler], name);
    gtk_entry_set_width_chars(
        GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),
        WahrscheinlichkeitTextBreite);
    gtk_entry_set_alignment(GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),
                            0.5);
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[tempzaehler]),
                       text);
    gtk_layout_put(GTK_LAYOUT(data), textfeldWahrscheinlichkeit[tempzaehler], 0,
                   0);
    gtk_widget_show_all(textfeldWahrscheinlichkeit[tempzaehler]);
    g_signal_connect(textfeldWahrscheinlichkeit[tempzaehler], "changed",
                     G_CALLBACK(wskeingabe), data);
    j++;
  }

  j += 2;
  while (dateiinhalt[j] != 0) {
    int l = 0, tempzaehler = 0;
    char tempzaehlerstring[22] = "", text[10000] = "";
    g_autofree char *name = g_malloc0(MAX_KNOTENNAME_BYTES + 1);
    while (dateiinhalt[j] != 10) {
      int k = 0;
      while (dateiinhalt[j] != 31) {
        if (l == 0) {
          tempzaehlerstring[k] = dateiinhalt[j];
        }
        if (l == 1) {
          name[k] = dateiinhalt[j];
        }
        if (l == 2) {
          text[k] = dateiinhalt[j];
        }
        k++;
        j++;
      }
      l++;
      j++;
    }
    tempzaehler = atoi(tempzaehlerstring);
    if (!knotenindex_gueltig(tempzaehler)) {
      free(dateiinhalt);
      return;
    }
    textfeldErgebnisWahrscheinlichkeit[tempzaehler] = gtk_entry_new();
    eingabefeld_absichern(textfeldErgebnisWahrscheinlichkeit[tempzaehler]);
    gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[tempzaehler], name);
    gtk_entry_set_width_chars(
        GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]),
        WahrscheinlichkeitErgebnisTextBreite);
    gtk_entry_set_alignment(
        GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]), 0.5);
    gtk_entry_set_text(
        GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[tempzaehler]), text);
    gtk_layout_put(
        GTK_LAYOUT(data), textfeldErgebnisWahrscheinlichkeit[tempzaehler],
        FensterRandLinks + RandLinks + (maxStufe + 1) * StufenBreite +
            (maxStufe + 1) * KnotenBreite + ErgebnisAbstand * 2 +
            ErgebnisBreite,
        FensterRandOben + RandOben + yerg[tempzaehler]);
    gtk_widget_show_all(textfeldErgebnisWahrscheinlichkeit[tempzaehler]);
    j++;
  }

  bruch = 0;
  arboretum_refresh_entry_overlines();
  for (i = 0; i <= maxzaehler; i++) {
    if (strchr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),
               '/')) {
      bruch = 1;
    }
  }

  positionsanpassungwsk(data);
  wskergebnisverschieben(NULL, NULL, data);

  /* Newly shown fields can receive GTK's automatic initial focus. Clear its
   * selection and focus only once, after the entire document is restored. */
  for (i = 0; i <= maxzaehler; i++) {
    gtk_editable_set_position(GTK_EDITABLE(textfeld[i]), -1);
    gtk_editable_set_position(GTK_EDITABLE(textfeldWahrscheinlichkeit[i]), -1);
  }
  for (i = 0; i <= maxzaehlererg; i++) {
    gtk_editable_set_position(GTK_EDITABLE(textfeldErgebnis[i]), -1);
    gtk_editable_set_position(
        GTK_EDITABLE(textfeldErgebnisWahrscheinlichkeit[i]), -1);
  }
  if (labelein) {
    labelein = 0;
    umwandeln(NULL, data);
  } else
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeld[0]));

  gtk_widget_queue_draw(da);
  g_free(dateiinhalt);
  g_strlcpy(aktuelledatei, dateiname, sizeof(aktuelledatei));
}
