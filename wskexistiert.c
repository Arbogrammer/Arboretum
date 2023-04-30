int wskexistiert(const gchar *knotenname)
{
  int i=0;
  for(i=0 ; i<= maxzaehler ; i++)
  {
    if(!knotenname)
    {
      return -1;
    }
    if(strcmp(gtk_widget_get_name(textfeldWahrscheinlichkeit[i]),knotenname) == 0)
    {
      return i;
    }
  }
  return -1;
}
