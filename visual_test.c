/* Deterministic GTK window and export captures; run through test-visual.py. */
#define main arboretum_application_main
#define ARBORETUM_INITIAL_WINDOW_WIDTH 1200
#define ARBORETUM_INITIAL_WINDOW_HEIGHT 900
#include "baumg.c"
#undef main

static const char *visual_output;

static void visual_capture(GtkWidget *widget, const char *name) {
  io_test_drain();
  io_test_drain();
  int width = gtk_widget_get_width(widget), height = gtk_widget_get_height(widget);
  g_assert_cmpint(width, >, 0);
  g_assert_cmpint(height, >, 0);
  GdkPaintable *paintable = gtk_widget_paintable_new(widget);
  GtkSnapshot *snapshot = gtk_snapshot_new();
  gdk_paintable_snapshot(paintable, GDK_SNAPSHOT(snapshot), width, height);
  GskRenderNode *node = gtk_snapshot_free_to_node(snapshot);
  g_assert_nonnull(node);
  graphene_rect_t viewport = GRAPHENE_RECT_INIT(0, 0, width, height);
  GskRenderer *renderer = gtk_native_get_renderer(gtk_widget_get_native(widget));
  GdkTexture *texture = gsk_renderer_render_texture(renderer, node, &viewport);
  g_autofree char *path = g_strdup_printf("%s/%s.png", visual_output, name);
  g_assert_true(gdk_texture_save_to_png(texture, path));
  g_object_unref(texture);
  gsk_render_node_unref(node);
  g_object_unref(paintable);
  g_printerr("Captured %s (%dx%d)\n", name, width, height);
}

static gboolean visual_form(gpointer unused) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0; i < g_list_model_get_n_items(windows); i++) {
    g_autoptr(GtkWindow) candidate = g_list_model_get_item(windows, i);
    if (GTK_IS_DIALOG(candidate) && gtk_widget_get_mapped(GTK_WIDGET(candidate))) {
      visual_capture(GTK_WIDGET(candidate), unused ? (const char *)unused : "form-dialog");
      gtk_dialog_response(GTK_DIALOG(candidate), GTK_RESPONSE_CANCEL);
      return G_SOURCE_REMOVE;
    }
  }
  return G_SOURCE_CONTINUE;
}

static gboolean visual_run(gpointer unused) {
  gpointer layout = gtk_widget_get_parent(textfeld[0]);
  /* A font chooser's initial selection depends on installed fonts. Pin both
   * label and entry fonts, not only GtkSettings, for repeatable captures. */
  g_strlcpy(schriftart, "DejaVu Sans 10", sizeof(schriftart));
  schriftartanpassen(NULL, NULL, NULL);
  gtk_window_unmaximize(GTK_WINDOW(window));
  const char *modes[] = {"ohne", "mit", "dazulegen", NULL};
  GtkWidget *fields[] = {gtk_entry_new(), gtk_entry_new(),
      gtk_spin_button_new_with_range(1, 100, 1),
      gtk_drop_down_new_from_strings(modes), layout};
  for (int i = 0; i < 4; i++) g_object_ref_sink(fields[i]);
  gtk_entry_set_text(GTK_ENTRY(fields[0]), "Äpfel, Grün");
  gtk_entry_set_text(GTK_ENTRY(fields[1]), "2, 3");
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(fields[2]), 2);
  KnotenAbstand = 150;
  KnotenTextBreite = 8;
  for (int i = 0; i < 2; i++) KnotenTextBreiteStufe[i] = 8;
  ergebnisseanzeigen = ergebnissewskanzeigen = TRUE;
  g_assert_true(urnen_generieren(gtk_dialog_new(), fields));
  for (int i = 0; i < 4; i++) g_object_unref(fields[i]);
  hintergrundfarbe = (GdkRGBA){1, 1, 1, 1};
  io_test_drain();
  /* Regression: drawing must not invalidate an allocated scrollbar. */
  GtkAdjustment *horizontal = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrollwindow));
  double previous_scroll = gtk_adjustment_get_value(horizontal);
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
  cairo_t *cr = cairo_create(surface);
  scrh = TRUE;
  zeichnelinien(GTK_DRAWING_AREA(da), cr, 1, 1, layout);
  g_assert_true(scrh);
  g_assert_cmpfloat(gtk_adjustment_get_value(horizontal), ==, previous_scroll);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  io_test_drain();
  g_assert_false(scrh);
  gtk_adjustment_set_value(horizontal, 0);
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    baumrichtung_aktualisieren(layout);
    gtk_window_set_focus(GTK_WINDOW(window), NULL);
    g_autofree char *entry_name = g_strdup_printf("entries-%s", vertical ? "vertical" : "horizontal");
    visual_capture(window, entry_name);
    g_assert_cmpint(gtk_widget_get_width(window), ==, 1200);
    g_assert_cmpint(gtk_widget_get_height(window), ==, 900);
    bruch = bruchou = TRUE;
    umwandeln(NULL, layout);
    gtk_window_set_focus(GTK_WINDOW(window), NULL);
    g_autofree char *fixed_name = g_strdup_printf("fractions-%s", vertical ? "vertical" : "horizontal");
    visual_capture(window, fixed_name);
    g_autofree char *export_path = g_strdup_printf("%s/export-%s.png", visual_output,
                                                  vertical ? "vertical" : "horizontal");
    g_assert_true(exportpng(export_path));
    umwandeln(NULL, layout);
  }
  /* Same tree with mixed decimal/fraction fields and both editor modes. */
  gboolean auto_rest = letzte_wahrscheinlichkeit_automatisch;
  letzte_wahrscheinlichkeit_automatisch = FALSE;
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[0]), "0,4");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[1]), "1/");
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    for (int stacked = 0; stacked < 2; stacked++) {
      bruchou = stacked;
      baumrichtung_aktualisieren(layout);
      gtk_window_set_focus(GTK_WINDOW(window), NULL);
      g_autofree char *name = g_strdup_printf("mixed-%s-%s",
          vertical ? "vertical" : "horizontal", stacked ? "stacked" : "inline");
      visual_capture(window, name);
    }
  }
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[0]), "2/5");
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[1]), "1/4");
  letzte_wahrscheinlichkeit_automatisch = auto_rest;
  baumrichtung_aktualisieren(layout);
  g_timeout_add(200, visual_form, NULL);
  formdialog(NULL, layout);
  ueberschrift_modus = 3;
  for (int vertical = 0; vertical < 2; vertical++) {
    baum_vertikal = vertical;
    baumrichtung_aktualisieren(layout);
    g_autofree char *name = g_strdup_printf("headings-entries-%s", vertical ? "vertical" : "horizontal");
    visual_capture(window, name);
    umwandeln(NULL, layout);
    io_test_drain();
    g_autofree char *out = g_strdup_printf("%s/headings-export-%s.png", visual_output, vertical ? "vertical" : "horizontal");
    g_assert_true(exportpng(out));
    umwandeln(NULL, layout);
  }
  ueberschrift_modus = 4;
  g_timeout_add(200, visual_form, "headings-dialog");
  ueberschrift_dialog(NULL, layout);
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}

int main(int argc, char **argv) {
  visual_output = g_getenv("ARBORETUM_VISUAL_OUTPUT");
  g_assert_nonnull(visual_output);
  gtk_init();
  g_object_set(gtk_settings_get_default(), "gtk-theme-name", "Adwaita",
      "gtk-font-name", "DejaVu Sans 10", "gtk-enable-animations", FALSE,
      "gtk-cursor-blink", FALSE, "gtk-xft-dpi", 96 * 1024, NULL);
  g_idle_add(visual_run, NULL);
  return arboretum_application_main(argc, argv);
}
