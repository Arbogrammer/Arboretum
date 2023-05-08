void ergebnistextneuschreiben(GtkWidget *widget)
{
  char tempnameerg[10000] = "";
  strcpy(tempnameerg,gtk_widget_get_name(widget));
  char ergebnistext[10000] = "";

  int position=(int)(strchr(tempnameerg+1,'-')-tempnameerg);
  int laenge = strlen(tempnameerg)-2;

  tempnameerg[position] = 0;

  while(knotenexistiert(tempnameerg) > -1)
  {
    strcat(ergebnistext,gtk_entry_get_text(GTK_ENTRY(textfeld[knotenexistiert(tempnameerg)])));
    if(ErgebnisTrenner && zeichenzaehlen(tempnameerg,'-') <= maxStufe)
    {
      if(ErgebnisTrenner!=127)
      {
        ergebnistext[strlen(ergebnistext)]=ErgebnisTrenner;
      }
    }
//    printf("Ergebnistext: %s\ntempnameerg: %s\n",ergebnistext,tempnameerg);
    if(position >= laenge)
    {
      break;
    }
    else
    {
      tempnameerg[position] = '-';
      position=(int)(strchr(tempnameerg+position+1,'-')-tempnameerg);
//      printf("Position: %i\n",position);
      tempnameerg[position] = 0;
    }
  }
//  printf("Ergebnistext: %s\n",ergebnistext);
  gtk_entry_set_text(GTK_ENTRY(widget),ergebnistext);
  int i=0;
  ErgebnisTextBreite=2;
  for(i=0;i<=maxzaehlererg;i++)
  {
    int laenge = g_utf8_strlen(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),-1);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i]));
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
    laenge -= count;
  
    if(2*laenge > ErgebnisTextBreite)
    {
      ErgebnisTextBreite = laenge*2;
    }
  }
  
  // EasterEggs! //
  
  if(!strcmp(ergebnistext,"Vulkanier") || !strcmp(ergebnistext,"vulkanier") || !strcmp(ergebnistext,"Vulkan") || !strcmp(ergebnistext,"vulkan"))
  {
    GtkWidget *egg = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_CLOSE,"%s","Lebe lang und in Frieden!");
    gtk_widget_show(egg);
    if(gtk_dialog_run (GTK_DIALOG (egg)) == GTK_RESPONSE_CLOSE)
    {
      gtk_widget_destroy(egg);
    }
  }
  if(!strcmp(ergebnistext,"42"))
  {
    GtkWidget *egg = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_CLOSE,"%s","Antworten ist leichter als Fragen.\nBloß in der Nacht zu schlafen, hieß die Sache nicht ernst zu nehmen.");
    gtk_widget_show(egg);
    if(gtk_dialog_run (GTK_DIALOG (egg)) == GTK_RESPONSE_CLOSE)
    {
      gtk_widget_destroy(egg);
    }
  }
  if(!strcmp(ergebnistext,"Hilfe") || !strcmp(ergebnistext,"hilfe"))
  {
    GtkWidget *egg = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_CLOSE,"%s","Ist nicht ein helfendes Leben ein zehnfaches?");
    gtk_widget_show(egg);
    if(gtk_dialog_run (GTK_DIALOG (egg)) == GTK_RESPONSE_CLOSE)
    {
      gtk_widget_destroy(egg);
    }
  }
}
