void schriftartanpassen(GtkFontChooser *self, gchar *fontname, gpointer user_data)
{
  if(labelein)
  {
    int i=0;
    for(i=0;i<=maxzaehler;i++)
    {
      char text[10000];
      strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeld[i])));
      char format[10000] = "";
      sprintf(format,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
      arboretum_label_set_formatted(GTK_LABEL(knotenlabel[i]), format, text);
    }
    if(bruchou && bruch)
    {
      for(i=0;i<=maxzaehler;i++)
      {
        char zaehlertext[10000];
        strcpy(zaehlertext, arboretum_label_get_encoded_text(GTK_LABEL(zaehlerlabel[i])));
        char nennertext[10000];
        strcpy(nennertext, arboretum_label_get_encoded_text(GTK_LABEL(nennerlabel[i])));
        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(zaehlerlabel[i]), formatz,
                                      zaehlertext);
        sprintf(formatn,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(nennerlabel[i]), formatn,
                                      nennertext);
      }
    }
    else
    {
      for(i=0;i<=maxzaehler;i++)
      {
        char text[10000];
        strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
        char format[10000] = "";
        sprintf(format,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(wahrscheinlichkeitlabel[i]),
                                      format, text);
      }
    }
    for(i=0;i<=maxzaehlererg;i++)
    {
      char text[10000];
      strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])));
      char format[10000] = "";
      sprintf(format,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
      arboretum_label_set_formatted(GTK_LABEL(ergebnislabel[i]), format, text);
    }
    if(bruchou && bruch)
    {
      for(i=0;i<=maxzaehlererg;i++)
      {
        char zaehlertext[10000];
        strcpy(zaehlertext, arboretum_label_get_encoded_text(GTK_LABEL(ergebniszaehlerlabel[i])));
        char nennertext[10000];
        strcpy(nennertext, arboretum_label_get_encoded_text(GTK_LABEL(ergebnisnennerlabel[i])));
        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(ergebniszaehlerlabel[i]),
                                      formatz, zaehlertext);
        sprintf(formatn,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(ergebnisnennerlabel[i]),
                                      formatn, nennertext);
      }
    }
    else
    {
      for(i=0;i<=maxzaehlererg;i++)
      {
        char text[10000];
        strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
        char format[10000] = "";
        sprintf(format,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        arboretum_label_set_formatted(GTK_LABEL(ergebniswsklabel[i]), format,
                                      text);
      }
    }
  }
  else
  {
    if(schriftart_provider)
    {
      arboretum_remove_css_provider(schriftart_provider);
      g_clear_object(&schriftart_provider);
    }
    if(schriftartwurdegewechselt && schriftart[0] && strrchr(schriftart,' '))
    {
      schriftart_provider = gtk_css_provider_new();
      char cssdaten[200000] = "";
      char schriftartnur[1000] = "";
      strcpy(schriftartnur, schriftart);
      if(strstr(schriftartnur, " Regular"))
      {
        memset(strstr(schriftartnur, " Regular"),0,1);
      }
      if(strstr(schriftartnur, " Bold"))
      {
        memset(strstr(schriftartnur, " Bold"),0,1);
      }
      if(strstr(schriftartnur, " Italic"))
      {
        memset(strstr(schriftartnur, " Italic"),0,1);
      }
      if(strstr(schriftartnur, " 1"))
      {
        memset(strstr(schriftartnur, " 1"),0,1);
      }
      if(strstr(schriftartnur, " 2"))
      {
        memset(strstr(schriftartnur, " 2"),0,1);
      }
      if(strstr(schriftartnur, " 3"))
      {
        memset(strstr(schriftartnur, " 3"),0,1);
      }
      if(strstr(schriftartnur, " 4"))
      {
        memset(strstr(schriftartnur, " 4"),0,1);
      }
      if(strstr(schriftartnur, " 5"))
      {
        memset(strstr(schriftartnur, " 5"),0,1);
      }
      if(strstr(schriftartnur, " 6"))
      {
        memset(strstr(schriftartnur, " 6"),0,1);
      }
      if(strstr(schriftartnur, " 7"))
      {
        memset(strstr(schriftartnur, " 7"),0,1);
      }
      if(strstr(schriftartnur, " 8"))
      {
        memset(strstr(schriftartnur, " 8"),0,1);
      }
      if(strstr(schriftartnur, " 9"))
      {
        memset(strstr(schriftartnur, " 9"),0,1);
      }
      if(strstr(schriftartnur, ","))
      {
        memset(strstr(schriftartnur, ","),0,1);
      }
      sprintf(cssdaten, "entry {font: %ipx \"%s\";}",atoi(strrchr(schriftart,' ')),schriftartnur);
      gtk_css_provider_load_from_data(schriftart_provider, cssdaten, -1);
      arboretum_add_css_provider(schriftart_provider);
    }
  }
}
