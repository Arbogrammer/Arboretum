# Arboretum 4.0.1

Dieses Fehlerkorrektur-Release behebt unter Windows die Meldung
„Speichern fehlgeschlagen: Permission denied“ beim Erzeugen von Urnenmodellen,
Binomialmodellen und Pfadvorlagen.

## Korrekturen

- Temporäre Modelldateien werden vor dem Speichern geschlossen. Dadurch
  blockiert Windows das anschließende Ersetzen der Datei nicht mehr.
- Binomialmodell und Pfadvorlage behandeln Datei- und Speicherfehler korrekt,
  statt mit einer fehlgeschlagenen Vorbereitung fortzufahren.
- Die Korrektur gilt für alle Plattformen. Dateiformat und Bedienung bleiben
  gegenüber 4.0.0 unverändert.

## Prüfungen

Der Fix wurde mit einem Windows-Testpaket manuell bestätigt. Die
Release-Workflows prüfen zusätzlich die neu gebauten Pakete unter Windows,
Linux und macOS; unter Linux laufen auch die Regressionstests und visuellen
Referenzvergleiche.

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

Alle drei Pakete werden aus dem Tag `v4.0.1` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt Version und vollständigen Quell-Commit. GitHub stellt
außerdem den Quelltext des Tags als ZIP und TAR.GZ bereit.
