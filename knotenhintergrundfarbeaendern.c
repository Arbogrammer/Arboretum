static void knotenhintergrundfarbeaendern ()
{
  if(knotenhintergrund_provider)
  {
    arboretum_remove_css_provider(knotenhintergrund_provider);
    g_clear_object(&knotenhintergrund_provider);
  }
  if(!knotenhintergrundfarbewurdegeaendert) return;
  knotenhintergrund_provider = gtk_css_provider_new();
  char cssdaten[1000] = "";
  char *rot=ftstr(knotenhintergrundfarbe.red*255);
  char *gruen=ftstr(knotenhintergrundfarbe.green*255);
  char *blau=ftstr(knotenhintergrundfarbe.blue*255);
  char *opak=ftstr(knotenhintergrundfarbe.alpha);
  sprintf(cssdaten, "entry {background-color: rgba(%s,%s,%s,%s);}",rot,gruen,blau,opak);
  free(rot);
  free(gruen);
  free(blau);
  free(opak);
  gtk_css_provider_load_from_data(knotenhintergrund_provider, cssdaten, -1);
  arboretum_add_css_provider(knotenhintergrund_provider);
}
