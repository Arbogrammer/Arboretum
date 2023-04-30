void labelverschieben(gpointer data)
{
  if(labelein)
  {
    int i=0;
    int j=0;
    int mh = 0, nh = 0, mw = 0, nw = 0;
    klbmax = 0;
    for(j=0;j<=maxzaehler;j++)
    {
      gtk_widget_get_preferred_width(knotenlabel[j], &mw, &KnotenLabelBreite);
      if(klbmax<KnotenLabelBreite)
      {
        klbmax=KnotenLabelBreite;
      }
    }
    klbmax += 2*paddingk;
    gtk_widget_get_preferred_height(knotenlabel[0], &mh, &KnotenLabelHoehe);
    int elbmax = 0;
    for(j=0;j<=maxzaehlererg;j++)
    {
    gtk_widget_get_preferred_width(ergebnislabel[j], &mw, &ErgebnisLabelBreite);
      if(elbmax<ErgebnisLabelBreite)
      {
        elbmax=ErgebnisLabelBreite;
      }
    }
    ErgebnisLabelBreite=elbmax;
    int welbmax=0;
    for(j=0;j<=maxzaehlererg;j++)
    {
      if(bruchou && bruch)
      {
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
      else
      {
        gtk_widget_get_preferred_width(ergebniswsklabel[j], &mw, &WahrscheinlichkeitErgebnisLabelBreite);
        if(welbmax<WahrscheinlichkeitErgebnisLabelBreite)
        {
          welbmax=WahrscheinlichkeitErgebnisLabelBreite;
        }
      }
    }
    WahrscheinlichkeitErgebnisLabelBreite=welbmax;

    if(bruchou && bruch)
    {
      for(i=0;i<=maxzaehler;i++)
      {
        GtkAllocation apz;
        GtkAllocation apn;
        gtk_widget_get_allocation(zaehlerlabel[i], &apz);
        gtk_widget_get_allocation(nennerlabel[i], &apn);
        gtk_widget_get_preferred_width(knotenlabel[i], &mw, &KnotenLabelBreite);
        KnotenLabelBreite += 2*paddingk;
        char unten[22] = "";
        sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
        int yunten = y[knotenexistiert(unten)];
        int Stufetemp = zeichenzaehlen(gtk_widget_get_name(zaehlerlabel[i]),'-')-1;
  
        int xA = Stufetemp*(StufenBreite+klbmax);
        int xB = Stufetemp*(StufenBreite+klbmax)+StufenBreite;
        int yA = (Stufetemp==0)?(y[0]+yunten+KnotenHoehe)/2:y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]+KnotenHoehe/2;
        int yB = y[i]+KnotenHoehe/2;
        int bZ = apz.width;
        int hZ = apz.height;
        int bN = apn.width;
        int hN = apn.height;
//        int bZN = ((bZ>bN)?bZ:bN);
//        double Laenge=sqrt(pow(xA-xB,2)+pow(yA-yB,2));
        int xwahrscheinlichkeitZ = FensterRandLinks+RandLinks+0.5*xA+0.5*xB-0.5*bZ+wskverschiebung;
//        printf("xwz: %i, FWL: %i\n",xwahrscheinlichkeitZ,FensterRandLinks);
        int ywahrscheinlichkeitZ = FensterRandOben+RandOben+0.5*yA+0.5*yB-0.5*bN*abs(yA-yB)/StufenBreite-hN-hZ+(double)wskverschiebung*(yB-yA)/(xB-xA);
        int xwahrscheinlichkeitN = FensterRandLinks+RandLinks+0.5*xA+0.5*xB-0.5*bN+wskverschiebung;
        int ywahrscheinlichkeitN = FensterRandOben+RandOben+0.5*yA+0.5*yB-0.5*bN*abs(yA-yB)/StufenBreite-hN+(double)wskverschiebung*(yB-yA)/(xB-xA);
//        printf("xa: %i, ya: %i, xb: %i, yb: %i, bz: %i, hz: %i, bn: %i, hn: %i, Laenge: %f, xwZ: %i, xywZ: %i, xwN: %i, ywN: %i\n",xA,yA,xB,yB,bZ,hZ,bN,hN,Laenge,xwahrscheinlichkeitZ,ywahrscheinlichkeitZ,xwahrscheinlichkeitN,ywahrscheinlichkeitN);
        gtk_layout_move (GTK_LAYOUT (data), zaehlerlabel[i], xwahrscheinlichkeitZ, ywahrscheinlichkeitZ);//FensterRandLinks+RandLinks+StufenBreite/2+natural_size.height*(y[i]-(y[0]+yunten)/2),FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2);
        gtk_layout_move (GTK_LAYOUT (data), nennerlabel[i], xwahrscheinlichkeitN, ywahrscheinlichkeitN);//FensterRandLinks+RandLinks+StufenBreite/2+natural_size.height*(y[i]-(y[0]+yunten)/2),FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2);
        gtk_layout_move (GTK_LAYOUT (data), knotenlabel[i], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+klbmax)*Stufetemp+(klbmax-KnotenLabelBreite)/2+paddingk,FensterRandOben+RandOben+y[i]+KnotenHoehe/2-KnotenLabelHoehe/2);
      }
    }
    else
    {
      for(i=0;i<=maxzaehler;i++)
      {
        GtkAllocation ap;
        gtk_widget_get_allocation(wahrscheinlichkeitlabel[i], &ap);
        gtk_widget_get_preferred_height(wahrscheinlichkeitlabel[i], &mh, &nh);
        gtk_widget_get_preferred_width(wahrscheinlichkeitlabel[i], &mw, &nw);
        gtk_widget_get_preferred_width(knotenlabel[i], &mw, &KnotenLabelBreite);
        KnotenLabelBreite += 2*paddingk;
        char unten[22] = "";
        sprintf(unten,"-%i",anzahlknoteninstufe(0)-1);
        int yunten = y[knotenexistiert(unten)];
        int Stufetemp = zeichenzaehlen(gtk_widget_get_name(wahrscheinlichkeitlabel[i]),'-')-1;
  
        int xA = Stufetemp*(StufenBreite+klbmax);
        int xB = Stufetemp*(StufenBreite+klbmax)+StufenBreite;
        int yA = (Stufetemp==0)?(y[0]+yunten+KnotenHoehe)/2:y[knotenexistiert(gtk_widget_get_name(*vorgaenger[i]))]+KnotenHoehe/2;
        int yB = y[i]+KnotenHoehe/2;
        int b = breitevordrehung[i];
        int h = hoehevordrehung[i];
        double Laenge=sqrt(pow(xA-xB,2)+pow(yA-yB,2));
        winkel[i] = -atan((((double)yB)-((double)yA))/(((double)xB)-((double)xA)))*180.0/G_PI;
        int xwahrscheinlichkeit = (winkel[i]>=0)?(int)(FensterRandLinks+RandLinks+(xA+xB)/2.0-(b/2.0)*(xB-xA)/Laenge+(yB-yA)/Laenge*h-padding*sin(winkel[i]*G_PI/180)+wskverschiebung):(int)(FensterRandLinks+RandLinks+(xA+xB)/2.0-(b/2.0)*(xB-xA)/Laenge-padding*sin(winkel[i]*G_PI/180)+wskverschiebung);
        int ywahrscheinlichkeit = (winkel[i]>=0)?(int)(FensterRandOben+RandOben+(yA+yB)/2.0-(b/2.0)*(yB-yA)/Laenge-nh-padding*cos(winkel[i]*G_PI/180)+(double)wskverschiebung*(yB-yA)/(xB-xA)):(int)(FensterRandOben+RandOben+(yA+yB)/2.0-(b/2.0)*(yB-yA)/Laenge-(xB-xA)/Laenge*h-padding*cos(winkel[i]*G_PI/180)+(double)wskverschiebung*(yB-yA)/(xB-xA));
        gtk_label_set_angle(GTK_LABEL(wahrscheinlichkeitlabel[i]),winkel[i]);
        gtk_layout_move (GTK_LAYOUT (data), wahrscheinlichkeitlabel[i], xwahrscheinlichkeit, ywahrscheinlichkeit);//FensterRandLinks+RandLinks+StufenBreite/2+natural_size.height*(y[i]-(y[0]+yunten)/2),FensterRandOben+RandOben+(((y[0]+yunten+KnotenHoehe)/2)+(y[i]+KnotenHoehe/2))/2);
        gtk_layout_move (GTK_LAYOUT (data), knotenlabel[i], FensterRandLinks+RandLinks+StufenBreite+(StufenBreite+klbmax)*Stufetemp+(klbmax-KnotenLabelBreite)/2+paddingk,FensterRandOben+RandOben+y[i]+KnotenHoehe/2-KnotenLabelHoehe/2);
      }
    }
    for(i=0;i<=maxzaehlererg;i++)
    {
      gtk_layout_move (GTK_LAYOUT (data), ergebnislabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2);
      if(bruchou && bruch)
      {
        GtkAllocation apz;
        GtkAllocation apn;
        gtk_widget_get_allocation(ergebniszaehlerlabel[i], &apz);
        gtk_widget_get_allocation(ergebnisnennerlabel[i], &apn);
        int bZ = apz.width;
//        int hZ = apz.height;
        int bN = apn.width;
//        int hN = apn.height;
//        int bZN = ((bZ>bN)?bZ:bN);
        gtk_layout_move (GTK_LAYOUT (data), ergebniszaehlerlabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisLabelBreite:0)+(welbmax-bZ)/2+2*knotenrahmenabstand,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2-ZaehlerErgebnisLabelHoehe/2);
        gtk_layout_move (GTK_LAYOUT (data), ergebnisnennerlabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisLabelBreite:0)+(welbmax-bN)/2+2*knotenrahmenabstand,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2+NennerErgebnisLabelHoehe/2);
      }
      else
      {
        gtk_layout_move (GTK_LAYOUT (data), ergebniswsklabel[i], FensterRandLinks+RandLinks+(maxStufe+1)*StufenBreite+(maxStufe+1)*klbmax+ErgebnisAbstand+((ergebnisseanzeigen)?ErgebnisAbstand+ErgebnisLabelBreite:0)+2*knotenrahmenabstand,FensterRandOben+RandOben+yerg[i]+KnotenHoehe/2-KnotenLabelHoehe/2);
      }
    }
  }
  else
  {
    return;
  }
  
}
