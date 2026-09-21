gboolean beenden(GtkWidget *widget, gpointer data)
{
  if(dateiveraendert)
  {
    GtkWidget *dialog = gtk_dialog_new_with_buttons ("Speichern?",GTK_WINDOW (window),GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,"Abbrechen",GTK_RESPONSE_CANCEL,"Speichern",GTK_RESPONSE_YES,"Nicht speichern",GTK_RESPONSE_NO,NULL);
    gtk_widget_show(dialog);
    gint response = gtk_dialog_run (GTK_DIALOG (dialog));
    switch(response)
    {
      case GTK_RESPONSE_CANCEL: gtk_widget_destroy(dialog) ; return TRUE;
      case GTK_RESPONSE_YES:
        /* Den modalen Rückfragedialog zuerst schließen. Andernfalls kann er
         * einen danach geöffneten Dateidialog blockieren. */
        gtk_widget_destroy(dialog);
        if(aktuelledatei[0])
        {
          speichern(aktuelledatei);
        }
        else if(!speicherdialog(NULL, NULL))
        {
          /* "Speichern unter" wurde abgebrochen: Fenster offen lassen. */
          return TRUE;
        }
        break;
      case GTK_RESPONSE_NO:
        gtk_widget_destroy(dialog);
        break;
      default:
        gtk_widget_destroy(dialog);
        return TRUE;
    }
  }
  if (arboretum_main_loop)
    g_main_loop_quit(arboretum_main_loop);
  return TRUE;
}
