/* Native Writer drawing objects, inspired by Arbogrammer/baum's ODT export.
 * The shared stored-ZIP writer avoids an external zip process/dependency.
 * The first, uncompressed mimetype entry is required by ODF. */
#define ODT_NS " xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"" \
  " xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\"" \
  " xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\"" \
  " xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\"" \
  " xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\"" \
  " xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\""

static const char *odt_number(double v) {
  static char buffers[16][G_ASCII_DTOSTR_BUF_SIZE];
  static unsigned slot;
  char *b = buffers[slot++ % G_N_ELEMENTS(buffers)];
  return g_ascii_formatd(b, G_ASCII_DTOSTR_BUF_SIZE, "%.6f", v);
}

static gchar *odt_color(GdkRGBA c) {
  return g_strdup_printf("#%02x%02x%02x", (int)round(c.red * 255),
                         (int)round(c.green * 255), (int)round(c.blue * 255));
}

static void odt_style(GString *out, const char *name, GdkRGBA stroke,
                      GdkRGBA fill, double width) {
  g_autofree char *s = odt_color(stroke), *f = odt_color(fill);
  g_string_append_printf(out,
      "<style:style style:name=\"%s\" style:family=\"graphic\">"
      "<style:graphic-properties draw:stroke=\"%s\" svg:stroke-color=\"%s\" "
      "svg:stroke-width=\"%scm\" svg:stroke-opacity=\"%s%%\" "
      "draw:fill=\"%s\" draw:fill-color=\"%s\" draw:opacity=\"%s%%\" "
      "fo:padding=\"0cm\" draw:textarea-vertical-align=\"middle\" "
      "draw:auto-grow-height=\"false\" draw:auto-grow-width=\"false\" "
      "fo:wrap-option=\"no-wrap\" style:wrap=\"run-through\" "
      "style:vertical-pos=\"from-top\" style:vertical-rel=\"page\" "
      "style:horizontal-pos=\"from-left\" style:horizontal-rel=\"page\"/>"
      "</style:style>", name, stroke.alpha > 0 ? "solid" : "none", s,
      odt_number(width), odt_number(stroke.alpha * 100),
      fill.alpha > 0 ? "solid" : "none", f, odt_number(fill.alpha * 100));
}

/* Preserve spaces, XML metacharacters, line breaks, and native overlines. */
static void odt_text(GString *out, const char *text) {
  for (const char *p = text; p && *p;) {
    const char *next = g_utf8_next_char(p);
    gboolean over = *next && g_utf8_get_char(next) == 0x0305;
    if (over) g_string_append(out, "<text:span text:style-name=\"Overline\">");
    if (*p == ' ') g_string_append(out, "<text:s/>");
    else if (*p == '\n') g_string_append(out, "<text:line-break/>");
    else if (*p == '\t') g_string_append(out, "<text:tab/>");
    else {
      g_autofree char *escaped = g_markup_escape_text(p, next - p);
      g_string_append(out, escaped);
    }
    if (over) g_string_append(out, "</text:span>");
    p = next;
    while (*p && g_utf8_get_char(p) == 0x0305) p = g_utf8_next_char(p);
  }
}

static void odt_line(GString *out, const char *style, double x1, double y1,
                     double x2, double y2, double unit) {
  g_string_append_printf(out,
      "<draw:line draw:style-name=\"%s\" svg:x1=\"%scm\" svg:y1=\"%scm\" "
      "svg:x2=\"%scm\" svg:y2=\"%scm\"/>", style,
      odt_number(1 + (x1 - FensterRandLinks) * unit),
      odt_number(1 + (y1 - FensterRandOben) * unit),
      odt_number(1 + (x2 - FensterRandLinks) * unit),
      odt_number(1 + (y2 - FensterRandOben) * unit));
}

static void odt_rect(GString *out, const char *style, double x, double y,
                     double w, double h, double unit) {
  g_string_append_printf(out,
      "<draw:rect draw:style-name=\"%s\" svg:x=\"%scm\" svg:y=\"%scm\" "
      "svg:width=\"%scm\" svg:height=\"%scm\"/>", style,
      odt_number(1 + (x - FensterRandLinks) * unit),
      odt_number(1 + (y - FensterRandOben) * unit),
      odt_number(w * unit), odt_number(h * unit));
}

static void odt_label(GString *out, GtkWidget *label, gboolean frame,
                      WskLayout *branch, double unit) {
  if (!label || !gtk_widget_get_visible(label)) return;
  double *pos = g_object_get_data(G_OBJECT(label), "arboretum-layout-position");
  if (!pos) return;
  int w, h;
  wsk_messen(label, &w, &h);
  if (frame)
    odt_rect(out, "Box", pos[0] - knotenrahmenabstand,
             pos[1] - knotenrahmenabstand, w + 2 * knotenrahmenabstand,
             h + 2 * knotenrahmenabstand, unit);
  double angle = branch ? branch->angle : 0;
  double cx = branch ? branch->cx : pos[0] + w / 2.;
  double cy = branch ? branch->cy : pos[1] + h / 2.;
  /* ODF rotation is counterclockwise; GTK screen coordinates point down. */
  double x = cx - cos(angle) * (w + 2) / 2. + sin(angle) * h / 2.;
  double y = cy - sin(angle) * (w + 2) / 2. - cos(angle) * h / 2.;
  g_string_append_printf(out,
      "<draw:rect draw:style-name=\"Text\" svg:width=\"%scm\" "
      "svg:height=\"%scm\" draw:transform=\"rotate (%s) translate (%scm %scm)\">"
      "<text:p text:style-name=\"Label\">",
      odt_number((w + 2) * unit), odt_number(h * unit), odt_number(-angle),
      odt_number(1 + (x - FensterRandLinks) * unit),
      odt_number(1 + (y - FensterRandOben) * unit));
  odt_text(out, arboretum_label_get_encoded_text(GTK_LABEL(label)));
  g_string_append(out, "</text:p></draw:rect>");
}

static void odt_fraction(GString *out, GtkWidget *z, GtkWidget *n,
                         gboolean frame, double unit) {
  if (!z || !gtk_widget_get_visible(z)) return;
  if (!*gtk_label_get_text(GTK_LABEL(n))) {
    odt_label(out, z, frame, NULL, unit); return;
  }
  double *zp = g_object_get_data(G_OBJECT(z), "arboretum-layout-position");
  double *np = g_object_get_data(G_OBJECT(n), "arboretum-layout-position");
  if (!zp || !np) return;
  int zw, zh, nw, nh;
  wsk_messen(z, &zw, &zh); wsk_messen(n, &nw, &nh);
  double left = MIN(zp[0], np[0]), right = MAX(zp[0] + zw, np[0] + nw);
  if (frame)
    odt_rect(out, "Box", left - knotenrahmenabstand, zp[1] - knotenrahmenabstand,
             right - left + 2 * knotenrahmenabstand,
             np[1] + nh - zp[1] + 2 * knotenrahmenabstand, unit);
  odt_line(out, "Fraction", left, np[1], right, np[1], unit);
  odt_label(out, z, FALSE, NULL, unit); odt_label(out, n, FALSE, NULL, unit);
}

gboolean exportodt(char *dateiname) {
  if (!labelein || wsklayout_count != maxzaehler + 1) {
    dateifehler("Export", dateiname, "Keine gültige Baumgeometrie vorhanden.");
    return FALSE;
  }
  double width = MAX(1, wsklayout_width - FensterRandLinks);
  double height = MAX(1, wsklayout_height - FensterRandOben);
  /* Writer supports pages up to 22 inches. Scale oversized trees uniformly. */
  double unit = MIN(2.54 / 96., 53.0 / MAX(width, height));
  GdkRGBA clear = {0, 0, 0, 0};
  g_autoptr(GString) out = g_string_new(
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<office:document-content" ODT_NS " office:version=\"1.2\">"
      "<office:automatic-styles>");
  odt_style(out, "Group", clear, clear, 0);
  odt_style(out, "Branch", zweigfarbe, clear, LinienDicke * unit);
  odt_style(out, "Fraction", schriftfarbe, clear, unit);
  odt_style(out, "Box", knotenrandfarbe, knotenhintergrundfarbe, LinienDicke * unit);
  odt_style(out, "Text", clear, clear, 0);
  odt_style(out, "Background", clear, hintergrundfarbe, 0);
  PangoFontDescription *font = pango_font_description_from_string(schriftart);
  double size = pango_font_description_get_size(font) / (double)PANGO_SCALE;
  if (pango_font_description_get_size_is_absolute(font)) size *= .75;
  if (size <= 0) size = 12;
  size *= unit / (2.54 / 96.);
  const char *family = pango_font_description_get_family(font);
  g_autofree char *escaped_family = g_markup_escape_text(family ? family : "Sans", -1);
  g_autofree char *color = odt_color(schriftfarbe);
  g_string_append_printf(out,
      "<style:style style:name=\"Label\" style:family=\"paragraph\">"
      "<style:paragraph-properties fo:margin=\"0cm\" fo:text-align=\"center\"/>"
      "<style:text-properties fo:font-family=\"%s\" fo:font-size=\"%spt\" "
      "fo:color=\"%s\" fo:font-weight=\"%s\" fo:font-style=\"%s\"/>"
      "</style:style><style:style style:name=\"Overline\" style:family=\"text\">"
      "<style:text-properties style:text-overline-style=\"solid\" "
      "style:text-overline-width=\"auto\" style:text-overline-color=\"font-color\"/>"
      "</style:style><style:style style:name=\"Page\" style:family=\"paragraph\" "
      "style:master-page-name=\"Standard\"/>"
      "</office:automatic-styles><office:body><office:text><text:p text:style-name=\"Page\">"
      "<draw:g draw:name=\"Baumdiagramm\" draw:style-name=\"Group\" "
      "text:anchor-type=\"page\" text:anchor-page-number=\"1\">",
      escaped_family, odt_number(size), color,
      pango_font_description_get_weight(font) >= PANGO_WEIGHT_BOLD ? "bold" : "normal",
      pango_font_description_get_style(font) != PANGO_STYLE_NORMAL ? "italic" : "normal");
  pango_font_description_free(font);
  if (hintergrundfarbe.alpha > 0)
    odt_rect(out, "Background", FensterRandLinks, FensterRandOben, width, height, unit);
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    odt_line(out, "Branch", p->x1, p->y1, p->x2, p->y2, unit);
  }
  for (int i = 0; i < wsklayout_count; i++) {
    odt_label(out, knotenlabel[i], TRUE, NULL, unit);
    if (wskanzeigen) {
      if (bruch && bruchou) odt_fraction(out, zaehlerlabel[i], nennerlabel[i], FALSE, unit);
      else odt_label(out, wahrscheinlichkeitlabel[i], FALSE, &wsklayout[i], unit);
    }
  }
  for (int i = 0; i <= maxzaehlererg; i++) {
    if (ergebnisseanzeigen) odt_label(out, ergebnislabel[i], TRUE, NULL, unit);
    if (ergebnissewskanzeigen) {
      if (bruch && bruchou)
        odt_fraction(out, ergebniszaehlerlabel[i], ergebnisnennerlabel[i], TRUE, unit);
      else odt_label(out, ergebniswsklabel[i], TRUE, NULL, unit);
    }
  }
  g_string_append(out, "</draw:g></text:p></office:text></office:body></office:document-content>");
  g_autofree char *styles = g_strdup_printf(
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<office:document-styles" ODT_NS " office:version=\"1.2\">"
      "<office:styles/><office:automatic-styles><style:page-layout style:name=\"Layout\">"
      "<style:page-layout-properties fo:page-width=\"%scm\" fo:page-height=\"%scm\" "
      "fo:margin=\"0cm\"/></style:page-layout></office:automatic-styles>"
      "<office:master-styles><style:master-page style:name=\"Standard\" "
      "style:page-layout-name=\"Layout\"/></office:master-styles></office:document-styles>",
      odt_number(MAX(3, width * unit + 2)), odt_number(MAX(3, height * unit + 2)));
  const char *manifest =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" manifest:version=\"1.2\">"
      "<manifest:file-entry manifest:full-path=\"/\" manifest:version=\"1.2\" manifest:media-type=\"application/vnd.oasis.opendocument.text\"/>"
      "<manifest:file-entry manifest:full-path=\"content.xml\" manifest:media-type=\"text/xml\"/>"
      "<manifest:file-entry manifest:full-path=\"styles.xml\" manifest:media-type=\"text/xml\"/>"
      "</manifest:manifest>";
  const char *names[] = {"mimetype", "content.xml", "styles.xml", "META-INF/manifest.xml"};
  const char *contents[] = {"application/vnd.oasis.opendocument.text", out->str, styles, manifest};
  g_autoptr(GByteArray) bytes = export_zip(names, contents, G_N_ELEMENTS(names));
  g_autoptr(GError) error = NULL;
  gboolean ok = g_file_set_contents_full(dateiname, (const char *)bytes->data, bytes->len,
      G_FILE_SET_CONTENTS_CONSISTENT, 0666, &error);
  if (!ok) dateifehler("Export", dateiname, error->message);
  return ok;
}
#undef ODT_NS
