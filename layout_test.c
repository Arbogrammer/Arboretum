/* GTK integration test: run with xvfb-run ./layout-test. */
#define main arboretum_application_main
#include "baumg.c"
#undef main

static gboolean layout_test(gpointer data) {
  g_autofree char *dir = g_dir_make_tmp("arboretum-layout-XXXXXX", NULL);
  g_assert_nonnull(dir);
  g_autofree char *path = g_build_filename(dir, "layout.bdg", NULL);
  g_assert_true(speichern(path));
  g_autofree char *saved = NULL;
  g_assert_true(g_file_get_contents(path, &saved, NULL, NULL));
  char *end = strchr(saved, '\n');
  g_autoptr(GString) fixture = g_string_new_len(saved, end - saved + 1);
  g_string_append(fixture, "\036\n");
  /* Three parents, each with three leaves, deliberately tight spacing. */
  for (int a = 0; a < 3; a++) {
    g_string_append_printf(fixture, "%d\037%d\037-%d\037A\037\n", a * 4,
                           (a * 3 + 1) * 18, a);
    for (int b = 0; b < 3; b++)
      g_string_append_printf(fixture, "%d\037%d\037-%d-%d\037B\037\n",
                             a * 4 + b + 1, (a * 3 + b) * 18, a, b);
  }
  g_string_append(fixture, "\036\n");
  for (int a = 0; a < 3; a++)
    for (int b = 0; b < 3; b++)
      g_string_append_printf(fixture, "%d\037%d\037-%d-%d-E\037AB\037\n",
                             a * 3 + b, (a * 3 + b) * 18, a, b);
  g_string_append(fixture, "\036\n");
  for (int a = 0; a < 3; a++) {
    g_string_append_printf(fixture, "%d\037-%dW\0371/3\037\n", a * 4, a);
    for (int b = 0; b < 3; b++)
      g_string_append_printf(fixture, "%d\037-%d-%dW\0371/3\037\n",
                             a * 4 + b + 1, a, b);
  }
  g_string_append(fixture, "\036\n");
  for (int a = 0; a < 3; a++)
    for (int b = 0; b < 3; b++)
      g_string_append_printf(fixture, "%d\037-%d-%d-EW\0371/9\037\n", a * 3 + b,
                             a, b);
  g_assert_true(g_file_set_contents(path, fixture->str, fixture->len, NULL));
  laden(data, path);
  g_assert_cmpint(maxzaehler, ==, 11);
  int original[12];
  memcpy(original, y, sizeof original);
  StufenBreite = 150;
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    for (int fraction = 0; fraction < 2; fraction++) {
      if (labelein)
        umwandeln(NULL, data);
      bruch = bruchou = fraction;
      umwandeln(NULL, data);
      for (int side = 0; side < 3; side++)
        for (int middle = 0; middle < 2; middle++) {
          wskseite = side;
          wskmitteunten = middle;
          wskautomatik = FALSE;
          labelverschieben(data);
          int before = 0;
          for (int i = 0; i <= maxzaehler; i++)
            before += wsk_kollision(i) >= 0;
          wskautomatik = TRUE;
          labelverschieben(data);
          g_printerr("Layout vertical=%d fraction=%d side=%d middle=%d "
                     "collisions=%d -> %d size=%dx%d\n",
                     vertical, fraction, side, middle, before,
                     wsklayout_kollisionen, wsklayout_width, wsklayout_height);
          g_assert_cmpint(wsklayout_kollisionen, ==, 0);
          for (int i = 0; i <= maxzaehler; i++) {
            WskLayout *p = &wsklayout[i];
            gboolean below =
                side == 1 ||
                (side == 2 && (p->rank == 2 || (p->rank == 1 && middle)));
            g_assert_cmpint(p->below, ==, below);
            g_assert_cmpint(y[i], ==, original[i]);
            g_assert_cmpfloat(p->shift, <=, 22.5);
          }
          double last = wsklayout[11].position;
          labelverschieben(data);
          g_assert_cmpfloat(wsklayout[11].position, ==, last);
          if (side == 2 && middle == 0) {
            hintergrundfarbe = (GdkRGBA){1, 1, 1, 1};
            io_test_drain();
            g_autofree char *image =
                g_strdup_printf("%s/tree-%d-%d.png", dir, vertical, fraction);
            g_assert_true(exportpng(image));
          }
          wskautomatik = FALSE;
          labelverschieben(data);
          for (int i = 0; i <= maxzaehler; i++)
            g_assert_cmpfloat(wsklayout[i].position, ==, original[i]);
        }
    }
  }
  wskseite = 1;
  wskmitteunten = TRUE;
  wskautomatik = TRUE;
  g_assert_true(speichern(path));
  wskseite = 0;
  wskmitteunten = FALSE;
  wskautomatik = FALSE;
  laden(data, path);
  g_assert_cmpint(wskseite, ==, 1);
  g_assert_true(wskmitteunten);
  g_assert_true(wskautomatik);
  tempspeichern();
  wskseite = 0;
  wskmitteunten = FALSE;
  wskautomatik = FALSE;
  templaden(data);
  g_assert_cmpint(wskseite, ==, 1);
  g_assert_true(wskmitteunten);
  g_assert_true(wskautomatik);
  g_printerr("Layout settings save/load/undo: OK; artifacts: %s\n", dir);
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}

static gboolean layout_test_start(gpointer unused) {
  /* The initial application setup has completed before this idle runs. */
  return layout_test(gtk_widget_get_parent(textfeld[0]));
}

int main(int argc, char **argv) {
  g_idle_add(layout_test_start, NULL);
  return arboretum_application_main(argc, argv);
}
