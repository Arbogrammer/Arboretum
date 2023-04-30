void knotenwskwechseln()
{
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  char name[10000] = "";
  if(aktuelleswidget)
  {
    strcpy(name,gtk_widget_get_name(aktuelleswidget));
    if(name[strlen(name)-1] == 'W')
    {
      name[strlen(name)-1] = 0;
      gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeld[knotenexistiert(name)]));
    }
    else
    {
      strcat(name,"W");
      gtk_entry_grab_focus_without_selecting(GTK_ENTRY(textfeldWahrscheinlichkeit[wskexistiert(name)]));
    }
  }
}
