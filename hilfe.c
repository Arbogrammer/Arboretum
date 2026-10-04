/* Text tags keep the help selectable and accessible while allowing headings
 * and emphasis without manually aligned spaces or hard-wrapped paragraphs. */
static void hilfe_absatz(GtkTextBuffer *buffer, const char *anfang,
                         const char *text, const char *tag) {
  GtkTextIter ende;
  gtk_text_buffer_get_end_iter(buffer, &ende);
  if (anfang)
    gtk_text_buffer_insert_with_tags_by_name(buffer, &ende, anfang, -1,
                                             tag, NULL);
  if (text)
    gtk_text_buffer_insert(buffer, &ende, text, -1);
  gtk_text_buffer_insert(buffer, &ende, "\n", 1);
}

static void hilfe_titel(GtkTextBuffer *buffer, const char *text) {
  hilfe_absatz(buffer, text, NULL, "abschnitt");
}

static void hilfe_befehl(GtkTextBuffer *buffer, const char *befehl,
                        const char *beschreibung) {
  hilfe_absatz(buffer, befehl, beschreibung, "fett");
}

void hilfe(GtkWidget *widget, gpointer user_data) {
  GtkWidget *HilfeFenster = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(HilfeFenster), "Hilfe");
  gtk_window_set_default_size(GTK_WINDOW(HilfeFenster), 800, 600);
  gtk_window_set_transient_for(GTK_WINDOW(HilfeFenster), GTK_WINDOW(window));

  GtkTextBuffer *text = gtk_text_buffer_new(NULL);
  gtk_text_buffer_create_tag(text, "titel", "weight", PANGO_WEIGHT_BOLD,
                             "scale", 1.5, "pixels-below-lines", 12, NULL);
  gtk_text_buffer_create_tag(text, "abschnitt", "weight", PANGO_WEIGHT_BOLD,
                             "scale", 1.15, "pixels-above-lines", 18,
                             "pixels-below-lines", 8, NULL);
  gtk_text_buffer_create_tag(text, "fett", "weight", PANGO_WEIGHT_BOLD, NULL);

  hilfe_absatz(text, "Arboretum – Kurzanleitung", NULL, "titel");
  hilfe_absatz(text, NULL,
      "Baumdiagramme bearbeiten, Wahrscheinlichkeiten berechnen und das fertige "
      "Diagramm exportieren. Die wichtigsten Befehle findest du auch in den Menüs.", NULL);

  hilfe_titel(text, "Baum bearbeiten: von links nach rechts");
  hilfe_befehl(text, "↓ Pfeil nach unten", " – Zum nächsten Knoten derselben Stufe wechseln oder einen anlegen.");
  hilfe_befehl(text, "↑ Pfeil nach oben", " – Zum vorherigen Knoten wechseln.");
  hilfe_befehl(text, "→ Pfeil nach rechts", " – Im Text nach rechts; am Textende zum ersten Kind wechseln oder eines anlegen.");
  hilfe_befehl(text, "← Pfeil nach links", " – Im Text nach links; am Textanfang zum übergeordneten Knoten wechseln.");
  hilfe_absatz(text, NULL,
      "In Wahrscheinlichkeitsfeldern wechseln Pfeil nach oben und unten zum vorherigen "
      "beziehungsweise nächsten Feld derselben Stufe – auch über Elternknoten hinweg.", NULL);

  hilfe_titel(text, "Baum bearbeiten: von oben nach unten");
  hilfe_befehl(text, "↓ Pfeil nach unten", " – Zum ersten Kind wechseln oder eines anlegen.");
  hilfe_befehl(text, "↑ Pfeil nach oben", " – Zum übergeordneten Knoten wechseln.");
  hilfe_befehl(text, "→ Pfeil nach rechts", " – Zum nächsten Knoten derselben Stufe wechseln oder einen anlegen.");
  hilfe_befehl(text, "← Pfeil nach links", " – Zum vorherigen Knoten derselben Stufe wechseln.");

  hilfe_titel(text, "In beiden Baumrichtungen");
  hilfe_befehl(text, "Pos1", " – Zum ersten Knoten wechseln.");
  hilfe_befehl(text, "Tab", " – Zwischen Knoten und zugehöriger Wahrscheinlichkeit wechseln.");
  hilfe_befehl(text, "Entf", " – Aktuellen Knoten einschließlich seiner Nachfolger löschen.");
  hilfe_befehl(text, "Strg+Entf", " – Den gesamten Baum zurücksetzen.");

  hilfe_titel(text, "Text und Wahrscheinlichkeiten");
  hilfe_absatz(text, NULL, "Text wird direkt in die Knoten- und Wahrscheinlichkeitsfelder eingegeben.", NULL);
  hilfe_befehl(text, "Strg+-", " – Am Cursor einen Überstrich für das vorhergehende Zeichen einfügen. "
      "Für mehrere Zeichen das Kürzel nach jedem Zeichen verwenden.");
  hilfe_befehl(text, "Brüche", " – Mit einem Schrägstrich eingeben, zum Beispiel 3/7. "
      "Bei vertikaler Darstellung springt / in den Nenner; ↑ und ↓ wechseln zwischen den Teilen. "
      "Rückschritt im leeren Nenner entfernt den Bruchstrich. Strg+A markiert den gesamten Wert. "
      "Dezimalzahlen bleiben einzeilig. Brüche und Dezimalzahlen können gemischt werden. Gemischte Pfade werden dezimal "
      "berechnet; ungültige Eingaben sind markiert.");
  hilfe_befehl(text, "Darstellung → Form", " – Hier lassen sich die vertikale Bruchdarstellung, "
      "das automatische Kürzen und das Ergänzen der letzten Wahrscheinlichkeit ein- oder ausschalten.");
  hilfe_befehl(text, "Position an den Zweigen", " – Für die fixierte Ansicht lässt sich unter "
      "Darstellung → Form die Zweigseite wählen: oberhalb, unterhalb oder nach oberer/unterer Hälfte. "
      "Der mittlere Zweig ist separat einstellbar. Vertikal gelten die Seiten links/rechts.");
  hilfe_befehl(text, "Optionale Automatik", " – Verschiebt Wahrscheinlichkeiten zunächst etwas "
      "nach rechts (vertikal nach unten) und erweitert bei Bedarf die Astabstände. "
      "Die gewählte Zweigseite bleibt dabei erhalten.");

  hilfe_titel(text, "Ansicht und Gestaltung");
  hilfe_befehl(text, "Strg+U / Darstellung → Fixieren", " – Zwischen Bearbeitungs- und fixierter Ansicht wechseln.");
  hilfe_befehl(text, "Strg+E", " – Ergebnisspalte ein- oder ausblenden.");
  hilfe_befehl(text, "Strg+W", " – Wahrscheinlichkeiten der Ergebnisse ein- oder ausblenden.");
  hilfe_befehl(text, "Strg+R / Darstellung → Von oben nach unten", " – Zwischen horizontaler und "
      "vertikaler Baumansicht wechseln. Der aktuell bearbeitete Knoten bleibt fokussiert.");
  hilfe_befehl(text, "Darstellung → Form", " – Abstände, Größen, Linienstärke, Bruchdarstellung "
      "und weitere Maße des Diagramms anpassen.");
  hilfe_befehl(text, "Darstellung → Schriftart", " – Die Schrift für die fixierte Ansicht wählen.");
  hilfe_befehl(text, "Farbe", " – Hintergrund, Zweige, Schrift, Knotenrahmen und Knotenflächen ändern.");

  hilfe_titel(text, "Ergebnisüberschriften");
  hilfe_befehl(text, "Darstellung → Ergebnisüberschriften", " – Überschriften für die Ergebnisse "
      "und ihre Wahrscheinlichkeiten auswählen. Zur Wahl stehen:");
  hilfe_befehl(text, "• Keine", " – Die Voreinstellung; es erscheinen keine Überschriften.");
  hilfe_befehl(text, "• ω | P(ω)", " – Ergebnis und Wahrscheinlichkeit in Kurzschreibweise.");
  hilfe_befehl(text, "• ω | P({ω})", " – Ergebnis und Wahrscheinlichkeit des Elementarereignisses.");
  hilfe_befehl(text, "• {ω} | P({ω})", " – Elementarereignis und seine Wahrscheinlichkeit.");
  hilfe_befehl(text, "• Eigene …", " – Zwei freie Texte, zum Beispiel „Ergebnis“ und „Wahrscheinlichkeit“. "
      "Je Feld sind bis zu 160 Zeichen möglich. Ein leeres Feld blendet die jeweilige Überschrift aus.");
  hilfe_absatz(text, NULL,
      "Bei waagerechten Bäumen stehen die Überschriften über den Ergebnisspalten, "
      "bei senkrechten Bäumen links neben den Ergebniszeilen. Wird eine Ergebnisspalte "
      "ausgeblendet, verschwindet auch ihre Überschrift.", NULL);
  hilfe_absatz(text, NULL,
      "Die Auswahl gilt für das Fenster und alle Exportformate. Sie wird zusammen mit eigenen "
      "Texten in der BDG-Datei gespeichert und lässt sich rückgängig machen. Ältere Dateien "
      "öffnen sich ohne Überschriften.", NULL);

  hilfe_titel(text, "Modellvorlagen");
  hilfe_befehl(text, "Abkürzungen", " – Die Vorlagen erzeugen sofort einen vollständigen Wahrscheinlichkeitsbaum.");
  hilfe_befehl(text, "Urnenmodell", " – Bezeichnungen und Anzahlen jeweils mit Kommas eingeben, "
      "zum Beispiel B, G, W und 2, 3, 1. Anschließend die Anzahl der Ziehungen sowie Ziehen "
      "ohne Zurücklegen, mit Zurücklegen oder mit Dazulegen wählen.");
  hilfe_befehl(text, "Binomialmodell", " – Wahrscheinlichkeit für das erste Ergebnis, beide Ergebnisnamen "
      "und die Anzahl der Stufen angeben.");
  hilfe_befehl(text, "Pfadvorlage", " – Ausgänge und zugehörige Wahrscheinlichkeiten jeweils mit Kommas "
      "trennen, zum Beispiel A, B und 0.5, 0.5. Wahrscheinlichkeiten können auch als Brüche wie 1/3 "
      "eingegeben werden und müssen zusammen 1 ergeben. Mit Gleichverteilung werden sie automatisch "
      "gleich verteilt; bei Bedarf als Brüche. Danach die Anzahl der Stufen wählen.");

  hilfe_titel(text, "Dateien und Bearbeitung");
  hilfe_befehl(text, "Einfg", " – Eine Datei öffnen.");
  hilfe_befehl(text, "Strg+S", " – Speichern.");
  hilfe_befehl(text, "Strg+Z", " – Letzte Änderung rückgängig machen.");
  hilfe_befehl(text, "Strg+Umschalt+Z", " – Änderung wiederherstellen.");
  hilfe_befehl(text, "F1", " – Diese Hilfe anzeigen.");
  hilfe_befehl(text, "Datei", " – Hier befinden sich auch Neu, Öffnen, Speichern und Speichern unter.");

  hilfe_titel(text, "Exportieren");
  hilfe_befehl(text, "1. Datei → Exportieren", " wählen (alternativ Strg+Umschalt+A).");
  hilfe_befehl(text, "2. Dateiendung angeben", " – Sie bestimmt das Exportformat:");
  hilfe_befehl(text, "• PNG, SVG, PDF, BMP oder JPG", " – Das Diagramm als Grafik oder PDF ausgeben.");
  hilfe_befehl(text, "• ODT (Writer) oder DOCX (Word)", " – Bearbeitbare Linien und Texte. "
      "Zum Bearbeiten im Office-Programm die Gruppe betreten oder die Gruppierung aufheben.");
  hilfe_befehl(text, "• TEX (LaTeX/TikZ)", " – Die erzeugte .tex-Datei mit LuaLaTeX oder XeLaTeX kompilieren.");
  hilfe_absatz(text, NULL, "Ohne Dateiendung wird das Diagramm als SVG exportiert. "
      "Der Export ist sowohl beim Bearbeiten als auch in der fixierten Ansicht verfügbar. "
      "Die gewählten Ergebnisüberschriften werden übernommen.", NULL);

  GtkWidget *HilfeTextView = gtk_text_view_new_with_buffer(text);
  g_object_unref(text);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(HilfeTextView), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(HilfeTextView), 24);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(HilfeTextView), 24);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(HilfeTextView), 20);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(HilfeTextView), 24);
  gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(HilfeTextView), 6);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(HilfeTextView), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(HilfeTextView), GTK_WRAP_WORD_CHAR);

  GtkWidget *HilfeScroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(HilfeScroll),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(HilfeScroll), HilfeTextView);
  gtk_window_set_child(GTK_WINDOW(HilfeFenster), HilfeScroll);
  gtk_window_present(GTK_WINDOW(HilfeFenster));
}
