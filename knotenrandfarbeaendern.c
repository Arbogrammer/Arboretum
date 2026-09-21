static void knotenrandfarbeaendern ()
{
  if(knotenrand_provider)
  {
    arboretum_remove_css_provider(knotenrand_provider);
    g_clear_object(&knotenrand_provider);
  }
  if(!knotenrahmenfarbewurdegeaendert) return;
  knotenrand_provider = gtk_css_provider_new();
  char cssdaten[1000] = "";
  char *rot=ftstr(knotenrandfarbe.red*255);
  char *gruen=ftstr(knotenrandfarbe.green*255);
  char *blau=ftstr(knotenrandfarbe.blue*255);
  char *opak=ftstr(knotenrandfarbe.alpha);
  sprintf(cssdaten, "entry {border-color: rgba(%s,%s,%s,%s);}",rot,gruen,blau,opak);
  free(rot);
  free(gruen);
  free(blau);
  free(opak);
  gtk_css_provider_load_from_data(knotenrand_provider, cssdaten, -1);
  arboretum_add_css_provider(knotenrand_provider);
}
