/* All formats use the same geometry and text renderer. Text outlines avoid
 * platform-specific glyph blitting on image surfaces. File output goes through
 * GLib so Windows paths (including Unicode) and write errors are handled alike. */
static cairo_status_t export_bytes(void *closure, const unsigned char *data,
                                   unsigned int length)
{
  g_byte_array_append(closure, data, length);
  return CAIRO_STATUS_SUCCESS;
}

static void export_label(cairo_t *cr, GtkWidget *widget)
{
  if(!widget || !gtk_widget_get_visible(widget))
    return;
  GtkWidget *parent = gtk_widget_get_parent(widget);
  GskTransform *transform = gtk_fixed_get_child_transform(GTK_FIXED(parent), widget);
  cairo_save(cr);
  cairo_translate(cr, -FensterRandLinks, -FensterRandOben);
  if(transform)
  {
    float xx, yx, xy, yy, dx, dy;
    gsk_transform_to_2d(transform, &xx, &yx, &xy, &yy, &dx, &dy);
    cairo_matrix_t matrix = {xx, yx, xy, yy, dx, dy};
    cairo_transform(cr, &matrix);
  }
  int x=0, y=0;
  gtk_label_get_layout_offsets(GTK_LABEL(widget), &x, &y);
  g_autoptr(PangoLayout) textlayout = pango_layout_copy(gtk_label_get_layout(GTK_LABEL(widget)));
  cairo_move_to(cr, x, y);
  cairo_set_source_rgba(cr, schriftfarbe.red, schriftfarbe.green,
                        schriftfarbe.blue, schriftfarbe.alpha);
  pango_cairo_layout_path(cr, textlayout);
  cairo_fill(cr);
  cairo_restore(cr);
}

static gboolean export_datei(const char *dateiname, const char *format)
{
  GtkAllocation alloc;
  gtk_widget_get_allocation(da, &alloc);
  const int breite = alloc.width, hoehe = alloc.height;
  gboolean raster = strcmp(format,"svg") && strcmp(format,"pdf");
  if(!labelein || breite <= 0 || hoehe <= 0 ||
      (raster && (guint64)breite * hoehe > 64 * 1024 * 1024))
  {
    dateifehler("Export", dateiname, "Ungültige oder zu große Bildabmessungen.");
    return FALSE;
  }
  if(g_getenv("ARBORETUM_DIAGNOSTIC"))
    g_printerr("Export %s: %s (%d x %d)\n", format, dateiname, breite, hoehe);
  g_autoptr(GByteArray) bytes = g_byte_array_new();
  cairo_surface_t *surface;
  if(!strcmp(format,"svg"))
    surface = cairo_svg_surface_create_for_stream(export_bytes, bytes, breite, hoehe);
  else if(!strcmp(format,"pdf"))
    surface = cairo_pdf_surface_create_for_stream(export_bytes, bytes, breite, hoehe);
  else
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, breite, hoehe);
  cairo_t *cr = cairo_create(surface);
  double alpha = hintergrundfarbe.alpha;
  if(!strcmp(format,"jpeg") || !strcmp(format,"bmp"))
    hintergrundfarbe.alpha = 1;
  zeichnelinien(NULL, cr, breite, hoehe, NULL);
  hintergrundfarbe.alpha = alpha;
  startup_trace("export geometry drawn");
  for(int i=0; i<=maxzaehler; i++)
  {
    export_label(cr, knotenlabel[i]);
    if(bruch && bruchou)
    {
      export_label(cr, zaehlerlabel[i]);
      export_label(cr, nennerlabel[i]);
    }
    else
      export_label(cr, wahrscheinlichkeitlabel[i]);
  }
  for(int i=0; i<=maxzaehlererg; i++)
  {
    if(ergebnisseanzeigen)
      export_label(cr, ergebnislabel[i]);
    if(ergebnissewskanzeigen)
    {
      if(bruch && bruchou)
      {
        export_label(cr, ergebniszaehlerlabel[i]);
        export_label(cr, ergebnisnennerlabel[i]);
      }
      else
        export_label(cr, ergebniswsklabel[i]);
    }
  }
  startup_trace("export text drawn");
  cairo_status_t status = cairo_status(cr);
  cairo_destroy(cr);
  if(status == CAIRO_STATUS_SUCCESS)
    status = cairo_surface_status(surface);
  g_autoptr(GError) error = NULL;
  if(status == CAIRO_STATUS_SUCCESS && raster)
  {
    if(!strcmp(format,"png"))
      status = cairo_surface_write_to_png_stream(surface, export_bytes, bytes);
    else
    {
      cairo_surface_flush(surface);
      g_autoptr(GdkPixbuf) bild = gdk_pixbuf_get_from_surface(surface, 0, 0, breite, hoehe);
      /* JPEG has no alpha channel. Some encoders reject an RGBA pixbuf even
       * when all pixels happen to be opaque. BMP is also emitted as RGB. */
      g_autoptr(GdkPixbuf) rgb = bild ? gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, breite, hoehe) : NULL;
      if(rgb)
      {
        gdk_pixbuf_fill(rgb, 0xffffffff);
        gdk_pixbuf_composite(bild, rgb, 0, 0, breite, hoehe,
                              0, 0, 1, 1, GDK_INTERP_NEAREST, 255);
      }
      gchar *buffer = NULL;
      gsize length = 0;
      if(!rgb)
        g_set_error_literal(&error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "Bild konnte nicht konvertiert werden.");
      else if(gdk_pixbuf_save_to_buffer(rgb, &buffer, &length, format, &error, NULL))
        g_byte_array_append(bytes, (guint8 *)buffer, length);
      g_free(buffer);
    }
  }
  cairo_surface_finish(surface);
  if(status == CAIRO_STATUS_SUCCESS)
    status = cairo_surface_status(surface);
  cairo_surface_destroy(surface);
  if(status != CAIRO_STATUS_SUCCESS && !error)
    g_set_error_literal(&error, G_FILE_ERROR, G_FILE_ERROR_FAILED, cairo_status_to_string(status));
  if(!error && !bytes->len)
    g_set_error_literal(&error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "Leere Exportdatei.");
  if(!error)
    g_file_set_contents_full(dateiname, (const gchar *)bytes->data, bytes->len,
                              G_FILE_SET_CONTENTS_CONSISTENT, 0666, &error);
  if(error)
  {
    dateifehler("Export", dateiname, error->message);
    return FALSE;
  }
  if(g_getenv("ARBORETUM_DIAGNOSTIC"))
    g_printerr("Export erfolgreich: %s (%u Bytes)\n", dateiname, bytes->len);
  return TRUE;
}
