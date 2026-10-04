# Arboretum 3.3.0

Arboretum 3.3.0 ergänzt drei Exportformate für die Weiterverarbeitung von
Baumdiagrammen in LaTeX, LibreOffice Writer und Word.

## Neue Exportformate

Unter **Datei → Exportieren** bestimmt die Dateiendung das gewünschte Format:

- **LaTeX/TikZ (`.tex`):** Eigenständiges Dokument für LuaLaTeX oder XeLaTeX.
  Texte bleiben bearbeitbar; Brüche und überstrichene Gegenereignisse werden
  als LaTeX-Ausdrücke ausgegeben. Benötigt werden `standalone`, TikZ,
  `fontspec` und `amsmath`.
- **Writer (`.odt`):** Gruppierte native Zeichenobjekte mit bearbeitbaren
  Linien, Texten, Rahmen und Bruchstrichen.
- **Word (`.docx`):** Gruppierte bearbeitbare Linien und Textfelder als
  klassische Word-Zeichenobjekte (VML im OOXML-Transitional-Format),
  einschließlich gedrehter Beschriftungen und Brüche.

Die Exporte übernehmen beide Baumrichtungen, Beschriftungspositionen,
Farben und sichtbare Ergebnisspalten. ODT und DOCX benötigen zum Export
weder eine Office-Installation noch ein externes ZIP-Programm. Sehr große
Office-Diagramme werden proportional auf höchstens 55 cm Seitenlänge verkleinert.
Änderungen im Office-Dokument berechnen keine Wahrscheinlichkeiten neu.

## Hinweise und Kompatibilität

- Zum Bearbeiten einzelner Office-Objekte die Gruppe betreten oder aufheben.
- Schriftmaße können zwischen Arboretum, LaTeX und Office abweichen.
  Der LaTeX-Export verwendet DejaVu Sans, ersatzweise Latin Modern Sans;
  DOCX-Schriftgrößen werden auf halbe Punkte gerundet.
- Öffnen, Speichern und Darstellung der Office-Exporte wurden mit LibreOffice
  geprüft. Ein direkter Microsoft-Word-Test steht noch aus.
- Das Baumdateiformat bleibt gegenüber Version 3.2.0 unverändert.

## Prüfungen

Die Regressionstests prüfen zusätzlich die neuen Exporte, darunter beide
Baumrichtungen, Brüche, Überstriche, Sonderzeichen und ausgeblendete
Beschriftungen. ODT und DOCX werden auf ZIP- und XML-Struktur sowie native
Zeichenobjekte geprüft; die DOCX-Geometrie wird mit der ODT-Ausgabe verglichen.
Die bisherigen Funktions-, Parser- und visuellen Tests bleiben enthalten.
Die Plattform-Workflows prüfen die fertig gepackten Anwendungen.

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

Alle drei Pakete werden aus dem Tag `v3.3.0` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt Version und vollständigen Quell-Commit. GitHub stellt
außerdem den Quelltext des Tags als ZIP und TAR.GZ bereit.
