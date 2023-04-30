int weiterunten(char *stringoben, const gchar *stringunten)
{
  char stringobentemp[10000] = "";
  strcpy(stringobentemp, stringoben);
  char stringuntentemp[10000] = "";
  strcpy(stringuntentemp, stringunten);
//  printf("stringoben=%s, stringunten=%s\n",stringobentemp,stringuntentemp);
  char *zeigeroben, *zeigerunten;
  zeigeroben = strtok(stringobentemp,"-");
  zeigerunten = strtok(stringuntentemp,"-");
//  printf("Vorher, zeigeroben=%s, zeigerunten=%s\n",zeigeroben,zeigerunten);
  int laengeobenkumulativ = 0, laengeuntenkumulativ = 0;
  while(zeigeroben != NULL && zeigerunten != NULL)
  {
    int laengeoben = strlen(zeigeroben);
    int laengeunten = strlen(zeigerunten);
    laengeobenkumulativ += laengeoben+1;
    laengeuntenkumulativ += laengeunten+1;
    if(atoi(zeigeroben)<atoi(zeigerunten))
    {
//      printf("Der zweite Knoten ist weiter unten!\n");
      return 1;
    }
    if(atoi(zeigeroben)>atoi(zeigerunten))
    {
//      printf("Der zweite Knoten ist weiter oben!\n");
      return 0;
    }
//    printf("laengeobenkumulativ=%i, laengeuntenkumulativ=%i\n",laengeobenkumulativ,laengeuntenkumulativ);
    zeigeroben = strtok(stringobentemp+laengeobenkumulativ+1,"-");
    zeigerunten = strtok(stringuntentemp+laengeuntenkumulativ+1,"-");
//    printf("Nachher, zeigeroben=%s, zeigerunten=%s\n",zeigeroben,zeigerunten);
  }
  return 0;
}
