gboolean pos1(GtkWidget *widget, gpointer data)
{
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY((textfeld[0])));
  zaehler=0;
  Stufe=0;
  Knoten[0]=0;
  return TRUE;
}
