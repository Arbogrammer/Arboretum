static void knotenranddickeaendern ()
{
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  char cssdaten[200000] = "";
  int i=0;
  for(i=0;i<=maxzaehler;i++)
  {
    sprintf(cssdaten+strlen(cssdaten),"entry#%s,",gtk_widget_get_name(textfeld[i]));
    sprintf(cssdaten+strlen(cssdaten),"entry#%s,",gtk_widget_get_name(textfeldWahrscheinlichkeit[i]));
  }
  for(i=0;i<=maxzaehlererg;i++)
  {
    sprintf(cssdaten+strlen(cssdaten),"entry#%s,",gtk_widget_get_name(textfeldErgebnis[i]));
    sprintf(cssdaten+strlen(cssdaten),"entry#%s,",gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]));
  }
  sprintf(cssdaten+strlen(cssdaten)-1, " {border-width: %ipx;}",knotenrahmendicke);
  gtk_css_provider_load_from_data(cssProvider, cssdaten,-1, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

