/* U+0305 remains the on-disk representation for backwards compatibility.
 * Labels, however, use Pango's native overline and omit the combining glyph. */
static gchar *arboretum_overline_markup(const gchar *text)
{
  GString *markup = g_string_new(NULL);
  gboolean in_overline = FALSE;

  for(const gchar *p = text; p && *p; )
  {
    const gchar *next = g_utf8_next_char(p);
    gboolean overlined = *next && g_utf8_get_char(next) == 0x0305;

    if(overlined != in_overline)
    {
      g_string_append(markup, overlined ? "<span overline=\"single\">" : "</span>");
      in_overline = overlined;
    }

    g_autofree gchar *escaped = g_markup_escape_text(p, next - p);
    g_string_append(markup, escaped);
    p = next;
    if(overlined)
    {
      /* More than one mark after a character still means one overline. */
      do p = g_utf8_next_char(p);
      while(*p && g_utf8_get_char(p) == 0x0305);
    }
  }

  if(in_overline)
    g_string_append(markup, "</span>");
  return g_string_free(markup, FALSE);
}

static void arboretum_label_set_formatted(GtkLabel *label,
                                           const gchar *format,
                                           const gchar *encoded_text)
{
  g_autofree gchar *content = arboretum_overline_markup(encoded_text);
  g_autofree gchar *markup = g_strdup_printf(format, content);
  gtk_label_set_markup(label, markup);
  g_object_set_data_full(G_OBJECT(label), "arboretum-encoded-text",
                         g_strdup(encoded_text), g_free);
}

static const gchar *arboretum_label_get_encoded_text(GtkLabel *label)
{
  const gchar *text = g_object_get_data(G_OBJECT(label), "arboretum-encoded-text");
  return text ? text : gtk_label_get_text(label);
}

/* GtkEntry must retain U+0305 because its plain-text buffer is also the model
 * that gets saved. Replace those marker glyphs with an empty Pango shape and
 * let Pango draw one continuous native overline over each marked run. */
static void arboretum_entry_apply_overlines(GtkEntry *entry)
{
  const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  g_autoptr(PangoAttrList) attributes = pango_attr_list_new();
  guint run_start = G_MAXUINT;

  for(const gchar *p = text; *p; )
  {
    const gchar *next = g_utf8_next_char(p);
    guint character_start = (guint)(p - text);
    gboolean overlined = *next && g_utf8_get_char(next) == 0x0305;

    if(overlined && run_start == G_MAXUINT)
      run_start = character_start;
    else if(!overlined && run_start != G_MAXUINT)
    {
      PangoAttribute *line = pango_attr_overline_new(PANGO_OVERLINE_SINGLE);
      line->start_index = run_start;
      line->end_index = character_start;
      pango_attr_list_insert(attributes, line);
      run_start = G_MAXUINT;
    }

    p = next;
    while(overlined && *p && g_utf8_get_char(p) == 0x0305)
    {
      const gchar *mark_end = g_utf8_next_char(p);
      PangoRectangle empty = { 0, 0, 0, 0 };
      PangoAttribute *hidden = pango_attr_shape_new(&empty, &empty);
      hidden->start_index = (guint)(p - text);
      hidden->end_index = (guint)(mark_end - text);
      pango_attr_list_insert(attributes, hidden);
      p = mark_end;
    }
  }

  if(run_start != G_MAXUINT)
  {
    PangoAttribute *line = pango_attr_overline_new(PANGO_OVERLINE_SINGLE);
    line->start_index = run_start;
    line->end_index = (guint)strlen(text);
    pango_attr_list_insert(attributes, line);
  }
  gtk_entry_set_attributes(entry, attributes);
}

static void arboretum_refresh_entry_overlines(void)
{
  for(int i = 0; i <= maxzaehler; i++)
  {
    if(textfeld[i])
      arboretum_entry_apply_overlines(GTK_ENTRY(textfeld[i]));
    if(textfeldWahrscheinlichkeit[i])
      arboretum_entry_apply_overlines(GTK_ENTRY(textfeldWahrscheinlichkeit[i]));
  }
  for(int i = 0; i <= maxzaehlererg; i++)
  {
    if(textfeldErgebnis[i])
      arboretum_entry_apply_overlines(GTK_ENTRY(textfeldErgebnis[i]));
    if(textfeldErgebnisWahrscheinlichkeit[i])
      arboretum_entry_apply_overlines(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]));
  }
}

static gboolean arboretum_attributes_contain(PangoAttrList *attributes,
                                               PangoAttrType type)
{
  if(!attributes)
    return FALSE;
  PangoAttrIterator *iterator = pango_attr_list_get_iterator(attributes);
  gboolean found = FALSE;
  do
  {
    if(pango_attr_iterator_get(iterator, type))
    {
      found = TRUE;
      break;
    }
  }
  while(pango_attr_iterator_next(iterator));
  pango_attr_iterator_destroy(iterator);
  return found;
}
