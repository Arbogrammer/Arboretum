gboolean linkswsk(GtkWidget *widget, gpointer data)
{
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  char name[10000] = "";
  strcpy (name,gtk_widget_get_name(aktuelleswidget));
  name[strlen(name)-1] = 0;    // lösche das W
  int anzahl = strlen(strrchr(name, '-'));
  int k=1;
  for(k=1;k<=anzahl;k++)
  {
    name[strlen(name)-1] = 0;
  }
  strcat(name,"W");
//  printf("%s\n",name);
  if(wskexistiert(name) != -1)
  {
    gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeldWahrscheinlichkeit[wskexistiert(name)])));
    gtk_editable_set_position(GTK_EDITABLE(textfeldWahrscheinlichkeit[wskexistiert(name)]),-1);
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}
