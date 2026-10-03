#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>

static void feld(FILE *datei, const char *wert) {
  fprintf(datei, "%s%c", wert, 31);
}

static void datensatz(FILE *datei, guint index, guint y, const char *name,
                      const char *text) {
  fprintf(datei, "%u%c%u%c%s%c%s%c\n", index, 31, y, 31, name, 31, text, 31);
}

int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  guint tiefe = (guint)g_ascii_strtoull(argv[2], NULL, 10);
  if (!tiefe || tiefe > 10000)
    return 2;
  FILE *datei = g_fopen(argv[1], "wb");
  if (!datei)
    return 1;

  for (int i = 0; i < 62; i++)
    feld(datei, i == 53 ? "Sans 12" : "0");
  fprintf(datei, "\n%c\n", 30);

  GString *name = g_string_new(NULL);
  for (guint i = 0; i < tiefe; i++) {
    g_string_append(name, "-0");
    datensatz(datei, i, i, name->str, "T");
  }
  fprintf(datei, "%c\n", 30);
  g_autofree gchar *ergebnisname = g_strdup_printf("%s-E", name->str);
  datensatz(datei, 0, tiefe - 1, ergebnisname, "E");
  fprintf(datei, "%c\n", 30);

  g_string_truncate(name, 0);
  for (guint i = 0; i < tiefe; i++) {
    g_string_append(name, "-0");
    g_autofree gchar *wname = g_strdup_printf("%sW", name->str);
    fprintf(datei, "%u%c%s%c1%c\n", i, 31, wname, 31, 31);
  }
  fprintf(datei, "%c\n", 30);
  g_autofree gchar *ewname = g_strdup_printf("%s-EW", name->str);
  fprintf(datei, "0%c%s%c1%c\n", 31, ewname, 31, 31);

  g_string_free(name, TRUE);
  gboolean fehler = fclose(datei) != 0;
  return fehler ? 1 : 0;
}
