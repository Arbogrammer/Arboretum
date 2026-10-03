/* GTK integration test: run with xvfb-run ./layout-test. */
#define main arboretum_application_main
#include "baumg.c"
#undef main

/* Check the allocated text area, not just the alignment property. An entry
 * icon can shift that area even while xalign remains 0.5. */
static void probability_entry_fits(GtkWidget *entry) {
  GtkWidget *text = GTK_WIDGET(gtk_editable_get_delegate(GTK_EDITABLE(entry)));
  graphene_rect_t bounds;
  g_assert_true(gtk_widget_compute_bounds(text, entry, &bounds));
  g_assert_cmpfloat_with_epsilon(bounds.origin.x + bounds.size.width / 2,
                                gtk_widget_get_width(entry) / 2.0, 1.0);
  g_assert_cmpfloat(gtk_editable_get_alignment(GTK_EDITABLE(entry)), ==, 0.5);
  PangoLayout *layout = gtk_widget_create_pango_layout(text,
      gtk_editable_get_text(GTK_EDITABLE(entry)));
  int width;
  pango_layout_get_pixel_size(layout, &width, NULL);
  g_object_unref(layout);
  g_assert_cmpint(gtk_widget_get_width(text), >, width);
}

static void probability_entry_layout_test(gpointer data, const char *path) {
  letzte_wahrscheinlichkeit_automatisch = FALSE;
  kuerzen = TRUE;
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[4]), "1/1234567");
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "1/7654321");
    io_test_drain();
    g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[3])),
                    ==, "1/9449772114007");
    probability_entry_fits(textfeldWahrscheinlichkeit[4]);
    probability_entry_fits(textfeldErgebnisWahrscheinlichkeit[3]);
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "1/");
    io_test_drain();
    probability_entry_fits(textfeldWahrscheinlichkeit[5]);
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "1/7654321");
    io_test_drain();
    probability_entry_fits(textfeldWahrscheinlichkeit[5]);
    g_assert_true(speichern((char *)path));
    laden(data, (char *)path);
    io_test_drain();
    probability_entry_fits(textfeldErgebnisWahrscheinlichkeit[3]);
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "1/3");
    rueckgaengig(NULL, data);
    io_test_drain();
    probability_entry_fits(textfeldErgebnisWahrscheinlichkeit[3]);
  }
  baum_vertikal = FALSE;
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[4]), "1/3");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "1/3");
  io_test_drain();
}

static void urnen_ergebnis_layout_test(gpointer data) {
  const char *modi[] = {"ohne", "mit", "dazulegen", NULL};
  GtkWidget *felder[] = {gtk_entry_new(), gtk_entry_new(),
      gtk_spin_button_new_with_range(1, 100, 1),
      gtk_drop_down_new_from_strings(modi), data};
  for (int i = 0; i < 4; i++)
    g_object_ref_sink(felder[i]);
  gtk_entry_set_text(GTK_ENTRY(felder[0]), "Weiss, Grün, WWW");
  gtk_entry_set_text(GTK_ENTRY(felder[1]), "2, 3, 2");
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(felder[2]), 3);
  ergebnisseanzeigen = ergebnissewskanzeigen = TRUE;
  if (labelein)
    umwandeln(NULL, data);
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    for (int modus = 0; modus < 3; modus++) {
      gtk_drop_down_set_selected(GTK_DROP_DOWN(felder[3]), modus);
      GtkWidget *dialog = gtk_dialog_new();
      g_assert_true(urnen_generieren(dialog, felder));
      io_test_drain();
      for (int i = 0; i <= maxzaehlererg; i++) {
        probability_entry_fits(textfeldErgebnis[i]);
        probability_entry_fits(textfeldErgebnisWahrscheinlichkeit[i]);
        graphene_rect_t result, probability;
        g_assert_true(gtk_widget_compute_bounds(textfeldErgebnis[i], data,
                                                &result));
        g_assert_true(gtk_widget_compute_bounds(
            textfeldErgebnisWahrscheinlichkeit[i], data, &probability));
        if (vertical)
          g_assert_cmpfloat(probability.origin.y, >=,
                            result.origin.y + result.size.height);
        else
          g_assert_cmpfloat(probability.origin.x, >=,
                            result.origin.x + result.size.width + ErgebnisAbstand);
      }
      for (int stacked = 0; stacked < 2; stacked++) {
        bruchou = stacked;
        umwandeln(NULL, data);
        /* Generating from an already fixed tree must refresh the labels as
         * well as preserve the selected representation. */
        g_assert_true(urnen_generieren(gtk_dialog_new(), felder));
        io_test_drain();
        g_assert_true(labelein);
        g_assert_true(bruch);
        g_assert_cmpint(bruchou, ==, stacked);
        for (int i = 0; i <= maxzaehler; i++) {
          if (stacked) {
            g_auto(GStrv) parts = g_strsplit(gtk_entry_get_text(
                GTK_ENTRY(textfeldWahrscheinlichkeit[i])), "/", 2);
            g_assert_true(gtk_widget_get_visible(zaehlerlabel[i]));
            g_assert_true(gtk_widget_get_visible(nennerlabel[i]));
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(zaehlerlabel[i])), ==, parts[0]);
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(nennerlabel[i])), ==, parts[1]);
          } else {
            g_assert_true(gtk_widget_get_visible(wahrscheinlichkeitlabel[i]));
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(wahrscheinlichkeitlabel[i])),
                ==, gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
          }
        }
        for (int i = 0; i <= maxzaehlererg; i++) {
          if (stacked) {
            g_auto(GStrv) parts = g_strsplit(gtk_entry_get_text(
                GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])), "/", 2);
            g_assert_true(gtk_widget_get_visible(ergebniszaehlerlabel[i]));
            g_assert_true(gtk_widget_get_visible(ergebnisnennerlabel[i]));
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(ergebniszaehlerlabel[i])), ==, parts[0]);
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(ergebnisnennerlabel[i])), ==, parts[1]);
          } else {
            g_assert_cmpstr(gtk_label_get_text(GTK_LABEL(ergebniswsklabel[i])),
                ==, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
          }
        }
        umwandeln(NULL, data);
      }
    }
  }
  for (int i = 0; i < 4; i++)
    g_object_unref(felder[i]);
  g_printerr("Urnenmodell: Ergebnisbreiten, Abstand und fixierte Bruchdarstellung in allen Modi OK\n");
}

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
  probability_entry_layout_test(data, path);

  letzte_wahrscheinlichkeit_automatisch = FALSE;
  genauigkeit = 4;
  kuerzen = TRUE;
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[0]), "1/2");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[1]), "0,5");
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0])), ==, "0,2500");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[4]), "0.5");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[5]), "0.5");
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[3])), ==, "0.2500");
  const char *invalid[] = {"x", "0.5x", "1/0", "-0.1", "1.1", "/2", "1/", "999999999999999999999/2", "nan", "   "};
  for (guint i = 0; i < G_N_ELEMENTS(invalid); i++) {
    gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[1]), invalid[i]);
    g_assert_true(gtk_widget_has_css_class(textfeldWahrscheinlichkeit[1], "error"));
    g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0])), ==, "");
  }
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[1]), "1/3");
  g_assert_false(gtk_widget_has_css_class(textfeldWahrscheinlichkeit[1], "error"));
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0])), ==, "1/6");
  io_test_drain();
  undo_leeren();
  letzte_wahrscheinlichkeit_automatisch = TRUE;
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[2]), "0,25");
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[3])), ==, "0,4167");
  rueckgaengig(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[2])), ==, "1/3");
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[3])), ==, "1/3");
  GtkEventController *controller = gtk_event_controller_key_new();
  gtk_widget_add_controller(window, controller);
  g_assert_true(keyfunc(GTK_EVENT_CONTROLLER_KEY(controller), GDK_KEY_Z, 0,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK, data));
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[2])), ==, "0,25");
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[3])), ==, "0,4167");
  gtk_widget_remove_controller(window, controller);
  letzte_wahrscheinlichkeit_automatisch = FALSE;
  io_test_drain();
  undo_leeren();
  gtk_entry_set_text(GTK_ENTRY(textfeld[0]), "Text");
  gtk_entry_set_text(GTK_ENTRY(textfeld[0]), "Textfolge");
  g_assert_cmpuint(undo_schritte->len, ==, 1);
  rueckgaengig(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), ==, "A");
  wiederherstellen(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), ==, "Textfolge");
  rueckgaengig(NULL, data);
  gtk_entry_set_text(GTK_ENTRY(textfeld[0]), "Neu");
  wiederherstellen(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), ==, "Neu");
  /* A pause starts a new group, without sleeping in the test. */
  undo_zeit -= 2 * G_USEC_PER_SEC;
  gtk_entry_set_text(GTK_ENTRY(textfeld[0]), "Weiter");
  rueckgaengig(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), ==, "Neu");
  gtk_entry_set_text(GTK_ENTRY(textfeld[1]), "Kind");
  rueckgaengig(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[1])), ==, "B");
  wiederherstellen(NULL, data);
  g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(textfeld[1])), ==, "Kind");
  gtk_widget_grab_focus(textfeld[1]);
  loeschen(data);
  g_assert_cmpint(maxzaehler, ==, 10);
  rueckgaengig(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 11);
  wiederherstellen(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 10);
  rueckgaengig(NULL, data);
  undo_leeren();
  gtk_widget_grab_focus(textfeld[11]);
  gtk_editable_set_position(GTK_EDITABLE(textfeld[11]), -1);
  rechts(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 12);
  rueckgaengig(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 11);
  wiederherstellen(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 12);
  reset(data);
  g_assert_cmpint(maxzaehler, ==, 0);
  rueckgaengig(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 12);
  wiederherstellen(NULL, data);
  g_assert_cmpint(maxzaehler, ==, 0);
  for (int richtung = 0; richtung < 2; richtung++) {
    baum_vertikal = richtung;
    g_assert_true(speichern(path));
    baum_vertikal = !richtung;
    laden(data, path);
    g_assert_cmpint(baum_vertikal, ==, richtung);
    g_assert_cmpint(gtk_check_button_get_active(baumrichtungsschalter), ==, richtung);
  }
  /* Legacy files have no direction field. */
  g_autofree char *modern = NULL;
  g_assert_true(g_file_get_contents(path, &modern, NULL, NULL));
  char *newline = strchr(modern, '\n');
  char *last = newline - 2;
  while (last > modern && last[-1] != 31) last--;
  memmove(last, newline, strlen(newline) + 1);
  g_assert_true(g_file_set_contents(path, modern, -1, NULL));
  baum_vertikal = TRUE;
  laden(data, path);
  g_assert_false(baum_vertikal);
  gtk_check_button_set_active(baumrichtungsschalter, TRUE);
  rueckgaengig(NULL, data);
  g_assert_false(baum_vertikal);
  wiederherstellen(NULL, data);
  g_assert_true(baum_vertikal);
  g_printerr("Editing regression: mixed values, invalid input, grouped undo/redo, delete, direction and legacy files OK\n");
  urnen_ergebnis_layout_test(data);
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}

static gboolean editing_test_start(gpointer unused) {
  return layout_test(gtk_widget_get_parent(textfeld[0]));
}
int main(int argc, char **argv) {
  g_idle_add(editing_test_start, NULL);
  return arboretum_application_main(argc, argv);
}
