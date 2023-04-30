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
      char *markup;
      markup = g_markup_printf_escaped (format, text);
      gtk_label_set_markup (GTK_LABEL (knotenlabel[i]), markup);
      g_free (markup);
    }
    if(bruchou && bruch)
    {
      for(i=0;i<=maxzaehler;i++)
      {
        char zaehlertext[10000];
        strcpy(zaehlertext, gtk_label_get_text(GTK_LABEL(zaehlerlabel[i])));
        char nennertext[10000];
        strcpy(nennertext, gtk_label_get_text(GTK_LABEL(nennerlabel[i])));
        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        char *markupz;
        markupz = g_markup_printf_escaped (formatz, zaehlertext);
        gtk_label_set_markup (GTK_LABEL (zaehlerlabel[i]), markupz);
        g_free (markupz);
        sprintf(formatn,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        char *markupn;
        markupn = g_markup_printf_escaped (formatn, nennertext);
        gtk_label_set_markup (GTK_LABEL (nennerlabel[i]), markupn);
        g_free (markupn);
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
        char *markup;
        markup = g_markup_printf_escaped (format, text);
        gtk_label_set_markup (GTK_LABEL (wahrscheinlichkeitlabel[i]), markup);
        g_free (markup);
      }
    }
    for(i=0;i<=maxzaehlererg;i++)
    {
      char text[10000];
      strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])));
      char format[10000] = "";
      sprintf(format,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
      char *markup;
      markup = g_markup_printf_escaped (format, text);
      gtk_label_set_markup (GTK_LABEL (ergebnislabel[i]), markup);
      g_free (markup);
    }
    if(bruchou && bruch)
    {
      for(i=0;i<=maxzaehlererg;i++)
      {
        char zaehlertext[10000];
        strcpy(zaehlertext, gtk_label_get_text(GTK_LABEL(ergebniszaehlerlabel[i])));
        char nennertext[10000];
        strcpy(nennertext, gtk_label_get_text(GTK_LABEL(ergebnisnennerlabel[i])));
        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        char *markupz;
        markupz = g_markup_printf_escaped (formatz, zaehlertext);
        gtk_label_set_markup (GTK_LABEL (ergebniszaehlerlabel[i]), markupz);
        g_free (markupz);
        sprintf(formatn,"<span font=\"%s\" style=\"normal\">\%s</span>",schriftart,"%s");
        char *markupn;
        markupn = g_markup_printf_escaped (formatn, nennertext);
        gtk_label_set_markup (GTK_LABEL (ergebnisnennerlabel[i]), markupn);
        g_free (markupn);
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
        char *markup;
        markup = g_markup_printf_escaped (format, text);
        gtk_label_set_markup (GTK_LABEL (ergebniswsklabel[i]), markup);
        g_free (markup);
      }
    }
  }
  else
  {
    if(schriftartwurdegewechselt)
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
      sprintf(cssdaten+strlen(cssdaten)-1, " {font: %ipx \"%s\";}",atoi(strrchr(schriftart,' ')),schriftartnur);
      gtk_css_provider_load_from_data(cssProvider, cssdaten,-1, NULL);
      gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
  }
}
