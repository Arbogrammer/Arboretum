// Kompilieren unter Linux:
// Mit `make` gegen GTK 4 kompilieren.
//

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "gtk4_compat.h"
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
                        gtk_layout_set_size(GTK_LAYOUT (layout),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (layout),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*(klbmax+2*knotenrahmenabstand)-knotenrahmenabstand+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisLabelBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
#define GROESSELAYOUTD if(labelein==0)\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_layout_set_size(GTK_LAYOUT (data),FensterRandLinks+FensterRandRechts+RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*(klbmax+2*knotenrahmenabstand)-knotenrahmenabstand+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisLabelBreite):0),FensterRandOben+FensterRandUnten+ymax+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
#define GROESSEDRAWINGAREA if(labelein==0)\
                      {\
                        gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*KnotenBreite+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisBreite):0),ymax+KnotenHoehe+RandUnten+RandOben);\
                      }\
                      else\
                      {\
                        gtk_widget_set_size_request (da, RandLinks+RandRechts+(maxStufe+1)*StufenBreite+(maxStufe+1)*(klbmax+2*knotenrahmenabstand)-knotenrahmenabstand+((ergebnisseanzeigen>0)?(ErgebnisAbstand+ErgebnisLabelBreite):0)+((ergebnissewskanzeigen>0)?(ErgebnisAbstand+WahrscheinlichkeitErgebnisLabelBreite):0),ymax+KnotenHoehe/2-KnotenLabelHoehe/2+KnotenLabelHoehe+RandUnten+RandOben);\
                      }
static gchar *arboretum_temp_path(int number)
{
  g_autofree gchar *name = g_strdup_printf("%i-Baumspeicher%i.bdg", (int)getpid(), number);
  return g_build_filename(g_get_tmp_dir(), name, NULL);
}

static void startup_trace(const char *stage)
{
  if(g_getenv("ARBORETUM_DIAGNOSTIC"))
  {
    fprintf(stderr, "Arboretum startup: %s\n", stage);
    fflush(stderr);
  }
}

#ifdef __APPLE__
  #define SETENV char arbv[5000]="";strcpy(arbv,argv[0]);char *ptr = strrchr(arbv,'/');*ptr=0;char xdg[5000] = ""; sprintf(xdg,"%s/../share",arbv);setenv("XDG_DATA_DIRS",xdg,1);
#else
  #define SETENV ;
#endif

/*
 * Zentrale Programmzustände
 * -------------------------
 * Arboretum ist historisch als ein einziges C-Übersetzungsmodul aufgebaut:
 * baumg.h bindet die vielen kleinen .c-Dateien weiter unten direkt ein. Daher
 * können diese Dateien gemeinsam auf die folgenden Variablen zugreifen.
 *
 * Das Namensschema verbindet die GTK-Widgets mit dem Baum:
 *   -0       erster Knoten
 *   -0-1     zweites Kind des ersten Knotens
 *   -0-1W    Wahrscheinlichkeitsfeld dieses Kindes
 *   -0-1-E   Ergebnisfeld
 * Die Suchfunktionen knotenexistiert() und wskexistiert() übersetzen diese
 * Namen zurück in die Indizes der unten stehenden Felder.
 */

GtkWidget *window;
GtkWidget *dateimenue_exportieren;
GMainLoop *arboretum_main_loop;
gboolean arboretum_layout_dirty = TRUE;
int arboretum_exit_status = 0;
enum { MAX_KNOTEN = 10000 };

GtkWidget *textfeld[MAX_KNOTEN];                 /* sichtbare Baumknoten */
GtkWidget *knotenlabel[MAX_KNOTEN];
GtkWidget *wahrscheinlichkeitlabel[MAX_KNOTEN];
GtkWidget *zaehlerlabel[MAX_KNOTEN];
GtkWidget *nennerlabel[MAX_KNOTEN];
GtkWidget *ergebnislabel[MAX_KNOTEN];
GtkWidget *ergebniswsklabel[MAX_KNOTEN];
GtkWidget *ergebniszaehlerlabel[MAX_KNOTEN];
GtkWidget *ergebnisnennerlabel[MAX_KNOTEN];
GtkWidget **vorgaenger[MAX_KNOTEN];
GtkWidget *textfeldErgebnis[MAX_KNOTEN];         /* Ergebnis je Pfad */
GtkWidget *textfeldWahrscheinlichkeit[MAX_KNOTEN];
GtkWidget *textfeldErgebnisWahrscheinlichkeit[MAX_KNOTEN];
GtkWidget *da;                              /* Zeichenfläche für die Zweige */
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
int zaehler = 0;                 /* aktuell ausgewählter Knoten */
int maxzaehler = 0;              /* höchster belegter Knotenindex */
int y[MAX_KNOTEN] = { 0 };       /* senkrechte Position jedes Knotens */
int zaehlererg = 0;
int maxzaehlererg = 0;
int yerg[MAX_KNOTEN] = { 0 };
int ymax = 0;
int Knoten[MAX_KNOTEN] = { 0 };
//int maxKnoten[MAX_KNOTEN] = { 0 };
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
GtkCssProvider *knotenhintergrund_provider = NULL;
GtkCssProvider *knotenrand_provider = NULL;
GtkCssProvider *knotenranddicke_provider = NULL;
GtkCssProvider *schriftfarbe_provider = NULL;
GtkCssProvider *schriftart_provider = NULL;


int ergebnisseanzeigen = 1;
int wskanzeigen = 1;
int ergebnissewskanzeigen = 1;
int labelein = 0;
int breitevordrehung[MAX_KNOTEN] = {0};
int hoehevordrehung[MAX_KNOTEN] = {0};
double winkel[MAX_KNOTEN] = {0};

static void knotenlimit_melden(const char *text)
{
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", text);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static gboolean knotenindex_gueltig(int index)
{
  if(index >= 0 && index < MAX_KNOTEN)
  {
    return TRUE;
  }

  knotenlimit_melden("Die Datei enthält einen ungültigen Knotenindex und kann nicht geladen werden.");
  return FALSE;
}

/* Darstellungsoptionen. Die Werte werden auch im .bdg-Dateiformat gespeichert. */

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
  int status = system((char *)datei);
  (void)status;
}

static GtkWidget *werkzeugknopf(GtkWidget *leiste, const char *text,
                                GCallback callback, gpointer data)
{
  GtkWidget *button = gtk_button_new_with_label(text);
  gtk_box_append(GTK_BOX(leiste), button);
  g_signal_connect(button, "clicked", callback, data);

  GtkWidget *popover = g_object_get_data(G_OBJECT(leiste),
                                         "arboretum-popover");
  if(popover)
  {
    g_signal_connect_swapped(button, "clicked",
                             G_CALLBACK(gtk_popover_popdown), popover);
  }

  return button;
}

static GtkWidget *werkzeuggruppe(GtkWidget *leiste, const char *text)
{
  GtkWidget *menuknopf = gtk_menu_button_new();
  GtkWidget *popover = gtk_popover_new();
  GtkWidget *inhalt = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

  gtk_menu_button_set_label(GTK_MENU_BUTTON(menuknopf), text);
  gtk_widget_set_margin_start(inhalt, 6);
  gtk_widget_set_margin_end(inhalt, 6);
  gtk_widget_set_margin_top(inhalt, 6);
  gtk_widget_set_margin_bottom(inhalt, 6);
  gtk_popover_set_child(GTK_POPOVER(popover), inhalt);
  gtk_menu_button_set_popover(GTK_MENU_BUTTON(menuknopf), popover);
  gtk_box_append(GTK_BOX(leiste), menuknopf);

  g_object_set_data(G_OBJECT(inhalt), "arboretum-popover", popover);
  return inhalt;
}

static gboolean startfeld_fokussieren(gpointer data)
{
  /* Nach gtk_window_present() weist GTK den Fokus erst im nächsten Durchlauf
   * der Ereignisschleife zuverlässig zu. */
  gtk_widget_grab_focus(GTK_WIDGET(data));
  gtk_editable_set_position(GTK_EDITABLE(data), -1);
  return G_SOURCE_REMOVE;
}

static gboolean pfeiltasten_smoketest(gpointer data)
{
  /* Optionaler Regressionstest. Er läuft nur, wenn die Umgebungsvariable
   * ARBORETUM_KEYBOARD_SMOKE_TEST gesetzt wurde. */
  GtkEventControllerKey *controller = GTK_EVENT_CONTROLLER_KEY(data);
  gpointer layout = g_object_get_data(G_OBJECT(controller), "arboretum-layout");
  gboolean phase_ok = gtk_event_controller_get_propagation_phase(
                        GTK_EVENT_CONTROLLER(controller)) == GTK_PHASE_CAPTURE;

  gtk_widget_grab_focus(textfeld[0]);
  g_signal_handlers_block_by_func(textfeld[0], G_CALLBACK(buchstabeneingabe), layout);
  gtk_editable_set_text(GTK_EDITABLE(textfeld[0]), "a");
  g_signal_handlers_unblock_by_func(textfeld[0], G_CALLBACK(buchstabeneingabe), layout);
  buchstabeneingabe(GTK_EDITABLE(textfeld[0]), layout);
  gboolean eingabe_ok = textfeld[0] &&
                        strcmp(gtk_entry_get_text(GTK_ENTRY(textfeld[0])), "a") == 0;
  gboolean standardstil_ok = knotenhintergrund_provider == NULL &&
                              knotenrand_provider == NULL &&
                              knotenranddicke_provider == NULL &&
                              schriftfarbe_provider == NULL &&
                              schriftart_provider == NULL;
  keyfunc(controller, GDK_KEY_Down, 0, 0, layout);
  gboolean down_ok = maxzaehler == 1 && knotenexistiert("-1") >= 0;
  keyfunc(controller, GDK_KEY_Right, 0, 0, layout);
  gboolean right_ok = maxzaehler == 2 && knotenexistiert("-1-0") >= 0;
  GtkWidget *kindfeld = gtk_window_get_focus(GTK_WINDOW(window));
  g_signal_handlers_block_by_func(kindfeld, G_CALLBACK(buchstabeneingabe), layout);
  gtk_editable_set_text(GTK_EDITABLE(kindfeld), "xy");
  g_signal_handlers_unblock_by_func(kindfeld, G_CALLBACK(buchstabeneingabe), layout);
  gtk_editable_set_position(GTK_EDITABLE(kindfeld), 1);
  gboolean cursor_links_ok = !keyfunc(controller, GDK_KEY_Left, 0, 0, layout) &&
                             gtk_window_get_focus(GTK_WINDOW(window)) == kindfeld;
  gboolean cursor_rechts_ok = !keyfunc(controller, GDK_KEY_Right, 0, 0, layout) &&
                              gtk_window_get_focus(GTK_WINDOW(window)) == kindfeld;
  gtk_editable_set_position(GTK_EDITABLE(kindfeld), 0);
  keyfunc(controller, GDK_KEY_Left, 0, 0, layout);
  gboolean left_ok = gtk_window_get_focus(GTK_WINDOW(window)) &&
                     strcmp(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))), "-1") == 0;
  keyfunc(controller, GDK_KEY_Up, 0, 0, layout);
  gboolean up_ok = gtk_window_get_focus(GTK_WINDOW(window)) &&
                   strcmp(gtk_widget_get_name(gtk_window_get_focus(GTK_WINDOW(window))), "-0") == 0;
  keyfunc(controller, GDK_KEY_Tab, 0, 0, layout);
  gboolean probability_focus_ok = gtk_window_get_focus(GTK_WINDOW(window)) ==
                                  textfeldWahrscheinlichkeit[0];

  GdkRGBA hintergrund_vor_wsk = hintergrundfarbe;
  g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[0],
                                  G_CALLBACK(wskeingabe), layout);
  gtk_editable_set_text(GTK_EDITABLE(textfeldWahrscheinlichkeit[0]), "0,5");
  g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[0],
                                    G_CALLBACK(wskeingabe), layout);
  wskeingabe(GTK_EDITABLE(textfeldWahrscheinlichkeit[0]), layout);
  gboolean wsk_eingabe_ok = textfeldWahrscheinlichkeit[0] &&
      strcmp(gtk_entry_get_text(GTK_ENTRY(textfeldWahrscheinlichkeit[0])), "0,5") == 0 &&
      fabs(wahrscheinlichkeit_einlesen("0,5") - 0.5) < 0.000001 &&
      fabs(wahrscheinlichkeit_einlesen("0.5") - 0.5) < 0.000001 &&
      strchr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0])), ',') != NULL &&
      fabs(hintergrundfarbe.red - hintergrund_vor_wsk.red) < 0.000001 &&
      fabs(hintergrundfarbe.green - hintergrund_vor_wsk.green) < 0.000001 &&
      fabs(hintergrundfarbe.blue - hintergrund_vor_wsk.blue) < 0.000001 &&
      fabs(hintergrundfarbe.alpha - hintergrund_vor_wsk.alpha) < 0.000001 &&
      hintergrundfarbe.alpha < 0.000001;
  gtk_editable_set_position(GTK_EDITABLE(textfeldWahrscheinlichkeit[0]), 1);
  gboolean wsk_cursor_ok = !keyfunc(controller, GDK_KEY_Left, 0, 0, layout) &&
                           gtk_window_get_focus(GTK_WINDOW(window)) ==
                           textfeldWahrscheinlichkeit[0];

  g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[0],
                                  G_CALLBACK(wskeingabe), layout);
  gtk_editable_set_text(GTK_EDITABLE(textfeldWahrscheinlichkeit[0]), "0.5");
  g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[0],
                                    G_CALLBACK(wskeingabe), layout);
  wskeingabe(GTK_EDITABLE(textfeldWahrscheinlichkeit[0]), layout);
  gboolean punkt_eingabe_ok =
      strchr(gtk_entry_get_text(GTK_ENTRY(textfeldErgebnisWahrscheinlichkeit[0])), '.') != NULL;

  cairo_surface_t *ui_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 8, 8);
  cairo_t *ui_cr = cairo_create(ui_surface);
  zeichnelinien(GTK_DRAWING_AREA(da), ui_cr, 8, 8, NULL);
  cairo_destroy(ui_cr);
  cairo_surface_flush(ui_surface);
  unsigned char *ui_pixel = cairo_image_surface_get_data(ui_surface);
  gboolean ui_hintergrund_ok = ui_pixel[0] == 255 && ui_pixel[1] == 255 &&
                               ui_pixel[2] == 255 && ui_pixel[3] == 255;
  cairo_surface_destroy(ui_surface);

  while(g_main_context_pending(NULL))
    g_main_context_iteration(NULL, FALSE);

  arboretum_layout_aktualisieren(layout);
  double probability_x = 0, probability_y = 0;
  gtk_fixed_get_child_position(GTK_FIXED(layout), textfeldWahrscheinlichkeit[0],
                               &probability_x, &probability_y);
  double probability_center = probability_x +
                              gtk_widget_get_width(textfeldWahrscheinlichkeit[0]) / 2.0;
  double expected_center = FensterRandLinks + RandLinks + StufenBreite / 2.0;
  gboolean probability_position_ok = fabs(probability_center - expected_center) < 1.0;

  cairo_surface_t *test_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
  cairo_t *test_cr = cairo_create(test_surface);
  zeichnelinien(NULL, test_cr, 800, 600, NULL);
  cairo_destroy(test_cr);
  cairo_surface_flush(test_surface);
  unsigned char *pixels = cairo_image_surface_get_data(test_surface);
  int stride = cairo_image_surface_get_stride(test_surface);
  gboolean branches_ok = FALSE;
  for(int py = 0; py < 600 && !branches_ok; py++)
    for(int px = 0; px < 800; px++)
      if(pixels[py * stride + px * 4 + 3]) { branches_ok = TRUE; break; }
  cairo_surface_destroy(test_surface);

  /* Eine Darstellungsänderung muss ebenso rückgängig werden wie der Baum. */
  GdkRGBA hintergrund_vorher = hintergrundfarbe;
  tempspeichern();
  hintergrundfarbe.red = hintergrundfarbe.red < 0.5 ? 0.9 : 0.1;
  hintergrundfarbe.green = hintergrundfarbe.green < 0.5 ? 0.8 : 0.2;
  hintergrundfarbe.blue = hintergrundfarbe.blue < 0.5 ? 0.7 : 0.3;
  rueckgaengig(NULL, layout);
  gboolean farbe_undo_ok =
      fabs(hintergrundfarbe.red - hintergrund_vorher.red) < 0.000001 &&
      fabs(hintergrundfarbe.green - hintergrund_vorher.green) < 0.000001 &&
      fabs(hintergrundfarbe.blue - hintergrund_vorher.blue) < 0.000001 &&
      fabs(hintergrundfarbe.alpha - hintergrund_vorher.alpha) < 0.000001;

  /* Regression: Hintergrund ändern, danach Zweigfarbe ändern, einmal Undo. */
  GdkRGBA test_hintergrund = { 0.25, 0.5, 0.75, 1.0 };
  GtkWidget *test_hintergrunddialog = gtk_color_chooser_dialog_new(
      "Test", GTK_WINDOW(window));
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(test_hintergrunddialog),
                             &test_hintergrund);
  dialogschliessen(GTK_DIALOG(test_hintergrunddialog), GTK_RESPONSE_OK, layout);
  gtk_widget_destroy(test_hintergrunddialog);

  GdkRGBA test_zweigfarbe = { 0.8, 0.2, 0.1, 1.0 };
  GtkWidget *test_zweigdialog = gtk_color_chooser_dialog_new(
      "Test", GTK_WINDOW(window));
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(test_zweigdialog),
                             &test_zweigfarbe);
  dialogzschliessen(GTK_DIALOG(test_zweigdialog), GTK_RESPONSE_OK, layout);
  gtk_widget_destroy(test_zweigdialog);
  rueckgaengig(NULL, layout);
  gboolean farbfolge_undo_ok =
      fabs(hintergrundfarbe.red - 0.25) < 0.000001 &&
      fabs(hintergrundfarbe.green - 0.5) < 0.000001 &&
      fabs(hintergrundfarbe.blue - 0.75) < 0.000001 &&
      fabs(hintergrundfarbe.alpha - 1.0) < 0.000001 &&
      fabs(tempzahl_laden("0,25") - 0.25) < 0.000001;

  /* Regression: Auch der letzte Knoten im Array muss sich entfernen lassen.
   * Dabei darf die Fokuswiederherstellung nicht auf den entfernten Index
   * zugreifen. */
  GtkWidget *zu_loeschender_knoten = textfeld[knotenexistiert("-1-0")];
  gtk_widget_grab_focus(zu_loeschender_knoten);
  keyfunc(controller, GDK_KEY_Delete, 0, 0, layout);
  GtkWidget *fokus_nach_loeschen = gtk_window_get_focus(GTK_WINDOW(window));
  gboolean loeschen_ok = maxzaehler == 1 &&
                         knotenexistiert("-1-0") < 0 &&
                         fokus_nach_loeschen != NULL &&
                         GTK_IS_EDITABLE(fokus_nach_loeschen);

  /* Auch der Ansichtsmodus wird geprüft: Ein schräger Zweig muss ein sichtbar
   * gedrehtes Wahrscheinlichkeitslabel besitzen. */
  g_signal_handlers_block_by_func(textfeldWahrscheinlichkeit[0],
                                  G_CALLBACK(wskeingabe), layout);
  gtk_entry_set_text(GTK_ENTRY(textfeldWahrscheinlichkeit[0]), "0,5");
  g_signal_handlers_unblock_by_func(textfeldWahrscheinlichkeit[0],
                                    G_CALLBACK(wskeingabe), layout);
  umwandeln(NULL, layout);
  arboretum_layout_aktualisieren(layout);
  GskTransform *label_transform =
      gtk_fixed_get_child_transform(GTK_FIXED(layout), wahrscheinlichkeitlabel[0]);
  float transform_skew_x = 0, transform_skew_y = 0;
  float transform_scale_x = 0, transform_scale_y = 0;
  float transform_angle = 0, transform_dx = 0, transform_dy = 0;
  if (label_transform)
    gsk_transform_to_2d_components(label_transform,
                                   &transform_skew_x, &transform_skew_y,
                                   &transform_scale_x, &transform_scale_y,
                                   &transform_angle, &transform_dx, &transform_dy);
  double *saved_position = g_object_get_data(
      G_OBJECT(wahrscheinlichkeitlabel[0]), "arboretum-layout-position");
  double position_tolerance = MAX(breitevordrehung[0], hoehevordrehung[0]) * 2.0;
  /* Eine Drehung verschiebt den Ursprung etwas. Er muss aber weiterhin in der
   * Nähe der berechneten Zweigposition liegen und darf nicht bei (0,0) landen. */
  gboolean fixed_position_ok = saved_position &&
      fabs(transform_dx - saved_position[0]) < position_tolerance &&
      fabs(transform_dy - saved_position[1]) < position_tolerance;
  gboolean fixed_view_ok = labelein && wahrscheinlichkeitlabel[0] &&
      breitevordrehung[0] > 0 && hoehevordrehung[0] > 0 &&
      (arboretum_label_get_angle(GTK_LABEL(wahrscheinlichkeitlabel[0])) > 0.001 ||
       arboretum_label_get_angle(GTK_LABEL(wahrscheinlichkeitlabel[0])) < -0.001) &&
      (transform_angle > 0.001 || transform_angle < -0.001) &&
      fixed_position_ok;

  g_print("Interaktions-Smoke-Test: capture=%s, Eingabe=%s, Standardstil=%s, runter=%s, rechts=%s, Textcursor-links=%s, Textcursor-rechts=%s, links=%s, hoch=%s, Wahrscheinlichkeit-Fokus=%s, Wahrscheinlichkeit-Textcursor=%s, Komma-Eingabe=%s, Punkt-Eingabe=%s, UI-Hintergrund=%s, Position=%s, Zweige=%s, Farbe-Undo=%s, Farbfolge-Undo=%s, Löschen=%s, Ansichtsmodus=%s\n",
          phase_ok ? "ok" : "FEHLER",
          eingabe_ok ? "ok" : "FEHLER",
          standardstil_ok ? "ok" : "FEHLER",
          down_ok ? "ok" : "FEHLER",
          right_ok ? "ok" : "FEHLER",
          cursor_links_ok ? "ok" : "FEHLER",
          cursor_rechts_ok ? "ok" : "FEHLER",
          left_ok ? "ok" : "FEHLER",
          up_ok ? "ok" : "FEHLER",
          probability_focus_ok ? "ok" : "FEHLER",
          wsk_cursor_ok ? "ok" : "FEHLER",
          wsk_eingabe_ok ? "ok" : "FEHLER",
          punkt_eingabe_ok ? "ok" : "FEHLER",
          ui_hintergrund_ok ? "ok" : "FEHLER",
          probability_position_ok ? "ok" : "FEHLER",
          branches_ok ? "ok" : "FEHLER",
          farbe_undo_ok ? "ok" : "FEHLER",
          farbfolge_undo_ok ? "ok" : "FEHLER",
          loeschen_ok ? "ok" : "FEHLER",
          fixed_view_ok ? "ok" : "FEHLER");
  arboretum_exit_status = (phase_ok && eingabe_ok && standardstil_ok && down_ok && right_ok &&
                           cursor_links_ok && cursor_rechts_ok && left_ok && up_ok &&
                           probability_focus_ok && wsk_cursor_ok && wsk_eingabe_ok && punkt_eingabe_ok && ui_hintergrund_ok && probability_position_ok && branches_ok &&
                           farbe_undo_ok && farbfolge_undo_ok && loeschen_ok && fixed_view_ok) ? 0 : 1;
  g_main_loop_quit(arboretum_main_loop);
  return G_SOURCE_REMOVE;
}

/* Programmeinstieg: Widgets anlegen, Signale verbinden, Ereignisschleife starten. */

int main (int argc, char *argv[])
{
  startup_trace("main entered");

  SETENV

  /* Desktop-Umgebungen ordnen das laufende Fenster über diesen Namen dem
   * Starter arboretum.desktop und seinem Programmsymbol zu. */
  g_set_prgname("arboretum");
  
  GtkWidget *gesamtbox;
  GtkWidget *layout;
  hintergrundfarbe.red=1;
  hintergrundfarbe.green=1;
  hintergrundfarbe.blue=1;
  /* Neue Bäume werden standardmäßig transparent exportiert. In der
   * Arbeitsansicht setzt zeichnelinien.c diese Farbe weiterhin über Weiß
   * zusammen, damit kein schwarzer Compositor-Puffer sichtbar werden kann. */
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

  startup_trace("before gtk_init");
  gtk_init ();
  startup_trace("gtk_init complete");
  gtk_window_set_default_icon_name("arboretum");
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

  window = gtk_window_new ();
  gesamtbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_window_set_title (GTK_WINDOW(window), Titel);
  gtk_window_set_icon_name(GTK_WINDOW(window), "arboretum");
  gtk_window_set_default_size (GTK_WINDOW(window), 800, 600);
//  gtk_window_set_icon_from_file (GTK_WINDOW(window),"baumicon.svg",NULL);
  gtk_window_maximize (GTK_WINDOW(window));
  g_signal_connect (window, "close-request", G_CALLBACK (beenden), NULL);

  y[0] = 0;
  ymax += y[0];
  yerg[0] = y[0];

  scrollwindow = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scrollwindow), FALSE);
  layout = gtk_layout_new (NULL, NULL);

  da = gtk_drawing_area_new();
//  gtk_widget_add_events(da,GDK_BUTTON_PRESS_MASK);
  GROESSEDRAWINGAREA
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(da), zeichnelinien, layout, NULL);
//  g_signal_connect (da, "button-press-event", G_CALLBACK (xy), layout);

  textfeld[0] = gtk_entry_new();
  gtk_widget_set_name(textfeld[0],"-0");
  gtk_entry_set_width_chars (GTK_ENTRY(textfeld[0]),KnotenTextBreite);
  gtk_entry_set_alignment (GTK_ENTRY(textfeld[0]),0.5);
//  g_signal_connect (textfeld[0], "draw", G_CALLBACK(zeichneknotenhintergrund),  NULL);

  vorgaenger[0] = &textfeld[0];
  GtkEventController *key_controller = gtk_event_controller_key_new();
  gtk_event_controller_set_propagation_phase(key_controller, GTK_PHASE_CAPTURE);
  g_object_set_data(G_OBJECT(key_controller), "arboretum-layout", layout);
  g_signal_connect (key_controller, "key-pressed", G_CALLBACK (keyfunc), layout);
  gtk_widget_add_controller(window, key_controller);
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


  GtkWidget *menueleiste = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  GtkWidget *menuescroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(menuescroll), FALSE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(menuescroll),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(menuescroll), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(menuescroll), menueleiste);
  GtkWidget *dateigruppe = werkzeuggruppe(menueleiste, "Datei");
  werkzeugknopf(dateigruppe, "Neu", G_CALLBACK(neueinstanz), argv[0]);
  werkzeugknopf(dateigruppe, "Öffnen", G_CALLBACK(oeffnen), layout);
  werkzeugknopf(dateigruppe, "Speichern", G_CALLBACK(speichernvor), NULL);
  werkzeugknopf(dateigruppe, "Speichern unter", G_CALLBACK(speicherdialog), NULL);
  dateimenue_exportieren = werkzeugknopf(dateigruppe, "Exportieren",
                                         G_CALLBACK(exportdialog), layout);

  werkzeugknopf(menueleiste, "Rückgängig", G_CALLBACK(rueckgaengig), layout);

  GtkWidget *darstellungsgruppe = werkzeuggruppe(menueleiste, "Darstellung");
  werkzeugknopf(darstellungsgruppe, "Form", G_CALLBACK(formdialog), layout);
  werkzeugknopf(darstellungsgruppe, "Schriftart", G_CALLBACK(schriftartwechseln), layout);
  werkzeugknopf(darstellungsgruppe, "Fixieren", G_CALLBACK(umwandeln), layout);
  werkzeugknopf(darstellungsgruppe, "Ergebnis", G_CALLBACK(ergebnisspalteanzeigen), layout);
  werkzeugknopf(darstellungsgruppe, "Wahrscheinlichkeit", G_CALLBACK(wskergebnisspalteanzeigen), layout);

  GtkWidget *farbgruppe = werkzeuggruppe(menueleiste, "Farbe");
  werkzeugknopf(farbgruppe, "Hintergrund", G_CALLBACK(hintergrundfarbewechseln), layout);
  werkzeugknopf(farbgruppe, "Zweige", G_CALLBACK(zweigfarbewechseln), layout);
  werkzeugknopf(farbgruppe, "Schrift", G_CALLBACK(schriftfarbewechseln), layout);
  werkzeugknopf(farbgruppe, "Knotenrahmen", G_CALLBACK(knotenrandfarbewechseln), layout);
  werkzeugknopf(farbgruppe, "Knotenfläche", G_CALLBACK(knotenhintergrundfarbewechseln), layout);

  werkzeugknopf(menueleiste, "Hilfe", G_CALLBACK(hilfe), layout);

  GROESSELAYOUT

  gtk_layout_put (GTK_LAYOUT (layout), da, FensterRandLinks,FensterRandOben);
  gtk_layout_put (GTK_LAYOUT (layout), textfeld[0], FensterRandLinks+RandLinks+StufenBreite,FensterRandOben+RandOben+y[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldErgebnis[0], FensterRandLinks+RandLinks+StufenBreite+KnotenBreite+ErgebnisAbstand,FensterRandOben+RandOben+yerg[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldWahrscheinlichkeit[0], FensterRandLinks+RandLinks+StufenBreite/2-WahrscheinlichkeitBreite/2,FensterRandOben+RandOben+y[0]);
  gtk_layout_put (GTK_LAYOUT (layout), textfeldErgebnisWahrscheinlichkeit[0], FensterRandLinks+RandLinks+StufenBreite+KnotenBreite+ErgebnisAbstand*2+ErgebnisBreite,FensterRandOben+RandOben+yerg[0]);
  gtk_layout_put (GTK_LAYOUT (layout), wahrscheinlichkeitlabel[0], 0,0);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollwindow), layout);
  gtk_box_append(GTK_BOX(gesamtbox), menuescroll);
  gtk_widget_set_vexpand(scrollwindow, TRUE);
  gtk_widget_set_hexpand(scrollwindow, TRUE);
  gtk_box_append(GTK_BOX(gesamtbox), scrollwindow);
  gtk_window_set_child(GTK_WINDOW(window), gesamtbox);
  startup_trace("before window presentation");
  gtk_window_present(GTK_WINDOW(window));
  startup_trace("window presented");
  gtk_widget_hide(wahrscheinlichkeitlabel[0]);
  gtk_widget_grab_focus(textfeld[0]);
  g_idle_add(startfeld_fokussieren, textfeld[0]);
  GtkWidget *fakedialog = gtk_font_chooser_dialog_new ("Schriftart auswählen", GTK_WINDOW (window));
  startup_trace("font chooser created");
  g_autofree gchar *initial_font = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(fakedialog));
  g_strlcpy(schriftart, initial_font ? initial_font : "Sans 12", sizeof(schriftart));
  gtk_window_destroy(GTK_WINDOW(fakedialog));
  startup_trace("font initialized");

  if(argc > 1)
  {
    laden(layout,argv[1]);
  }
  startup_trace("before initial undo save");
  tempspeichern();
  startup_trace("initial undo save complete");

  arboretum_main_loop = g_main_loop_new(NULL, FALSE);
  if(g_getenv("ARBORETUM_KEYBOARD_SMOKE_TEST"))
    g_idle_add(pfeiltasten_smoketest, key_controller);
  startup_trace("entering event loop");
  g_main_loop_run(arboretum_main_loop);
  g_main_loop_unref(arboretum_main_loop);
  arboretum_main_loop = NULL;

  int i;
//  printf("%i\n",dateinummerierung);
  for(i = 1 ; i < dateinummerierung ; i++)
  {
    g_autofree gchar *dateiname = arboretum_temp_path(i);
    g_remove(dateiname);
  }
  return arboretum_exit_status;
}
