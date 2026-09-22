#ifndef ARBORETUM_GTK4_COMPAT_H
#define ARBORETUM_GTK4_COMPAT_H

#include <math.h>

/*
 * GTK4-Kompatibilitätsschicht
 * --------------------------
 * Der ursprüngliche Programmcode wurde für GTK3 geschrieben. Hier stehen
 * kleine Übersetzungen für entfernte oder umbenannte Funktionen. Dadurch
 * bleibt die eigentliche Baumlogik in den übrigen Dateien gut erkennbar.
 */

#define GTK_LAYOUT(obj) GTK_FIXED(obj)
#define gtk_layout_new(hadjustment, vadjustment) gtk_fixed_new()
#define gtk_layout_put(layout, child, x, y) \
  gtk_fixed_put(GTK_FIXED(layout), child, (double)(x), (double)(y))

/* GtkFixed speichert auch seine normale Positionierung als Transformation.
 * Sobald wir diese zum Drehen ersetzen, kann GTK die ursprünglichen x/y-Werte
 * nicht mehr zuverlässig zurückliefern. Deshalb merken wir die von Arboretum
 * berechnete Position zusätzlich direkt am Kind-Widget. */
static inline void arboretum_layout_move(GtkFixed *layout, GtkWidget *child,
                                         double x, double y)
{
  double *position = g_new(double, 2);
  position[0] = x;
  position[1] = y;
  g_object_set_data_full(G_OBJECT(child), "arboretum-layout-position",
                         position, g_free);
  gtk_fixed_move(layout, child, x, y);
}
#define gtk_layout_move(layout, child, x, y) \
  arboretum_layout_move(GTK_FIXED(layout), GTK_WIDGET(child), \
                        (double)(x), (double)(y))
#define gtk_layout_set_size(layout, width, height) \
  gtk_widget_set_size_request(GTK_WIDGET(layout), (width), (height))

#define gtk_entry_get_text(entry) gtk_editable_get_text(GTK_EDITABLE(entry))
#define gtk_entry_set_text(entry, text) gtk_editable_set_text(GTK_EDITABLE(entry), (text))
#define gtk_entry_set_width_chars(entry, width)                              \
  do                                                                         \
  {                                                                          \
    gtk_editable_set_width_chars(GTK_EDITABLE(entry), (width));              \
    gtk_editable_set_max_width_chars(GTK_EDITABLE(entry), (width));          \
  } while (0)
/* gtk_entry_grab_focus_without_selecting() also exists in GTK4. Use it
 * directly: plain gtk_widget_grab_focus() would select the entire text. */
#define gtk_widget_show_all(widget) gtk_widget_set_visible(GTK_WIDGET(widget), TRUE)

/* GTK4 fokussiert intern ein GtkText innerhalb des GtkEntry. Der übrige Code
 * benötigt aber das äußere Eingabefeld und dessen Arboretum-Namen. */
static inline GtkWidget *arboretum_window_get_focus(GtkWindow *window)
{
  GtkWidget *focus = gtk_window_get_focus(window);
  if (focus && !GTK_IS_ENTRY(focus))
  {
    GtkWidget *entry = gtk_widget_get_ancestor(focus, GTK_TYPE_ENTRY);
    if (entry)
      return entry;
  }
  return focus;
}
#define gtk_window_get_focus(window) arboretum_window_get_focus(GTK_WINDOW(window))

extern gboolean arboretum_layout_dirty;

/* Diese Variante löst nur das Neuzeichnen aus. Sie wird intern benutzt, wenn
 * die Positionen bereits aktuell sind. */
static inline void arboretum_widget_queue_draw_clean(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
}
static inline void arboretum_widget_queue_draw(GtkWidget *widget)
{
  /* Vor dem nächsten Zeichnen müssen die Widgetpositionen neu berechnet werden. */
  arboretum_layout_dirty = TRUE;
  gtk_widget_queue_draw(widget);
}
#define gtk_widget_queue_draw(widget) arboretum_widget_queue_draw(GTK_WIDGET(widget))

static inline void arboretum_widget_get_allocation(GtkWidget *widget,
                                                    GtkAllocation *allocation)
{
  graphene_rect_t bounds = GRAPHENE_RECT_INIT(0, 0,
                                               gtk_widget_get_width(widget),
                                               gtk_widget_get_height(widget));
  GtkWidget *parent = gtk_widget_get_parent(widget);
  if (parent) {
    gboolean have_bounds = gtk_widget_compute_bounds(widget, parent, &bounds);
    (void)have_bounds;
  }
  allocation->x = (int)bounds.origin.x;
  allocation->y = (int)bounds.origin.y;
  allocation->width = (int)bounds.size.width;
  allocation->height = (int)bounds.size.height;
}
#define gtk_widget_get_allocation(widget, allocation) \
  arboretum_widget_get_allocation(GTK_WIDGET(widget), allocation)

static inline void arboretum_widget_destroy(GtkWidget *widget)
{
  if (GTK_IS_WINDOW(widget))
    gtk_window_destroy(GTK_WINDOW(widget));
  else
    gtk_widget_unparent(widget);
}
#define gtk_widget_destroy(widget) arboretum_widget_destroy(GTK_WIDGET(widget))

static void arboretum_dialog_response(GtkDialog *dialog, gint response, gpointer data);

static inline gint arboretum_dialog_run(GtkDialog *dialog)
{
  /* GTK4 kennt gtk_dialog_run() nicht mehr. Eine kleine eigene Ereignisschleife
   * wartet hier auf die Antwort des Benutzers und liefert sie synchron zurück. */
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  gint response = GTK_RESPONSE_NONE;
  gulong handler = g_signal_connect(dialog, "response",
                                    G_CALLBACK(arboretum_dialog_response), NULL);
  g_object_set_data(G_OBJECT(dialog), "arboretum-loop", loop);
  g_object_set_data(G_OBJECT(dialog), "arboretum-response", &response);
  gtk_window_present(GTK_WINDOW(dialog));
  g_main_loop_run(loop);
  g_signal_handler_disconnect(dialog, handler);
  g_object_set_data(G_OBJECT(dialog), "arboretum-loop", NULL);
  g_object_set_data(G_OBJECT(dialog), "arboretum-response", NULL);
  g_main_loop_unref(loop);
  return response;
}

static void arboretum_dialog_response(GtkDialog *dialog, gint response, gpointer data)
{
  (void)data;
  gint *result = g_object_get_data(G_OBJECT(dialog), "arboretum-response");
  GMainLoop *loop = g_object_get_data(G_OBJECT(dialog), "arboretum-loop");
  if (result)
    *result = response;
  if (loop)
    g_main_loop_quit(loop);
}

#define gtk_dialog_run(dialog) arboretum_dialog_run(GTK_DIALOG(dialog))

static inline char *arboretum_file_chooser_get_filename(GtkFileChooser *chooser)
{
  GFile *file = gtk_file_chooser_get_file(chooser);
  char *path = file ? g_file_get_path(file) : NULL;
  g_clear_object(&file);
  return path;
}
#define gtk_file_chooser_get_filename(chooser) arboretum_file_chooser_get_filename(chooser)

static inline gboolean arboretum_file_chooser_set_filename(GtkFileChooser *chooser,
                                                            const char *filename)
{
  GFile *file = g_file_new_for_path(filename);
  gboolean result = gtk_file_chooser_set_file(chooser, file, NULL);
  g_object_unref(file);
  return result;
}
#define gtk_file_chooser_set_filename(chooser, filename) \
  arboretum_file_chooser_set_filename((chooser), (filename))
#define gtk_file_chooser_set_do_overwrite_confirmation(chooser, value) ((void)0)

static inline void arboretum_preferred_size(GtkWidget *widget,
                                             GtkOrientation orientation,
                                             int *minimum, int *natural)
{
  gtk_widget_measure(widget, orientation, -1, minimum, natural, NULL, NULL);
}
#define gtk_widget_get_preferred_width(widget, minimum, natural) \
  arboretum_preferred_size(GTK_WIDGET(widget), GTK_ORIENTATION_HORIZONTAL, minimum, natural)
#define gtk_widget_get_preferred_height(widget, minimum, natural) \
  arboretum_preferred_size(GTK_WIDGET(widget), GTK_ORIENTATION_VERTICAL, minimum, natural)

static inline void arboretum_add_css_provider(GtkCssProvider *provider)
{
  GdkDisplay *display = gdk_display_get_default();
  if (display)
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
}
static inline void arboretum_remove_css_provider(GtkCssProvider *provider)
{
  GdkDisplay *display = gdk_display_get_default();
  if (display && provider)
    gtk_style_context_remove_provider_for_display(display, GTK_STYLE_PROVIDER(provider));
}
#define gtk_style_context_add_provider_for_screen(screen, provider, priority) \
  arboretum_add_css_provider(GTK_CSS_PROVIDER(provider))

/* GTK4 hat gtk_label_set_angle() entfernt. GtkFixed kann seine Kinder jedoch
 * mit einer GSK-Transformation darstellen. Wir drehen um den Mittelpunkt des
 * Labels; dadurch bleibt die vorhandene Positionsrechnung gültig. */
static inline void arboretum_label_set_angle(GtkLabel *label, double angle)
{
  double *stored = g_new(double, 1);
  *stored = angle;
  g_object_set_data_full(G_OBJECT(label), "arboretum-angle", stored, g_free);

  GtkWidget *widget = GTK_WIDGET(label);
  GtkWidget *parent = gtk_widget_get_parent(widget);
  if (!parent || !GTK_IS_FIXED(parent))
    return;

  if (angle > -0.001 && angle < 0.001)
  {
    /* labelverschieben() hat unmittelbar vorher gtk_layout_move() aufgerufen
     * und damit eine eventuell vorhandene Drehung bereits entfernt. */
    return;
  }

  int width = gtk_widget_get_width(widget);
  int height = gtk_widget_get_height(widget);
  if (width <= 0 || height <= 0)
    gtk_widget_measure(widget, GTK_ORIENTATION_HORIZONTAL, -1,
                       NULL, &width, NULL, NULL);
  if (height <= 0)
    gtk_widget_measure(widget, GTK_ORIENTATION_VERTICAL, -1,
                       NULL, &height, NULL, NULL);

  double x = 0.0, y = 0.0;
  double *position = g_object_get_data(G_OBJECT(widget),
                                       "arboretum-layout-position");
  if (position)
  {
    x = position[0];
    y = position[1];
  }
  else
  {
    gtk_fixed_get_child_position(GTK_FIXED(parent), widget, &x, &y);
  }

  /* GSKs eigene 2D-Bausteine erhalten die Transformation ausdrücklich als
   * 2D-Transformation. Die Reihenfolge lautet von rechts nach links:
   * erst den Label-Mittelpunkt zum Ursprung schieben, dort drehen und den
   * Mittelpunkt anschließend an seine berechnete Position zurückschieben. */
  double center_x = width / 2.0;
  double center_y = height / 2.0;
  graphene_point_t destination = GRAPHENE_POINT_INIT(x + center_x,
                                                       y + center_y);
  graphene_point_t origin = GRAPHENE_POINT_INIT(-center_x, -center_y);
  GskTransform *transform = gsk_transform_translate(NULL, &destination);
  transform = gsk_transform_rotate(transform, (float)-angle);
  transform = gsk_transform_translate(transform, &origin);
  gtk_fixed_set_child_transform(GTK_FIXED(parent), widget, transform);
  gsk_transform_unref(transform);
}

static inline double arboretum_label_get_angle(GtkLabel *label)
{
  double *stored = g_object_get_data(G_OBJECT(label), "arboretum-angle");
  return stored ? *stored : 0.0;
}

#define gtk_label_set_angle(label, angle) arboretum_label_set_angle((label), (angle))
#define gtk_label_get_angle(label) arboretum_label_get_angle((label))

#endif
