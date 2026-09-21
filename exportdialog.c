void exportdialog(GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;
  gboolean bearbeitungsmodus = labelein == 0;

  if (bearbeitungsmodus)
  {
    umwandeln (NULL, data);
  }

  dialog = gtk_file_chooser_dialog_new ("Datei speichern",GTK_WINDOW(window),action,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_ACCEPT,NULL);
  chooser = GTK_FILE_CHOOSER (dialog);

  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);


  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *dateiname;

    dateiname = gtk_file_chooser_get_filename (chooser);

    if (dateiname != NULL)
    {
      const char *basisname = strrchr (dateiname, G_DIR_SEPARATOR);
      const char *endung;

      basisname = basisname == NULL ? dateiname : basisname + 1;
      endung = strrchr (basisname, '.');

      if (endung == NULL || endung == basisname || endung[1] == '\0')
      {
        char *dateiname_mit_endung = g_strconcat (dateiname, ".svg", NULL);
        g_free (dateiname);
        dateiname = dateiname_mit_endung;
      }
    }

    if(dateiname != NULL && g_str_has_suffix (dateiname, ".png"))
    {
      exportpng(dateiname);
    }
    else if(dateiname != NULL && g_str_has_suffix (dateiname, ".svg"))
    {
      exportsvg(dateiname);
    }
    else if(dateiname != NULL && g_str_has_suffix (dateiname, ".jpg"))
    {
      exportjpg(dateiname);
    }
    else if(dateiname != NULL && g_str_has_suffix (dateiname, ".jpeg"))
    {
      exportjpg(dateiname);
    }
    else if(dateiname != NULL && g_str_has_suffix (dateiname, ".bmp"))
    {
      exportbmp(dateiname);
    }
    else if(dateiname != NULL && g_str_has_suffix (dateiname, ".pdf"))
    {
      exportpdf(dateiname);
    }

    g_free (dateiname);
  }

  gtk_widget_destroy (dialog);

  if (bearbeitungsmodus)
  {
    umwandeln (NULL, data);
  }
}
