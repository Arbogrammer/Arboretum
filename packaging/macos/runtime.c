/* Finder must launch the Mach-O executable itself, not a shell wrapper. */
#include <mach-o/dyld.h>

static gchar *macos_pixbuf_cache;

static void macos_cleanup_cache(void)
{
  if(macos_pixbuf_cache) g_remove(macos_pixbuf_cache);
  g_clear_pointer(&macos_pixbuf_cache, g_free);
}

static gboolean macos_prepare_bundle(void)
{
  uint32_t size = 0;
  _NSGetExecutablePath(NULL, &size);
  g_autofree gchar *executable = g_malloc(size);
  if(_NSGetExecutablePath(executable, &size)) return FALSE;
  g_autofree gchar *directory = g_path_get_dirname(executable);
  g_autofree gchar *contents = g_path_get_dirname(directory);
  g_autofree gchar *frameworks = g_build_filename(contents, "Frameworks", NULL);
  g_autofree gchar *share = g_build_filename(contents, "Resources", "share", NULL);
  g_autofree gchar *schemas = g_build_filename(share, "glib-2.0", "schemas", NULL);
  g_autofree gchar *query = g_build_filename(frameworks, "gdk-pixbuf-query-loaders", NULL);
  if(!g_file_test(query, G_FILE_TEST_IS_EXECUTABLE)) return TRUE; /* developer build */
  g_setenv("XDG_DATA_DIRS", share, TRUE);
  g_setenv("GSETTINGS_SCHEMA_DIR", schemas, TRUE);
  g_setenv("GDK_PIXBUF_MODULEDIR", frameworks, TRUE);
  g_autoptr(GPtrArray) args = g_ptr_array_new_with_free_func(g_free);
  g_ptr_array_add(args, g_strdup(query));
  g_autoptr(GDir) modules = g_dir_open(frameworks, 0, NULL);
  if(!modules) return FALSE;
  const gchar *name;
  while((name = g_dir_read_name(modules)))
    if(g_str_has_prefix(name, "libpixbufloader-") && g_str_has_suffix(name, ".so"))
      g_ptr_array_add(args, g_build_filename(frameworks, name, NULL));
  g_ptr_array_add(args, NULL);
  g_autofree gchar *output = NULL;
  g_autoptr(GError) error = NULL;
  gint status;
  if(!g_spawn_sync(NULL, (gchar **)args->pdata, NULL, 0, NULL, NULL,
                   &output, NULL, &status, &error) ||
      !g_spawn_check_wait_status(status, &error))
  {
    g_printerr("Bildformat-Module: %s\n", error->message);
    return FALSE;
  }
  int fd = g_file_open_tmp("arboretum-pixbuf-XXXXXX", &macos_pixbuf_cache, &error);
  if(fd < 0) return FALSE;
  close(fd);
  atexit(macos_cleanup_cache);
  if(!g_file_set_contents(macos_pixbuf_cache, output, -1, &error)) return FALSE;
  g_setenv("GDK_PIXBUF_MODULE_FILE", macos_pixbuf_cache, TRUE);
  return TRUE;
}
