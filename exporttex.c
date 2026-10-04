/* Editable TikZ output. Coordinates use the same pixel geometry as GTK;
 * 1 px = 0.75 bp. LuaLaTeX/XeLaTeX preserve Unicode text. */
static const char *tex_number(double value) {
  /* Each statement below uses fewer than 16 numbers. */
  static char buffers[16][G_ASCII_DTOSTR_BUF_SIZE];
  static unsigned slot;
  char *buffer = buffers[slot++ % G_N_ELEMENTS(buffers)];
  return g_ascii_formatd(buffer, G_ASCII_DTOSTR_BUF_SIZE, "%.6f", value);
}

static gchar *tex_text(const char *text) {
  GString *out = g_string_new(NULL);
  gboolean overline = FALSE;
  for (const char *p = text; p && *p;) {
    const char *next = g_utf8_next_char(p);
    gboolean marked = *next && g_utf8_get_char(next) == 0x0305;
    if (marked != overline) {
      g_string_append(out, marked ? "\\ensuremath{\\overline{\\text{" : "}}}");
      overline = marked;
    }
    switch (*p) {
    case '\\': g_string_append(out, "\\textbackslash{}"); break;
    case '{': case '}': case '$': case '&': case '#': case '%': case '_':
      g_string_append_c(out, '\\'); g_string_append_c(out, *p); break;
    case '~': g_string_append(out, "\\textasciitilde{}"); break;
    case '^': g_string_append(out, "\\textasciicircum{}"); break;
    case '\n': case '\r': case '\t': g_string_append_c(out, ' '); break;
    default: g_string_append_len(out, p, next - p); break;
    }
    p = next;
    if (marked)
      while (*p && g_utf8_get_char(p) == 0x0305) p = g_utf8_next_char(p);
  }
  if (overline) g_string_append(out, "}}}");
  return g_string_free(out, FALSE);
}

static void tex_color(GString *out, const char *name, GdkRGBA color) {
  g_string_append_printf(out, "\\definecolor{%s}{rgb}{%s,%s,%s}\n", name,
                         tex_number(color.red), tex_number(color.green),
                         tex_number(color.blue));
}

static void tex_box(GString *out, double x, double y, double w, double h) {
  double pad = knotenrahmenabstand;
  g_string_append_printf(out,
      "\\path[fill=arbfill,fill opacity=%s,draw=arbborder,draw opacity=%s,"
      "line width=%sbp] (%s,%s) rectangle (%s,%s);\n",
      tex_number(knotenhintergrundfarbe.alpha), tex_number(knotenrandfarbe.alpha),
      tex_number(LinienDicke * .75), tex_number(x - pad), tex_number(y - pad),
      tex_number(x + w + pad), tex_number(y + h + pad));
}

static void tex_node(GString *out, double x, double y, double angle,
                      const char *text) {
  g_string_append_printf(out,
      "\\node[anchor=center,inner sep=0pt,outer sep=0pt,text=arbtext,"
      "text opacity=%s,rotate=%s] at (%s,%s) {%s};\n",
      tex_number(schriftfarbe.alpha), tex_number(angle),
      tex_number(x), tex_number(y), text);
}

static void tex_label(GString *out, GtkWidget *label, gboolean frame) {
  if (!label || !gtk_widget_get_visible(label)) return;
  double *pos = g_object_get_data(G_OBJECT(label), "arboretum-layout-position");
  if (!pos) return;
  int w, h;
  wsk_messen(label, &w, &h);
  double x = pos[0] - FensterRandLinks, y = pos[1] - FensterRandOben;
  if (frame) tex_box(out, x, y, w, h);
  g_autofree char *text = tex_text(arboretum_label_get_encoded_text(GTK_LABEL(label)));
  tex_node(out, x + w / 2., y + h / 2., 0, text);
}

static void tex_fraction(GString *out, GtkWidget *z, GtkWidget *n,
                          gboolean frame) {
  if (!z || !gtk_widget_get_visible(z)) return;
  const char *denominator = arboretum_label_get_encoded_text(GTK_LABEL(n));
  if (!*denominator) { tex_label(out, z, frame); return; }
  double *zp = g_object_get_data(G_OBJECT(z), "arboretum-layout-position");
  double *np = g_object_get_data(G_OBJECT(n), "arboretum-layout-position");
  if (!zp || !np) return;
  int zw, zh, nw, nh;
  wsk_messen(z, &zw, &zh);
  wsk_messen(n, &nw, &nh);
  double x = MIN(zp[0], np[0]) - FensterRandLinks;
  double y = zp[1] - FensterRandOben;
  double w = MAX(zp[0] + zw, np[0] + nw) - MIN(zp[0], np[0]);
  double h = np[1] + nh - zp[1];
  if (frame) tex_box(out, x, y, w, h);
  g_autofree char *a = tex_text(arboretum_label_get_encoded_text(GTK_LABEL(z)));
  g_autofree char *b = tex_text(denominator);
  g_autofree char *fraction = g_strdup_printf("$\\dfrac{\\text{%s}}{\\text{%s}}$", a, b);
  tex_node(out, x + w / 2., y + h / 2., 0, fraction);
}

gboolean exporttex(char *dateiname) {
  if (!labelein || wsklayout_count != maxzaehler + 1) {
    dateifehler("Export", dateiname, "Keine gültige Baumgeometrie vorhanden.");
    return FALSE;
  }
  GString *out = g_string_new(
      "% Arboretum: mit LuaLaTeX oder XeLaTeX kompilieren.\n"
      "% Den tikzpicture-Block können Sie in eigene Dokumente übernehmen.\n"
      "% Koordinaten: Pixel; Schriftmaße können von der Bildschirmansicht abweichen.\n"
      "\\documentclass[tikz,border=2bp]{standalone}\n"
      "\\usepackage{fontspec}\n\\usepackage{amsmath}\n"
      "\\IfFontExistsTF{DejaVu Sans}{\\setmainfont{DejaVu Sans}}"
      "{\\setmainfont{Latin Modern Sans}}\n");
  tex_color(out, "arbtext", schriftfarbe);
  tex_color(out, "arbbranch", zweigfarbe);
  tex_color(out, "arbborder", knotenrandfarbe);
  tex_color(out, "arbfill", knotenhintergrundfarbe);
  tex_color(out, "arbbackground", hintergrundfarbe);
  PangoFontDescription *font = pango_font_description_from_string(schriftart);
  double size = pango_font_description_get_size(font) / (double)PANGO_SCALE;
  if (pango_font_description_get_size_is_absolute(font)) size *= .75;
  if (size <= 0) size = 12;
  g_string_append_printf(out,
      "\\begin{document}\n\\begin{tikzpicture}[x=0.75bp,y=-0.75bp,"
      "every node/.style={font=\\fontsize{%s}{%s}\\selectfont%s}]\n",
      tex_number(size), tex_number(size * 1.2),
      pango_font_description_get_weight(font) >= PANGO_WEIGHT_BOLD ? "\\bfseries" : "");
  pango_font_description_free(font);
  if (hintergrundfarbe.alpha > 0)
    g_string_append_printf(out,
        "\\path[fill=arbbackground,fill opacity=%s] (0,0) rectangle (%s,%s);\n",
        tex_number(hintergrundfarbe.alpha),
        tex_number(wsklayout_width - FensterRandLinks),
        tex_number(wsklayout_height - FensterRandOben));
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    g_string_append_printf(out,
        "\\draw[draw=arbbranch,draw opacity=%s,line width=%sbp] "
        "(%s,%s) -- (%s,%s);\n",
        tex_number(zweigfarbe.alpha), tex_number(LinienDicke * .75),
        tex_number(p->x1 - FensterRandLinks), tex_number(p->y1 - FensterRandOben),
        tex_number(p->x2 - FensterRandLinks), tex_number(p->y2 - FensterRandOben));
    tex_label(out, knotenlabel[i], TRUE);
    if (wskanzeigen) {
      if (bruch && bruchou)
        tex_fraction(out, zaehlerlabel[i], nennerlabel[i], FALSE);
      else {
        g_autofree char *text = tex_text(arboretum_label_get_encoded_text(
            GTK_LABEL(wahrscheinlichkeitlabel[i])));
        tex_node(out, p->cx - FensterRandLinks, p->cy - FensterRandOben,
                  -p->angle * 180 / G_PI, text);
      }
    }
  }
  for (int i = 0; i <= maxzaehlererg; i++) {
    if (ergebnisseanzeigen) tex_label(out, ergebnislabel[i], TRUE);
    if (ergebnissewskanzeigen) {
      if (bruch && bruchou)
        tex_fraction(out, ergebniszaehlerlabel[i], ergebnisnennerlabel[i], TRUE);
      else tex_label(out, ergebniswsklabel[i], TRUE);
    }
  }
  g_string_append(out, "\\end{tikzpicture}\n\\end{document}\n");
  g_autoptr(GError) error = NULL;
  gboolean ok = g_file_set_contents_full(dateiname, out->str, out->len,
      G_FILE_SET_CONTENTS_CONSISTENT, 0666, &error);
  g_string_free(out, TRUE);
  if (!ok) dateifehler("Export", dateiname, error->message);
  return ok;
}
