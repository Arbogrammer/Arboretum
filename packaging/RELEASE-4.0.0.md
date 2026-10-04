# Arboretum 4.0.0

Arboretum 4.0.0 bringt vertikale Brüche direkt in die Eingabe und ergänzt
Überschriften für Ergebnisse. Seit 3.0.0 sind außerdem eine Layoutautomatik,
Wiederherstellen und bearbeitbare LaTeX-, Writer- und Word-Exporte hinzugekommen.

## Neu in 4.0.0

- **Vertikale Brüche beim Bearbeiten:** Die Option „Bruchdarstellung vertikal“
  unter **Darstellung → Form** gilt jetzt auch für Eingabefelder. `/` stellt
  Zähler und Nenner untereinander dar und setzt den Cursor in den Nenner.
  Mit ↑/↓ wechselt man zwischen beiden Teilen; Rückschritt im leeren Nenner
  kehrt zur einzeiligen Eingabe zurück. Eingefügte Werte wie `1/2` werden erkannt.
- **Auswahl und Zwischenablage:** `Strg+A` markiert den ganzen Wert zum Ersetzen,
  Kopieren oder Ausschneiden. Der Export ist über **Datei → Exportieren** oder
  `Strg+Umschalt+A` erreichbar.
- **Passende Abstände:** Brüche erhalten in der Bearbeitungsansicht den nötigen
  Platz, Dezimalzahlen bleiben kompakt. Gespeicherte Baumpositionen bleiben
  erhalten. Beide Baumrichtungen und gemischte Bruch-/Dezimalfelder werden
  berücksichtigt; bei ausgeschalteter Option bleiben Brüche einzeilig.
- **Ergebnisüberschriften:** Unter **Darstellung → Ergebnisüberschriften** stehen
  **Keine**, **ω | P(ω)**, **ω | P({ω})**, **{ω} | P({ω})** und **Eigene …**
  zur Wahl. Eigene Texte dürfen jeweils bis zu 160 Zeichen umfassen.
  Überschriften folgen der Baumrichtung und der Sichtbarkeit der Ergebnisse,
  erscheinen in allen Exportformaten und unterstützen Speichern sowie Undo/Redo.
- Die integrierte Hilfe beschreibt die neue Bedienung.

## Seit 3.0.0 ebenfalls hinzugekommen

- Wählbare Beschriftungsseiten und automatische Kollisionsvermeidung für
  Wahrscheinlichkeiten in horizontalen und vertikalen Bäumen.
- Wiederherstellen, bis zu 100 Zustände und zusammengefasste Texteingaben.
- Gemeinsame Berechnung von Brüchen und Dezimalzahlen, Markierung ungültiger
  Eingaben, gespeicherte Baumrichtung und zusätzliche Parserabsicherung.
- Bearbeitbare Exporte als LaTeX/TikZ, ODT und DOCX mit Linien, Texten und Brüchen.
  Office-Exporte benötigen keine installierte Office-Anwendung.

## Kompatibilität

Ältere Baumdateien bleiben lesbar und öffnen ohne Ergebnisüberschriften.
Dateien mit gespeicherten Überschrifteneinstellungen benötigen **Arboretum
4.0.0 oder eine spätere kompatible Version**. Ohne solche Einstellungen bleibt
das Dateiformat von 3.2.0/3.3.0 erhalten. Die vertikale Brucheingabe ändert die
gespeicherten Wahrscheinlichkeitswerte nicht.

ODT und DOCX enthalten bearbeitbare Zeichenobjekte; Änderungen im Office-Dokument
berechnen keine Wahrscheinlichkeiten neu. Schriftmaße können zwischen Programmen
abweichen. Die Office-Exporte wurden mit LibreOffice geprüft; ein direkter
Microsoft-Word-Test steht noch aus.

## Prüfungen

Die Regressionstests umfassen Tastaturbedienung, Dateioperationen, Exporte,
Bearbeitung, Undo/Redo, Layout, Parserprüfungen mit AddressSanitizer und
UndefinedBehaviorSanitizer sowie Bäume mit 5000 Ebenen. 16 visuelle
Referenzvergleiche decken unter anderem die neue Brucheingabe, gemischte Werte,
Ergebnisüberschriften und beide Baumrichtungen ab. Die Plattform-Workflows
prüfen zusätzlich die fertig gepackten Anwendungen.

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

Alle drei Pakete werden aus dem Tag `v4.0.0` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt Version und vollständigen Quell-Commit. GitHub stellt
außerdem den Quelltext des Tags als ZIP und TAR.GZ bereit.
