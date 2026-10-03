static void schriftartwechseln(GtkWidget *button, gpointer data) {
  GtkWidget *dialog;

  dialog =
      gtk_font_chooser_dialog_new("Schriftart auswählen", GTK_WINDOW(window));
  gtk_font_chooser_set_font(GTK_FONT_CHOOSER(dialog), schriftart);
  g_signal_connect(GTK_FONT_CHOOSER(dialog), "response",
                   G_CALLBACK(schriftartanpassen), data);

  g_signal_connect(dialog, "response", G_CALLBACK(dialogsschliessen), data);

  gtk_widget_show_all(dialog);
}
