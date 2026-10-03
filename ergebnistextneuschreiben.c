void ergebnistextneuschreiben(GtkWidget *widget) {
  g_autofree gchar *tempnameerg = g_strdup(gtk_widget_get_name(widget));
  g_autoptr(GString) ergebnis = g_string_sized_new(128);

  int position = (int)(strchr(tempnameerg + 1, '-') - tempnameerg);
  int laenge = strlen(tempnameerg) - 2;

  tempnameerg[position] = 0;

  while (knotenexistiert(tempnameerg) > -1) {
    g_string_append(ergebnis, gtk_entry_get_text(GTK_ENTRY(
                                  textfeld[knotenexistiert(tempnameerg)])));
    if (ErgebnisTrenner && zeichenzaehlen(tempnameerg, '-') <= maxStufe) {
      if (ErgebnisTrenner != 127) {
        g_string_append_c(ergebnis, ErgebnisTrenner);
      }
    }
    if (position >= laenge) {
      break;
    } else {
      tempnameerg[position] = '-';
      position = (int)(strchr(tempnameerg + position + 1, '-') - tempnameerg);
      tempnameerg[position] = 0;
    }
  }
  gtk_entry_set_text(GTK_ENTRY(widget), ergebnis->str);
  int i = 0;
  ErgebnisTextBreite = 2;
  for (i = 0; i <= maxzaehlererg; i++) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i]));
    int breite = textbreite_in_zeichen(textfeldErgebnis[i], text);

    if (breite > ErgebnisTextBreite) {
      ErgebnisTextBreite = breite;
    }
  }

  if (!strcmp(ergebnis->str, "Vulkanier") ||
      !strcmp(ergebnis->str, "vulkanier") || !strcmp(ergebnis->str, "Vulkan") ||
      !strcmp(ergebnis->str, "vulkan")) {
    GtkWidget *egg = gtk_message_dialog_new(
        GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, "%s",
        "Lebe lang und in Frieden!");
    gtk_widget_show(egg);
    if (gtk_dialog_run(GTK_DIALOG(egg)) == GTK_RESPONSE_CLOSE) {
      gtk_widget_destroy(egg);
    }
  }
  if (!strcmp(ergebnis->str, "42")) {
    GtkWidget *egg = gtk_message_dialog_new(
        GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, "%s",
        "Antworten ist leichter als Fragen.\nBloß in der Nacht zu schlafen, "
        "hieß die Sache nicht ernst zu nehmen.");
    gtk_widget_show(egg);
    if (gtk_dialog_run(GTK_DIALOG(egg)) == GTK_RESPONSE_CLOSE) {
      gtk_widget_destroy(egg);
    }
  }
  if (!strcmp(ergebnis->str, "Hilfe") || !strcmp(ergebnis->str, "hilfe")) {
    GtkWidget *egg = gtk_message_dialog_new(
        GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, "%s",
        "Ist nicht ein helfendes Leben ein zehnfaches?");
    gtk_widget_show(egg);
    if (gtk_dialog_run(GTK_DIALOG(egg)) == GTK_RESPONSE_CLOSE) {
      gtk_widget_destroy(egg);
    }
  }
}
