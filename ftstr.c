char *ftstr(double zahl) {
  /* %f has no useful fixed upper bound (for example for DBL_MAX).  Let GLib
   * allocate exactly the required amount instead of writing into a guessed
   * buffer. */
  char *zahlstring = g_strdup_printf("%f", zahl);
  char *komma = strchr(zahlstring, ',');
  if (komma) {
    komma[0] = '.';
  }
  return zahlstring;
}
