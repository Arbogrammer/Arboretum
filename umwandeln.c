void umwandeln(GtkWidget *widget, gpointer data)
{
  if(labelein==0)
  {
    labelein=1;
    int i=0;
    for(i=0; i<=maxzaehler; i++)
    {
      char name[10000];
      strcpy(name, gtk_widget_get_name(GTK_WIDGET(textfeld[i])));
      char text[10000];
      strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeld[i])));
      gtk_widget_hide(textfeld[i]);
      knotenlabel[i] = gtk_label_new(NULL);
      gtk_widget_set_name(knotenlabel[i], name);
      
      char format[10000] = "";
      sprintf(format,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
      arboretum_label_set_formatted(GTK_LABEL(knotenlabel[i]), format, text);
      
      int tempStufe = zeichenzaehlen(name,'-')-1;
      gtk_layout_put (GTK_LAYOUT (data), knotenlabel[i], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*tempStufe+KnotenBreite/2,FensterRandOben+RandOben+y[i]+KnotenHoehe/4);
      gtk_widget_show(knotenlabel[i]);
    }

    if(bruchou && bruch)
    {
      for(i=0; i<=maxzaehler; i++)
      {
        char namez[10000] = "";
        char namen[10000] = "";
        sprintf(namez, "%sZ",gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])));
        sprintf(namen, "%sN",gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])));
        char zaehlertext[10000] = "";
        char nennertext[10000] = "";
        char *strich;
        strcpy(zaehlertext, gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
        strich = strchr(zaehlertext,'/');
        if(strich)
        {
          zaehlertext[strich-zaehlertext] = 0;
          strcpy(nennertext, strich+1);
        }
        gtk_widget_hide(textfeldWahrscheinlichkeit[i]);
        zaehlerlabel[i] = gtk_label_new(zaehlertext);
        nennerlabel[i] = gtk_label_new(nennertext);
        gtk_widget_set_name(zaehlerlabel[i], namez);
        gtk_widget_set_name(nennerlabel[i], namen);

        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        sprintf(formatn,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        arboretum_label_set_formatted(GTK_LABEL(zaehlerlabel[i]), formatz,
                                      zaehlertext);
        arboretum_label_set_formatted(GTK_LABEL(nennerlabel[i]), formatn,
                                      nennertext);
/*
        if(zeichenzaehlen(gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])),'-') == 1)
        {
          char unten[22] = "";
          sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
*/          gtk_layout_put (GTK_LAYOUT (data), zaehlerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
          gtk_layout_put (GTK_LAYOUT (data), nennerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
/*        }
        else
        {
          int Stufetemp = zeichenzaehlen(gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])),'-')-1;
          int ywsktemp=(y[knotenexistiert(gtk_widget_get_name(GTK_WIDGET(*vorgaenger[i])))]+y[i])/2;
          gtk_layout_put (GTK_LAYOUT (data), zaehlerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
          gtk_layout_put (GTK_LAYOUT (data), nennerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
        }*/
      }
    }
    else
    {
      for(i=0; i<=maxzaehler; i++)
      {
        char name[10000];
        strcpy(name, gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])));
        char text[10000];
        strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])));
        gtk_widget_hide(textfeldWahrscheinlichkeit[i]);
        wahrscheinlichkeitlabel[i] = gtk_label_new(text);
        gtk_widget_set_name(wahrscheinlichkeitlabel[i], name);
        gtk_label_set_angle(GTK_LABEL(wahrscheinlichkeitlabel[i]),0);

        char format[10000] = "";
        sprintf(format,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        arboretum_label_set_formatted(GTK_LABEL(wahrscheinlichkeitlabel[i]),
                                      format, text);

        if(zeichenzaehlen(gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])),'-') == 1)
        {
          char unten[22] = "";
          sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
          gtk_layout_put (GTK_LAYOUT (data), wahrscheinlichkeitlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
        }
        else
        {
          int Stufetemp = zeichenzaehlen(gtk_widget_get_name(GTK_WIDGET(textfeldWahrscheinlichkeit[i])),'-')-1;
          int ywsktemp=(y[knotenexistiert(gtk_widget_get_name(GTK_WIDGET(*vorgaenger[i])))]+y[i])/2;
          gtk_layout_put (GTK_LAYOUT (data), wahrscheinlichkeitlabel[i], ((FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*Stufetemp)+(FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+KnotenBreite)*(Stufetemp-1)+KnotenBreite))/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+ywsktemp);
        }
      }
    }


    for(i=0; i<=maxzaehler; i++)
    {
      gtk_widget_queue_draw (da);
      if(bruchou && bruch)
      {
        gtk_widget_show(zaehlerlabel[i]);
        gtk_widget_show(nennerlabel[i]);
      }
      else
      {
        gtk_widget_show(wahrscheinlichkeitlabel[i]);
      }
    }

    for(i=0; i<=maxzaehlererg; i++)
    {
      char name[10000];
      strcpy(name, gtk_widget_get_name(GTK_WIDGET((textfeldErgebnis[i]))));
      char text[10000];
      strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])));
      gtk_widget_hide(textfeldErgebnis[i]);
      ergebnislabel[i] = gtk_label_new(text);
      gtk_widget_set_name(ergebnislabel[i], name);

      char format[10000] = "";
      sprintf(format,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
      arboretum_label_set_formatted(GTK_LABEL(ergebnislabel[i]), format, text);

      gtk_layout_put (GTK_LAYOUT (data), ergebnislabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenLabelBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/4);
      gtk_widget_show(ergebnislabel[i]);
    }
    if(bruchou && bruch)
    {
      for(i=0; i<=maxzaehlererg; i++)
      {
        char namez[10000] = "";
        char namen[10000] = "";
        sprintf(namez, "%sZ",gtk_widget_get_name(GTK_WIDGET(textfeldErgebnisWahrscheinlichkeit[i])));
        sprintf(namen, "%sN",gtk_widget_get_name(GTK_WIDGET(textfeldErgebnisWahrscheinlichkeit[i])));
        char zaehlertext[10000] = "";
        char nennertext[10000] = "";
        char *strich;
        strcpy(zaehlertext, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
        strich = strchr(zaehlertext,'/');
        if(strich)
        {
          zaehlertext[strich-zaehlertext] = 0;
          strcpy(nennertext, strich+1);
        }
        gtk_widget_hide(textfeldErgebnisWahrscheinlichkeit[i]);
        ergebniszaehlerlabel[i] = gtk_label_new(zaehlertext);
        ergebnisnennerlabel[i] = gtk_label_new(nennertext);
        gtk_widget_set_name(ergebniszaehlerlabel[i], namez);
        gtk_widget_set_name(ergebnisnennerlabel[i], namen);

        char formatz[10000] = "";
        char formatn[10000] = "";
        sprintf(formatz,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        sprintf(formatn,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        arboretum_label_set_formatted(GTK_LABEL(ergebniszaehlerlabel[i]),
                                      formatz, zaehlertext);
        arboretum_label_set_formatted(GTK_LABEL(ergebnisnennerlabel[i]),
                                      formatn, nennertext);
        gtk_layout_put (GTK_LAYOUT (data), ergebniszaehlerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
        gtk_layout_put (GTK_LAYOUT (data), ergebnisnennerlabel[i], 0,0);//FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2-WahrscheinlichkeitHoehe/2);
      }
    }
    else
    {
      for(i=0; i<=maxzaehlererg; i++)
      {
        char name[10000];
        strcpy(name, gtk_widget_get_name(GTK_WIDGET(textfeldErgebnisWahrscheinlichkeit[i])));
        char text[10000];
        strcpy(text, gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])));
        gtk_widget_hide(textfeldErgebnisWahrscheinlichkeit[i]);
        ergebniswsklabel[i] = gtk_label_new(text);
        gtk_widget_set_name(ergebniswsklabel[i], name);

        char format[10000] = "";
        sprintf(format,"<span font=\"%s\" foreground=\"#%02X%02X%02X\" style=\"normal\">\%s</span>",schriftart,(int)(schriftfarbe.red*255),(int)(schriftfarbe.green*255),(int)(schriftfarbe.blue*255),"%s");
        arboretum_label_set_formatted(GTK_LABEL(ergebniswsklabel[i]), format,
                                      text);

        gtk_layout_put (GTK_LAYOUT (data), ergebniswsklabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenLabelBreite+ErgebnisAbstand*2+ErgebnisLabelBreite,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/4);
        gtk_widget_show(ergebniswsklabel[i]);
      }
    }


    for(i=0; i<=maxzaehlererg; i++)
    {
      if(bruchou && bruch)
      {
        gtk_widget_show(ergebniszaehlerlabel[i]);
        gtk_widget_show(ergebnisnennerlabel[i]);
      }
      else
      {
        gtk_widget_show(ergebniswsklabel[i]);
      }
    }
  }
  else
  {
    labelein=0;
    int i=0;
    for(i=0; i<=maxzaehler; i++)
    {
      gtk_widget_hide(knotenlabel[i]);
      gtk_widget_show(textfeld[i]);
      if(bruchou && bruch)
      {
        gtk_widget_hide(zaehlerlabel[i]);
        gtk_widget_hide(nennerlabel[i]);
      }
      else
      {
        gtk_widget_hide(wahrscheinlichkeitlabel[i]);
      }
      gtk_widget_show(textfeldWahrscheinlichkeit[i]);
    }
    for(i=0; i<=maxzaehlererg; i++)
    {
      gtk_widget_hide(ergebnislabel[i]);
      gtk_widget_show(textfeldErgebnis[i]);
      if(bruchou && bruch)
      {
        gtk_widget_hide(ergebniszaehlerlabel[i]);
        gtk_widget_hide(ergebnisnennerlabel[i]);
      }
      else
      {
        gtk_widget_hide(ergebniswsklabel[i]);
      }
      gtk_widget_show(textfeldErgebnisWahrscheinlichkeit[i]);
    }
  }
  gtk_widget_queue_draw (da);
}
