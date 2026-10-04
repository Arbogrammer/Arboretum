void hilfe(GtkWidget *widget, gpointer user_data) {
  GtkWidget *HilfeFenster = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(HilfeFenster), "Hilfe");
  gtk_window_set_default_size(GTK_WINDOW(HilfeFenster), 800, 600);
  gtk_window_set_transient_for(GTK_WINDOW(HilfeFenster), GTK_WINDOW(window));

  GtkTextBuffer *HilfeText = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(
      HilfeText,
      "ARBORETUM – KURZANLEITUNG\n"
      "\n"
      "Baum bearbeiten (von links nach rechts)\n"
      "  Pfeil nach unten   Zum nächsten Knoten derselben Stufe wechseln oder "
      "einen anlegen\n"
      "  Pfeil nach oben    Zum vorherigen Knoten wechseln\n"
      "  Pfeil nach rechts  Im Text nach rechts; am Textende zum ersten Kind "
      "wechseln oder eines anlegen\n"
      "  Pfeil nach links   Im Text nach links; am Textanfang zum "
      "übergeordneten Knoten wechseln\n"
      "  Pos1               Zum ersten Knoten wechseln\n"
      "  Tab                Zwischen Knoten und zugehöriger Wahrscheinlichkeit "
      "wechseln\n"
      "  In Wahrscheinlichkeitsfeldern wechseln Pfeil nach oben und unten zum "
      "vorherigen\n"
      "  beziehungsweise nächsten Feld derselben Stufe – auch über "
      "Elternknoten hinweg.\n"
      "  Entf               Aktuellen Knoten einschließlich seiner Nachfolger "
      "löschen\n"
      "  Strg+Entf          Den gesamten Baum zurücksetzen\n"
      "\n"
      "Baum bearbeiten (von oben nach unten)\n"
      "  Pfeil nach unten   Zum ersten Kind wechseln oder eines anlegen\n"
      "  Pfeil nach oben    Zum übergeordneten Knoten wechseln\n"
      "  Pfeil nach rechts  Zum nächsten Knoten derselben Stufe wechseln oder "
      "einen anlegen\n"
      "  Pfeil nach links   Zum vorherigen Knoten derselben Stufe wechseln\n"
      "\n"
      "Text und Wahrscheinlichkeiten\n"
      "  Text wird direkt in die Knoten- und Wahrscheinlichkeitsfelder "
      "eingegeben.\n"
      "  Strg+-             Am Cursor einen Überstrich für das vorhergehende "
      "Zeichen einfügen\n"
      "                     (für mehrere Zeichen das Kürzel nach jedem Zeichen "
      "verwenden)\n"
      "  Brüche werden mit einem Schrägstrich eingegeben, zum Beispiel 3/7.\n"
      "  Brüche und Dezimalzahlen können gemischt werden. Gemischte Pfade\n"
      "  werden dezimal berechnet; ungültige Eingaben sind markiert.\n"
      "  Die vertikale Bruchdarstellung, das automatische Kürzen und das "
      "Ergänzen der\n"
      "  letzten Wahrscheinlichkeit lassen sich unter\n"
      "  Darstellung > Form ein- oder ausschalten.\n"
      "  Dort lässt sich für die fixierte Ansicht auch die Zweigseite wählen:\n"
      "  oberhalb, unterhalb oder nach oberer/unterer Hälfte. Der mittlere\n"
      "  Zweig ist separat einstellbar. Vertikal gelten die Seiten links/rechts.\n"
      "  Die optionale Automatik verschiebt Wahrscheinlichkeiten zunächst\n"
      "  etwas nach rechts (vertikal nach unten) und erweitert bei Bedarf\n"
      "  die Astabstände. Die gewählte Zweigseite bleibt dabei erhalten.\n"
      "\n"
      "Ansicht und Ergebnis\n"
      "  Strg+U             Zwischen Bearbeitungs- und fixierter Ansicht "
      "wechseln\n"
      "  Strg+E             Ergebnisspalte ein- oder ausblenden\n"
      "  Strg+W             Wahrscheinlichkeiten der Ergebnisse ein- oder "
      "ausblenden\n"
      "  Strg+R / Darstellung > Von oben nach unten\n"
      "                     Schaltet zwischen der bisherigen horizontalen und "
      "der vertikalen\n"
      "                     Baumansicht um. Der aktuell bearbeitete Knoten "
      "bleibt fokussiert.\n"
      "  Unter Darstellung > Form können Abstände, Größen, Linienstärke, "
      "Bruchdarstellung\n"
      "  und weitere Maße des Diagramms angepasst werden.\n"
      "  Unter Darstellung > Schriftart wird die Schrift für die fixierte "
      "Ansicht gewählt.\n"
      "  Das Menü Farbe ändert Hintergrund, Zweige, Schrift, Knotenrahmen und "
      "Knotenflächen.\n"
      "\n"
      "Modellvorlagen\n"
      "  Unter Abkürzungen erzeugen Vorlagen sofort einen vollständigen "
      "Wahrscheinlichkeitsbaum.\n"
      "  Urnenmodell        Bezeichnungen und Anzahlen jeweils mit Kommas "
      "eingeben,\n"
      "                     zum Beispiel B, G, W und 2, 3, 1. Anschließend "
      "Anzahl der\n"
      "                     Ziehungen sowie Ziehen ohne Zurücklegen, mit "
      "Zurücklegen\n"
      "                     oder mit Dazulegen wählen.\n"
      "  Binomialmodell     Wahrscheinlichkeit für das erste Ergebnis, beide "
      "Ergebnisnamen\n"
      "                     und die Anzahl der Stufen angeben.\n"
      "  Pfadvorlage        Ausgänge und zugehörige Wahrscheinlichkeiten jeweils "
      "mit\n"
      "                     Kommas trennen, zum Beispiel A, B und 0.5, 0.5. "
      "Die\n"
      "                     Wahrscheinlichkeiten können auch als Brüche wie "
      "1/3 eingegeben\n"
      "                     werden und müssen zusammen 1 ergeben. Mit "
      "Gleichverteilung\n"
      "                     werden sie automatisch gleich verteilt; bei Bedarf "
      "als Brüche.\n"
      "                     Danach die Anzahl der Stufen wählen.\n"
      "\n"
      "Dateien und Bearbeitung\n"
      "  Einfg               Eine Datei öffnen\n"
      "  Strg+S              Speichern\n"
      "  Strg+Z              Letzte Änderung rückgängig machen\n"
      "  Strg+Umschalt+Z     Änderung wiederherstellen\n"
      "  F1                  Diese Hilfe anzeigen\n"
      "  Die Befehle Neu, Öffnen, Speichern und Speichern unter befinden sich "
      "auch im Menü Datei.\n"
      "\n"
      "Exportieren\n"
      "  1. Datei > Exportieren wählen (alternativ Strg+A).\n"
      "  2. Durch die Dateiendung das Format festlegen: .png, .svg, .pdf, .bmp "
      "oder .jpg sowie .odt (Writer), .docx (Word) und .tex (LaTeX/TikZ).\n"
      "     .odt/.docx: bearbeitbare Linien und Texte; Gruppierung zum Bearbeiten aufheben.\n"
      "     .tex-Dateien mit LuaLaTeX oder XeLaTeX kompilieren.\n"
      "     Ohne Dateiendung wird das Diagramm als SVG exportiert.\n"
      "  Der Export ist sowohl beim Bearbeiten als auch in der fixierten "
      "Ansicht verfügbar.\n",
      -1);

  GtkWidget *HilfeTextView = gtk_text_view_new_with_buffer(HilfeText);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(HilfeTextView), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(HilfeTextView), 10);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(HilfeTextView), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(HilfeTextView), TRUE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(HilfeTextView), GTK_WRAP_WORD_CHAR);

  GtkWidget *HilfeScroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(HilfeScroll),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(HilfeScroll),
                                HilfeTextView);
  gtk_window_set_child(GTK_WINDOW(HilfeFenster), HilfeScroll);
  gtk_window_present(GTK_WINDOW(HilfeFenster));
}
