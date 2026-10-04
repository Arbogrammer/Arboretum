/* Adjustments invalidate scrollbar allocations. Never change them from a
 * snapshot/draw callback; process pending scrolling after painting instead. */
static gboolean arboretum_scroll_geplant = FALSE;

static gboolean arboretum_scroll_aktualisieren(gpointer unused) {
  arboretum_scroll_geplant = FALSE;
  if (labelein) return G_SOURCE_REMOVE;
  if (scrh) {
    gtk_adjustment_set_value(
        gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrollwindow)),
        gtk_adjustment_get_upper(gtk_scrolled_window_get_hadjustment(
            GTK_SCROLLED_WINDOW(scrollwindow))));
    scrh = 0;
  }
  GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(window));
  if (focus) {
    const gchar *focusname = gtk_widget_get_name(focus);
    int fokusindex = strchr(focusname, 'W') ? wskexistiert(focusname)
                                            : knotenexistiert(focusname);
    if (fokusindex >= 0 &&
        (eingabe_y_position(y[fokusindex]) + KnotenHoehe + KnotenAbstand + LayoutRandOben -
                 gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(
                     GTK_SCROLLED_WINDOW(scrollwindow))) >
             gtk_widget_get_allocated_height(scrollwindow) ||
         eingabe_y_position(y[fokusindex]) <
             gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(
                 GTK_SCROLLED_WINDOW(scrollwindow))))) {
      gtk_adjustment_set_value(
          gtk_scrolled_window_get_vadjustment(
              GTK_SCROLLED_WINDOW(scrollwindow)),
          eingabe_y_position(y[fokusindex]) - gtk_widget_get_allocated_height(scrollwindow) +
              KnotenHoehe + KnotenAbstand + LayoutRandOben);
      scrv = 0;
    }
  }
  return G_SOURCE_REMOVE;
}

/* Verhindert doppelte Idle-Callbacks, darf aber niemals über den ausgeführten
 * Callback hinaus gesetzt bleiben. Bei schnellem Tastatur-Autorepeat kann
 * zwischen Layout-Aktualisierung und dem anschließenden Zeichnen bereits die
 * nächste Änderung eintreffen. Ein nur im Draw-Callback zurückgesetztes Flag
 * würde dann dauerhaft TRUE bleiben und alle weiteren Aktualisierungen
 * unterdrücken. */
static gboolean arboretum_layout_geplant = FALSE;

static gboolean arboretum_layout_aktualisieren(gpointer data) {
  /* Positionen dürfen in GTK4 nicht während des Zeichnens verändert werden.
   * Diese Funktion läuft deshalb kurz danach als sogenannter Idle-Callback. */
  arboretum_layout_geplant = FALSE;

  if (labelein)
    labelverschieben(data);
  else {
    /* Measure first: GTK 4 does not synchronously allocate widgets when the
     * window is presented, unlike the old GTK 3 show-all path. */
    groesseneu(NULL, NULL, data);
    wskergebnisverschieben(NULL, NULL, data);
    positionsanpassungwsk(data);
  }

  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  wsk_layout_groesse(data);
  arboretum_layout_dirty = FALSE;
  arboretum_widget_queue_draw_clean(da);
  return G_SOURCE_REMOVE;
}

static void zeichnelinien(GtkDrawingArea *widget, cairo_t *cr, int width,
                          int height, gpointer data) {
  /* Cairo zeichnet nur die grafischen Bestandteile (Hintergrund, Rahmen und
   * Zweige). Die Eingabefelder selbst sind normale GTK-Widgets darüber. */
  if (data && arboretum_layout_dirty && !arboretum_layout_geplant) {
    arboretum_layout_geplant = TRUE;
    g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, arboretum_layout_aktualisieren,
                    data, NULL);
  }

  /* Auf dem Bildschirm darf eine transparente Cairo-Zeichenfläche nicht als
   * schwarzer Puffer sichtbar werden. Exporte (widget == NULL) behalten ihre
   * echte Transparenz; im Programmfenster wird sie über Weiß zusammengesetzt.
   */
  if (widget) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
  }
  cairo_set_source_rgba(cr, hintergrundfarbe.red, hintergrundfarbe.green,
                        hintergrundfarbe.blue, hintergrundfarbe.alpha);
  cairo_paint(cr);

  if (labelein) {
    int i = 0;
    for (i = 0; i <= maxzaehler; i++) {
      GtkAllocation alloc;
      gtk_widget_get_allocation(knotenlabel[i], &alloc);
      cairo_set_source_rgba(
          cr, knotenhintergrundfarbe.red, knotenhintergrundfarbe.green,
          knotenhintergrundfarbe.blue, knotenhintergrundfarbe.alpha);
      cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                      alloc.y - FensterRandOben - knotenrahmenabstand,
                      alloc.width + 2 * knotenrahmenabstand,
                      alloc.height + 2 * knotenrahmenabstand);
      cairo_fill(cr);
      cairo_set_source_rgba(cr, knotenrandfarbe.red, knotenrandfarbe.green,
                            knotenrandfarbe.blue, knotenrandfarbe.alpha);
      cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                      alloc.y - FensterRandOben - knotenrahmenabstand,
                      alloc.width + 2 * knotenrahmenabstand,
                      alloc.height + 2 * knotenrahmenabstand);
      cairo_set_line_width(cr, LinienDicke);
      cairo_stroke(cr);
    }
    if (ergebnisseanzeigen) {
      for (i = 0; i <= maxzaehlererg; i++) {
        GtkAllocation alloc;
        gtk_widget_get_allocation(ergebnislabel[i], &alloc);
        cairo_set_source_rgba(
            cr, knotenhintergrundfarbe.red, knotenhintergrundfarbe.green,
            knotenhintergrundfarbe.blue, knotenhintergrundfarbe.alpha);
        cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                        alloc.y - FensterRandOben - knotenrahmenabstand,
                        alloc.width + 2 * knotenrahmenabstand,
                        alloc.height + 2 * knotenrahmenabstand);
        cairo_fill(cr);
        cairo_set_source_rgba(cr, knotenrandfarbe.red, knotenrandfarbe.green,
                              knotenrandfarbe.blue, knotenrandfarbe.alpha);
        cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                        alloc.y - FensterRandOben - knotenrahmenabstand,
                        alloc.width + 2 * knotenrahmenabstand,
                        alloc.height + 2 * knotenrahmenabstand);
        cairo_set_line_width(cr, LinienDicke);
        cairo_stroke(cr);
      }
    }
    if (ergebnissewskanzeigen) {
      for (i = 0; i <= maxzaehlererg; i++) {
        if (bruchou && bruch) {
          GtkAllocation allocz;
          GtkAllocation allocn;
          gtk_widget_get_allocation(ergebniszaehlerlabel[i], &allocz);
          gtk_widget_get_allocation(ergebnisnennerlabel[i], &allocn);
          int bruchhoehe = allocz.height + allocn.height;
          int bruchbreite =
              ((allocz.width > allocn.width) ? allocz.width : allocn.width);
          int bruchx = ((allocz.x < allocn.x) ? allocz.x : allocn.x);
          int bruchy = allocz.y;
          cairo_set_source_rgba(
              cr, knotenhintergrundfarbe.red, knotenhintergrundfarbe.green,
              knotenhintergrundfarbe.blue, knotenhintergrundfarbe.alpha);
          cairo_rectangle(cr, bruchx - FensterRandLinks - knotenrahmenabstand,
                          bruchy - FensterRandOben - knotenrahmenabstand,
                          bruchbreite + 2 * knotenrahmenabstand,
                          bruchhoehe + 2 * knotenrahmenabstand);
          cairo_fill(cr);
          cairo_set_source_rgba(cr, knotenrandfarbe.red, knotenrandfarbe.green,
                                knotenrandfarbe.blue, knotenrandfarbe.alpha);
          cairo_rectangle(cr, bruchx - FensterRandLinks - knotenrahmenabstand,
                          bruchy - FensterRandOben - knotenrahmenabstand,
                          bruchbreite + 2 * knotenrahmenabstand,
                          bruchhoehe + 2 * knotenrahmenabstand);
          cairo_set_line_width(cr, LinienDicke);
          cairo_stroke(cr);
        } else {
          GtkAllocation alloc;
          gtk_widget_get_allocation(ergebniswsklabel[i], &alloc);
          cairo_set_source_rgba(
              cr, knotenhintergrundfarbe.red, knotenhintergrundfarbe.green,
              knotenhintergrundfarbe.blue, knotenhintergrundfarbe.alpha);
          cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                          alloc.y - FensterRandOben - knotenrahmenabstand,
                          alloc.width + 2 * knotenrahmenabstand,
                          alloc.height + 2 * knotenrahmenabstand);
          cairo_fill(cr);
          cairo_set_source_rgba(cr, knotenrandfarbe.red, knotenrandfarbe.green,
                                knotenrandfarbe.blue, knotenrandfarbe.alpha);
          cairo_rectangle(cr, alloc.x - FensterRandLinks - knotenrahmenabstand,
                          alloc.y - FensterRandOben - knotenrahmenabstand,
                          alloc.width + 2 * knotenrahmenabstand,
                          alloc.height + 2 * knotenrahmenabstand);
          cairo_set_line_width(cr, LinienDicke);
          cairo_stroke(cr);
        }
      }
    }
  }

  if (labelein) {
    wsk_layout_zeichnen(cr);
    return;
  }

  /* Eingabeansicht: Zweige direkt zwischen den sichtbaren Eingabefeldern. */
  char unten[22];
  sprintf(unten, "-%i", anzahlknoteninstufe(0) - 1);
  int last = knotenexistiert(unten);
  double root = (eingabe_y_position(y[0]) + eingabe_y_position(y[last >= 0 ? last : 0]) +
                 (baum_vertikal ? KnotenBreite : KnotenHoehe)) / 2.;
  cairo_set_source_rgba(cr, zweigfarbe.red, zweigfarbe.green,
                       zweigfarbe.blue, zweigfarbe.alpha);
  cairo_set_line_width(cr, LinienDicke);
  for (int i = 0; i <= maxzaehler; i++) {
    GtkAllocation ziel, quelle;
    gtk_widget_get_allocation(textfeld[i], &ziel);
    int stage = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    double x1 = LayoutRandLinks + (baum_vertikal ? root : 0);
    double y1 = LayoutRandOben + (baum_vertikal ? 0 : root);
    if (stage > 0) {
      int parent = knotenexistiert(gtk_widget_get_name(*vorgaenger[i]));
      if (parent < 0) continue;
      gtk_widget_get_allocation(textfeld[parent], &quelle);
      x1 = quelle.x - FensterRandLinks + (baum_vertikal ? quelle.width / 2. : quelle.width);
      y1 = quelle.y - FensterRandOben + (baum_vertikal ? quelle.height : quelle.height / 2.);
    }
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, ziel.x - FensterRandLinks + (baum_vertikal ? ziel.width / 2. : 0),
                  ziel.y - FensterRandOben + (baum_vertikal ? 0 : ziel.height / 2.));
    cairo_stroke(cr);
  }

  if (widget && data && !arboretum_scroll_geplant) {
    arboretum_scroll_geplant = TRUE;
    g_idle_add(arboretum_scroll_aktualisieren, NULL);
  }
}
