/* Optional integration test of the actual save/load and export paths. Runs on
 * the Windows runner as well as locally. Outputs stay in a fresh temp folder. */
static void io_test_drain(void)
{
  for(int i=0; i<20; i++)
  {
    while(g_main_context_iteration(NULL, FALSE)) {}
    g_usleep(10000);
  }
}

typedef struct
{
  const char *directory;
  const char *filename;
  guint attempts;
  gboolean initialized;
  gboolean finished;
} IoSaveDialogTest;

/* Exercise the real chooser/response callback as well as the serializer. Only
 * used by the opt-in test, with a unique temporary directory as destination. */
static gboolean io_test_choose_save(gpointer data)
{
  IoSaveDialogTest *test = data;
  test->attempts++;
  GListModel *windows = gtk_window_get_toplevels();
  for(guint i=0; i<g_list_model_get_n_items(windows); i++)
  {
    g_autoptr(GtkWindow) candidate = g_list_model_get_item(windows, i);
    if(!GTK_IS_FILE_CHOOSER(candidate) || !GTK_IS_DIALOG(candidate))
      continue;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(candidate);
    if(test->attempts >= 200)
    {
      g_printerr("IO-Test Speicherdialog: Zeitüberschreitung\n");
      test->finished = TRUE;
      gtk_dialog_response(GTK_DIALOG(candidate), GTK_RESPONSE_CANCEL);
      return G_SOURCE_REMOVE;
    }
    if(!test->initialized)
    {
      g_autoptr(GFile) folder = g_file_new_for_path(test->directory);
      gtk_file_chooser_set_current_folder(chooser, folder, NULL);
      gtk_file_chooser_set_current_name(chooser, test->filename);
      test->initialized = TRUE;
      return G_SOURCE_CONTINUE;
    }
    g_autofree gchar *selected = gtk_file_chooser_get_filename(chooser);
    g_autofree gchar *expected = g_build_filename(test->directory, test->filename, NULL);
    if(g_strcmp0(selected, expected) == 0)
    {
      test->finished = TRUE;
      gtk_dialog_response(GTK_DIALOG(candidate), GTK_RESPONSE_ACCEPT);
      return G_SOURCE_REMOVE;
    }
  }
  return G_SOURCE_CONTINUE;
}

static gboolean io_smoketest(gpointer data)
{
  g_autoptr(GError) error = NULL;
  g_autofree gchar *dir = g_dir_make_tmp("Arboretum-IO-XXXXXX", &error);
  gboolean ok = dir != NULL;
  if(!ok)
  {
    g_printerr("IO-Test: %s\n", error->message);
    arboretum_exit_status = 1;
    g_main_loop_quit(arboretum_main_loop);
    return G_SOURCE_REMOVE;
  }
  g_printerr("IO-Test Ausgabeordner: %s\n", dir);
  g_signal_handlers_block_by_func(textfeld[0], G_CALLBACK(buchstabeneingabe), data);
  gtk_editable_set_text(GTK_EDITABLE(textfeld[0]), "Äpfel Ω");
  g_signal_handlers_unblock_by_func(textfeld[0], G_CALLBACK(buchstabeneingabe), data);
  buchstabeneingabe(GTK_EDITABLE(textfeld[0]), data);
  io_test_drain();
  g_autofree gchar *pfad = g_build_filename(dir, "Prüfung Ω.bdg", NULL);
  dateiveraendert = 1;
  ok = speichern(pfad) && !dateiveraendert;
  if(ok)
  {
    dateiveraendert = 1;
    ok = speichern(pfad) && !dateiveraendert;
    laden(data, pfad);
    ok &= !strcmp(gtk_editable_get_text(GTK_EDITABLE(textfeld[0])), "Äpfel Ω");
  }
  g_printerr("IO-Test Unicode speichern/laden: %s\n", ok ? "ok" : "FEHLER");
  IoSaveDialogTest dialog_test = {dir, "Dialog-Ä.bdg", 0, FALSE, FALSE};
  guint dialog_source = g_timeout_add(100, io_test_choose_save, &dialog_test);
  gboolean dialog_ok = speicherdialog(NULL, data);
  if(!dialog_test.finished)
    g_source_remove(dialog_source);
  g_autofree gchar *dialog_path = g_build_filename(dir, dialog_test.filename, NULL);
  dialog_ok = dialog_ok && g_file_test(dialog_path, G_FILE_TEST_IS_REGULAR) &&
              !strcmp(aktuelledatei, dialog_path);
  g_printerr("IO-Test Speicherdialog: %s\n", dialog_ok ? "ok" : "FEHLER");
  ok &= dialog_ok;
  g_autofree gchar *bad = g_build_filename(dir, "fehlt", "nicht-schreibbar.bdg", NULL);
  dateiveraendert = 7;
  gboolean failed_save_ok = !speichern(bad) && dateiveraendert == 7;
  g_printerr("IO-Test Schreibfehler bleibt ungespeichert: %s\n", failed_save_ok ? "ok" : "FEHLER");
  ok &= failed_save_ok;

  for(int scene=0; scene<3; scene++)
  {
    if(scene == 1)
    {
      gtk_widget_grab_focus(textfeld[0]);
      runter(NULL, data);
      runter(NULL, data);
    }
    if(scene > 0)
    {
      for(int i=0; i<=maxzaehler; i++)
      {
        g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[i], G_CALLBACK(wskeingabe), data);
        gtk_editable_set_text(GTK_EDITABLE(textfeldWahrscheinlichkeit[i]), scene == 2 ? "1/2" : "0.5");
        g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[i], G_CALLBACK(wskeingabe), data);
      }
    }
    bruchou = scene == 2;
    bruch = scene == 2;
    ergebnisseanzeigen = 1;
    ergebnissewskanzeigen = 1;
    io_test_drain();
    umwandeln(NULL, data);
    io_test_drain();
    arboretum_layout_aktualisieren(data);
    io_test_drain();
    const char *formats[] = {"svg", "png", "jpeg", "bmp", "pdf"};
    for(guint i=0; i<G_N_ELEMENTS(formats); i++)
    {
      g_autofree gchar *name = g_strdup_printf("Bild-Ä-%d.%s", scene, formats[i]);
      g_autofree gchar *image_path = g_build_filename(dir, name, NULL);
      g_printerr("IO-Test START %s\n", name);
      gboolean exported = export_datei(image_path, formats[i]);
      g_autofree gchar *contents = NULL;
      gsize length = 0;
      exported = exported && g_file_get_contents(image_path, &contents, &length, NULL) && length > 20;
      if(exported && (!strcmp(formats[i],"png") || !strcmp(formats[i],"bmp") || !strcmp(formats[i],"jpeg")))
      {
        /* Decode the complete result to catch corrupt or empty image output. */
        g_autoptr(GdkPixbufLoader) loader = gdk_pixbuf_loader_new();
        exported = gdk_pixbuf_loader_write(loader, (guint8 *)contents, length, NULL);
        gboolean closed = gdk_pixbuf_loader_close(loader, NULL);
        exported = exported && closed && gdk_pixbuf_loader_get_pixbuf(loader);
      }
      g_printerr("IO-Test %s: %s (%" G_GSIZE_FORMAT " Bytes)\n", name, exported ? "ok" : "FEHLER", length);
      ok &= exported;
    }
    gboolean failed_export_ok = !export_datei(bad, "png");
    ok &= failed_export_ok;
    umwandeln(NULL, data);
    io_test_drain();
  }
  g_printerr("IO-Test: %s\n", ok ? "ok" : "FEHLER");
  arboretum_exit_status = ok ? 0 : 1;
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}
