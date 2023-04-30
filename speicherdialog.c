void speicherdialog()
{
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Datei speichern",GTK_WINDOW(window),action,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_ACCEPT,NULL);
  chooser = GTK_FILE_CHOOSER (dialog);

  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

/*
  if (user_edited_a_new_document)
    gtk_file_chooser_set_current_name (chooser,"Untitled document");
  else
    gtk_file_chooser_set_filename (chooser,existing_filename);
*/

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *dateiname;

    dateiname = gtk_file_chooser_get_filename (chooser);
    if(dateiname[strlen(dateiname)-1] != 'g' || dateiname[strlen(dateiname)-2] != 'd' || dateiname[strlen(dateiname)-3] != 'b' || dateiname[strlen(dateiname)-4] != '.')
    {
      strcat(dateiname,".bdg");
    } 
    speichern (dateiname);
    g_free (dateiname);
  }

  gtk_widget_destroy (dialog);
}
