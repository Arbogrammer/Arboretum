// Kompilieren unter Linux:
// gcc -Wall -g ~/Programme/c/baumg/baumg.c -o ~/Programme/baumg $(pkg-config --libs --cflags gtk+-3.0)
//

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <unistd.h>
#include <cairo-svg.h> 
#include <cairo-pdf.h> 
#include <locale.h>

#define GROESSELAYOUT if(labelein==0)\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (layout),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (layout),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenLabelBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
#define GROESSELAYOUTD if(labelein==0)\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenLabelBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
#define GROESSEDRAWINGAREA if(labelein==0)\
                      {\
                        gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisBreite):0),ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*(klbmax+2*knotenrahmenabstand)-knotenrahmenabstand+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisLabelBreite):0),ymax+KnotenHoehe/2-KnotenLabelHoehe/2+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
#ifdef _WIN32
  #define TEMPDATEI char dateiname[100]=""; \
                    sprintf(dateiname,"%s%i-Baumspeicher%i.bdg",getenv("tmp"),getpid(),
#else
  #define TEMPDATEI char dateiname[100]=""; \
                    sprintf(dateiname,"/tmp/%i-Baumspeicher%i.bdg",getpid(),
#endif

#ifdef __APPLE__
  #define SETENV char arbv[5000]="";strcpy(arbv,argv[0]);char *ptr = strrchr(arbv,'/');*ptr=0;char xdg[5000] = ""; sprintf(xdg,"%s/../share",arbv);setenv("XDG_DATA_DIRS",xdg,1);
#else
  #define SETENV ;
#endif

///////////////////////
// Globale Variablen //
///////////////////////

GtkWidget *window;
GtkWidget *dateimenue_exportieren;
GtkWidget *textfeld[10000];
GtkWidget *knotenlabel[10000];
GtkWidget *wahrscheinlichkeitlabel[10000];
GtkWidget *zaehlerlabel[10000];
GtkWidget *nennerlabel[10000];
GtkWidget *ergebnislabel[10000];
GtkWidget *ergebniswsklabel[10000];
GtkWidget *ergebniszaehlerlabel[10000];
GtkWidget *ergebnisnennerlabel[10000];
GtkWidget **vorgaenger[10000];
GtkWidget *textfeldErgebnis[10000];
GtkWidget *textfeldWahrscheinlichkeit[10000];
GtkWidget *textfeldErgebnisWahrscheinlichkeit[10000];
GtkWidget *da;
GtkWidget *scrollwindow;
GdkRGBA hintergrundfarbe;
GdkRGBA zweigfarbe;
GdkRGBA knotenhintergrundfarbe;
GdkRGBA knotenrandfarbe;
GdkRGBA schriftfarbe;
cairo_surface_t *surface;
cairo_t *kontext;

int scrv=0, scrh=0;
char aktuelledatei[5000] = "";
int zaehler = 0;
int maxzaehler = 0;
int y[10000] = { 0 };
int zaehlererg = 0;
int maxzaehlererg = 0;
int yerg[10000] = { 0 };
int ymax = 0;
int Knoten[10000] = { 0 };
//int maxKnoten[10000] = { 0 };
int maximalKnoten = 0 ;
int Stufe = 0;
int maxStufe = 0;
int dateinummerierung = 1;

int dateiveraendert = 0;
int zurueck=0;
int schriftartwurdegewechselt = 0;
int knotenrahmendickegeaendert = 0;
int schriftfarbewurdegeaendert = 0;
int knotenrahmenfarbewurdegeaendert = 0;
int knotenhintergrundfarbewurdegeaendert = 0;


int ergebnisseanzeigen = 1;
int wskanzeigen = 1;
int ergebnissewskanzeigen = 1;
int labelein = 0;
int breitevordrehung[10000] = {0};
int hoehevordrehung[10000] = {0};
double winkel[10000] = {0};

////////////////////////////////////////////
// Hier kommen die Einstellungsvariablen. //
////////////////////////////////////////////

int RandLinks=20;
int RandRechts=20;
int RandOben=20;
int RandUnten=20;
int FensterRandLinks=10;
int FensterRandRechts=10;
int FensterRandOben=10;
int FensterRandUnten=10;
int StufenBreite=200;
int KnotenAbstand=40;
int KnotenHoehe=29;
int KnotenBreite=40;
int ErgebnisAbstand=30;
int ErgebnisTextBreite=2;
int ErgebnisBreite=40;
char ErgebnisTrenner=127;
int LinienDicke=1;
int KnotenTextBreite=2;
int KnotenLabelBreite=40;
int klbmax=40;
int KnotenLabelHoehe=29;
int ErgebnisLabelBreite=40;
int WahrscheinlichkeitTextBreite=3;
int WahrscheinlichkeitBreite=42;
int WahrscheinlichkeitHoehe=29;
int WahrscheinlichkeitErgebnisTextBreite=6;
int WahrscheinlichkeitErgebnisBreite=42;
int WahrscheinlichkeitErgebnisHoehe=29;
int WahrscheinlichkeitErgebnisLabelBreite=42;
int ZaehlerErgebnisLabelBreite=42;
int NennerErgebnisLabelBreite=42;
int ZaehlerErgebnisLabelHoehe=29;
int NennerErgebnisLabelHoehe=29;
int Schriftgroesse=10;
double padding=0;
double paddingk=10;  // Mindest-Abstand der Knotenlabel zu den Zweigen
int genauigkeit=4;
char schriftart[1000] = "";
int bruch = 0;
int kuerzen=1;
int bruchou=1;
int knotenrahmenabstand=10;	//Abstand vom Knoteninneren, damit es nicht so gequetscht aussieht
int wskverschiebung=0;          // Nach links (negative Werte), nach rechts (positive Werte).
int knotenrahmendicke=1;

////////////////
// Funktionen //
////////////////


#include "baumg.h"

void neueinstanz(GtkWidget *widget, gpointer *datei)
{
  system((char *)datei);
}

///////////////////
// Hauptfunktion //
///////////////////

int main (int argc, char *argv[])
{

  SETENV
  
  GtkWidget *gesamtbox;
  GtkWidget *layout;
  hintergrundfarbe.red=1;
  hintergrundfarbe.green=1;
  hintergrundfarbe.blue=1;
  hintergrundfarbe.alpha=0;

  zweigfarbe.red=0;
  zweigfarbe.green=0;
  zweigfarbe.blue=0;
  zweigfarbe.alpha=1;

  knotenhintergrundfarbe.red=1;
  knotenhintergrundfarbe.green=1;
  knotenhintergrundfarbe.blue=1;
  knotenhintergrundfarbe.alpha=0;

  knotenrandfarbe.red=1;
  knotenrandfarbe.green=1;
  knotenrandfarbe.blue=1;
  knotenrandfarbe.alpha=0;

  schriftfarbe.red=0;
  schriftfarbe.green=0;
  schriftfarbe.blue=0;
  schriftfarbe.alpha=1;

  gtk_init (&argc, &argv);
  if(argc > 1)
  {
    strcpy(aktuelledatei,argv[1]);
  }
  char Titel[5010] = "Arboretum";
  if(aktuelledatei[0])
  {
    strcat(Titel," - ");
    if(strrchr(aktuelledatei,'/'))
    {
      strcat(Titel,strrchr(aktuelledatei,'/')+1);
    }
    else
    {
      strcat(Titel,aktuelledatei);
    }
  }

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gesamtbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_window_set_title (GTK_WINDOW(window), Titel);
  gtk_window_set_default_size (GTK_WINDOW(window), 800, 600);
//  gtk_window_set_icon_from_file (GTK_WINDOW(window),"baumicon.svg",NULL);
  gtk_window_maximize (GTK_WINDOW(window));
  g_signal_connect (window, "delete-event", G_CALLBACK (beenden), NULL);

  y[0] = 0;
  ymax += y[0];
  yerg[0] = y[0];

  scrollwindow = gtk_scrolled_window_new (NULL,NULL);
  layout = gtk_layout_new (NULL, NULL);

  da = gtk_drawing_area_new();
//  gtk_widget_add_events(da,GDK_BUTTON_PRESS_MASK);
  GROESSEDRAWINGAREA
  g_signal_connect (da, "draw", G_CALLBACK(zeichnelinien),  layout);
//  g_signal_connect (da, "button-press-event", G_CALLBACK (xy), layout);

  textfeld[0] = gtk_entry_new();
  gtk_widget_set_name(textfeld[0],"-0");
  gtk_entry_set_width_chars (GTK_ENTRY(textfeld[0]),KnotenTextBreite);
  gtk_entry_set_alignment (GTK_ENTRY(textfeld[0]),0.5);
//  g_signal_connect (textfeld[0], "draw", G_CALLBACK(zeichneknotenhintergrund),  NULL);

  vorgaenger[0] = &textfeld[0];
  g_signal_connect (window, "key_press_event", G_CALLBACK (keyfunc), layout);
  g_signal_connect (textfeld[0], "changed", G_CALLBACK (buchstabeneingabe), layout);

  textfeldErgebnis[0] = gtk_entry_new();
  gtk_widget_set_name(textfeldErgebnis[0],"-0-E");
  gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnis[0]),3*(Stufe+1));
  gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnis[0]),0.5);

  textfeldWahrscheinlichkeit[0] = gtk_entry_new();
  gtk_widget_set_name(textfeldWahrscheinlichkeit[0],"-0W");
  gtk_entry_set_width_chars (GTK_ENTRY(textfeldWahrscheinlichkeit[0]),WahrscheinlichkeitTextBreite);
  gtk_entry_set_alignment (GTK_ENTRY(textfeldWahrscheinlichkeit[0]),0.5);
  g_signal_connect (textfeldWahrscheinlichkeit[0], "changed", G_CALLBACK (wskeingabe), layout);

  textfeldErgebnisWahrscheinlichkeit[0] = gtk_entry_new();
  gtk_widget_set_name(textfeldErgebnisWahrscheinlichkeit[0],"-0-EW");
  gtk_entry_set_width_chars (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0]),WahrscheinlichkeitErgebnisTextBreite);
  gtk_entry_set_alignment (GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0]),0.5);
  
  wahrscheinlichkeitlabel[0] = gtk_label_new(NULL);


  GtkWidget *menue_datei = gtk_menu_new ();
  GtkWidget *menue_bearbeiten = gtk_menu_new ();
  GtkWidget *menue_ansicht = gtk_menu_new ();
  GtkWidget *menue_farben = gtk_menu_new ();
  GtkWidget *menue_hilfe = gtk_menu_new ();
  GtkWidget *menueleiste = gtk_menu_bar_new ();
  GtkWidget *dateimenue = gtk_menu_item_new_with_label ("Datei");
  GtkWidget *dateimenue_neu = gtk_menu_item_new_with_label ("Neu");
  GtkWidget *dateimenue_oeffnen = gtk_menu_item_new_with_label ("Öffnen...");
  GtkWidget *dateimenue_speichern = gtk_menu_item_new_with_label ("Speichern (Strg+S)");
  GtkWidget *dateimenue_speichernunter = gtk_menu_item_new_with_label ("Speichern unter...");
  dateimenue_exportieren = gtk_menu_item_new_with_label ("Exportieren...");
  GtkWidget *dateimenue_schliessen = gtk_menu_item_new_with_label ("Programm Beenden");
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_neu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_oeffnen);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_speichern);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_speichernunter);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_exportieren);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_datei), dateimenue_schliessen);
  gtk_menu_shell_append(GTK_MENU_SHELL(menueleiste), dateimenue);
  
  gtk_widget_set_sensitive(dateimenue_exportieren, FALSE);

  GtkWidget *bearbeitenmenue = gtk_menu_item_new_with_label ("Bearbeiten");
  GtkWidget *bearbeitenmenue_rueck = gtk_menu_item_new_with_label ("Rückgängig (Strg+Z)");
  GtkWidget *bearbeitenmenue_form = gtk_menu_item_new_with_label ("Form...");
  GtkWidget *bearbeitenmenue_schriftart = gtk_menu_item_new_with_label ("Schriftart...");
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_bearbeiten), bearbeitenmenue_rueck);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_bearbeiten), bearbeitenmenue_form);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_bearbeiten), bearbeitenmenue_schriftart);
  gtk_menu_shell_append(GTK_MENU_SHELL(menueleiste), bearbeitenmenue);

  GtkWidget *ansichtmenue = gtk_menu_item_new_with_label ("Ansicht");
  GtkWidget *ansichtmenue_fixieren = gtk_menu_item_new_with_label ("Baum fixieren/bearbeiten (Strg+U)");
  GtkWidget *ansichtmenue_ergebnis = gtk_menu_item_new_with_label ("Ergebnisspalte ein/ausblenden (Strg+E)");
  GtkWidget *ansichtmenue_wskergebnis = gtk_menu_item_new_with_label ("Wahrscheinlichkeiten-Ergebnisspalte ein/ausblenden (Strg+W)");
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_ansicht), ansichtmenue_fixieren);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_ansicht), ansichtmenue_ergebnis);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_ansicht), ansichtmenue_wskergebnis);
  gtk_menu_shell_append(GTK_MENU_SHELL(menueleiste), ansichtmenue);

  GtkWidget *farbenmenue = gtk_menu_item_new_with_label ("Farben");
  GtkWidget *farbenmenue_hfarbe = gtk_menu_item_new_with_label ("Hintergrund...");
  GtkWidget *farbenmenue_zfarbe = gtk_menu_item_new_with_label ("Zweige...");
  GtkWidget *farbenmenue_sfarbe = gtk_menu_item_new_with_label ("Schrift...");
  GtkWidget *farbenmenue_krfarbe = gtk_menu_item_new_with_label ("Knotenrahmen...");
  GtkWidget *farbenmenue_khfarbe = gtk_menu_item_new_with_label ("Knotenhintergrund...");
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_farben), farbenmenue_hfarbe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_farben), farbenmenue_zfarbe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_farben), farbenmenue_sfarbe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_farben), farbenmenue_krfarbe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_farben), farbenmenue_khfarbe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menueleiste), farbenmenue);

  GtkWidget *hilfemenue = gtk_menu_item_new_with_label ("Hilfe");
  GtkWidget *hilfemenue_hilfe = gtk_menu_item_new_with_label ("Hilfe (F1)");
  gtk_menu_shell_append(GTK_MENU_SHELL(menue_hilfe), hilfemenue_hilfe);
  gtk_menu_shell_append(GTK_MENU_SHELL(menueleiste), hilfemenue);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(dateimenue), menue_datei);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(bearbeitenmenue), menue_bearbeiten);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(ansichtmenue), menue_ansicht);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(farbenmenue), menue_farben);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(hilfemenue), menue_hilfe);

//  printf("Aktuelle Datei: %s\n",aktuelledatei);
  g_signal_connect(G_OBJECT(dateimenue_neu), "activate",G_CALLBACK(neueinstanz), argv[0]);
  g_signal_connect(G_OBJECT(dateimenue_oeffnen), "activate",G_CALLBACK(oeffnen), layout);
  g_signal_connect(G_OBJECT(dateimenue_speichern), "activate",G_CALLBACK(speichernvor), NULL);
  g_signal_connect(G_OBJECT(dateimenue_speichernunter), "activate",G_CALLBACK(speicherdialog), NULL);
  g_signal_connect(G_OBJECT(dateimenue_exportieren), "activate",G_CALLBACK(exportdialog), NULL);
  g_signal_connect(G_OBJECT(dateimenue_schliessen), "activate",G_CALLBACK(beenden), NULL);
  g_signal_connect(G_OBJECT(bearbeitenmenue_schriftart), "activate",G_CALLBACK(schriftartwechseln), layout);
  g_signal_connect(G_OBJECT(farbenmenue_zfarbe), "activate",G_CALLBACK(zweigfarbewechseln), layout);
  g_signal_connect(G_OBJECT(farbenmenue_hfarbe), "activate",G_CALLBACK(hintergrundfarbewechseln), layout);
  g_signal_connect(G_OBJECT(farbenmenue_khfarbe), "activate",G_CALLBACK(knotenhintergrundfarbewechseln), layout);
  g_signal_connect(G_OBJECT(farbenmenue_sfarbe), "activate",G_CALLBACK(schriftfarbewechseln), layout);
  g_signal_connect(G_OBJECT(farbenmenue_krfarbe), "activate",G_CALLBACK(knotenrandfarbewechseln), layout);
  g_signal_connect(G_OBJECT(bearbeitenmenue_form), "activate",G_CALLBACK(formdialog), layout);
  g_signal_connect(G_OBJECT(bearbeitenmenue_rueck), "activate",G_CALLBACK(rueckgaengig), layout);
  g_signal_connect(G_OBJECT(ansichtmenue_fixieren), "activate",G_CALLBACK(umwandeln), layout);
  g_signal_connect(G_OBJECT(ansichtmenue_ergebnis), "activate",G_CALLBACK(ergebnisspalteanzeigen), layout);
  g_signal_connect(G_OBJECT(ansichtmenue_wskergebnis), "activate",G_CALLBACK(wskergebnisspalteanzeigen), layout);
  g_signal_connect(G_OBJECT(hilfemenue_hilfe), "activate",G_CALLBACK(hilfe), layout);

  GROESSELAYOUT

  gtk_layout_put (GTK_LAYOUT (layout), da, FensterRandLinks,FensterRandOben);
  gtk_layout_put (GTK_LAYOUT (layout), textfeld[0], FensterRandLinks+RandLinks+StufenBreite,FensterRandOben+RandOben+y[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldErgebnis[0], FensterRandLinks+RandLinks+StufenBreite+KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldWahrscheinlichkeit[0], FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+y[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldErgebnisWahrscheinlichkeit[0], FensterRandLinks+RandLinks+StufenBreite+KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[0]);
  gtk_layout_put (GTK_LAYOUT (layout), wahrscheinlichkeitlabel[0], 0,0);
  gtk_container_add (GTK_CONTAINER (scrollwindow), layout);
  gtk_box_pack_start (GTK_BOX (gesamtbox), menueleiste,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (gesamtbox), scrollwindow,TRUE,TRUE,0);
  gtk_container_add (GTK_CONTAINER (window), gesamtbox);
  gtk_widget_show_all(window);
  gtk_widget_hide(wahrscheinlichkeitlabel[0]);
  gtk_widget_grab_focus(textfeld[0]);
  KnotenHoehe = gtk_widget_get_allocated_height(textfeld[0]);
  KnotenBreite = gtk_widget_get_allocated_width(textfeld[0]);
  WahrscheinlichkeitHoehe = gtk_widget_get_allocated_height(textfeldWahrscheinlichkeit[0]);
  WahrscheinlichkeitBreite = gtk_widget_get_allocated_width(textfeldWahrscheinlichkeit[0]);
  ErgebnisBreite = gtk_widget_get_allocated_width(textfeldErgebnis[0]);
  WahrscheinlichkeitErgebnisBreite= gtk_widget_get_allocated_width(textfeldErgebnisWahrscheinlichkeit[0]);
  WahrscheinlichkeitErgebnisHoehe=gtk_widget_get_allocated_height(textfeldErgebnisWahrscheinlichkeit[0]);;
  GtkWidget *fakedialog = gtk_font_chooser_dialog_new ("Schriftart auswählen", GTK_WINDOW (window));
  char bisherschriftart[1000] = "";
  sprintf(bisherschriftart,"%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fakedialog)));
  strcpy(schriftart,bisherschriftart);

  if(argc > 1)
  {
    laden(layout,argv[1]);
  }
  tempspeichern();

  gtk_main ();

  int i;
//  printf("%i\n",dateinummerierung);
  for(i = 1 ; i < dateinummerierung ; i++)
  {
    TEMPDATEI i);
    remove(dateiname);
  }
  return 0;
}
