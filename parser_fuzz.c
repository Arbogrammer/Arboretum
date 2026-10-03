#define main arboretum_application_main
#include "baumg.c"
#undef main

int LLVMFuzzerTestOneInput(const unsigned char *daten, size_t laenge) {
  const char *grund = NULL;
  bdg_struktur_pruefen((const char *)daten, (gsize)laenge, &grund);
  return 0;
}
