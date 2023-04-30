void exportpng (char *dateiname)
{

  Schriftgroesse=10;
  if(strrchr(schriftart,' '))
  {
    if(atoi(strrchr(schriftart,' ')))
    {
      Schriftgroesse = atoi(strrchr(schriftart,' '));
    }
  }
  cairo_surface_t *surface;
  cairo_t *cr;
  GtkAllocation *alloc = malloc(sizeof(*alloc));
  gtk_widget_get_allocation(da,alloc);
  int breiteda = alloc->width;
  int hoeheda = alloc->height;
  free(alloc);

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, breiteda, hoeheda);
  cr = cairo_create(surface);
  
  zeichnelinien(NULL,cr,NULL);
  if(labelein == 0)
  {
    return;
  }

  int i=0;
  for(i=0;i<=maxzaehler;i++)
  {
    GtkAllocation *allo = malloc(sizeof(*allo));
    
    gtk_widget_get_allocation(knotenlabel[i], allo);
    cairo_move_to(cr, allo->x-FensterRandLinks, allo->y-FensterRandOben);
    free(allo);

    PangoLayout *layout = pango_cairo_create_layout (cr);
    pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(knotenlabel[i])), -1);
    PangoFontDescription *desc = pango_font_description_from_string (schriftart);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);
    pango_cairo_show_layout (cr, layout);
  }
  double paddingpng=padding;
  for(i=0;i<=maxzaehler;i++)
  {
    if(bruch && bruchou)
    {
      GtkAllocation *alloz = malloc(sizeof(*alloz));
      GtkAllocation *allon = malloc(sizeof(*allon));
    
      gtk_widget_get_allocation(zaehlerlabel[i], alloz);
      gtk_widget_get_allocation(nennerlabel[i], allon);
      cairo_move_to(cr, alloz->x-FensterRandLinks, alloz->y-FensterRandOben-paddingpng);
      PangoLayout *layout = pango_cairo_create_layout (cr);
      pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(zaehlerlabel[i])), -1);
      PangoFontDescription *desc = pango_font_description_from_string (schriftart);
      pango_layout_set_font_description (layout, desc);
      pango_cairo_show_layout (cr, layout);
      cairo_move_to(cr, allon->x-FensterRandLinks, allon->y-FensterRandOben-paddingpng);
      pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(nennerlabel[i])), -1);
      pango_cairo_show_layout (cr, layout);
      pango_font_description_free (desc);
      free(alloz);
      free(allon);
    }
    else
    {
      GtkAllocation *allo = malloc(sizeof(*allo));
    
      gtk_widget_get_allocation(wahrscheinlichkeitlabel[i], allo);
      cairo_save(cr);
      if(winkel[i] >= 0)
      {
        cairo_move_to(cr,allo->x-FensterRandLinks+(allo->width*tan(winkel[i]*G_PI/180.0)-allo->height)/(tan(winkel[i]*G_PI/180.0)-1.0/tan(winkel[i]*G_PI/180.0))-(paddingpng+hoehevordrehung[i])*sin(winkel[i]*G_PI/180), allo->y+allo->height-FensterRandOben-(paddingpng+hoehevordrehung[i])*cos(winkel[i]*G_PI/180));
      }
      else
      {
        cairo_move_to(cr,allo->x-FensterRandLinks-(paddingpng+hoehevordrehung[i])*sin(winkel[i]*G_PI/180), allo->y+(allo->width-allo->height/tan(-winkel[i]*G_PI/180.0))/(tan(-winkel[i]*G_PI/180.0)-1.0/tan(-winkel[i]*G_PI/180.0))-FensterRandOben-(paddingpng+hoehevordrehung[i])*cos(winkel[i]*G_PI/180));
      }
      cairo_rotate (cr, -winkel[i] * G_PI / 180.0);
      free(allo);
      PangoLayout *layout = pango_cairo_create_layout (cr);
      pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(wahrscheinlichkeitlabel[i])), -1);
      PangoFontDescription *desc = pango_font_description_from_string (schriftart);
      pango_layout_set_font_description (layout, desc);
      pango_font_description_free (desc);
      pango_cairo_show_layout (cr, layout);
      cairo_restore(cr);
    }
  }
  if(ergebnisseanzeigen)
  {
    for(i=0;i<=maxzaehlererg;i++)
    {
      GtkAllocation *allo = malloc(sizeof(*allo));
    
      gtk_widget_get_allocation(ergebnislabel[i], allo);
      cairo_move_to(cr, allo->x-FensterRandLinks, allo->y-FensterRandOben);
      free(allo);
      PangoLayout *layout = pango_cairo_create_layout (cr);
      pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(ergebnislabel[i])), -1);
      PangoFontDescription *desc = pango_font_description_from_string (schriftart);
      pango_layout_set_font_description (layout, desc);
      pango_font_description_free (desc);
      pango_cairo_show_layout (cr, layout);
    }
  }
  if(ergebnissewskanzeigen)
  {
    for(i=0;i<=maxzaehlererg;i++)
    {
      if(bruch && bruchou)
      {
        GtkAllocation *alloz = malloc(sizeof(*alloz));
        GtkAllocation *allon = malloc(sizeof(*allon));
    
        gtk_widget_get_allocation(ergebniszaehlerlabel[i], alloz);
        gtk_widget_get_allocation(ergebnisnennerlabel[i], allon);
        cairo_move_to(cr, alloz->x-FensterRandLinks, alloz->y-FensterRandOben);
        PangoLayout *layout = pango_cairo_create_layout (cr);
        pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(ergebniszaehlerlabel[i])), -1);
        PangoFontDescription *desc = pango_font_description_from_string (schriftart);
        pango_layout_set_font_description (layout, desc);
        pango_cairo_show_layout (cr, layout);
        cairo_move_to(cr, allon->x-FensterRandLinks, allon->y-FensterRandOben);
        pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(ergebnisnennerlabel[i])), -1);
        pango_cairo_show_layout (cr, layout);
        pango_font_description_free (desc);
        free(alloz);
        free(allon);
      }
      else
      {
        GtkAllocation *allo = malloc(sizeof(*allo));
    
        gtk_widget_get_allocation(ergebniswsklabel[i], allo);
        cairo_move_to(cr, allo->x-FensterRandLinks, allo->y-FensterRandOben);
        free(allo);
        PangoLayout *layout = pango_cairo_create_layout (cr);
        pango_layout_set_text (layout, gtk_label_get_text(GTK_LABEL(ergebniswsklabel[i])), -1);
        PangoFontDescription *desc = pango_font_description_from_string (schriftart);
        pango_layout_set_font_description (layout, desc);
        pango_font_description_free (desc);
        pango_cairo_show_layout (cr, layout);
      }
      
    }
  }

  cairo_surface_write_to_png(surface, dateiname);
  cairo_surface_destroy(surface);
  cairo_destroy(cr);

}
