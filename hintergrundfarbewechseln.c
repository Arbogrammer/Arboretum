static void hintergrundfarbewechseln (GtkWidget *button, gpointer data)
{
  GtkWidget *dialog;

  dialog = gtk_color_chooser_dialog_new ("Hintergrundfarbe und Exporttransparenz auswählen", GTK_WINDOW (window));
  /* Die Arbeitsansicht setzt transparente Farben in zeichnelinien.c über
   * Weiß zusammen. Der Alpha-Wert bleibt trotzdem erhalten, damit PNG- und
   * SVG-Exporte einen transparenten Hintergrund bekommen können. */
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(dialog), TRUE);
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog), &hintergrundfarbe);

  g_signal_connect(dialog,"response",G_CALLBACK(dialogschliessen),data);

  gtk_widget_show_all (dialog);
}
