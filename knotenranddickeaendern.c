static void knotenranddickeaendern ()
{
  if(knotenranddicke_provider)
  {
    arboretum_remove_css_provider(knotenranddicke_provider);
    g_clear_object(&knotenranddicke_provider);
  }
  if(!knotenrahmendickegeaendert) return;
  knotenranddicke_provider = gtk_css_provider_new();
  char cssdaten[1000] = "";
  sprintf(cssdaten, "entry {border-width: %ipx;}",knotenrahmendicke);
  gtk_css_provider_load_from_data(knotenranddicke_provider, cssdaten, -1);
  arboretum_add_css_provider(knotenranddicke_provider);
}
