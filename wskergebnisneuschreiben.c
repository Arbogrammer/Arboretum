void wskergebnisneuschreiben(GtkWidget *widget)
{
  int i=0;
  bruch=0;
  for(i=0;i<=maxzaehler;i++)
  {
    if(strchr(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[i])),'/'))
    {
      bruch=1;
    }
  }

  char tempnameergw[10000] = "";
  strcpy(tempnameergw,gtk_widget_get_name(widget));
  double wsk = 1;
  long long int zaehler = 1;
  long long int nenner = 1;

  int position=(int)(strchr(tempnameergw+1,'-')-tempnameergw);
  int laenge = strlen(tempnameergw)-3;
  tempnameergw[position] = 87; // W
  tempnameergw[position+1] = 0;

  while(wskexistiert(tempnameergw) > -1)
  {
    char einzelwsk[1000] = "";
    strcpy(einzelwsk,gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[wskexistiert(tempnameergw)])));
    if(bruch)
    {
      zaehler *= atoll(einzelwsk);
      if(strchr(einzelwsk,'/'))
      {
        nenner *= atoll(strchr(einzelwsk,'/')+1);
      }
    }
    else
    {
      if(einzelwsk[0])
      {
        wsk *= atof(einzelwsk);
      }
    }
    if(position >= laenge)
    {
      break;
    }
    else
    {
      strcpy(tempnameergw,gtk_widget_get_name(widget));
      position=(int)(strchr(tempnameergw+position+1,'-')-tempnameergw);
      tempnameergw[position] = 87; // W
      tempnameergw[position+1] = 0;
    }
  }
  char wsktext[100] = "";
  if(bruch)
  {
    sprintf(wsktext,"%lld/%lld",zaehler,nenner);
    if(kuerzen && ggt(zaehler,nenner) != 0)
    {
      sprintf(wsktext,"%lld/%lld",zaehler/ggt(zaehler,nenner),nenner/ggt(zaehler,nenner));
    }
  }
  else
  {
    sprintf(wsktext,"%*.*f",genauigkeit+2,genauigkeit,wsk);
  }
  gtk_entry_set_text(GTK_ENTRY(widget),wsktext);
}
