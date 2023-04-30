static void zweigfarbewechseln (GtkWidget *button, gpointer data)
{
  GtkWidget *dialog;

  dialog = gtk_color_chooser_dialog_new ("Zweigfarbe auswählen", GTK_WINDOW (window));
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog), &zweigfarbe);

  g_signal_connect(dialog,"response",G_CALLBACK(dialogzschliessen),data);

  gtk_widget_show_all (dialog);
}

