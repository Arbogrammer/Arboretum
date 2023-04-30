static void dialogsschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      schriftartwurdegewechselt=1;
      strcpy(schriftart,gtk_font_chooser_get_font (GTK_FONT_CHOOSER (dialog)));
      schriftartanpassen(NULL,NULL,NULL);
      gtk_widget_queue_draw (da);
    }
  gtk_widget_hide (GTK_WIDGET (dialog));
}

