# Arboretum 4.0.2

Dieses Fehlerkorrektur-Release verbessert Rückgängig und Wiederherstellen
beim Erzeugen von Modellen und das Laden bei fixierten Beschriftungen.

## Korrekturen

- Urnenmodelle, Binomialmodelle und Pfadvorlagen lassen sich mit genau einem
  Schritt vollständig rückgängig machen und wiederherstellen. Beschriftungen,
  Wahrscheinlichkeiten und berechnete Ergebnisse werden dabei wiederhergestellt.
- Beim Laden mit fixierten Beschriftungen greift die Layoutberechnung nicht mehr
  auf bereits entfernte Beschriftungen des vorherigen Baums zu.

## Prüfungen

Neue Regressionstests prüfen Rückgängig und Wiederherstellen für alle drei
Urnenmodi sowie Binomialmodelle und Pfadvorlagen, jeweils mit bearbeitbaren und
fixierten Beschriftungen. Die Release-Workflows prüfen die neu gebauten Pakete
unter Windows, Linux und macOS; unter Linux laufen zusätzlich Regressionstests
und visuelle Referenzvergleiche.

## Downloads

- **Windows (Intel/AMD 64-Bit):** `Arboretum-Windows-x64.zip`.
  Vollständig entpacken und `Arboretum.bat` starten.
- **Linux (Intel/AMD 64-Bit):** `Arboretum-Linux-x64.AppImage`.
  Datei ausführbar machen und starten. Ohne FUSE ist der Start mit
  `APPIMAGE_EXTRACT_AND_RUN=1` möglich.
- **macOS (Intel):** `Arboretum-macOS-x64.zip`.
  Enthält eine ad-hoc-signierte, nicht von Apple notarisierte App.
  Es ist kein nativer Apple-Silicon-Build enthalten.
- **Prüfsummen:** `SHA256SUMS.txt` enthält SHA-256-Prüfsummen der drei Pakete.

Alle drei Pakete werden aus dem Tag `v4.0.2` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt Version und vollständigen Quell-Commit. GitHub stellt
außerdem den Quelltext des Tags als ZIP und TAR.GZ bereit.
