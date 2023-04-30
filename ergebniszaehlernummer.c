int ergebniszaehlernummer(const gchar *ergebnisname)
{
  int i=0;
  for(i=0 ; i<= maxzaehlererg ; i++)
  {
    if(strcmp(gtk_widget_get_name(textfeldErgebnis[i]),ergebnisname) == 0)
    {
      return i;
    }
  }
  return -1;
}
