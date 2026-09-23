# Arboretum 2.0.2

Dieses Release bündelt die Stabilitätskorrekturen nach Version 2.0.1.
Quelltext und alle drei Programmpakete werden aus demselben Tag `v2.0.2`
gebaut. Die Datei `BUILD-INFO.txt` in jedem Paket nennt den vollständigen Commit.

## Änderungen

- Stabilere Texteingabe und Knotenbearbeitung; unnötiger Neuaufbau der
  Eingabefelder während des Tippens entfällt.
- Windows: korrigierte temporäre Dateipfade und ein Workaround für den
  beobachteten Absturz beim Löschen von Knoten.
- Windows: Beim Öffnen per Doppelklick werden auch Umlaute und andere
  Unicode-Zeichen im übergebenen Dateipfad korrekt gelesen.
- macOS: Bibliotheken mit relativen Ladepfaden werden vollständig ins
  App-Bundle übernommen und vor dem Signieren überprüft.
- macOS: Bildformat-Module einschließlich BMP werden mitgeliefert und auf
  die gebündelten Bibliotheken abgestimmt.
- Speichern und Laden unterstützen Unicode-Dateipfade; Schreibfehler werden
  angezeigt, statt einen erfolgreichen Speichervorgang vorzutäuschen.
- Überarbeiteter PNG-, JPEG-, BMP-, SVG- und PDF-Export. JPEG und BMP werden
  ohne Transparenz ausgegeben; Texte in SVG/PDF sind Vektorpfade.
- Nach dem Öffnen einer Datei sind Texte nicht mehr automatisch markiert.
- Automatische Prüfungen für Eingabe, Löschen, Speichern/Laden und Exporte
  laufen mit den fertig gepackten Anwendungen.

## Downloads

- `Arboretum-2.0.2-Linux-x64.AppImage`: Linux, 64-Bit Intel/AMD. Gebaut auf
  Ubuntu 24.04 (glibc 2.39); eine entsprechend kompatible Distribution ist nötig.
  Datei ausführbar machen und starten. Falls FUSE fehlt, ist auch
  `APPIMAGE_EXTRACT_AND_RUN=1 ./Arboretum-2.0.2-Linux-x64.AppImage` möglich.
- `Arboretum-2.0.2-Windows-x64.zip`: Windows, 64-Bit Intel/AMD. Vollständig
  entpacken und `Arboretum.bat` starten. `Diagnose.bat` und `Dateitest.bat`
  helfen bei Problemen.
- `Arboretum-2.0.2-macOS-x64.zip`: macOS, Intel. Enthält eine ad-hoc-signierte,
  nicht von Apple notarisierte App. Dies ist kein nativer Apple-Silicon-Build.
- `Source code`: Quelltext exakt des Release-Tags, automatisch von GitHub.

Windows verwendet vorläufig die einfache GTK-Eingabemethode. Native
IME-Komposition (z. B. für ostasiatische Eingabesysteme) ist damit eingeschränkt.
Automatische Tests ersetzen keine vollständigen Tests auf allen Zielrechnern.
