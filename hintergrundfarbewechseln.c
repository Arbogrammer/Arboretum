static void hintergrundfarbewechseln (GtkWidget *button, gpointer data)
{
  GtkWidget *dialog;

  dialog = gtk_color_chooser_dialog_new ("Hintergrundfarbe auswählen", GTK_WINDOW (window));
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog), &hintergrundfarbe);

  g_signal_connect(dialog,"response",G_CALLBACK(dialogschliessen),data);

  gtk_widget_show_all (dialog);
}

