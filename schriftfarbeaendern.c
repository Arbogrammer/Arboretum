static void schriftfarbeaendern ()
{
  if(schriftfarbe_provider)
  {
    arboretum_remove_css_provider(schriftfarbe_provider);
    g_clear_object(&schriftfarbe_provider);
  }
  if(!schriftfarbewurdegeaendert) return;
  schriftfarbe_provider = gtk_css_provider_new();
  char cssdaten[1000] = "";
  char *rot=ftstr(schriftfarbe.red*255);
  char *gruen=ftstr(schriftfarbe.green*255);
  char *blau=ftstr(schriftfarbe.blue*255);
  char *opak=ftstr(schriftfarbe.alpha);
  sprintf(cssdaten, "entry {color: rgba(%s,%s,%s,%s);}",rot,gruen,blau,opak);
  free(rot);
  free(gruen);
  free(blau);
  free(opak);

  gtk_css_provider_load_from_data(schriftfarbe_provider, cssdaten, -1);
  arboretum_add_css_provider(schriftfarbe_provider);
  gtk_widget_queue_draw(da);
}
