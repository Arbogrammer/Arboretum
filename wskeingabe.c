void wskeingabe(GtkEditable *editable, gpointer data)
{
  dateiveraendert++;
//  tempspeichern();
  char tempname[10000] = "";
  strcpy(tempname,gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))));

  int i=0;

  WahrscheinlichkeitTextBreite=3;
  for(i=0;i<=maxzaehler;i++)
  {
    if(WahrscheinlichkeitTextBreite<strlen(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i]))))
    {
      WahrscheinlichkeitTextBreite=strlen(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
    }
  }

  for(i=0;i<=maxzaehlererg;i++)
  {
    if(strlen(tempname) < strlen(gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i])))
    {
      if(strncmp(gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),tempname,strlen(tempname)-1) == 0)
      {
        wskergebnisneuschreiben(textfeldErgebnisWahrscheinlichkeit[i]);
      }
    }
  }

  tempspeichern();
  templaden(data);
}
