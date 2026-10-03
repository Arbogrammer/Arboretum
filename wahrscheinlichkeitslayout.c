/* Gemeinsame Geometrie für Anzeige, Kollisionsprüfung und Export.
 * Die Optimierung verändert ausschließlich diese abgeleiteten Koordinaten;
 * y/yerg und die vom Benutzer eingestellten Abstände bleiben erhalten. */
typedef struct {
  double x1, y1, x2, y2;
  double cx, cy, w, h, angle, shift;
  int parent, stage, rank, siblings;
  int zw, zh, nw, nh;
  double position;
  gboolean below, leaf;
} WskLayout;

static WskLayout *wsklayout;
static int wsklayout_count;
static int wsklayout_width, wsklayout_height;
static int wsklayout_kollisionen;
static GtkWidget *wsklayout_hinweis;

static int wsk_stufe_y(int stage) {
  return FensterRandOben + RandOben + MAX(1, StufenBreite) +
         stage * (MAX(1, StufenBreite) +
                  MAX(KnotenHoehe, KnotenLabelHoehe + 2 * paddingk));
}

static void wsk_eltern_zentrieren(void) {
  g_autofree double *lo = g_new(double, wsklayout_count);
  g_autofree double *hi = g_new(double, wsklayout_count);
  for (int i = 0; i < wsklayout_count; i++) {
    lo[i] = G_MAXDOUBLE;
    hi[i] = -G_MAXDOUBLE;
  }
  /* Eltern werden vor ihren Kindern angelegt und geladen. */
  for (int i = wsklayout_count - 1; i >= 0; i--) {
    WskLayout *p = &wsklayout[i];
    if (lo[i] != G_MAXDOUBLE)
      p->position = (lo[i] + hi[i]) / 2;
    if (p->parent >= 0) {
      lo[p->parent] = MIN(lo[p->parent], p->position);
      hi[p->parent] = MAX(hi[p->parent], p->position);
    }
  }
}

static int wsk_position_vergleichen(gconstpointer a, gconstpointer b) {
  double delta =
      wsklayout[*(const int *)a].position - wsklayout[*(const int *)b].position;
  return (delta > 0) - (delta < 0);
}

static void wsk_messen(GtkWidget *widget, int *w, int *h) {
  gtk_widget_measure(widget, GTK_ORIENTATION_HORIZONTAL, -1, NULL, w, NULL,
                     NULL);
  gtk_widget_measure(widget, GTK_ORIENTATION_VERTICAL, -1, NULL, h, NULL, NULL);
}

static void wsk_zweige_berechnen(void) {
  double first = G_MAXDOUBLE, last = -G_MAXDOUBLE;
  for (int i = 0; i < wsklayout_count; i++)
    if (wsklayout[i].parent < 0) {
      first = MIN(first, wsklayout[i].position);
      last = MAX(last, wsklayout[i].position);
    }
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    double parent_y =
        p->parent < 0 ? (first + last) / 2 : wsklayout[p->parent].position;
    if (baum_vertikal) {
      p->x1 = FensterRandLinks + RandLinks + parent_y + KnotenBreite / 2.;
      p->x2 = FensterRandLinks + RandLinks + p->position + KnotenBreite / 2.;
      p->y1 = p->parent < 0
                  ? FensterRandOben + RandOben
                  : wsk_stufe_y(p->stage - 1) +
                        MAX(KnotenHoehe, KnotenLabelHoehe + 2 * paddingk);
      p->y2 = wsk_stufe_y(p->stage);
    } else {
      p->x1 = FensterRandLinks + RandLinks + p->stage * (StufenBreite + klbmax);
      p->x2 = p->x1 + MAX(1, StufenBreite);
      p->y1 = FensterRandOben + RandOben + parent_y + KnotenHoehe / 2.;
      p->y2 = FensterRandOben + RandOben + p->position + KnotenHoehe / 2.;
    }
  }
}

static void wsk_label_berechnen(WskLayout *p, double shift) {
  double dx = p->x2 - p->x1, dy = p->y2 - p->y1;
  double length = MAX(1., hypot(dx, dy));
  double ux = dx / length, uy = dy / length;
  /* In der vertikalen Ansicht entspricht oben links, unten rechts. */
  double nx = baum_vertikal ? -uy : uy;
  double ny = baum_vertikal ? ux : -ux;
  double distance;
  p->angle = (bruch && bruchou) ? 0 : atan2(dy, dx);
  if (bruch && bruchou)
    distance = fabs(nx) * p->w / 2 + fabs(ny) * p->h / 2;
  else
    distance = p->h / 2;
  distance += padding + LinienDicke / 2. + 2;
  if (p->below)
    distance = -distance;
  double t = .5 + (wskverschiebung + shift) / MAX(1., (double)StufenBreite);
  p->cx = p->x1 + t * dx + nx * distance;
  p->cy = p->y1 + t * dy + ny * distance;
  p->shift = shift;
}

/* Segment gegen das um den Labelmittelpunkt gedrehte Textrechteck.
 * Liang–Barsky in lokalen Koordinaten, einschließlich Sicherheitsabstand. */
static gboolean wsk_schnitt(const WskLayout *p, const WskLayout *branch) {
  double c = cos(p->angle), s = sin(p->angle);
  double ax = (branch->x1 - p->cx) * c + (branch->y1 - p->cy) * s;
  double ay = -(branch->x1 - p->cx) * s + (branch->y1 - p->cy) * c;
  double bx = (branch->x2 - p->cx) * c + (branch->y2 - p->cy) * s;
  double by = -(branch->x2 - p->cx) * s + (branch->y2 - p->cy) * c;
  double margin = LinienDicke / 2. + 1;
  double half[] = {p->w / 2 + margin, p->h / 2 + margin};
  double a[] = {ax, ay}, d[] = {bx - ax, by - ay};
  double low = 0, high = 1;
  for (int k = 0; k < 2; k++) {
    if (fabs(d[k]) < 1e-9) {
      if (fabs(a[k]) > half[k])
        return FALSE;
    } else {
      double t1 = (-half[k] - a[k]) / d[k];
      double t2 = (half[k] - a[k]) / d[k];
      low = MAX(low, MIN(t1, t2));
      high = MIN(high, MAX(t1, t2));
      if (low > high)
        return FALSE;
    }
  }
  return TRUE;
}

static int wsk_kollision(int i) {
  WskLayout *p = &wsklayout[i];
  if (p->w <= 0)
    return -1;
  for (int j = 0; j < wsklayout_count; j++) {
    /* Normalerweise nur dieselbe Stufe; Nachbarstufen bleiben für breite
     * Beschriftungen und manuelle Verschiebungen ebenfalls berücksichtigt. */
    WskLayout *b = &wsklayout[j];
    double radius = hypot(p->w, p->h) / 2 + LinienDicke + 2;
    if (p->cx + radius < MIN(b->x1, b->x2) ||
        p->cx - radius > MAX(b->x1, b->x2) ||
        p->cy + radius < MIN(b->y1, b->y2) ||
        p->cy - radius > MAX(b->y1, b->y2))
      continue;
    if (wsk_schnitt(p, b))
      return j;
  }
  /* Auch zwei ausweichende Wahrscheinlichkeiten dürfen nicht aufeinander
   * geschoben werden. Separating-axis test für die gedrehten Rechtecke. */
  for (int j = 0; j < wsklayout_count; j++) {
    if (i == j)
      continue;
    WskLayout *q = &wsklayout[j];
    if (q->w <= 0)
      continue;
    double radius = (hypot(p->w, p->h) + hypot(q->w, q->h)) / 2 + 2;
    if (fabs(p->cx - q->cx) > radius || fabs(p->cy - q->cy) > radius)
      continue;
    gboolean separated = FALSE;
    double axes[] = {p->angle, p->angle + G_PI / 2, q->angle,
                     q->angle + G_PI / 2};
    for (int k = 0; k < 4; k++) {
      double a = axes[k];
      double distance =
          fabs((p->cx - q->cx) * cos(a) + (p->cy - q->cy) * sin(a));
      double extent =
          (fabs(cos(p->angle - a)) * p->w + fabs(sin(p->angle - a)) * p->h +
           fabs(cos(q->angle - a)) * q->w + fabs(sin(q->angle - a)) * q->h) /
              2 +
          2;
      if (distance > extent) {
        separated = TRUE;
        break;
      }
    }
    if (!separated)
      return j;
  }
  return -1;
}

static void wsk_layout_berechnen(void) {
  wsklayout_count = maxzaehler + 1;
  wsklayout = g_renew(WskLayout, wsklayout, wsklayout_count);
  g_autofree int *counts = g_new0(int, wsklayout_count + 1);
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    memset(p, 0, sizeof(*p));
    p->stage = zeichenzaehlen(gtk_widget_get_name(textfeld[i]), '-') - 1;
    p->parent =
        p->stage ? knotenexistiert(gtk_widget_get_name(*vorgaenger[i])) : -1;
    p->position = y[i];
    counts[p->parent + 1]++;
    if (bruch && bruchou) {
      wsk_messen(zaehlerlabel[i], &p->zw, &p->zh);
      wsk_messen(nennerlabel[i], &p->nw, &p->nh);
      p->w = MAX(p->zw, p->nw);
      p->h = p->zh + p->nh;
    } else {
      int w, h;
      wsk_messen(wahrscheinlichkeitlabel[i], &w, &h);
      p->w = w;
      p->h = h;
    }
  }
  /* Geschwisterreihenfolge ergibt sich aus der Knotennummer, nicht aus der
   * Einfügereihenfolge (nachträglich ergänzte Zweige liegen im Array hinten).
   */
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    const char *name = gtk_widget_get_name(textfeld[i]);
    p->rank = atoi(strrchr(name, '-') + 1);
    p->siblings = counts[p->parent + 1];
    p->leaf = counts[i + 1] == 0;
    int middle = 2 * p->rank - (p->siblings - 1);
    p->below =
        wskseite == 1 ||
        (wskseite == 2 && (middle > 0 || (middle == 0 && wskmitteunten)));
  }
  if (wskautomatik) {
    /* Die Ausgangsabstände dürfen auch bei großen Schriften und Brüchen
     * keine übereinanderliegenden Blatt-/Ergebnisbeschriftungen erzwingen. */
    int minimum = 0;
    for (int i = 0; i < wsklayout_count; i++) {
      int w, h;
      wsk_messen(knotenlabel[i], &w, &h);
      minimum = MAX(minimum, baum_vertikal ? w : h);
    }
    for (int i = 0; i <= maxzaehlererg; i++) {
      int w, h;
      if (ergebnisseanzeigen) {
        wsk_messen(ergebnislabel[i], &w, &h);
        minimum = MAX(minimum, baum_vertikal ? w : h);
      }
      if (ergebnissewskanzeigen) {
        if (bruch && bruchou) {
          int nw, nh;
          wsk_messen(ergebniszaehlerlabel[i], &w, &h);
          wsk_messen(ergebnisnennerlabel[i], &nw, &nh);
          w = MAX(w, nw);
          h += nh;
        } else
          wsk_messen(ergebniswsklabel[i], &w, &h);
        minimum = MAX(minimum, baum_vertikal ? w : h);
      }
    }
    minimum += 2 * knotenrahmenabstand + 4;
    g_autofree int *leaves = g_new(int, wsklayout_count);
    int n = 0;
    for (int i = 0; i < wsklayout_count; i++)
      if (wsklayout[i].leaf)
        leaves[n++] = i;
    qsort(leaves, n, sizeof(int), wsk_position_vergleichen);
    double extra = 0;
    for (int i = 1; i < n; i++) {
      WskLayout *p = &wsklayout[leaves[i]];
      p->position += extra;
      double delta =
          MAX(0., wsklayout[leaves[i - 1]].position + minimum - p->position);
      p->position += delta;
      extra += delta;
    }
    if (extra > 0)
      wsk_eltern_zentrieren();
  }
  wsklayout_kollisionen = 0;
  /* Begrenzte Iteration schützt auch bei extremen manuellen Werten vor
   * endlosen Vergrößerungen. Jeder Durchlauf startet mit minimalem Versatz. */
  for (int pass = 0; pass <= 128; pass++) {
    wsk_zweige_berechnen();
    for (int i = 0; i < wsklayout_count; i++)
      wsk_label_berechnen(&wsklayout[i], 0);
    int failed = -1, obstacle = -1;
    wsklayout_kollisionen = 0;
    for (int i = 0; i < wsklayout_count; i++) {
      WskLayout *p = &wsklayout[i];
      wsk_label_berechnen(p, 0);
      if (!wskautomatik || !wskanzeigen)
        continue;
      int collision = wsk_kollision(i);
      double limit = MIN(24., MAX(0., StufenBreite * .15));
      for (double shift = 2; collision >= 0 && shift <= limit; shift += 2) {
        wsk_label_berechnen(p, shift);
        collision = wsk_kollision(i);
      }
      if (collision >= 0) {
        wsklayout_kollisionen++;
        if (failed < 0 && i != collision &&
            fabs(p->position - wsklayout[collision].position) > 1) {
          failed = i;
          obstacle = collision;
        }
      }
    }
    if (failed < 0 || pass == 128)
      break;
    double boundary =
        (wsklayout[failed].position + wsklayout[obstacle].position) / 2;
    for (int i = 0; i < wsklayout_count; i++)
      if (wsklayout[i].leaf && wsklayout[i].position > boundary)
        wsklayout[i].position += 8;
    wsk_eltern_zentrieren();
  }
}

static void wsk_extent(double x, double y, double w, double h) {
  wsklayout_width =
      MAX(wsklayout_width, (int)ceil(x + w + RandRechts + knotenrahmenabstand));
  wsklayout_height =
      MAX(wsklayout_height, (int)ceil(y + h + RandUnten + knotenrahmenabstand));
}

static void wsk_place(gpointer data, GtkWidget *widget, double x, double y,
                      int w, int h) {
  gtk_layout_move(GTK_LAYOUT(data), widget, x, y);
  wsk_extent(x, y, w, h);
}

static void wsk_layout_groesse(gpointer data) {
  if (!labelein || !wsklayout_count)
    return;
  gtk_layout_set_size(GTK_LAYOUT(data), wsklayout_width + FensterRandRechts,
                      wsklayout_height + FensterRandUnten);
  gtk_widget_set_size_request(da, MAX(1, wsklayout_width - FensterRandLinks),
                              MAX(1, wsklayout_height - FensterRandOben));
}

static void wsk_bruchstrich(cairo_t *cr, GtkWidget *z, GtkWidget *n) {
  if (!*gtk_label_get_text(GTK_LABEL(n))) return;
  double *zp = g_object_get_data(G_OBJECT(z), "arboretum-layout-position");
  double *np = g_object_get_data(G_OBJECT(n), "arboretum-layout-position");
  if (!zp || !np)
    return;
  int zw, zh, nw, nh;
  wsk_messen(z, &zw, &zh);
  wsk_messen(n, &nw, &nh);
  cairo_set_line_width(cr, 1);
  cairo_move_to(cr, MIN(zp[0], np[0]) - FensterRandLinks,
                np[1] - FensterRandOben);
  cairo_line_to(cr, MAX(zp[0] + zw, np[0] + nw) - FensterRandLinks,
                np[1] - FensterRandOben);
  cairo_stroke(cr);
}

static void wsk_layout_zeichnen(cairo_t *cr) {
  cairo_set_source_rgba(cr, zweigfarbe.red, zweigfarbe.green, zweigfarbe.blue,
                        zweigfarbe.alpha);
  cairo_set_line_width(cr, LinienDicke);
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    cairo_move_to(cr, p->x1 - FensterRandLinks, p->y1 - FensterRandOben);
    cairo_line_to(cr, p->x2 - FensterRandLinks, p->y2 - FensterRandOben);
  }
  cairo_stroke(cr);
  if (bruch && bruchou) {
    cairo_set_source_rgba(cr, schriftfarbe.red, schriftfarbe.green,
                          schriftfarbe.blue, schriftfarbe.alpha);
    if (wskanzeigen)
      for (int i = 0; i < wsklayout_count; i++)
        wsk_bruchstrich(cr, zaehlerlabel[i], nennerlabel[i]);
    if (ergebnissewskanzeigen)
      for (int i = 0; i <= maxzaehlererg; i++)
        wsk_bruchstrich(cr, ergebniszaehlerlabel[i], ergebnisnennerlabel[i]);
  }
}
