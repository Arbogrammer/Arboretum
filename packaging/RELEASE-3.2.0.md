# Arboretum 3.2.0

Arboretum 3.2.0 erweitert die Änderungshistorie um Wiederherstellen und behebt
Fehler bei Wahrscheinlichkeitsberechnungen, beim Speichern der Baumrichtung
und bei der Darstellung von Eingabefeldern.

## Neue Funktionen

- **Wiederherstellen** über die Werkzeugleiste oder `Strg+Umschalt+Z`.
- Rückgängig und Wiederherstellen behalten bis zu 100 Zustände.
  Zusammenhängende Texteingaben im selben Feld werden bis zu einer Tipp-Pause
  von einer Sekunde gebündelt. Feldwechsel und andere Bearbeitungen beenden
  die Gruppe; eine neue Bearbeitung verwirft die Wiederherstellungsschritte.
- Die horizontale oder vertikale Baumrichtung wird in der Baumdatei gespeichert
  und beim Öffnen wiederhergestellt.

## Fehlerbehebungen

- Brüche und Dezimalzahlen mit Punkt oder Komma können gemeinsam in einem Pfad
  verwendet werden. Reine Bruchpfade bleiben als Bruch erhalten; gemischte Pfade
  werden dezimal ausgegeben.
- Ungültige Wahrscheinlichkeiten werden im Eingabefeld markiert. Für betroffene
  Pfade erscheint kein irreführendes Ergebnis. Berechnungen und das Einlesen
  fehlerhafter Dateien sind gegen weitere Überlauf- und Parserfehler abgesichert.
- Eingabe- und Ergebnisfelder passen ihre Breite an alle angezeigten Werte an,
  auch nach dem Laden oder Rückgängigmachen.
- Scrollbewegungen erfolgen außerhalb des Zeichenvorgangs, um GTK-Warnungen
  und Darstellungsprobleme zu vermeiden.
- Ein leerer Ergebnistrenner wird im Formdialog korrekt angezeigt und gespeichert.
- Layout, Wahrscheinlichkeitsmarkierungen und Baumrichtung werden nach
  Bearbeitungen, Laden und Wiederherstellen konsistent aktualisiert.

## Dateiformat

Ältere Baumdateien bleiben lesbar und öffnen standardmäßig horizontal, wenn
keine Baumrichtung gespeichert ist. Neu gespeicherte Dateien enthalten ein
zusätzliches Feld für die Baumrichtung und benötigen **Arboretum 3.2.0 oder
eine spätere kompatible Version**. Version 3.1.0 kann diese Dateien nicht öffnen.

## Prüfungen

Die Regressionstests umfassen Tastaturbedienung, Dateioperationen und Exporte,
Bearbeitung und Undo/Redo, Layout, Parserprüfungen mit AddressSanitizer und
UndefinedBehaviorSanitizer sowie Bäume mit 5000 Ebenen. Sieben visuelle
Referenzvergleiche prüfen Eingabefelder, Bruchdarstellung, Exporte und Formdialog.
Die Plattform-Builds prüfen außerdem die gepackten Anwendungen; unter macOS
wird das Öffnen von Dokumenten über den Finder getestet.

## Downloads

- **Linux (Intel/AMD 64-Bit):** `Arboretum-Linux-x64.AppImage`.
  Gebaut auf Ubuntu 24.04 (glibc 2.39). Datei ausführbar machen und starten.
  Ohne FUSE ist der Start mit `APPIMAGE_EXTRACT_AND_RUN=1` möglich.
- **Windows (Intel/AMD 64-Bit):** `Arboretum-Windows-x64.zip`.
  Vollständig entpacken und `Arboretum.bat` starten.
- **macOS (Intel):** `Arboretum-macOS-x64.zip`.
  Enthält eine ad-hoc-signierte, nicht von Apple notarisierte App.
  Es ist kein nativer Apple-Silicon-Build enthalten.
- **Prüfsummen:** `SHA256SUMS.txt` enthält SHA-256-Prüfsummen der drei Pakete.

Alle drei Pakete werden aus dem Tag `v3.2.0` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt Version und vollständigen Quell-Commit. GitHub stellt
außerdem den Quelltext des Tags als ZIP und TAR.GZ bereit.
