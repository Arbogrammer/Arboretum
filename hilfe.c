void hilfe(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *HilfeFenster = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(HilfeFenster), "Hilfe");
  gtk_window_set_default_size (GTK_WINDOW(HilfeFenster), 800, 600);
  GtkTextBuffer *HilfeText = gtk_text_buffer_new (NULL);
  gtk_text_buffer_insert_at_cursor (HilfeText, "Mit den Pfeiltasten erstellt man neue Knotenpunkte oder wechselt in vorhandene.\nMit der Tabulatortaste springt man zur zugehörigen Wahrscheinlichkeit (oder umgekehrt).\nMit dem Befehl \"Ansicht -> Baum fixieren/bearbeiten\" wechselt man vom Bearbeitungsmodus in den Ansichtsmodus.\n\nExportieren:\n - Exportieren funktioniert nur aus dem Ansichtsmodus heraus!\n - Unterstützte Exportformate: png, svg, pdf, bmp, jpg\n - Um Bilder in diesen Formaten zu erhalten, einfach bei der Namensgebung die entsprechende Endung anhängen.\n\nMöchte man Brüche verwenden, so schreibt man die Wahrscheinlichkeiten mit Schrägstrich, zum Beispiel 3/7.\nSobald ein Schrägstrich in einer Wahrscheinlichkeit vorkommt, rechnet das Programm nur mit Brüchen.", -1);
  GtkWidget *HilfeTextView = gtk_text_view_new_with_buffer(HilfeText);
  gtk_text_view_set_editable (GTK_TEXT_VIEW(HilfeTextView),FALSE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_top_margin (GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(HilfeTextView),FALSE);
  gtk_text_view_set_monospace (GTK_TEXT_VIEW(HilfeTextView),TRUE);
  gtk_container_add (GTK_CONTAINER (HilfeFenster), HilfeTextView);
  gtk_widget_show_all(HilfeFenster);
}
