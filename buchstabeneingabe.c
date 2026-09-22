/* Keep the entries and their native input contexts alive while editing.
 * Defer geometry updates until GTK has finished processing the edit. */
static guint eingabe_neuaufbau_id = 0;

static gboolean eingabe_neuaufbauen(gpointer data)
{
  eingabe_neuaufbau_id = 0;
  for(int i=0; i<=maxzaehler; i++)
  {
    gtk_entry_set_width_chars(GTK_ENTRY(textfeld[i]), KnotenTextBreite);
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldWahrscheinlichkeit[i]), WahrscheinlichkeitTextBreite);
  }
  for(int i=0; i<=maxzaehlererg; i++)
  {
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnis[i]), ErgebnisTextBreite);
    gtk_entry_set_width_chars(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i]), WahrscheinlichkeitErgebnisTextBreite);
  }
  groesseneu(NULL, NULL, data);
  for(int i=0; i<=maxzaehlererg; i++)
    gtk_layout_move(GTK_LAYOUT(data), textfeldErgebnisWahrscheinlichkeit[i],
        FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+
        (maxStufe+1)*KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,
        FensterRandOben+RandOben+yerg[i]);
  positionsanpassungwsk(data);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  gtk_widget_queue_draw(da);
  return G_SOURCE_REMOVE;
}

static void eingabe_neuaufbau_planen(gpointer data)
{
  if(!eingabe_neuaufbau_id)
    eingabe_neuaufbau_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
        eingabe_neuaufbauen, g_object_ref(data), g_object_unref);
}

void buchstabeneingabe(GtkEditable *editable, gpointer data)
{

  dateiveraendert++;

  char tempname[10000] = "";

  int i=0;
  KnotenTextBreite=2;

  for(i=0;i<=maxzaehler;i++)
  {
    int laenge = g_utf8_strlen(gtk_entry_get_text(GTK_ENTRY(textfeld[i])),-1);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(textfeld[i]));
    gunichar unicode_char = 0x0305; // Unicode-Wert für den kombinierenden Überstrich
    int count = 0;
    
    gchar *pos = (gchar*) text;
    while (*pos != '\0')
    {
      gunichar c = g_utf8_get_char(pos);
      if (c == unicode_char) {
        count++;
      }
      pos = g_utf8_next_char(pos);
    }
    laenge -= count;  //Da die Überstriche keinen Platz brauchen, werden die aus der Länge des Textes wieder abgezogen.
    if(KnotenTextBreite<2*laenge)
    {
      KnotenTextBreite=laenge*2;
    }
  }


  strcpy(tempname,gtk_widget_get_name(GTK_WIDGET(editable)));
  i=0;
  for(i=0;i<=maxzaehlererg;i++)
  {
    if(strlen(tempname) < strlen(gtk_widget_get_name(textfeldErgebnis[i])))
    {
      if(strncmp(gtk_widget_get_name(textfeldErgebnis[i]),tempname,strlen(tempname)) == 0)
      {
        ergebnistextneuschreiben(textfeldErgebnis[i]);
      }
    }
  }
  tempspeichern();
  eingabe_neuaufbau_planen(data);
}
