void exportdialog()
{
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Datei speichern",GTK_WINDOW(window),action,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_ACCEPT,NULL);
  chooser = GTK_FILE_CHOOSER (dialog);

  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);


  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *dateiname;

    dateiname = gtk_file_chooser_get_filename (chooser);

    if(dateiname[strlen(dateiname)-1] == 'g' && dateiname[strlen(dateiname)-2] == 'n' && dateiname[strlen(dateiname)-3] == 'p' && dateiname[strlen(dateiname)-4] == '.')
    {
      exportpng(dateiname);
    } 
    if(dateiname[strlen(dateiname)-1] == 'g' && dateiname[strlen(dateiname)-2] == 'v' && dateiname[strlen(dateiname)-3] == 's' && dateiname[strlen(dateiname)-4] == '.')
    {
      exportsvg(dateiname);
    } 
    if(dateiname[strlen(dateiname)-1] == 'g' && dateiname[strlen(dateiname)-2] == 'p' && dateiname[strlen(dateiname)-3] == 'j' && dateiname[strlen(dateiname)-4] == '.')
    {
      exportjpg(dateiname);
    } 
    if(dateiname[strlen(dateiname)-1] == 'g' && dateiname[strlen(dateiname)-2] == 'e' && dateiname[strlen(dateiname)-3] == 'p' && dateiname[strlen(dateiname)-4] == 'j' && dateiname[strlen(dateiname)-5] == '.')
    {
      exportjpg(dateiname);
    } 
    if(dateiname[strlen(dateiname)-1] == 'p' && dateiname[strlen(dateiname)-2] == 'm' && dateiname[strlen(dateiname)-3] == 'b' && dateiname[strlen(dateiname)-4] == '.')
    {
      exportbmp(dateiname);
    } 
    if(dateiname[strlen(dateiname)-1] == 'f' && dateiname[strlen(dateiname)-2] == 'd' && dateiname[strlen(dateiname)-3] == 'p' && dateiname[strlen(dateiname)-4] == '.')
    {
      exportpdf(dateiname);
    } 

    g_free (dateiname);
  }

  gtk_widget_destroy (dialog);
}
