/* A shared pair of labels is used on screen and by every exporter. The extra
 * margins are derived, so turning headings off restores the original geometry. */
static void ueberschrift_vorbereiten(gpointer data) {
  static const char *texte[][2] = {
      {"", ""}, {"ω", "P(ω)"}, {"ω", "P({ω})"}, {"{ω}", "P({ω})"}};
  ueberschrift_rand_links = ueberschrift_rand_oben = 0;
  for (int k = 0; k < 2; k++) {
    if (!ueberschrift_label[k]) {
      ueberschrift_label[k] = gtk_label_new("");
      gtk_layout_put(GTK_LAYOUT(data), ueberschrift_label[k], 0, 0);
    }
    const char *text = ueberschrift_modus == 4 ? ueberschrift_eigen[k]
                                               : texte[ueberschrift_modus][k];
    gtk_label_set_text(GTK_LABEL(ueberschrift_label[k]), text);
    PangoAttrList *attrs = pango_attr_list_new();
    PangoFontDescription *font = pango_font_description_from_string(schriftart);
    pango_attr_list_insert(attrs, pango_attr_font_desc_new(font));
    pango_font_description_free(font);
    pango_attr_list_insert(attrs, pango_attr_foreground_new(
        schriftfarbe.red * 65535, schriftfarbe.green * 65535, schriftfarbe.blue * 65535));
    pango_attr_list_insert(attrs, pango_attr_foreground_alpha_new(schriftfarbe.alpha * 65535));
    gtk_label_set_attributes(GTK_LABEL(ueberschrift_label[k]), attrs);
    pango_attr_list_unref(attrs);
    gboolean sichtbar = ueberschrift_modus && *text &&
                        (k == 0 ? ergebnisseanzeigen : ergebnissewskanzeigen);
    gtk_widget_set_visible(ueberschrift_label[k], sichtbar);
    ueberschrift_breite[k] = ueberschrift_hoehe[k] = 0;
    if (sichtbar) {
      wsk_messen(ueberschrift_label[k], &ueberschrift_breite[k], &ueberschrift_hoehe[k]);
      if (baum_vertikal)
        ueberschrift_rand_links = MAX(ueberschrift_rand_links, ueberschrift_breite[k] + 20);
      else
        ueberschrift_rand_oben = MAX(ueberschrift_rand_oben, ueberschrift_hoehe[k] + 20);
    }
  }
  if (baum_vertikal && ueberschrift_rand_links) {
    int ueberhang = 0;
    for (int i = 0; i <= maxzaehlererg; i++) {
      int w, h;
      if (ergebnisseanzeigen) {
        wsk_messen(labelein ? ergebnislabel[i] : textfeldErgebnis[i], &w, &h);
        ueberhang = MAX(ueberhang, w / 2);
      }
      if (ergebnissewskanzeigen) {
        wsk_messen(labelein ? (bruch && bruchou ? ergebniszaehlerlabel[i] : ergebniswsklabel[i])
                            : textfeldErgebnisWahrscheinlichkeit[i], &w, &h);
        ueberhang = MAX(ueberhang, w / 2);
        if (labelein && bruch && bruchou) {
          wsk_messen(ergebnisnennerlabel[i], &w, &h);
          ueberhang = MAX(ueberhang, w / 2);
        }
      }
    }
    ueberschrift_rand_links += ueberhang;
  }
}

static void ueberschrift_positionieren(gpointer data) {
  for (int k = 0; k < 2; k++) {
    if (!ueberschrift_label[k] || !gtk_widget_get_visible(ueberschrift_label[k])) continue;
    GtkWidget *wert = !labelein
        ? (k == 0 ? textfeldErgebnis[0] : textfeldErgebnisWahrscheinlichkeit[0])
        : (k == 0 ? ergebnislabel[0] : bruch && bruchou ? ergebniszaehlerlabel[0] : ergebniswsklabel[0]);
    double *pos = g_object_get_data(G_OBJECT(wert), "arboretum-layout-position");
    if (!pos) continue;
    int w, h;
    wsk_messen(wert, &w, &h);
    if (labelein && k == 1 && bruch && bruchou) {
      int nw, nh;
      wsk_messen(ergebnisnennerlabel[0], &nw, &nh);
      h += nh;
    }
    double x, yy;
    if (baum_vertikal) {
      x = FensterRandLinks + RandLinks + MAX(ueberschrift_breite[0], ueberschrift_breite[1]) - ueberschrift_breite[k];
      yy = pos[1] + (h - ueberschrift_hoehe[k]) / 2.;
    } else {
      int spaltenbreite = labelein
          ? (k == 0 ? ErgebnisLabelBreite : WahrscheinlichkeitErgebnisLabelBreite)
          : (k == 0 ? ErgebnisBreite : WahrscheinlichkeitErgebnisBreite);
      x = pos[0] + (spaltenbreite - ueberschrift_breite[k]) / 2.;
      if (labelein && k == 1 && bruch && bruchou)
        x = pos[0] + (w - ueberschrift_breite[k]) / 2.;
      yy = FensterRandOben + RandOben;
    }
    wsk_place(data, ueberschrift_label[k], x, yy, ueberschrift_breite[k], ueberschrift_hoehe[k]);
  }
}

static void ueberschrift_eigene_anzeigen(GObject *wahl, GParamSpec *pspec, gpointer felder) {
  gtk_widget_set_visible(GTK_WIDGET(felder),
      gtk_drop_down_get_selected(GTK_DROP_DOWN(wahl)) == 4);
}

static void ueberschrift_text_bereinigen(GtkEditable *editable, gpointer unused) {
  const char *text = gtk_editable_get_text(editable);
  g_autofree char *sauber = g_strdup(text);
  for (char *p = sauber; *p; p++)
    if ((unsigned char)*p < 32 || *p == 127) *p = ' ';
  if (strcmp(text, sauber)) gtk_editable_set_text(editable, sauber);
}

static void ueberschrift_dialog(GtkWidget *button, gpointer data) {
  const char *optionen[] = {"Keine", "ω  |  P(ω)", "ω  |  P({ω})",
                            "{ω}  |  P({ω})", "Eigene …", NULL};
  GtkWidget *dialog = gtk_dialog_new_with_buttons("Ergebnisüberschriften", GTK_WINDOW(window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      "Abbrechen", GTK_RESPONSE_CANCEL, "Übernehmen", GTK_RESPONSE_OK, NULL);
  GtkWidget *box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_widget_set_margin_start(box, 16);
  gtk_widget_set_margin_end(box, 16);
  gtk_widget_set_margin_top(box, 16);
  gtk_widget_set_margin_bottom(box, 16);
  gtk_box_set_spacing(GTK_BOX(box), 12);
  GtkWidget *wahl = gtk_drop_down_new_from_strings(optionen);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(wahl), ueberschrift_modus);
  gtk_box_append(GTK_BOX(box), wahl);
  GtkWidget *felder = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(felder), 8);
  gtk_grid_set_column_spacing(GTK_GRID(felder), 12);
  GtkWidget *eingabe[2];
  for (int k = 0; k < 2; k++) {
    GtkWidget *label = gtk_label_new(k == 0 ? "Ergebnis" : "Wahrscheinlichkeit");
    eingabe[k] = gtk_entry_new();
    g_signal_connect(eingabe[k], "changed", G_CALLBACK(ueberschrift_text_bereinigen), NULL);
    gtk_entry_set_max_length(GTK_ENTRY(eingabe[k]), 160);
    gtk_entry_set_text(GTK_ENTRY(eingabe[k]), ueberschrift_modus == 4 || *ueberschrift_eigen[k]
        ? ueberschrift_eigen[k] : k == 0 ? "Ergebnis" : "Wahrscheinlichkeit");
    gtk_grid_attach(GTK_GRID(felder), label, 0, k, 1, 1);
    gtk_grid_attach(GTK_GRID(felder), eingabe[k], 1, k, 1, 1);
  }
  gtk_box_append(GTK_BOX(box), felder);
  g_signal_connect(wahl, "notify::selected", G_CALLBACK(ueberschrift_eigene_anzeigen), felder);
  ueberschrift_eigene_anzeigen(G_OBJECT(wahl), NULL, felder);
  gtk_box_append(GTK_BOX(box), gtk_label_new("Gilt für das Fenster und alle Exportformate."));
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    int modus = gtk_drop_down_get_selected(GTK_DROP_DOWN(wahl));
    const char *a = gtk_entry_get_text(GTK_ENTRY(eingabe[0]));
    const char *b = gtk_entry_get_text(GTK_ENTRY(eingabe[1]));
    if (modus != ueberschrift_modus || (modus == 4 &&
        (strcmp(a, ueberschrift_eigen[0]) || strcmp(b, ueberschrift_eigen[1])))) {
      tempspeichern();
      ueberschrift_modus = modus;
      if (modus == 4) {
        g_strlcpy(ueberschrift_eigen[0], a, sizeof(ueberschrift_eigen[0]));
        g_strlcpy(ueberschrift_eigen[1], b, sizeof(ueberschrift_eigen[1]));
      }
      dateiveraendert++;
      baumrichtung_aktualisieren(data);
    }
  }
  gtk_widget_destroy(dialog);
}
