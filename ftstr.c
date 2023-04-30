char *ftstr(double zahl)
{
  char *zahlstring = calloc(100,sizeof(char));
  sprintf(zahlstring,"%f",zahl);
  char *komma = strchr(zahlstring,',');
  if(komma)
  {
    komma[0] = '.';
  }
  return zahlstring;
}
