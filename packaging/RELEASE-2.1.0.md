# Arboretum 2.1.0

Arboretum 2.1.0 verbessert die Darstellung überstrichener Zeichen und macht
die Einstellmöglichkeiten leichter verständlich.
Quelltext und alle drei Programmpakete werden aus demselben Tag `v2.1.0`
gebaut. Die Datei `BUILD-INFO.txt` in jedem Paket nennt den vollständigen Commit.

## Änderungen

- Überstrichene Zeichen werden mit der nativen GTK-4/Pango-Überstreichung
  dargestellt. Die bisher gespeicherte Unicode-Kodierung mit U+0305 bleibt
  dabei vollständig kompatibel.
- Überstriche erscheinen beim Bearbeiten, nach dem Laden und in fixierten
  Knoten- und Ergebnistexten konsistent.
- Der Ein-/Ausgabe-Smoke-Test prüft die native Überstreichung zusätzlich zum
  Speichern, Laden und Export.
- Werkzeugleiste und Formdialog enthalten erklärende Tooltips für die
  wichtigsten Aktionen und Werte.

## Downloads

- `Arboretum-2.1.0-Linux-x64.AppImage`: Linux, 64-Bit Intel/AMD. Gebaut auf
  Ubuntu 24.04 (glibc 2.39); eine entsprechend kompatible Distribution ist nötig.
  Datei ausführbar machen und starten. Falls FUSE fehlt, ist auch
  `APPIMAGE_EXTRACT_AND_RUN=1 ./Arboretum-2.1.0-Linux-x64.AppImage` möglich.
- `Arboretum-2.1.0-Windows-x64.zip`: Windows, 64-Bit Intel/AMD. Vollständig
  entpacken und `Arboretum.bat` starten. `Diagnose.bat` und `Dateitest.bat`
  helfen bei Problemen.
- `Arboretum-2.1.0-macOS-x64.zip`: macOS, Intel. Enthält eine ad-hoc-signierte,
  nicht von Apple notarisierte App. Dies ist kein nativer Apple-Silicon-Build.
- `Source code`: Quelltext exakt des Release-Tags, automatisch von GitHub.

Windows verwendet vorläufig die einfache GTK-Eingabemethode. Native
IME-Komposition (z. B. für ostasiatische Eingabesysteme) ist damit eingeschränkt.
Automatische Tests ersetzen keine vollständigen Tests auf allen Zielrechnern.
