/* Optional integration test of the actual save/load and export paths. Runs on
 * the Windows runner as well as locally. Outputs stay in a fresh temp folder. */
static const char *arboretum_test_executable;

static gboolean startup_file_smoketest(gpointer unused)
{
  const char *expected = g_getenv("ARBORETUM_STARTUP_EXPECTED_PATH");
  static int attempts;
  if(g_getenv("ARBORETUM_FINDER_SMOKE_TEST") && !aktuelledatei[0] && ++attempts < 150)
    return G_SOURCE_CONTINUE;
  const char *actual = aktuelledatei;
#ifdef __APPLE__
  /* Finder resolves /var to /private/var and may return decomposed Unicode. */
  g_autofree gchar *resolved_expected = expected ? realpath(expected, NULL) : NULL;
  g_autofree gchar *resolved_actual = realpath(actual, NULL);
  g_autofree gchar *normalized_expected = resolved_expected ? g_utf8_normalize(resolved_expected, -1, G_NORMALIZE_NFC) : NULL;
  g_autofree gchar *normalized_actual = resolved_actual ? g_utf8_normalize(resolved_actual, -1, G_NORMALIZE_NFC) : NULL;
  expected = normalized_expected;
  actual = normalized_actual;
#endif
  gboolean ok = expected && actual && !g_strcmp0(actual, expected) && maxzaehler == 0 &&
      !strcmp(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), "Ä̅pfel Ω");
  if(!ok)
    g_printerr("Startup test expected=%s actual=%s nodes=%d text=%s\n",
        expected ? expected : "(null)", actual ? actual : "(null)", maxzaehler,
        gtk_entry_get_text(GTK_ENTRY(textfeld[0])));
  g_printerr("IO-Test Unicode-Datei als Startargument: %s\n", ok ? "ok" : "FEHLER");
  arboretum_exit_status = ok ? 0 : 1;
  const char *result_path = g_getenv("ARBORETUM_FINDER_RESULT");
  if(result_path)
    g_file_set_contents(result_path, ok ? "ok" : "FAILED", -1, NULL);
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}

static gboolean io_test_startup_file(const char *directory)
{
  g_autofree gchar *folder = g_build_filename(directory, "Bäume und Grüße Ω", NULL);
  g_autofree gchar *path = g_build_filename(folder, "Äpfel Öl Übung.bdg", NULL);
  if(g_mkdir(folder, 0700) != 0 || !speichern(path))
    return FALSE;
  g_auto(GStrv) env = g_get_environ();
  env = g_environ_unsetenv(env, "ARBORETUM_IO_SMOKE_TEST");
  env = g_environ_unsetenv(env, "ARBORETUM_KEYBOARD_SMOKE_TEST");
  env = g_environ_setenv(env, "ARBORETUM_STARTUP_SMOKE_TEST", "1", TRUE);
  env = g_environ_setenv(env, "ARBORETUM_STARTUP_EXPECTED_PATH", path, TRUE);
  gchar *args[] = {(gchar *)arboretum_test_executable, path, NULL};
  g_autofree gchar *out = NULL;
  g_autofree gchar *err = NULL;
  g_autoptr(GError) error = NULL;
  gint status = 0;
  gboolean ok = g_spawn_sync(NULL, args, env, G_SPAWN_SEARCH_PATH, NULL, NULL,
                             &out, &err, &status, &error);
  if(out) g_printerr("%s", out);
  if(err) g_printerr("%s", err);
  if(ok) ok = g_spawn_check_wait_status(status, &error);
  if(error) g_printerr("IO-Test Startargument: %s\n", error->message);
  return ok;
}

static void io_test_drain(void)
{
  for(int i=0; i<20; i++)
  {
    for(int event=0; event<100 && g_main_context_iteration(NULL, FALSE); event++) {}
    g_usleep(10000);
  }
}

#ifdef G_OS_WIN32
#include <windows.h>
#endif

static gchar *io_test_canonical_path(const char *path)
{
  if(!path)
    return NULL;
  g_autofree gchar *canonical = g_canonicalize_filename(path, NULL);
#ifdef G_OS_WIN32
  /* The runner's TEMP contains RUNNER~1, whereas the chooser may return the
   * long directory name. Resolve the existing parent, not the unsaved file. */
  g_autofree gchar *parent = g_path_get_dirname(canonical);
  g_autofree gchar *name = g_path_get_basename(canonical);
  g_autofree gunichar2 *wide = g_utf8_to_utf16(parent, -1, NULL, NULL, NULL);
  DWORD size = wide ? GetLongPathNameW((LPCWSTR)wide, NULL, 0) : 0;
  if(size)
  {
    g_autofree gunichar2 *long_name = g_new0(gunichar2, size);
    DWORD length = GetLongPathNameW((LPCWSTR)wide, (LPWSTR)long_name, size);
    if(length && length < size)
    {
      g_autofree gchar *long_parent = g_utf16_to_utf8(long_name, -1, NULL, NULL, NULL);
      if(long_parent)
      {
        g_free(canonical);
        canonical = g_build_filename(long_parent, name, NULL);
      }
    }
  }
  return g_utf8_casefold(canonical, -1);
#else
  return g_steal_pointer(&canonical);
#endif
}

static gboolean io_test_same_path(const char *a, const char *b)
{
  g_autofree gchar *first = io_test_canonical_path(a);
  g_autofree gchar *second = io_test_canonical_path(b);
  return first && second && !strcmp(first, second);
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
    g_autofree gchar *selected = gtk_file_chooser_get_filename(chooser);
    g_autofree gchar *expected = g_build_filename(test->directory, test->filename, NULL);
    if(test->attempts == 2 || test->attempts == 199)
      g_printerr("IO-Test Dialogpfad: gewählt=%s; erwartet=%s\n",
                 selected ? selected : "(noch kein Pfad)", expected);
    if(test->initialized && io_test_same_path(selected, expected))
    {
      test->finished = TRUE;
      gtk_dialog_response(GTK_DIALOG(candidate), GTK_RESPONSE_ACCEPT);
      return G_SOURCE_REMOVE;
    }
    /* Restoring the chooser's last-used folder is asynchronous and can replace
     * an early test selection. Retry setup, but never accept another folder. */
    if(gtk_widget_get_mapped(GTK_WIDGET(candidate)) &&
        (!test->initialized || test->attempts % 10 == 0))
    {
      g_autoptr(GFile) folder = g_file_new_for_path(test->directory);
      g_autoptr(GError) error = NULL;
      if(!gtk_file_chooser_set_current_folder(chooser, folder, &error))
      {
        g_printerr("IO-Test Dialogordner: %s\n", error ? error->message : "Ordnerwechsel fehlgeschlagen");
        test->finished = TRUE;
        gtk_dialog_response(GTK_DIALOG(candidate), GTK_RESPONSE_CANCEL);
        return G_SOURCE_REMOVE;
      }
      gtk_file_chooser_set_current_name(chooser, test->filename);
      test->initialized = TRUE;
      return G_SOURCE_CONTINUE;
    }
  }
  return G_SOURCE_CONTINUE;
}

static gboolean io_test_loaded_selection(void)
{
  int start, end;
  gboolean ok = TRUE;
  for(int i=0; i<=maxzaehler; i++)
  {
    ok &= !gtk_editable_get_selection_bounds(GTK_EDITABLE(textfeld[i]), &start, &end);
    ok &= !gtk_editable_get_selection_bounds(GTK_EDITABLE(textfeldWahrscheinlichkeit[i]), &start, &end);
  }
  for(int i=0; i<=maxzaehlererg; i++)
  {
    ok &= !gtk_editable_get_selection_bounds(GTK_EDITABLE(textfeldErgebnis[i]), &start, &end);
    ok &= !gtk_editable_get_selection_bounds(GTK_EDITABLE(textfeldErgebnisWahrscheinlichkeit[i]), &start, &end);
  }
  ok &= gtk_window_get_focus(GTK_WINDOW(window)) == textfeld[0];
  ok &= gtk_editable_get_position(GTK_EDITABLE(textfeld[0])) ==
        g_utf8_strlen(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), -1);
  g_printerr("IO-Test Laden ohne Textmarkierung (%d Knoten): %s\n", maxzaehler+1, ok ? "ok" : "FEHLER");
  return ok;
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
  gtk_editable_set_text(GTK_EDITABLE(textfeld[0]), "Ä̅pfel Ω");
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
    ok &= !strcmp(gtk_editable_get_text(GTK_EDITABLE(textfeld[0])), "Ä̅pfel Ω");
    ok &= arboretum_attributes_contain(
              gtk_entry_get_attributes(GTK_ENTRY(textfeld[0])),
              PANGO_ATTR_OVERLINE);
  }
  g_printerr("IO-Test Unicode speichern/laden: %s\n", ok ? "ok" : "FEHLER");
  ok &= io_test_startup_file(dir);
  io_test_drain();
  ok &= io_test_loaded_selection();
  IoSaveDialogTest dialog_test = {dir, "Dialog-Ä.bdg", 0, FALSE, FALSE};
  guint dialog_source = g_timeout_add(100, io_test_choose_save, &dialog_test);
  gboolean dialog_ok = speicherdialog(NULL, data);
  if(!dialog_test.finished)
    g_source_remove(dialog_source);
  g_autofree gchar *dialog_path = g_build_filename(dir, dialog_test.filename, NULL);
  dialog_ok = dialog_ok && g_file_test(dialog_path, G_FILE_TEST_IS_REGULAR) &&
              io_test_same_path(aktuelledatei, dialog_path);
  g_printerr("IO-Test Speicherdialog: %s\n", dialog_ok ? "ok" : "FEHLER");
  ok &= dialog_ok;
  g_autofree gchar *bad = g_build_filename(dir, "fehlt", "nicht-schreibbar.bdg", NULL);
  dateiveraendert = 7;
  g_printerr("IO-Test START erwarteter Schreibfehler (Ordner fehlt absichtlich)\n");
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
      gtk_entry_set_text(GTK_ENTRY(textfeld[1]), "B");
      gtk_entry_set_text(GTK_ENTRY(textfeld[2]), "C");
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
    if(scene == 1)
    {
      g_autofree gchar *multi_path = g_build_filename(dir, "Mehrere-Knoten.bdg", NULL);
      gboolean saved = speichern(multi_path);
      ok &= saved;
      if(saved)
      {
        laden(data, multi_path);
        io_test_drain();
        ok &= io_test_loaded_selection();
        ok &= maxzaehler == 2 &&
              !strcmp(gtk_entry_get_text(GTK_ENTRY(textfeld[1])), "B") &&
              !strcmp(gtk_entry_get_text(GTK_ENTRY(textfeld[2])), "C");
      }
    }
    umwandeln(NULL, data);
    io_test_drain();
    if(scene == 0)
    {
      gboolean overline_ok =
          !strcmp(gtk_label_get_text(GTK_LABEL(knotenlabel[0])), "Äpfel Ω") &&
          arboretum_attributes_contain(
              pango_layout_get_attributes(
                  gtk_label_get_layout(GTK_LABEL(knotenlabel[0]))),
              PANGO_ATTR_OVERLINE);
      g_printerr("IO-Test nativer Überstrich: %s\n",
                 overline_ok ? "ok" : "FEHLER");
      ok &= overline_ok;
    }
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
    g_printerr("IO-Test START erwarteter Exportfehler (Ordner fehlt absichtlich)\n");
    gboolean failed_export_ok = !export_datei(bad, "png");
    g_printerr("IO-Test erwarteter Exportfehler: %s\n", failed_export_ok ? "ok" : "FEHLER");
    ok &= failed_export_ok;
    umwandeln(NULL, data);
    io_test_drain();
  }
  g_printerr("IO-Test: %s\n", ok ? "ok" : "FEHLER");
  arboretum_exit_status = ok ? 0 : 1;
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}
