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
  templaden(data);
}
