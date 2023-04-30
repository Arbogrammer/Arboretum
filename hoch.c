gboolean hoch(GtkWidget *widget, gpointer data)
{
  if(labelein==1)
  {
    return FALSE;
  }
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  if(aktuelleswidget == NULL)
  {
    aktuelleswidget = textfeld[0];
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[0])));
    gtk_editable_set_position(GTK_EDITABLE(textfeld[0]),-1);
    return FALSE;
  }

  if(strrchr(gtk_widget_get_name(aktuelleswidget),'W'))
  {
    hochwsk(widget, data);
    return FALSE;
  }

  // Einschub: Folgende drei Zeilen sind nur notwendig, wenn man mit der Maus die Felder auswählen will!
  zaehler = knotenexistiert(gtk_widget_get_name(aktuelleswidget));
  if(zaehler==-1)
  {
    return FALSE;
  }
  Stufe = zeichenzaehlen(gtk_widget_get_name(aktuelleswidget),'-')-1;
  Knoten[Stufe] = atoi(gtk_widget_get_name(aktuelleswidget)+(int)(strrchr(gtk_widget_get_name(aktuelleswidget),'-')-&(gtk_widget_get_name(aktuelleswidget)[0])+1));
  // Einschub Ende

//  printf("Von:  %s\nStufe: %i\tmaxStufe: %i\tZähler: %i\tmaxZähler: %i\tKnoten: %i\n",gtk_widget_get_name(aktuelleswidget),Stufe,maxStufe,zaehler,maxzaehler,Knoten[Stufe]);

  gchar name[10000];
  snprintf(name,(int)(strrchr(gtk_widget_get_name(aktuelleswidget),'-')-&(gtk_widget_get_name(aktuelleswidget)[0])+1),"%s",gtk_widget_get_name(aktuelleswidget));
  gchar temp[30];
  sprintf(temp,"-%i",(Knoten[Stufe]>0)?Knoten[Stufe]-1:0);
  strcat(name,temp);

  int i=0;
  while(strcmp(name,gtk_widget_get_name(textfeld[i])))
  {
    i++;
  }
  zaehler=i;
  if(Knoten[Stufe] > 0)
  {
    Knoten[Stufe] -= 1;
  }
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[zaehler])));
  gtk_editable_set_position(GTK_EDITABLE(textfeld[zaehler]),-1);
  gtk_widget_queue_draw(da);
//  printf("Nach: %s\nStufe: %i\tmaxStufe: %i\tZähler: %i\tmaxZähler: %i\tKnoten: %i\n\n",gtk_widget_get_name(textfeld[zaehler]),Stufe,maxStufe,zaehler,maxzaehler,Knoten[Stufe]);
  return TRUE;
}
