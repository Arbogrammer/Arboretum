static void dialogkrfschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      tempspeichern();
      knotenrahmenfarbewurdegeaendert = 1;
      gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (dialog), &knotenrandfarbe);
      knotenrandfarbe.alpha = 1.0;
      dateiveraendert++;
      knotenrandfarbeaendern();
      gtk_widget_queue_draw (da);
    }
  gtk_widget_hide (GTK_WIDGET (dialog));
}
