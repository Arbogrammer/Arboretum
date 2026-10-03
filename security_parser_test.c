#define main arboretum_application_main
#include "baumg.c"
#undef main

static void feld(GString *datei, const char *wert) {
  g_string_append(datei, wert);
  g_string_append_c(datei, 31);
}

static void abschnittsende(GString *datei) {
  g_string_append_c(datei, 30);
  g_string_append_c(datei, '\n');
}

static void header(GString *datei) {
  for (int i = 0; i < 62; i++)
    feld(datei, i == 53 ? "Sans 12" : "0");
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
}

static GString *minimaldatei(const char *index, const char *text) {
  GString *datei = g_string_new(NULL);
  header(datei);
  feld(datei, index);
  feld(datei, "0");
  feld(datei, "-0");
  feld(datei, text);
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "0");
  feld(datei, "-0-E");
  feld(datei, "Ergebnis");
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "-0W");
  feld(datei, "1");
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "-0-EW");
  feld(datei, "1");
  g_string_append_c(datei, '\n');
  return datei;
}

static gboolean gueltig(GString *datei) {
  const char *grund = NULL;
  return bdg_struktur_pruefen(datei->str, datei->len, &grund);
}

static void test_minimaldatei(void) {
  g_autoptr(GString) datei = minimaldatei("0", "Äpfel Ω");
  g_assert_true(gueltig(datei));
}

static void test_textgrenze(void) {
  g_autofree char *genau = g_strnfill(MAX_EINGABE_BYTES, 'A');
  g_autofree char *zuviel = g_strnfill(MAX_EINGABE_BYTES + 1, 'A');
  g_autoptr(GString) erlaubt = minimaldatei("0", genau);
  g_autoptr(GString) abgelehnt = minimaldatei("0", zuviel);
  g_assert_true(gueltig(erlaubt));
  g_assert_false(gueltig(abgelehnt));
}

static void test_trenner_und_bytes(void) {
  g_autoptr(GString) ohne_trenner = minimaldatei("0", "Text");
  char *erster = memchr(ohne_trenner->str, 31, ohne_trenner->len);
  *erster = 'X';
  g_assert_false(gueltig(ohne_trenner));

  g_autoptr(GString) nullbyte = minimaldatei("0", "Text");
  char *text = g_strstr_len(nullbyte->str, nullbyte->len, "Text");
  *text = '\0';
  g_assert_false(gueltig(nullbyte));

  const char kaputt[] = {'A', (char)0xc3, '(', 0};
  g_autoptr(GString) utf8 = minimaldatei("0", kaputt);
  g_assert_false(gueltig(utf8));
}

static void test_indizes(void) {
  const char *ungueltig[] = {"-1", "1", "10000", "x"};
  for (guint i = 0; i < G_N_ELEMENTS(ungueltig); i++) {
    g_autoptr(GString) datei = minimaldatei(ungueltig[i], "Text");
    g_assert_false(gueltig(datei));
  }
}

static GString *datei_mit_knoten(guint anzahl) {
  GString *datei = g_string_new(NULL);
  header(datei);
  for (guint i = 0; i < anzahl; i++) {
    g_autofree char *zahl = g_strdup_printf("%u", i);
    g_autofree char *name = g_strdup_printf("-%u", i);
    feld(datei, zahl);
    feld(datei, "0");
    feld(datei, name);
    feld(datei, "T");
    g_string_append_c(datei, '\n');
  }
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "0");
  feld(datei, "-0-E");
  feld(datei, "E");
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "-0W");
  feld(datei, "1");
  g_string_append_c(datei, '\n');
  abschnittsende(datei);
  feld(datei, "0");
  feld(datei, "-0-EW");
  feld(datei, "1");
  g_string_append_c(datei, '\n');
  return datei;
}

static void test_knotenlimit(void) {
  g_autoptr(GString) maximum = datei_mit_knoten(MAX_KNOTEN);
  g_autoptr(GString) zuviele = datei_mit_knoten(MAX_KNOTEN + 1);
  g_assert_true(gueltig(maximum));
  g_assert_false(gueltig(zuviele));
}

static void test_fehlender_vorgaenger(void) {
  g_autoptr(GString) datei = minimaldatei("0", "Text");
  char *erste_grenze = memchr(datei->str, 30, datei->len);
  char *zweite_grenze =
      memchr(erste_grenze + 1, 30,
             datei->len - (gsize)(erste_grenze + 1 - datei->str));
  g_autoptr(GString) zeile = g_string_new(NULL);
  feld(zeile, "1");
  feld(zeile, "0");
  feld(zeile, "-9-0");
  feld(zeile, "T");
  g_string_append_c(zeile, '\n');
  g_string_insert_len(datei, zweite_grenze - datei->str, zeile->str,
                      zeile->len);
  g_assert_false(gueltig(datei));
}

static void test_testdatenordner(void) {
  g_autoptr(GError) error = NULL;
  g_autoptr(GDir) ordner = g_dir_open("testdaten/bdg", 0, &error);
  g_assert_no_error(error);
  g_assert_nonnull(ordner);
  const char *name;
  guint anzahl = 0;
  while ((name = g_dir_read_name(ordner))) {
    gboolean erwartet;
    if (g_str_has_prefix(name, "gueltig_"))
      erwartet = TRUE;
    else if (g_str_has_prefix(name, "ungueltig_"))
      erwartet = FALSE;
    else
      continue;
    g_autofree char *pfad = g_build_filename("testdaten/bdg", name, NULL);
    g_autofree char *daten = NULL;
    gsize laenge = 0;
    g_assert_true(g_file_get_contents(pfad, &daten, &laenge, &error));
    g_assert_no_error(error);
    const char *grund = NULL;
    gboolean ist_gueltig = bdg_struktur_pruefen(daten, laenge, &grund);
    g_assert_cmpint(ist_gueltig, ==, erwartet);
    anzahl++;
  }
  g_assert_cmpuint(anzahl, ==, 18);
}

int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/bdg/minimal", test_minimaldatei);
  g_test_add_func("/bdg/textgrenze", test_textgrenze);
  g_test_add_func("/bdg/trenner-und-bytes", test_trenner_und_bytes);
  g_test_add_func("/bdg/indizes", test_indizes);
  g_test_add_func("/bdg/knotenlimit", test_knotenlimit);
  g_test_add_func("/bdg/fehlender-vorgaenger", test_fehlender_vorgaenger);
  g_test_add_func("/bdg/testdatenordner", test_testdatenordner);
  return g_test_run();
}
