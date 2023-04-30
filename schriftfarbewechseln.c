static void schriftfarbewechseln (GtkWidget *button, gpointer data)
{
  GtkWidget *dialog;

  dialog = gtk_color_chooser_dialog_new ("Schriftfarbe auswählen", GTK_WINDOW (window));
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog), &schriftfarbe);

  g_signal_connect(dialog,"response",G_CALLBACK(dialogsfschliessen),data);

  gtk_widget_show_all (dialog);
}

