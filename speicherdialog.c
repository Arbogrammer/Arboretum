gboolean speicherdialog(GtkWidget *widget, gpointer data)
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
    char *auswahl = gtk_file_chooser_get_filename (chooser);
    char *dateiname = auswahl;
    size_t laenge = auswahl ? strlen(auswahl) : 0;

    if(!auswahl)
    {
      gtk_widget_destroy (dialog);
      return FALSE;
    }

    if(laenge < 4 || strcmp(auswahl + laenge - 4, ".bdg") != 0)
      dateiname = g_strconcat(auswahl, ".bdg", NULL);

    speichern (dateiname);
    g_strlcpy(aktuelledatei, dateiname, sizeof(aktuelledatei));

    if(dateiname != auswahl)
      g_free(dateiname);
    g_free(auswahl);
    gtk_widget_destroy (dialog);
    return TRUE;
  }

  gtk_widget_destroy (dialog);
  return FALSE;
}
