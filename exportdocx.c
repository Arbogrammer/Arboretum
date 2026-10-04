/* Editable OOXML Transitional drawing group (VML). Word and Writer both
 * support these native lines/text boxes, including arbitrary text rotation.
 * DrawingML grouped text currently renders unrotated in the bundled Writer.
 * Text stays in w:txbxContent; the archive contains no pictures or macros. */
#define DOCX_W "http://schemas.openxmlformats.org/wordprocessingml/2006/main"

typedef struct {
  GString *xml;
  double emu;
  unsigned id;
  char *run_properties;
} DocxExport;

/* Group coordinates and page sizes are integers, independent of locale. */
static const char *docx_integer(double value) {
  static char buffers[16][G_ASCII_DTOSTR_BUF_SIZE];
  static unsigned slot;
  char *buffer = buffers[slot++ % G_N_ELEMENTS(buffers)];
  return g_ascii_formatd(buffer, G_ASCII_DTOSTR_BUF_SIZE, "%.0f", round(value));
}

static const char *docx_decimal(double value) {
  static char buffers[16][G_ASCII_DTOSTR_BUF_SIZE];
  static unsigned slot;
  char *buffer = buffers[slot++ % G_N_ELEMENTS(buffers)];
  return g_ascii_formatd(buffer, G_ASCII_DTOSTR_BUF_SIZE, "%.6f", value);
}

static void docx_shape(DocxExport *doc, const char *name, double x, double y,
                        double w, double h, double angle, gboolean line,
                        gboolean flipx, gboolean flipy, GdkRGBA stroke,
                        GdkRGBA fill, double thickness, const char *text) {
  GString *xml = doc->xml;
  unsigned id = ++doc->id;
  const char *tag = line ? "line" : "rect";
  x = (x - FensterRandLinks) * doc->emu;
  y = (y - FensterRandOben) * doc->emu;
  w *= doc->emu; h *= doc->emu;
  g_string_append_printf(xml,
      "<v:%s id=\"%s_%u\" filled=\"%s\" stroked=\"%s\" "
      "fillcolor=\"#%02X%02X%02X\" strokecolor=\"#%02X%02X%02X\" strokeweight=\"%spt\" ",
      tag, name, id, fill.alpha > 0 ? "t" : "f", stroke.alpha > 0 ? "t" : "f",
      (int)round(fill.red * 255), (int)round(fill.green * 255), (int)round(fill.blue * 255),
      (int)round(stroke.red * 255), (int)round(stroke.green * 255), (int)round(stroke.blue * 255),
      docx_decimal(thickness * doc->emu / 12700.));
  if (line)
    g_string_append_printf(xml, "from=\"%s,%s\" to=\"%s,%s\">",
        docx_integer(x + (flipx ? w : 0)), docx_integer(y + (flipy ? h : 0)),
        docx_integer(x + (flipx ? 0 : w)), docx_integer(y + (flipy ? 0 : h)));
  else
    g_string_append_printf(xml,
        "style=\"position:absolute;left:%s;top:%s;width:%s;height:%s;rotation:%s;"
        "v-text-anchor:middle\">", docx_integer(x), docx_integer(y),
        docx_integer(w), docx_integer(h), docx_decimal(angle * 180 / G_PI));
  g_string_append_printf(xml, "<v:fill opacity=\"%s\"/><v:stroke opacity=\"%s\"/>",
                         docx_decimal(fill.alpha), docx_decimal(stroke.alpha));
  if (text) {
    g_autofree char *escaped = g_markup_escape_text(text, -1);
    g_string_append_printf(xml,
        "<v:textbox inset=\"0,0,0,0\" style=\"mso-fit-shape-to-text:f;v-text-anchor:middle\">"
        "<w:txbxContent><w:p><w:pPr>"
        "<w:spacing w:before=\"0\" w:after=\"0\" w:line=\"240\" w:lineRule=\"auto\"/>"
        "<w:jc w:val=\"center\"/></w:pPr><w:r>%s<w:t xml:space=\"preserve\">%s"
        "</w:t></w:r></w:p></w:txbxContent></v:textbox>", doc->run_properties, escaped);
  }
  g_string_append_printf(xml, "</v:%s>", tag);
}

static void docx_line(DocxExport *doc, const char *name, double x1, double y1,
                       double x2, double y2, GdkRGBA color, double thickness) {
  GdkRGBA clear = {0, 0, 0, 0};
  docx_shape(doc, name, MIN(x1, x2), MIN(y1, y2), fabs(x2 - x1), fabs(y2 - y1),
             0, TRUE, x2 < x1, y2 < y1, color, clear, thickness, NULL);
}

static void docx_box(DocxExport *doc, double x, double y, double w, double h) {
  docx_shape(doc, "Rahmen", x - knotenrahmenabstand, y - knotenrahmenabstand,
             w + 2 * knotenrahmenabstand, h + 2 * knotenrahmenabstand,
             0, FALSE, FALSE, FALSE, knotenrandfarbe, knotenhintergrundfarbe,
             LinienDicke, NULL);
}

static void docx_label(DocxExport *doc, GtkWidget *label, gboolean frame,
                        WskLayout *branch) {
  if (!label || !gtk_widget_get_visible(label)) return;
  double *pos = g_object_get_data(G_OBJECT(label), "arboretum-layout-position");
  if (!pos) return;
  int w, h;
  wsk_messen(label, &w, &h);
  if (frame) docx_box(doc, pos[0], pos[1], w, h);
  double cx = branch ? branch->cx : pos[0] + w / 2.;
  double cy = branch ? branch->cy : pos[1] + h / 2.;
  GdkRGBA clear = {0, 0, 0, 0};
  /* VML rotates about the centre of the unrotated bounding box. */
  docx_shape(doc, "Beschriftung", cx - (w + 2) / 2., cy - h / 2., w + 2, h,
             branch ? branch->angle : 0, FALSE, FALSE, FALSE, clear, clear, 0,
             arboretum_label_get_encoded_text(GTK_LABEL(label)));
}

static void docx_fraction(DocxExport *doc, GtkWidget *z, GtkWidget *n, gboolean frame) {
  if (!z || !gtk_widget_get_visible(z)) return;
  if (!*gtk_label_get_text(GTK_LABEL(n))) {
    docx_label(doc, z, frame, NULL); return;
  }
  double *zp = g_object_get_data(G_OBJECT(z), "arboretum-layout-position");
  double *np = g_object_get_data(G_OBJECT(n), "arboretum-layout-position");
  if (!zp || !np) return;
  int zw, zh, nw, nh;
  wsk_messen(z, &zw, &zh); wsk_messen(n, &nw, &nh);
  double left = MIN(zp[0], np[0]), right = MAX(zp[0] + zw, np[0] + nw);
  if (frame) docx_box(doc, left, zp[1], right - left, np[1] + nh - zp[1]);
  docx_line(doc, "Bruchstrich", left, np[1], right, np[1], schriftfarbe, 1);
  docx_label(doc, z, FALSE, NULL); docx_label(doc, n, FALSE, NULL);
}

gboolean exportdocx(char *dateiname) {
  if (!labelein || wsklayout_count != maxzaehler + 1) {
    dateifehler("Export", dateiname, "Keine gültige Baumgeometrie vorhanden.");
    return FALSE;
  }
  double width = MAX(1, wsklayout_width - FensterRandLinks);
  double height = MAX(1, wsklayout_height - FensterRandOben);
  /* 914400 EMU/inch; 96 px/inch. Keep the page within Word's 22-inch limit,
   * including a 1 cm margin around the diagram, as in the ODT exporter. */
  double emu = MIN(9525., 53. * 360000 / MAX(width, height));
  g_autoptr(GString) xml = g_string_new(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
      "<w:document xmlns:w=\"" DOCX_W "\" xmlns:v=\"urn:schemas-microsoft-com:vml\" "
      "xmlns:o=\"urn:schemas-microsoft-com:office:office\"><w:body><w:p><w:pPr>"
      "<w:spacing w:after=\"0\" w:line=\"20\" w:lineRule=\"exact\"/>"
      "</w:pPr><w:r><w:pict>");
  g_string_append_printf(xml,
      "<v:group id=\"Baumdiagramm\" coordorigin=\"0,0\" coordsize=\"%s,%s\" "
      "style=\"position:absolute;margin-left:28.346457pt;margin-top:28.346457pt;"
      "width:%spt;height:%spt;mso-position-horizontal-relative:page;"
      "mso-position-vertical-relative:page\">",
      docx_integer(width * emu), docx_integer(height * emu),
      docx_decimal(width * emu / 12700.), docx_decimal(height * emu / 12700.));
  PangoFontDescription *font = pango_font_description_from_string(schriftart);
  double size = pango_font_description_get_size(font) / (double)PANGO_SCALE;
  if (pango_font_description_get_size_is_absolute(font)) size *= .75;
  if (size <= 0) size = 12;
  int halfpoints = MAX(2, (int)round(2 * size * emu / 9525.));
  const char *family = pango_font_description_get_family(font);
  g_autofree char *escaped_family = g_markup_escape_text(family ? family : "Sans", -1);
  g_autofree char *properties = g_strdup_printf(
      "<w:rPr><w:rFonts w:ascii=\"%s\" w:hAnsi=\"%s\" w:eastAsia=\"%s\" w:cs=\"%s\"/>"
      "%s%s"
      "<w:color w:val=\"%02X%02X%02X\"/><w:sz w:val=\"%d\"/><w:szCs w:val=\"%d\"/>"
      "</w:rPr>", escaped_family, escaped_family, escaped_family, escaped_family,
      pango_font_description_get_weight(font) >= PANGO_WEIGHT_BOLD ? "<w:b/>" : "",
      pango_font_description_get_style(font) != PANGO_STYLE_NORMAL ? "<w:i/>" : "",
      (int)round(schriftfarbe.red * 255), (int)round(schriftfarbe.green * 255),
      (int)round(schriftfarbe.blue * 255), halfpoints, halfpoints);
  pango_font_description_free(font);
  DocxExport doc = {xml, emu, 1, properties};
  GdkRGBA clear = {0, 0, 0, 0};
  if (hintergrundfarbe.alpha > 0)
    docx_shape(&doc, "Hintergrund", FensterRandLinks, FensterRandOben, width, height,
               0, FALSE, FALSE, FALSE, clear, hintergrundfarbe, 0, NULL);
  for (int i = 0; i < wsklayout_count; i++) {
    WskLayout *p = &wsklayout[i];
    docx_line(&doc, "Zweig", p->x1, p->y1, p->x2, p->y2, zweigfarbe, LinienDicke);
  }
  for (int i = 0; i < wsklayout_count; i++) {
    docx_label(&doc, knotenlabel[i], TRUE, NULL);
    if (wskanzeigen) {
      if (bruch && bruchou) docx_fraction(&doc, zaehlerlabel[i], nennerlabel[i], FALSE);
      else docx_label(&doc, wahrscheinlichkeitlabel[i], FALSE, &wsklayout[i]);
    }
  }
  for (int k = 0; k < 2; k++)
    docx_label(&doc, ueberschrift_label[k], FALSE, NULL);
  for (int i = 0; i <= maxzaehlererg; i++) {
    if (ergebnisseanzeigen) docx_label(&doc, ergebnislabel[i], TRUE, NULL);
    if (ergebnissewskanzeigen) {
      if (bruch && bruchou)
        docx_fraction(&doc, ergebniszaehlerlabel[i], ergebnisnennerlabel[i], TRUE);
      else docx_label(&doc, ergebniswsklabel[i], TRUE, NULL);
    }
  }
  g_string_append_printf(xml,
      "</v:group></w:pict></w:r></w:p>"
      "<w:sectPr><w:pgSz w:w=\"%s\" w:h=\"%s\"/>"
      "<w:pgMar w:top=\"0\" w:right=\"0\" w:bottom=\"0\" w:left=\"0\" "
      "w:header=\"0\" w:footer=\"0\" w:gutter=\"0\"/></w:sectPr></w:body></w:document>",
      docx_integer(MAX(1080000, width * emu + 720000) / 635.),
      docx_integer(MAX(1080000, height * emu + 720000) / 635.));
  const char *types =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
      "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
      "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
      "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
      "<Override PartName=\"/word/settings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml\"/>"
      "</Types>";
  const char *rels =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
      "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
      "</Relationships>";
  const char *docrels =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
      "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings\" Target=\"settings.xml\"/>"
      "</Relationships>";
  const char *settings =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<w:settings xmlns:w=\"" DOCX_W "\"><w:compat>"
      "<w:compatSetting w:name=\"compatibilityMode\" w:uri=\"http://schemas.microsoft.com/office/word\" w:val=\"15\"/>"
      "</w:compat></w:settings>";
  const char *names[] = {"[Content_Types].xml", "_rels/.rels", "word/document.xml",
                         "word/_rels/document.xml.rels", "word/settings.xml"};
  const char *contents[] = {types, rels, xml->str, docrels, settings};
  g_autoptr(GByteArray) bytes = export_zip(names, contents, G_N_ELEMENTS(names));
  g_autoptr(GError) error = NULL;
  gboolean ok = g_file_set_contents_full(dateiname, (const char *)bytes->data, bytes->len,
      G_FILE_SET_CONTENTS_CONSISTENT, 0666, &error);
  if (!ok) dateifehler("Export", dateiname, error->message);
  return ok;
}
#undef DOCX_W
