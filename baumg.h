void buchstabeneingabe(GtkEditable *editable, gpointer data);
void wskeingabe(GtkEditable *editable, gpointer data);
static gboolean keyfunc (GtkWidget *widget, GdkEventKey *event, gpointer data);
void oeffnen(GtkWidget *widget, gpointer data);
void speicherdialog();
void speichernvor(GtkWidget *widget, gpointer dateiname);
void speichern(char *dateiname);
void tempspeichern();
void reset(gpointer data);
void templaden(gpointer data);
void laden(gpointer data,char *dateiname);
void exportdialog ();
void exportpng (char *dateiname);
void exportsvg (char *dateiname);
void exportbmp (char *dateiname);
void exportjpg (char *dateiname);
void exportpdf (char *dateiname);
void rueckgaengig(GtkWidget *widget, gpointer data);
void loeschen();
gboolean pos1(GtkWidget *widget, gpointer data);
gboolean runter(GtkWidget *widget, gpointer data);
gboolean rechts(GtkWidget *widget, gpointer data);
gboolean hoch(GtkWidget *widget, gpointer data);
gboolean links(GtkWidget *widget, gpointer data);
gboolean runterwsk(GtkWidget *widget, gpointer data);
gboolean rechtswsk(GtkWidget *widget, gpointer data);
gboolean hochwsk(GtkWidget *widget, gpointer data);
gboolean linkswsk(GtkWidget *widget, gpointer data);
gboolean positionsanpassung(GtkWidget *widget, gpointer data);
int zaehleruntenrechts(int zaehlerlinks);
int zeichenzaehlen(const gchar *string, int zeichen);
int knotenexistiert(const gchar *knotenname);
int wskexistiert(const gchar *knotenname);
int ergebniszaehlernummer(const gchar *ergebnisname);
int weiterunten(char *stringoben, const gchar *stringunten);
static gboolean zeichnelinien(GtkWidget *widget, cairo_t *cr, gpointer data);
int anzahlnachfolger(int i);
int nachfolger(int i, int nachfolgernummer);
// static gboolean zeichneknotenhintergrund(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean beenden(GtkWidget *widget, GdkEvent *event, gpointer data);
void ergebnisspalteanzeigen();
void wskergebnisspalteanzeigen();
void ergebnistextneuschreiben(GtkWidget *widget);
void neugroesse(GtkWidget *widget, GtkAllocation *allocation, void *data);
static void hintergrundfarbewechseln (GtkWidget *button, gpointer data);
static void dialogschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
static void formdialog(GtkWidget *button, gpointer data);
void xy(GtkWidget *widget, GdkEventButton *event, gpointer data);
void positionneu(int KnotenAbstandVorher,gpointer data);
int anzahlknoteninstufe(int i);
void positionsanpassungwsk();
void wskergebnisverschieben(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
void wskergebnisneuschreiben(GtkWidget *widget);
void umwandeln(GtkMenuItem *menuitem, gpointer data);
//void wsklabelverschieben(GtkWidget *widget, GdkRectangle *allocation, gpointer *data);
void labelverschieben(gpointer data);
//void wsklabelverschiebentest(GtkWidget widget, gpointer data);
void wsklabelgroesse(GtkWidget *widget, GdkRectangle *allocation, gpointer data);
void groesseneu(GtkWidget *widget, GdkRectangle *ap, gpointer data);
static void zweigfarbewechseln (GtkWidget *button, gpointer data);
static void knotenhintergrundfarbewechseln (GtkWidget *button, gpointer data);
static void schriftfarbewechseln (GtkWidget *button, gpointer data);
static void knotenrandfarbewechseln (GtkWidget *button, gpointer data);
static void schriftartwechseln (GtkWidget *button, gpointer data);
static void dialogzschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
static void dialogsschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
static void dialogsfschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
static void dialogkhfschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
static void dialogkrfschliessen(GtkDialog *dialog,gint response_id,  gpointer user_data);
void schriftartanpassen(GtkFontChooser *self, gchar *fontname, gpointer user_data);
void hilfe(GtkMenuItem *menuitem, gpointer user_data);
int ggt(long long int x, long long int y);
static void knotenhintergrundfarbeaendern ();
static void knotenrandfarbeaendern ();
static void knotenranddickeaendern ();
static void schriftfarbeaendern ();
void knotenwskwechseln();
char *ftstr(double zahl);
void ueberstreichen();


#include <math.h>
#include "buchstabeneingabe.c"
#include "wskeingabe.c"
#include "keyfunc.c"
#include "oeffnen.c"
#include "speicherdialog.c"
#include "speichernvor.c"
#include "speichern.c"
#include "tempspeichern.c"
#include "reset.c"
#include "templaden.c"
#include "laden.c"
#include "exportdialog.c"
#include "exportpng.c"
#include "exportsvg.c"
#include "exportjpg.c"
#include "exportbmp.c"
#include "exportpdf.c"
#include "rueckgaengig.c"
#include "loeschen.c"
#include "pos1.c"
#include "runter.c"
#include "rechts.c"
#include "hoch.c"
#include "links.c"
#include "runterwsk.c"
#include "rechtswsk.c"
#include "hochwsk.c"
#include "linkswsk.c"
#include "positionsanpassung.c"
#include "zaehleruntenrechts.c"
#include "zeichenzaehlen.c"
#include "knotenexistiert.c"
#include "wskexistiert.c"
#include "ergebniszaehlernummer.c"
#include "weiterunten.c"
#include "zeichnelinien.c"
#include "anzahlnachfolger.c"
#include "nachfolger.c"
//#include "zeichneknotenhintergrund.c"
#include "beenden.c"
#include "ergebnisspalteanzeigen.c"
#include "wskergebnisspalteanzeigen.c"
#include "ergebnistextneuschreiben.c"
#include "neugroesse.c"
#include "hintergrundfarbewechseln.c"
#include "dialogschliessen.c"
#include "formdialog.c"
#include "xy.c"
#include "positionneu.c"
#include "anzahlknoteninstufe.c"
#include "positionsanpassungwsk.c"
#include "wskergebnisverschieben.c"
#include "wskergebnisneuschreiben.c"
#include "umwandeln.c"
#include "labelverschieben.c"
//#include "wsklabelverschiebentest.c"
#include "wsklabelgroesse.c"
#include "groesseneu.c"
#include "zweigfarbewechseln.c"
#include "dialogzschliessen.c"
#include "schriftartwechseln.c"
#include "dialogsschliessen.c"
#include "schriftartanpassen.c"
#include "hilfe.c"
#include "ggt.c"
#include "knotenhintergrundfarbewechseln.c"
#include "schriftfarbewechseln.c"
#include "knotenrandfarbewechseln.c"
#include "dialogkhfschliessen.c"
#include "dialogsfschliessen.c"
#include "dialogkrfschliessen.c"
#include "knotenhintergrundfarbeaendern.c"
#include "knotenrandfarbeaendern.c"
#include "knotenranddickeaendern.c"
#include "schriftfarbeaendern.c"
#include "knotenwskwechseln.c"
#include "ftstr.c"
#include "ueberstreichen.c"
