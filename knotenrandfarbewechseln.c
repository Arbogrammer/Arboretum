static void knotenrandfarbewechseln (GtkWidget *button, gpointer data)
{
  GtkWidget *dialog;

  dialog = gtk_color_chooser_dialog_new ("Rahmenfarbe der Knotenpunkte auswählen", GTK_WINDOW (window));
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog), &knotenrandfarbe);

  g_signal_connect(dialog,"response",G_CALLBACK(dialogkrfschliessen),data);

  gtk_widget_show_all (dialog);
}

