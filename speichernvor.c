void speichernvor(GtkWidget *widget, gpointer dateiname)
{
  if(aktuelledatei[0] == 0)
  {
    speicherdialog();
  }
  else
  {
    speichern(aktuelledatei);
  }
}
