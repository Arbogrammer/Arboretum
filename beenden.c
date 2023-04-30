gboolean beenden(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  if(dateiveraendert)
  {
    GtkWidget *dialog = gtk_dialog_new_with_buttons ("Speichern?",GTK_WINDOW (window),GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_YES,"Nicht speichern",GTK_RESPONSE_NO,NULL);
    gtk_widget_show(dialog);
    gint response = gtk_dialog_run (GTK_DIALOG (dialog));
    switch(response)
    {
      case GTK_RESPONSE_CANCEL: gtk_widget_destroy(dialog) ; return TRUE;
      case GTK_RESPONSE_YES: speicherdialog();
    }
  }
  gtk_main_quit();
  return TRUE;
}
