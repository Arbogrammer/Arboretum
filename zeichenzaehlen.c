int zeichenzaehlen(const gchar *string, int zeichen)
{
  int anzahl = 0;
  char *adresse = strchr(string,zeichen);
  while(adresse != NULL)
  {
    anzahl++;
    adresse = strchr(adresse+2,zeichen);
  }
  return anzahl;
}
