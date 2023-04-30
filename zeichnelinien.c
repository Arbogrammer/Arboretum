static gboolean zeichnelinien(GtkWidget *widget, cairo_t *cr, gpointer data)
{

  cairo_set_source_rgba(cr, hintergrundfarbe.red, hintergrundfarbe.green, hintergrundfarbe.blue, hintergrundfarbe.alpha);
  cairo_paint(cr);

  if(labelein)
  {
    int i=0;
    for(i=0;i<=maxzaehler;i++)
    {
      GtkAllocation alloc;
      gtk_widget_get_allocation(knotenlabel[i],&alloc);
      cairo_set_source_rgba(cr,knotenhintergrundfarbe.red,knotenhintergrundfarbe.green,knotenhintergrundfarbe.blue,knotenhintergrundfarbe.alpha);
      cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
      cairo_fill(cr);
      cairo_set_source_rgba(cr,knotenrandfarbe.red,knotenrandfarbe.green,knotenrandfarbe.blue,knotenrandfarbe.alpha);
      cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
      cairo_set_line_width(cr, LinienDicke);
      cairo_stroke(cr);
    }
    if(ergebnisseanzeigen)
    {
      for(i=0;i<=maxzaehlererg;i++)
      {
        GtkAllocation alloc;
        gtk_widget_get_allocation(ergebnislabel[i],&alloc);
        cairo_set_source_rgba(cr,knotenhintergrundfarbe.red,knotenhintergrundfarbe.green,knotenhintergrundfarbe.blue,knotenhintergrundfarbe.alpha);
        cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
        cairo_fill(cr);
        cairo_set_source_rgba(cr,knotenrandfarbe.red,knotenrandfarbe.green,knotenrandfarbe.blue,knotenrandfarbe.alpha);
        cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
        cairo_set_line_width(cr, LinienDicke);
        cairo_stroke(cr);
      }
    }
    if(ergebnissewskanzeigen)
    {
      for(i=0;i<=maxzaehlererg;i++)
      {
        if(bruchou && bruch)
        {
          GtkAllocation allocz;
          GtkAllocation allocn;
          gtk_widget_get_allocation(ergebniszaehlerlabel[i],&allocz);
          gtk_widget_get_allocation(ergebnisnennerlabel[i],&allocn);
          int bruchhoehe = allocz.height+allocn.height;
          int bruchbreite = ((allocz.width>allocn.width)?allocz.width:allocn.width);
          int bruchx = ((allocz.x<allocn.x)?allocz.x:allocn.x);
          int bruchy = allocz.y;
          cairo_set_source_rgba(cr,knotenhintergrundfarbe.red,knotenhintergrundfarbe.green,knotenhintergrundfarbe.blue,knotenhintergrundfarbe.alpha);
          cairo_rectangle(cr,bruchx-FensterRandLinks-knotenrahmenabstand,bruchy-FensterRandOben-knotenrahmenabstand,bruchbreite+2*knotenrahmenabstand,bruchhoehe+2*knotenrahmenabstand);
          cairo_fill(cr);
          cairo_set_source_rgba(cr,knotenrandfarbe.red,knotenrandfarbe.green,knotenrandfarbe.blue,knotenrandfarbe.alpha);
          cairo_rectangle(cr,bruchx-FensterRandLinks-knotenrahmenabstand,bruchy-FensterRandOben-knotenrahmenabstand,bruchbreite+2*knotenrahmenabstand,bruchhoehe+2*knotenrahmenabstand);
          cairo_set_line_width(cr, LinienDicke);
          cairo_stroke(cr);
        }
        else
        {
          GtkAllocation alloc;
          gtk_widget_get_allocation(ergebniswsklabel[i],&alloc);
          cairo_set_source_rgba(cr,knotenhintergrundfarbe.red,knotenhintergrundfarbe.green,knotenhintergrundfarbe.blue,knotenhintergrundfarbe.alpha);
          cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
          cairo_fill(cr);
          cairo_set_source_rgba(cr,knotenrandfarbe.red,knotenrandfarbe.green,knotenrandfarbe.blue,knotenrandfarbe.alpha);
          cairo_rectangle(cr,alloc.x-FensterRandLinks-knotenrahmenabstand,alloc.y-FensterRandOben-knotenrahmenabstand,alloc.width+2*knotenrahmenabstand,alloc.height+2*knotenrahmenabstand);
          cairo_set_line_width(cr, LinienDicke);
          cairo_stroke(cr);
        }
      }
    }
  }


  if(data)
  {
    if(labelein)
    {
      labelverschieben(data);
    }
    else
    {
      wskergebnisverschieben(NULL, NULL, data);
      positionsanpassungwsk(data);
      groesseneu(NULL, NULL, data);
    }
//    schriftartanpassen(NULL,NULL,data);
  }
  int i=0;
  klbmax = 0;
  if(labelein)
  {
    for(i=0;i<=maxzaehler;i++)
    {
      if(klbmax < gtk_widget_get_allocated_width(knotenlabel[i]))
      {
        klbmax = gtk_widget_get_allocated_width(knotenlabel[i]);
      }
    }
  }
  klbmax += 2*paddingk;


  
  
  i=0;
  char knotenname[10000] = "";
  sprintf(knotenname,"-%i",i);
  int zaehlertemp = knotenexistiert(knotenname);
  char unten[22] = "";
  int knotennummer = 0, j = 0;
  for(j=0;j<=maxzaehler;j++)
  {
    if(textfeld[j])
    {
      if(zeichenzaehlen(gtk_widget_get_name(textfeld[j]),'-') == 1)
      {
        knotennummer +=1;
      }
    }
  }
  sprintf(unten,"-%i",knotennummer-1);
  int ytemp = y[knotenexistiert(unten)];
  while(zaehlertemp > -1)
  {
    cairo_set_source_rgba(cr,zweigfarbe.red,zweigfarbe.green,zweigfarbe.blue,zweigfarbe.alpha);
    cairo_set_line_width(cr, LinienDicke);
    cairo_move_to(cr, RandLinks,RandOben+(y[0]+ytemp+KnotenHoehe)/2);
    cairo_line_to(cr, RandLinks+StufenBreite,RandOben+y[zaehlertemp]+KnotenHoehe/2);
    cairo_stroke(cr);
    i++;
    memset(knotenname,0,10000);
    sprintf(knotenname,"-%i",i);
    zaehlertemp = knotenexistiert(knotenname);
  }

  for(i=0; i <= maxzaehler; i++)
  {
    if(textfeld[i])
    {
      int tempstufe = zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-');
      if(labelein)
      {
        KnotenLabelBreite = gtk_widget_get_allocated_width(knotenlabel[i])+2*paddingk;
      }
      if(tempstufe <= maxStufe)
      {
        int j=0;
        for(j=0; j<anzahlnachfolger(i); j++)
        {
          cairo_set_source_rgba(cr,zweigfarbe.red,zweigfarbe.green,zweigfarbe.blue,zweigfarbe.alpha);
          cairo_set_line_width(cr, LinienDicke);
          cairo_move_to(cr, RandLinks+(StufenBreite+((labelein==0)?KnotenBreite:klbmax))*tempstufe,RandOben+y[i]+KnotenHoehe/2);
          cairo_line_to(cr, RandLinks+(StufenBreite+((labelein==0)?KnotenBreite:klbmax))*tempstufe+StufenBreite,RandOben+y[nachfolger(i,j)]+KnotenHoehe/2);
          cairo_stroke(cr);
        }
      }
    }
  }
  
  
    if(bruchou && bruch && labelein)
    {
      for(i=0;i<=maxzaehler;i++)
      {
        GtkAllocation apz;
        GtkAllocation apn;
        gtk_widget_get_allocation(zaehlerlabel[i], &apz);
        gtk_widget_get_allocation(nennerlabel[i], &apn);
        char unten[22] = "";
        sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
        int yunten = y[knotenexistiert(unten)];
        int Stufetemp = zeichenzaehlen(gtk_widget_get_name(textfeld[i]),'-')-1;
        int xA = Stufetemp*(StufenBreite+klbmax);
        int xB = Stufetemp*(StufenBreite+klbmax)+StufenBreite;
        int yA = (Stufetemp==0)?(y[0]+yunten+KnotenHoehe)/2:y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]+KnotenHoehe/2;
        int yB = y[i]+KnotenHoehe/2;
        int bZ = apz.width;
//        int hZ = apz.height;
        int bN = apn.width;
        int hN = apn.height;
//        double Laenge=sqrt(pow(xA-xB,2)+pow(yA-yB,2));
        int xwahrscheinlichkeitZ = RandLinks+0.5*xA+0.5*xB-0.5*bZ;
//        int ywahrscheinlichkeitZ = RandOben+0.5*yA+0.5*yB-0.5*bN*abs(yA-yB)/StufenBreite-hN-hZ;
        int xwahrscheinlichkeitN = RandLinks+0.5*xA+0.5*xB-0.5*bN;
        int ywahrscheinlichkeitN = RandOben+0.5*yA+0.5*yB-0.5*bN*abs(yA-yB)/StufenBreite-hN;

        cairo_set_source_rgba(cr,schriftfarbe.red,schriftfarbe.green,schriftfarbe.blue,schriftfarbe.alpha);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, ((xwahrscheinlichkeitN>xwahrscheinlichkeitZ)?xwahrscheinlichkeitZ:xwahrscheinlichkeitN)+wskverschiebung,ywahrscheinlichkeitN+(double)wskverschiebung*(yB-yA)/(xB-xA));
        cairo_line_to(cr, ((xwahrscheinlichkeitN>xwahrscheinlichkeitZ)?xwahrscheinlichkeitZ:xwahrscheinlichkeitN)+((bN>bZ)?bN:bZ)+wskverschiebung,ywahrscheinlichkeitN+(double)wskverschiebung*(yB-yA)/(xB-xA));
        cairo_stroke(cr);        
      }
    }
  
    if(bruchou && bruch && labelein)
    {
      int welbmax=0;
      for(j=0;j<=maxzaehlererg;j++)
      {
        int mw = 0;
        gtk_widget_get_preferred_width(ergebniszaehlerlabel[j], &mw, &ZaehlerErgebnisLabelBreite);
        gtk_widget_get_preferred_width(ergebnisnennerlabel[j], &mw, &NennerErgebnisLabelBreite);
        gtk_widget_get_preferred_height(ergebniszaehlerlabel[j], &mw, &ZaehlerErgebnisLabelHoehe);
        gtk_widget_get_preferred_height(ergebnisnennerlabel[j], &mw, &NennerErgebnisLabelHoehe);
        if(welbmax<ZaehlerErgebnisLabelBreite)
        {
          welbmax=ZaehlerErgebnisLabelBreite;
        }
        if(welbmax<NennerErgebnisLabelBreite)
        {
          welbmax=NennerErgebnisLabelBreite;
        }
      }
      for(i=0;i<=maxzaehlererg;i++)
      {
        GtkAllocation apz;
        GtkAllocation apn;
        gtk_widget_get_allocation(ergebniszaehlerlabel[i], &apz);
        gtk_widget_get_allocation(ergebnisnennerlabel[i], &apn);
        int bZ = apz.width;
//        int hZ = apz.height;
        int bN = apn.width;
//        int hN = apn.height;
        int bZN = ((bZ>bN)?bZ:bN);
        cairo_set_source_rgba(cr,schriftfarbe.red,schriftfarbe.green,schriftfarbe.blue,schriftfarbe.alpha);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisLabelBreite:0)+(welbmax-bZN)/2+2*knotenrahmenabstand,RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2+ZaehlerErgebnisLabelHoehe/2);
        cairo_line_to(cr, RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisLabelBreite:0)+welbmax-(welbmax-bZN)/2+2*knotenrahmenabstand,RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2+ZaehlerErgebnisLabelHoehe/2);
        cairo_stroke(cr);        
      }
    }
  
  
  
  if(data == NULL)
  {
    return TRUE;
  }
  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
  GROESSEDRAWINGAREA
  GROESSELAYOUTD
  if(scrh)
  {
    gtk_adjustment_set_value(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrollwindow)),gtk_adjustment_get_upper(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrollwindow))));
    scrh=0;
  }
  if(gtk_window_get_focus(GTK_WINDOW(window)))
  {
    if(y[knotenexistiert(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))))] + KnotenHoehe + KnotenAbstand + RandOben - gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollwindow))) > gtk_widget_get_allocated_height(scrollwindow) || y[knotenexistiert(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))))] < gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollwindow))))
    {
      gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollwindow)),y[knotenexistiert(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))))]-gtk_widget_get_allocated_height(scrollwindow)+KnotenHoehe+KnotenAbstand+RandOben);
      scrv=0;
    }
  }
  return FALSE;
}
