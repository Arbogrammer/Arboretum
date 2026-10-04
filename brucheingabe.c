/* The GtkEntry buffer remains the document model ("1/2"). Only its visible
 * editor changes: native GtkText for plain values, two GtkTexts for fractions.
 * Keeping a real GtkEntry preserves the existing persistence/undo interface. */
typedef struct {
  GtkEntry parent;
  GtkWidget *plain, *box, *numerator, *denominator;
  gboolean vertical, syncing, whole;
} ArboretumFractionEntry;
typedef GtkEntryClass ArboretumFractionEntryClass;
G_DEFINE_TYPE(ArboretumFractionEntry, arboretum_fraction_entry, GTK_TYPE_ENTRY)

static ArboretumFractionEntry *bruchfeld(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, arboretum_fraction_entry_get_type())
      ? (ArboretumFractionEntry *)widget : NULL;
}

static GtkWidget *bruchfeld_teil(ArboretumFractionEntry *self) {
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(self));
  GtkWidget *focus = root ? gtk_root_get_focus(root) : NULL;
  return focus == self->numerator || focus == self->denominator ? focus : NULL;
}

static void bruchfeld_fokus(ArboretumFractionEntry *self, gboolean denominator,
                            int position) {
  GtkWidget *part = self->vertical
      ? (denominator ? self->denominator : self->numerator) : self->plain;
  gtk_text_grab_focus_without_selecting(GTK_TEXT(part));
  gtk_editable_set_position(GTK_EDITABLE(part), position);
}

static void bruchfeld_aktualisieren(ArboretumFractionEntry *self) {
  if (!self || self->syncing) return;
  self->syncing = TRUE;
  const char *value = gtk_editable_get_text(GTK_EDITABLE(self));
  const char *slash = strchr(value, '/');
  gboolean vertical = bruchou && slash && !strchr(slash + 1, '/');
  gboolean changed = self->vertical != vertical;
  GtkWidget *part = bruchfeld_teil(self);
  gboolean focused = part || gtk_widget_has_focus(self->plain);
  int position = part ? gtk_editable_get_position(GTK_EDITABLE(part))
                      : gtk_editable_get_position(GTK_EDITABLE(self));
  int split = slash ? g_utf8_pointer_to_offset(value, slash) : 0;
  if (vertical) {
    g_autofree char *num = g_strndup(value, slash - value);
    if (strcmp(gtk_editable_get_text(GTK_EDITABLE(self->numerator)), num))
      gtk_editable_set_text(GTK_EDITABLE(self->numerator), num);
    if (strcmp(gtk_editable_get_text(GTK_EDITABLE(self->denominator)), slash + 1))
      gtk_editable_set_text(GTK_EDITABLE(self->denominator), slash + 1);
    int width = MAX(gtk_editable_get_width_chars(GTK_EDITABLE(self)), 3);
    for (int i = 0; i < 2; i++) {
      GtkEditable *edit = GTK_EDITABLE(i ? self->denominator : self->numerator);
      gtk_editable_set_width_chars(edit, width);
      gtk_editable_set_max_width_chars(edit, width);
      gtk_editable_set_editable(edit, gtk_editable_get_editable(GTK_EDITABLE(self)));
    }
  }
  self->vertical = vertical;
  if (changed) gtk_widget_queue_resize(GTK_WIDGET(self));
  gtk_widget_set_visible(self->plain, !vertical);
  gtk_widget_set_visible(self->box, vertical);
  if (changed && focused) {
    if (vertical)
      bruchfeld_fokus(self, TRUE, -1);
    else
      bruchfeld_fokus(self, FALSE, part == self->denominator ? split + 1 + position : position);
  }
  self->syncing = FALSE;
}

static void bruchfeld_model_changed(GtkEditable *editable, gpointer unused) {
  bruchfeld_aktualisieren((ArboretumFractionEntry *)editable);
}

static void bruchfeld_cursor(GtkEditable *part, GParamSpec *spec, gpointer data) {
  ArboretumFractionEntry *self = data;
  if (self->syncing || bruchfeld_teil(self) != GTK_WIDGET(part)) return;
  int pos = gtk_editable_get_position(part);
  if (GTK_WIDGET(part) == self->denominator)
    pos += g_utf8_strlen(gtk_editable_get_text(GTK_EDITABLE(self->numerator)), -1) + 1;
  gtk_editable_set_position(GTK_EDITABLE(self), pos);
}

static void bruchfeld_part_changed(GtkEditable *part, gpointer data) {
  ArboretumFractionEntry *self = data;
  if (self->syncing) return;
  g_autofree char *value = g_strdup_printf("%s/%s",
      gtk_editable_get_text(GTK_EDITABLE(self->numerator)),
      gtk_editable_get_text(GTK_EDITABLE(self->denominator)));
  self->syncing = TRUE;
  gtk_editable_set_text(GTK_EDITABLE(self), value);
  self->syncing = FALSE;
  bruchfeld_aktualisieren(self);
}

static void bruchfeld_ersetzen(ArboretumFractionEntry *self, const char *value) {
  self->whole = FALSE;
  gtk_editable_set_text(GTK_EDITABLE(self), value);
  bruchfeld_aktualisieren(self);
  bruchfeld_fokus(self, self->vertical, -1);
}

static void bruchfeld_insert(GtkEditable *part, const char *text, int length,
                              int *position, gpointer data) {
  ArboretumFractionEntry *self = data;
  if (self->syncing) return;
  if (self->whole || memchr(text, '/', length)) {
    g_signal_stop_emission_by_name(part, "insert-text");
    g_autofree char *value = g_strndup(text, length);
    if (!self->whole && !strcmp(value, "/"))
      bruchfeld_fokus(self, TRUE, -1);
    else
      bruchfeld_ersetzen(self, value);
  }
}

static void bruchfeld_copy(GtkText *part, gpointer data) {
  ArboretumFractionEntry *self = data;
  if (!self->whole) return;
  g_signal_stop_emission_by_name(part, "copy-clipboard");
  gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(self)),
                         gtk_editable_get_text(GTK_EDITABLE(self)));
}

static void bruchfeld_cut(GtkText *part, gpointer data) {
  ArboretumFractionEntry *self = data;
  if (!self->whole) return;
  g_signal_stop_emission_by_name(part, "cut-clipboard");
  gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(self)),
                         gtk_editable_get_text(GTK_EDITABLE(self)));
  bruchfeld_ersetzen(self, "");
}

static void bruchfeld_paste_ready(GObject *clipboard, GAsyncResult *result,
                                  gpointer data) {
  GtkWidget *part = data;
  ArboretumFractionEntry *self = bruchfeld(gtk_widget_get_ancestor(part, arboretum_fraction_entry_get_type()));
  g_autofree char *text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(clipboard), result, NULL);
  if (self && text && gtk_editable_get_editable(GTK_EDITABLE(self))) {
    if (self->whole || strchr(text, '/'))
      bruchfeld_ersetzen(self, text);
    else {
      gtk_editable_delete_selection(GTK_EDITABLE(part));
      int pos = gtk_editable_get_position(GTK_EDITABLE(part));
      gtk_editable_insert_text(GTK_EDITABLE(part), text, -1, &pos);
      gtk_editable_set_position(GTK_EDITABLE(part), pos);
    }
  }
  g_object_unref(part);
}

static void bruchfeld_paste(GtkText *part, gpointer data) {
  g_signal_stop_emission_by_name(part, "paste-clipboard");
  gdk_clipboard_read_text_async(gtk_widget_get_clipboard(GTK_WIDGET(part)), NULL,
                                bruchfeld_paste_ready, g_object_ref(part));
}

static void bruchfeld_auswahl_aufheben(ArboretumFractionEntry *self);

/* Called before the window's tree-navigation shortcuts. */
static gboolean bruchfeld_taste(guint key, GdkModifierType state) {
  GtkWidget *entry = gtk_window_get_focus(GTK_WINDOW(window));
  ArboretumFractionEntry *self = bruchfeld(entry);
  if (!self || !self->vertical) return FALSE;
  GtkWidget *part = bruchfeld_teil(self);
  if (!part) return FALSE;
  gboolean control = (state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK;
  if (control && (key == GDK_KEY_a || key == GDK_KEY_A)) {
    self->whole = TRUE;
    gtk_editable_select_region(GTK_EDITABLE(self->numerator), 0, -1);
    gtk_editable_select_region(GTK_EDITABLE(self->denominator), 0, -1);
    return TRUE;
  }
  if (self->whole && !(state & (GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SUPER_MASK)) &&
      (key == GDK_KEY_BackSpace || key == GDK_KEY_Delete)) {
    bruchfeld_ersetzen(self, "");
    return TRUE;
  }
  if (!(state & (GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SUPER_MASK))) {
    gunichar character = gdk_keyval_to_unicode(key);
    if (self->whole && character && !g_unichar_iscntrl(character)) {
      char value[7] = {0};
      g_unichar_to_utf8(character, value);
      bruchfeld_ersetzen(self, value);
      return TRUE;
    }
    if (key == GDK_KEY_slash || key == GDK_KEY_KP_Divide) {
      bruchfeld_fokus(self, TRUE, -1);
      return TRUE;
    }
    if (!(state & GDK_SHIFT_MASK)) {
      if (self->whole && (key == GDK_KEY_Left || key == GDK_KEY_Right ||
                          key == GDK_KEY_Home || key == GDK_KEY_End)) {
        bruchfeld_auswahl_aufheben(self);
        gboolean end = key == GDK_KEY_Right || key == GDK_KEY_End;
        bruchfeld_fokus(self, end, end ? -1 : 0);
        return TRUE;
      }
      if ((key == GDK_KEY_Down && part == self->numerator) ||
          (key == GDK_KEY_Up && part == self->denominator)) {
        bruchfeld_auswahl_aufheben(self);
        bruchfeld_fokus(self, key == GDK_KEY_Down, -1);
        return TRUE;
      }
      if (key == GDK_KEY_BackSpace && part == self->denominator &&
          !*gtk_editable_get_text(GTK_EDITABLE(part))) {
        g_autofree char *value = g_strdup(gtk_editable_get_text(GTK_EDITABLE(self->numerator)));
        bruchfeld_ersetzen(self, value);
        return TRUE;
      }
    }
  }
  if (key == GDK_KEY_Left || key == GDK_KEY_Right || key == GDK_KEY_Home ||
      key == GDK_KEY_End || key == GDK_KEY_Tab)
    bruchfeld_auswahl_aufheben(self);
  return FALSE;
}

static gboolean bruchfeld_grab_focus(GtkWidget *widget) {
  ArboretumFractionEntry *self = (ArboretumFractionEntry *)widget;
  if (!self->vertical)
    return GTK_WIDGET_CLASS(arboretum_fraction_entry_parent_class)->grab_focus(widget);
  if (!bruchfeld_teil(self)) bruchfeld_fokus(self, FALSE, -1);
  return TRUE;
}

static void bruchfeld_snapshot(GtkWidget *widget, GtkSnapshot *snapshot) {
  ArboretumFractionEntry *self = (ArboretumFractionEntry *)widget;
  if (self->vertical)
    gtk_widget_snapshot_child(widget, self->box, snapshot);
  else
    GTK_WIDGET_CLASS(arboretum_fraction_entry_parent_class)->snapshot(widget, snapshot);
}

static void bruchfeld_measure(GtkWidget *widget, GtkOrientation orientation,
                               int for_size, int *minimum, int *natural,
                               int *minimum_baseline, int *natural_baseline) {
  ArboretumFractionEntry *self = (ArboretumFractionEntry *)widget;
  if (self->vertical)
    gtk_widget_measure(self->box, orientation, for_size, minimum, natural,
                        minimum_baseline, natural_baseline);
  else
    GTK_WIDGET_CLASS(arboretum_fraction_entry_parent_class)->measure(widget,
        orientation, for_size, minimum, natural, minimum_baseline, natural_baseline);
}

static void bruchfeld_allocate(GtkWidget *widget, int width, int height, int baseline) {
  ArboretumFractionEntry *self = (ArboretumFractionEntry *)widget;
  if (self->vertical)
    gtk_widget_allocate(self->box, width, height, -1, NULL);
  else
    GTK_WIDGET_CLASS(arboretum_fraction_entry_parent_class)->size_allocate(widget, width, height, baseline);
}

static void bruchfeld_dispose(GObject *object) {
  ArboretumFractionEntry *self = (ArboretumFractionEntry *)object;
  if (self->box) {
    gtk_widget_unparent(self->box);
    self->box = NULL;
  }
  G_OBJECT_CLASS(arboretum_fraction_entry_parent_class)->dispose(object);
}

static void arboretum_fraction_entry_class_init(ArboretumFractionEntryClass *klass) {
  GTK_WIDGET_CLASS(klass)->snapshot = bruchfeld_snapshot;
  GTK_WIDGET_CLASS(klass)->measure = bruchfeld_measure;
  GTK_WIDGET_CLASS(klass)->size_allocate = bruchfeld_allocate;
  GTK_WIDGET_CLASS(klass)->grab_focus = bruchfeld_grab_focus;
  G_OBJECT_CLASS(klass)->dispose = bruchfeld_dispose;
}

static void bruchfeld_auswahl_aufheben(ArboretumFractionEntry *self) {
  if (!self->whole) return;
  self->whole = FALSE;
  GtkEditable *a = GTK_EDITABLE(self->numerator), *b = GTK_EDITABLE(self->denominator);
  gtk_editable_set_position(a, gtk_editable_get_position(a));
  gtk_editable_set_position(b, gtk_editable_get_position(b));
}

static void bruchfeld_click(GtkGestureClick *gesture, int n, double x, double y,
                            gpointer data) {
  bruchfeld_auswahl_aufheben(data);
}

static void bruchfeld_leave(GtkEventControllerFocus *controller, gpointer data) {
  bruchfeld_auswahl_aufheben(data);
}

static void bruchfeld_strich(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data) {
  GdkRGBA color;
  gtk_style_context_get_color(gtk_widget_get_style_context(GTK_WIDGET(area)), &color);
  gdk_cairo_set_source_rgba(cr, &color);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);
}

static void arboretum_fraction_entry_init(ArboretumFractionEntry *self) {
  self->plain = GTK_WIDGET(gtk_editable_get_delegate(GTK_EDITABLE(self)));
  self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
  gtk_widget_set_hexpand(self->box, TRUE);
  self->numerator = gtk_text_new();
  self->denominator = gtk_text_new();
  gtk_box_append(GTK_BOX(self->box), self->numerator);
  GtkWidget *bar = gtk_drawing_area_new();
  gtk_widget_set_size_request(bar, -1, 1);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(bar), bruchfeld_strich, NULL, NULL);
  gtk_box_append(GTK_BOX(self->box), bar);
  gtk_box_append(GTK_BOX(self->box), self->denominator);
  for (int i = 0; i < 2; i++) {
    GtkWidget *part = i ? self->denominator : self->numerator;
    gtk_editable_set_alignment(GTK_EDITABLE(part), 0.5);
    gtk_editable_set_enable_undo(GTK_EDITABLE(part), FALSE);
    gtk_text_set_max_length(GTK_TEXT(part), MAX_EINGABE_BYTES);
    gtk_accessible_update_property(GTK_ACCESSIBLE(part), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   i ? "Nenner" : "Zähler", -1);
    g_signal_connect(part, "changed", G_CALLBACK(bruchfeld_part_changed), self);
    g_signal_connect(part, "insert-text", G_CALLBACK(bruchfeld_insert), self);
    g_signal_connect(part, "notify::cursor-position", G_CALLBACK(bruchfeld_cursor), self);
    g_signal_connect(part, "copy-clipboard", G_CALLBACK(bruchfeld_copy), self);
    g_signal_connect(part, "cut-clipboard", G_CALLBACK(bruchfeld_cut), self);
    g_signal_connect(part, "paste-clipboard", G_CALLBACK(bruchfeld_paste), self);
  }
  gtk_widget_set_parent(self->box, GTK_WIDGET(self));
  gtk_widget_hide(self->box);
  GtkGesture *click = gtk_gesture_click_new();
  gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(click), GTK_PHASE_CAPTURE);
  g_signal_connect(click, "pressed", G_CALLBACK(bruchfeld_click), self);
  gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(click));
  GtkEventController *focus = gtk_event_controller_focus_new();
  g_signal_connect(focus, "leave", G_CALLBACK(bruchfeld_leave), self);
  gtk_widget_add_controller(GTK_WIDGET(self), focus);
  g_signal_connect(self, "changed", G_CALLBACK(bruchfeld_model_changed), NULL);
}

static GtkWidget *bruchfeld_neu(void) {
  return g_object_new(arboretum_fraction_entry_get_type(), NULL);
}

static gboolean arboretum_entry_grab_focus_without_selecting(GtkEntry *entry) {
  ArboretumFractionEntry *self = bruchfeld(GTK_WIDGET(entry));
  if (self && self->vertical) return bruchfeld_grab_focus(GTK_WIDGET(entry));
  return gtk_entry_grab_focus_without_selecting(entry);
}
#define gtk_entry_grab_focus_without_selecting(entry) arboretum_entry_grab_focus_without_selecting(entry)

/* Existing tree navigation addresses the logical entry. Translate explicit
 * cursor positioning to the currently visible part, using Unicode offsets. */
static void arboretum_editable_set_position(GtkEditable *editable, int position) {
  gtk_editable_set_position(editable, position);
  ArboretumFractionEntry *self = bruchfeld(GTK_WIDGET(editable));
  if (!self || !self->vertical) return;
  int split = g_utf8_strlen(gtk_editable_get_text(GTK_EDITABLE(self->numerator)), -1);
  gboolean denominator = position < 0 || position > split;
  GtkWidget *part = denominator ? self->denominator : self->numerator;
  int local = position < 0 ? -1 : denominator ? position - split - 1 : position;
  if (bruchfeld_teil(self)) bruchfeld_fokus(self, denominator, local);
  else gtk_editable_set_position(GTK_EDITABLE(part), local);
}
#define gtk_editable_set_position(editable, position) arboretum_editable_set_position(editable, position)
