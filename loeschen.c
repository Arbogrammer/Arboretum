void loeschen(gpointer data)
{
  GtkWidget *aktuelleswidget;
  aktuelleswidget = gtk_window_get_focus(GTK_WINDOW(window));
  int tempzaehler = knotenexistiert(gtk_widget_get_name(aktuelleswidget));
//  gtk_widget_grab_focus(textfeld[0]);
  printf("%i\n",tempzaehler);
  gchar name[10000] = "";
  strcat(name,gtk_widget_get_name(aktuelleswidget));
  printf("%s\n",name);
  gchar nameBeginn[10000] = "";
  strcat(nameBeginn,name);
  int k=0;
  while(name[strlen(name)-k-1] != '-')
  {
    nameBeginn[strlen(name)-k-1] = 0;
    k++;
  }
  nameBeginn[strlen(name)-k-1] = 0;
  printf("%s\n",nameBeginn);
  gchar nameEnde[10] = "";
  k=0;
  while(name[strlen(name)-k-1] != '-')
  {
    nameEnde[strlen(name)-strlen(nameBeginn)-k-2] = name[strlen(name)-k-1];
    k++;
  }
  printf("%s\n",nameEnde);
  

//  gtk_widget_destroy(textfeld[tempzaehler]);
  
  int i=0;
  FILE *datei;
  TEMPDATEI dateinummerierung);
  dateinummerierung += 1;
  datei = fopen(dateiname,"w+");
  int j=0;
  for(i=0 ; i<=maxzaehler ; i++)
  {
    gchar altername[10000] = "";
    strcpy(altername,gtk_widget_get_name(textfeld[i]));
    if(strncmp(name,altername,strlen(name)) != 0)
    {
      if(strncmp(nameBeginn,altername,strlen(nameBeginn)) != 0)
      {
        fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,y[i],31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),31);
        printf("%i%c%i%c%s%c%s%c\n",i-j,32,y[i],32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),32);
      }
      else
      {
        gchar neuername[10000] = "";
        strcat(neuername,nameBeginn);
        gchar nummer[10] = "";
        int l=0;
        printf("%li\n",strlen(nameBeginn)+l+1);
        while(altername[strlen(nameBeginn)+l+1] != '-' && altername[strlen(nameBeginn)+l+1] != 0 )
        {
          nummer[l] = altername[strlen(nameBeginn)+l+1];
          l++;
        }
        printf("bisheriger Name: %s, altername: %s, nummer: %s und nameEnde: %s\n",gtk_widget_get_name(textfeld[i]),altername,nummer,nameEnde);
        if(atoi(nummer) > atoi(nameEnde))
        {
          gchar neuenummer[10] = "";
          sprintf(neuenummer,"%i",atoi(nummer)-1);
          printf("neuenummer: %s\n",neuenummer);
          strcat(neuername,"-");
          strcat(neuername,neuenummer);
          strcat(neuername,altername+strlen(nameBeginn)+strlen(nummer)+1);
          fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,y[i],31,neuername,31,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),31);
          printf("%i%c%i%c%s%c%s%c\n",i-j,32,y[i],32,neuername,32,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),32);
        }
        else
        {
          fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,y[i],31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),31);
          printf("%i%c%i%c%s%c%s%c\n",i-j,32,y[i],32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeld[i])),32);
        }
      }
    }
    else
    {
      j++;
    }
  }
  fprintf(datei,"%c\n",30);
  j=0;
  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    gchar altername[10000] = "";
    strcpy(altername,gtk_widget_get_name(textfeldErgebnis[i]));
    if(strncmp(name,altername,strlen(name)) != 0)
    {
      if(strncmp(nameBeginn,altername,strlen(nameBeginn)) != 0)
      {
        fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,yerg[i],31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),31);
        printf("%i%c%i%c%s%c%s%c\n",i-j,32,yerg[i],32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),32);
      }
      else
      {
        gchar neuername[10000] = "";
        strcat(neuername,nameBeginn);
        gchar nummer[10] = "";
        int l=0;
        printf("%li\n",strlen(nameBeginn)+l+1);
        while(altername[strlen(nameBeginn)+l+1] != '-' && altername[strlen(nameBeginn)+l+1] != 0 && altername[strlen(nameBeginn)+l+1] != 'E')
        {
          nummer[l] = altername[strlen(nameBeginn)+l+1];
          l++;
        }
        printf("bisheriger Name: %s, altername: %s, nummer: %s und nameEnde: %s\n",gtk_widget_get_name(textfeldErgebnis[i]),altername,nummer,nameEnde);
        if(atoi(nummer) > atoi(nameEnde))
        {
          gchar neuenummer[10] = "";
          sprintf(neuenummer,"%i",atoi(nummer)-1);
          printf("neuenummer: %s\n",neuenummer);
          strcat(neuername,"-");
          strcat(neuername,neuenummer);
          strcat(neuername,altername+strlen(nameBeginn)+strlen(nummer)+1);
          fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,yerg[i],31,neuername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),31);
          printf("%i%c%i%c%s%c%s%c\n",i-j,32,y[i],32,neuername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),32);
        }
        else
        {
          fprintf(datei,"%i%c%i%c%s%c%s%c\n",i-j,31,yerg[i],31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),31);
          printf("%i%c%i%c%s%c%s%c\n",i-j,32,y[i],32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnis[i])),32);
        }
      }
    }
    else
    {
      j++;
    }
  }
  fprintf(datei,"%c\n",30);
  j=0;
  for(i=0 ; i<=maxzaehler ; i++)
  {
    gchar altername[10000] = "";
    strcpy(altername,gtk_widget_get_name(textfeldWahrscheinlichkeit[i]));
    if(strncmp(name,altername,strlen(name)) != 0)
    {
      if(strncmp(nameBeginn,altername,strlen(nameBeginn)) != 0)
      {
        fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),31);
        printf("%i%c%s%c%s%c\n",i-j,32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),32);
      }
      else
      {
        gchar neuername[10000] = "";
        strcat(neuername,nameBeginn);
        gchar nummer[10] = "";
        int l=0;
        printf("%li\n",strlen(nameBeginn)+l+1);
        while(altername[strlen(nameBeginn)+l+1] != '-' && altername[strlen(nameBeginn)+l+1] != 0 && altername[strlen(nameBeginn)+l+1] != 'W')
        {
          nummer[l] = altername[strlen(nameBeginn)+l+1];
          l++;
        }
        printf("bisheriger Name: %s, altername: %s, nummer: %s und nameEnde: %s\n",gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),altername,nummer,nameEnde);
        if(atoi(nummer) > atoi(nameEnde))
        {
          gchar neuenummer[10] = "";
          sprintf(neuenummer,"%i",atoi(nummer)-1);
          printf("neuenummer: %s\n",neuenummer);
          strcat(neuername,"-");
          strcat(neuername,neuenummer);
          strcat(neuername,altername+strlen(nameBeginn)+strlen(nummer)+1);
          fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,neuername,31,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),31);
          printf("%i%c%s%c%s%c\n",i-j,32,neuername,32,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),32);
        }
        else
        {
          fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),31);
          printf("%i%c%s%c%s%c\n",i-j,32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),32);
        }
      }
    }
    else
    {
      j++;
    }
  }

  fprintf(datei,"%c\n",30);
  j=0;
  for(i=0 ; i<=maxzaehlererg ; i++)
  {
    gchar altername[10000] = "";
    strcpy(altername,gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]));
    if(strncmp(name,altername,strlen(name)) != 0)
    {
      if(strncmp(nameBeginn,altername,strlen(nameBeginn)) != 0)
      {
        fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),31);
        printf("%i%c%s%c%s%c\n",i-j,32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),32);
      }
      else
      {
        gchar neuername[10000] = "";
        strcat(neuername,nameBeginn);
        gchar nummer[10] = "";
        int l=0;
        printf("%li\n",strlen(nameBeginn)+l+1);
        while(altername[strlen(nameBeginn)+l+1] != '-' && altername[strlen(nameBeginn)+l+1] != 0 && altername[strlen(nameBeginn)+l+1] != 'E')
        {
          nummer[l] = altername[strlen(nameBeginn)+l+1];
          l++;
        }
        printf("bisheriger Name: %s, altername: %s, nummer: %s und nameEnde: %s\n",gtk_widget_get_name(textfeldErgebnisWahrscheinlichkeit[i]),altername,nummer,nameEnde);
        if(atoi(nummer) > atoi(nameEnde))
        {
          gchar neuenummer[10] = "";
          sprintf(neuenummer,"%i",atoi(nummer)-1);
          printf("neuenummer: %s\n",neuenummer);
          strcat(neuername,"-");
          strcat(neuername,neuenummer);
          strcat(neuername,altername+strlen(nameBeginn)+strlen(nummer)+1);
          fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,neuername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),31);
          printf("%i%c%s%c%s%c\n",i-j,32,neuername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),32);
        }
        else
        {
          fprintf(datei,"%i%c%s%c%s%c\n",i-j,31,altername,31,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),31);
          printf("%i%c%s%c%s%c\n",i-j,32,altername,32,gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[i])),32);
        }
      }
    }
    else
    {
      j++;
    }
  }

  fclose(datei);
  
  templaden(data);
  alleknotenneupositionieren(data);
}
