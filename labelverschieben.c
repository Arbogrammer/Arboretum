void labelverschieben(gpointer data) {
  if (!labelein)
    return;
  int w, h;
  klbmax = 0;
  for (int i = 0; i <= maxzaehler; i++) {
    wsk_messen(knotenlabel[i], &w, &h);
    klbmax = MAX(klbmax, w);
  }
  klbmax += 2 * paddingk;
  wsk_messen(knotenlabel[0], &w, &KnotenLabelHoehe);
  ErgebnisLabelBreite = WahrscheinlichkeitErgebnisLabelBreite = 0;
  for (int i = 0; i <= maxzaehlererg; i++) {
    wsk_messen(ergebnislabel[i], &w, &h);
    ErgebnisLabelBreite = MAX(ErgebnisLabelBreite, w);
    if (bruch && bruchou) {
      int nw, nh;
      wsk_messen(ergebniszaehlerlabel[i], &w, &h);
      wsk_messen(ergebnisnennerlabel[i], &nw, &nh);
      w = MAX(w, nw);
    } else
      wsk_messen(ergebniswsklabel[i], &w, &h);
    WahrscheinlichkeitErgebnisLabelBreite =
        MAX(WahrscheinlichkeitErgebnisLabelBreite, w);
  }
  wsk_layout_berechnen();
  wsklayout_width = wsklayout_height = 1;
  for (int i = 0; i <= maxzaehler; i++) {
    WskLayout *p = &wsklayout[i];
    wsk_messen(knotenlabel[i], &w, &h);
    double x = baum_vertikal ? p->x2 - w / 2. : p->x2 + (klbmax - w) / 2.;
    double yy =
        baum_vertikal
            ? p->y2 +
                  (MAX(KnotenHoehe, KnotenLabelHoehe + 2 * paddingk) - h) / 2.
            : p->y2 - h / 2.;
    wsk_place(data, knotenlabel[i], x, yy, w, h);
    if (bruch && bruchou) {
      wsk_place(data, zaehlerlabel[i], p->cx - p->zw / 2., p->cy - p->h / 2.,
                p->zw, p->zh);
      wsk_place(data, nennerlabel[i], p->cx - p->nw / 2.,
                p->cy - p->h / 2. + p->zh, p->nw, p->nh);
    } else {
      GtkWidget *label = wahrscheinlichkeitlabel[i];
      gtk_layout_move(GTK_LAYOUT(data), label, p->cx - p->w / 2.,
                      p->cy - p->h / 2.);
      graphene_point_t center = GRAPHENE_POINT_INIT(p->cx, p->cy);
      graphene_point_t origin = GRAPHENE_POINT_INIT(-p->w / 2., -p->h / 2.);
      GskTransform *t = gsk_transform_translate(NULL, &center);
      t = gsk_transform_rotate(t, p->angle * 180 / G_PI);
      t = gsk_transform_translate(t, &origin);
      gtk_fixed_set_child_transform(GTK_FIXED(data), label, t);
      gsk_transform_unref(t);
      winkel[i] = -p->angle * 180 / G_PI;
      double *stored_angle = g_new(double, 1);
      *stored_angle = winkel[i];
      g_object_set_data_full(G_OBJECT(label), "arboretum-angle", stored_angle,
                             g_free);
      breitevordrehung[i] = p->w;
      hoehevordrehung[i] = p->h;
      double rw = fabs(cos(p->angle)) * p->w + fabs(sin(p->angle)) * p->h;
      double rh = fabs(sin(p->angle)) * p->w + fabs(cos(p->angle)) * p->h;
      wsk_extent(p->cx - rw / 2, p->cy - rh / 2, rw, rh);
    }
  }
  for (int i = 0; i <= maxzaehlererg; i++) {
    const char *name = gtk_widget_get_name(textfeldErgebnis[i]);
    g_autofree char *leafname = g_strndup(name, strrchr(name, '-') - name);
    int leaf = knotenexistiert(leafname);
    double pos = leaf >= 0 ? wsklayout[leaf].position : yerg[i];
    double x = FensterRandLinks + RandLinks +
               (maxStufe + 1) * (StufenBreite + klbmax) + ErgebnisAbstand;
    double yy = FensterRandOben + RandOben + pos + KnotenHoehe / 2.;
    wsk_messen(ergebnislabel[i], &w, &h);
    if (baum_vertikal) {
      x = FensterRandLinks + RandLinks + pos + KnotenBreite / 2.;
      yy = wsk_stufe_y(maxStufe) +
           MAX(KnotenHoehe, KnotenLabelHoehe + 2 * paddingk) + ErgebnisAbstand +
           h / 2.;
    }
    if (ergebnisseanzeigen)
      wsk_place(data, ergebnislabel[i], baum_vertikal ? x - w / 2. : x,
                yy - h / 2., w, h);
    if (ergebnisseanzeigen) {
      if (baum_vertikal)
        yy += h + ErgebnisAbstand;
      else
        x += ErgebnisLabelBreite + ErgebnisAbstand;
    }
    if (ergebnissewskanzeigen) {
      if (bruch && bruchou) {
        int nw, nh;
        wsk_messen(ergebniszaehlerlabel[i], &w, &h);
        wsk_messen(ergebnisnennerlabel[i], &nw, &nh);
        double cx =
            baum_vertikal ? x : x + WahrscheinlichkeitErgebnisLabelBreite / 2.;
        wsk_place(data, ergebniszaehlerlabel[i], cx - w / 2.,
                  yy - (h + nh) / 2., w, h);
        wsk_place(data, ergebnisnennerlabel[i], cx - nw / 2.,
                  yy - (h + nh) / 2. + h, nw, nh);
      } else {
        wsk_messen(ergebniswsklabel[i], &w, &h);
        wsk_place(data, ergebniswsklabel[i], baum_vertikal ? x - w / 2. : x,
                  yy - h / 2., w, h);
      }
    }
  }
  wsk_layout_groesse(data);
  if (wsklayout_hinweis) {
    gtk_widget_set_visible(wsklayout_hinweis,
                           wskautomatik && wsklayout_kollisionen > 0);
    if (wskautomatik && wsklayout_kollisionen > 0) {
      g_autofree char *text = g_strdup_printf(
          "Automatik: %d Überschneidungen verbleiben. Bitte den Stufenabstand "
          "erhöhen oder die manuelle Verschiebung verringern.",
          wsklayout_kollisionen);
      gtk_label_set_text(GTK_LABEL(wsklayout_hinweis), text);
    }
  }
}
