#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>

#define US ((char)31)
#define RS ((char)30)

static void feld(GString *s, const char *wert) {
  g_string_append(s, wert);
  g_string_append_c(s, US);
}

static void zeile4(GString *s, const char *index, const char *position,
                   const char *name, const char *text) {
  feld(s, index);
  feld(s, position);
  feld(s, name);
  feld(s, text);
  g_string_append_c(s, '\n');
}

static void zeile3(GString *s, const char *index, const char *name,
                   const char *text) {
  feld(s, index);
  feld(s, name);
  feld(s, text);
  g_string_append_c(s, '\n');
}

static void abschnitt(GString *s) {
  g_string_append_c(s, RS);
  g_string_append_c(s, '\n');
}

static GString *anfang(void) {
  GString *s = g_string_new(NULL);
  for (guint i = 0; i < 62; i++)
    feld(s, i == 53 ? "Sans 12" : (i == 15 ? "|" : "0"));
  g_string_append_c(s, '\n');
  abschnitt(s);
  return s;
}

static GString *minimal(const char *knotenname, const char *knotentext) {
  GString *s = anfang();
  zeile4(s, "0", "0", knotenname, knotentext);
  abschnitt(s);
  zeile4(s, "0", "0", "-0-E", "Ergebnis");
  abschnitt(s);
  zeile3(s, "0", "-0W", "1");
  abschnitt(s);
  zeile3(s, "0", "-0-EW", "1");
  return s;
}

static gboolean schreiben(const char *ordner, const char *name, GString *s) {
  g_autofree char *pfad = g_build_filename(ordner, name, NULL);
  g_autoptr(GError) fehler = NULL;
  gboolean ok = g_file_set_contents(pfad, s->str, (gssize)s->len, &fehler);
  if (!ok)
    g_printerr("%s: %s\n", pfad, fehler->message);
  g_string_free(s, TRUE);
  return ok;
}

static GString *kette(guint tiefe) {
  GString *s = anfang();
  GString *name = g_string_new(NULL);
  for (guint i = 0; i < tiefe; i++) {
    g_string_append(name, "-0");
    g_autofree char *index = g_strdup_printf("%u", i);
    g_autofree char *position = g_strdup_printf("%u", i);
    zeile4(s, index, position, name->str, "Knoten");
  }
  abschnitt(s);
  g_autofree char *ename = g_strdup_printf("%s-E", name->str);
  zeile4(s, "0", "0", ename, "Ende");
  abschnitt(s);
  g_string_truncate(name, 0);
  for (guint i = 0; i < tiefe; i++) {
    g_string_append(name, "-0");
    g_autofree char *index = g_strdup_printf("%u", i);
    g_autofree char *wname = g_strdup_printf("%sW", name->str);
    zeile3(s, index, wname, "1");
  }
  abschnitt(s);
  g_autofree char *ewname = g_strdup_printf("%s-EW", name->str);
  zeile3(s, "0", ewname, "1");
  g_string_free(name, TRUE);
  return s;
}

static GString *verzweigt(void) {
  GString *s = anfang();
  zeile4(s, "0", "0", "-0", "Wurzel");
  zeile4(s, "1", "1", "-0-0", "Links");
  zeile4(s, "2", "1", "-0-1", "Rechts");
  zeile4(s, "3", "2", "-0-0-0", "Tief");
  abschnitt(s);
  zeile4(s, "0", "0", "-0-0-0-E", "A");
  zeile4(s, "1", "0", "-0-1-E", "B");
  abschnitt(s);
  zeile3(s, "0", "-0W", "1");
  zeile3(s, "1", "-0-0W", "0.5");
  zeile3(s, "2", "-0-1W", "0.5");
  zeile3(s, "3", "-0-0-0W", "1");
  abschnitt(s);
  zeile3(s, "0", "-0-0-0-EW", "0.25");
  zeile3(s, "1", "-0-1-EW", "0.75");
  return s;
}

int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  if (g_mkdir_with_parents(argv[1], 0755) != 0) {
    g_printerr("%s: %s\n", argv[1], g_strerror(errno));
    return 1;
  }
  gboolean ok = TRUE;
  ok &= schreiben(argv[1], "gueltig_01_minimal.bdg", minimal("-0", "Text"));
  ok &= schreiben(argv[1], "gueltig_02_unicode.bdg",
                  minimal("-0", "Äpfel, Ω und 🌳"));
  ok &= schreiben(argv[1], "gueltig_03_verzweigt.bdg", verzweigt());
  ok &= schreiben(argv[1], "gueltig_04_tiefe_100.bdg", kette(100));

  g_autofree char *maxtext = g_strnfill(9999, 'A');
  ok &= schreiben(argv[1], "gueltig_05_text_9999_bytes.bdg",
                  minimal("-0", maxtext));

  GString *positionen = anfang();
  zeile4(positionen, "0", "-2147483648", "-0", "Minimum");
  zeile4(positionen, "1", "2147483647", "-0-0", "Maximum");
  abschnitt(positionen);
  zeile4(positionen, "0", "0", "-0-0-E", "E");
  abschnitt(positionen);
  zeile3(positionen, "0", "-0W", "1");
  zeile3(positionen, "1", "-0-0W", "1");
  abschnitt(positionen);
  zeile3(positionen, "0", "-0-0-EW", "1");
  ok &= schreiben(argv[1], "gueltig_06_positionsgrenzen.bdg", positionen);

  ok &= schreiben(argv[1], "ungueltig_01_leer.bdg", g_string_new(NULL));

  GString *s = minimal("-0", "Text");
  g_string_truncate(s, s->len / 2);
  ok &= schreiben(argv[1], "ungueltig_02_abgeschnitten.bdg", s);

  s = minimal("-0", "Text");
  char *text = g_strstr_len(s->str, s->len, "Text");
  *text = '\0';
  ok &= schreiben(argv[1], "ungueltig_03_nullbyte.bdg", s);

  s = minimal("-0", "Text");
  text = g_strstr_len(s->str, s->len, "Text");
  text[0] = (char)0xc3;
  text[1] = '(';
  ok &= schreiben(argv[1], "ungueltig_04_utf8.bdg", s);

  GString *fehlender = anfang();
  zeile4(fehlender, "0", "0", "-0", "Wurzel");
  zeile4(fehlender, "1", "1", "-0-9-0", "Ohne Elternknoten");
  abschnitt(fehlender);
  zeile4(fehlender, "0", "0", "-0-E", "E");
  abschnitt(fehlender);
  zeile3(fehlender, "0", "-0W", "1");
  abschnitt(fehlender);
  zeile3(fehlender, "0", "-0-EW", "1");
  ok &= schreiben(argv[1], "ungueltig_05_fehlender_vorgaenger.bdg", fehlender);

  GString *doppelt = anfang();
  zeile4(doppelt, "0", "0", "-0", "A");
  zeile4(doppelt, "1", "1", "-0", "B");
  abschnitt(doppelt);
  zeile4(doppelt, "0", "0", "-0-E", "E");
  abschnitt(doppelt);
  zeile3(doppelt, "0", "-0W", "1");
  abschnitt(doppelt);
  zeile3(doppelt, "0", "-0-EW", "1");
  ok &= schreiben(argv[1], "ungueltig_06_doppelter_name.bdg", doppelt);

  ok &= schreiben(argv[1], "ungueltig_07_knotenname.bdg",
                  minimal("kein-knotenname", "Text"));

  g_autofree char *zuviel = g_strnfill(10000, 'B');
  ok &= schreiben(argv[1], "ungueltig_08_text_10000_bytes.bdg",
                  minimal("-0", zuviel));

  s = minimal("-0", "Text");
  char *knotenbeginn = memchr(s->str, RS, s->len);
  knotenbeginn += 2;
  *knotenbeginn = '1';
  ok &= schreiben(argv[1], "ungueltig_09_index_startet_bei_1.bdg", s);

  s = minimal("-0", "Text");
  g_string_append_c(s, RS);
  ok &= schreiben(argv[1], "ungueltig_10_daten_nach_dateiende.bdg", s);

  s = minimal("-0", "Text");
  char *trenner = memchr(s->str, US, s->len);
  *trenner = 'X';
  ok &= schreiben(argv[1], "ungueltig_11_feldtrenner_fehlt.bdg", s);

  s = minimal("-0", "Text");
  g_string_truncate(s, s->len - 1);
  ok &= schreiben(argv[1], "ungueltig_12_letzter_zeilenumbruch_fehlt.bdg", s);
  return ok ? 0 : 1;
}
