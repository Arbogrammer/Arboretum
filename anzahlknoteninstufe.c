int anzahlknoteninstufe(int i)
{
  int knotennummer = 0, j = 0;
  for(j=0;j<=maxzaehler;j++)
  {
    if(textfeld[j])
    {
      if(zeichenzaehlen(gtk_widget_get_name(textfeld[j]),'-') == i+1)
      {
        knotennummer +=1;
      }
    }
  }
  return knotennummer;
}
