static void dateifehler(const char *vorgang, const char *pfad, const char *grund)
{
  g_printerr("%s fehlgeschlagen: %s: %s\n", vorgang, pfad ? pfad : "(kein Pfad)", grund);
  if(g_getenv("ARBORETUM_IO_SMOKE_TEST"))
    return;
  GtkWidget *meldung = gtk_message_dialog_new(GTK_WINDOW(window),
      GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
      "%s fehlgeschlagen", vorgang);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(meldung),
      "%s\n%s", pfad ? pfad : "Kein Dateipfad ausgewählt", grund);
  g_signal_connect_swapped(meldung, "response", G_CALLBACK(gtk_window_destroy), meldung);
  gtk_window_present(GTK_WINDOW(meldung));
}

gboolean speicherdialog(GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Datei speichern",GTK_WINDOW(window),action,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_ACCEPT,NULL);
  chooser = GTK_FILE_CHOOSER (dialog);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_file_chooser_set_current_name(chooser, "Baumdiagramm.bdg");

  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

/*
  if (user_edited_a_new_document)
    gtk_file_chooser_set_current_name (chooser,"Untitled document");
  else
    gtk_file_chooser_set_filename (chooser,existing_filename);
*/

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if(g_getenv("ARBORETUM_DIAGNOSTIC"))
    g_printerr("Speicherdialog Antwort: %d\n", res);
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *auswahl = gtk_file_chooser_get_filename (chooser);
    char *dateiname = auswahl;
    size_t laenge = auswahl ? strlen(auswahl) : 0;

    if(!auswahl)
    {
      gtk_widget_destroy (dialog);
      dateifehler("Speichern", NULL, "Der Dateidialog hat keinen lokalen Pfad geliefert.");
      return FALSE;
    }

    if(laenge < 4 || strcmp(auswahl + laenge - 4, ".bdg") != 0)
      dateiname = g_strconcat(auswahl, ".bdg", NULL);

    gboolean erfolg = FALSE;
    if(strlen(dateiname) >= sizeof(aktuelledatei))
      dateifehler("Speichern", dateiname, "Der Dateipfad ist zu lang.");
    else
      erfolg = speichern(dateiname);
    if(erfolg)
      g_strlcpy(aktuelledatei, dateiname, sizeof(aktuelledatei));

    if(dateiname != auswahl)
      g_free(dateiname);
    g_free(auswahl);
    gtk_widget_destroy (dialog);
    return erfolg;
  }

  gtk_widget_destroy (dialog);
  return FALSE;
}
