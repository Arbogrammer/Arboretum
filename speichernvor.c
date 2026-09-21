void speichernvor(GtkWidget *widget, gpointer dateiname)
{
  if(aktuelledatei[0] == 0)
  {
    speicherdialog(widget, dateiname);
  }
  else
  {
    speichern(aktuelledatei);
  }
}
