static void schriftfarbeaendern ()
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
  char *rot=ftstr(schriftfarbe.red*255);
  char *gruen=ftstr(schriftfarbe.green*255);
  char *blau=ftstr(schriftfarbe.blue*255);
  char *opak=ftstr(schriftfarbe.alpha);
  sprintf(cssdaten+strlen(cssdaten)-1, " {color: rgba(%s,%s,%s,%s);}",rot,gruen,blau,opak);
  free(rot);
  free(gruen);
  free(blau);
  free(opak);

  gtk_css_provider_load_from_data(cssProvider, cssdaten,-1, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_widget_queue_draw(da);
}

