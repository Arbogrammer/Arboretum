# Arboretum 2.1.1

Arboretum 2.1.1 behebt zwei Fehler in der Bedienung. Quelltext und alle drei
Programmpakete werden aus demselben Tag `v2.1.1` gebaut. Die Datei
`BUILD-INFO.txt` in jedem Paket nennt den vollständigen Commit.

## Änderungen

- „Neu“ startet eine weitere Arboretum-Instanz ohne den UI-Thread der
  bestehenden Instanz zu blockieren.
- Die Easter-Egg-Nachrichten sind dem Arboretum-Hauptfenster zugeordnet und
  werden als modale Dialoge darüber angezeigt.

## Downloads

- `Arboretum-2.1.1-Linux-x64.AppImage`: Linux, 64-Bit Intel/AMD. Gebaut auf
  Ubuntu 24.04 (glibc 2.39); eine entsprechend kompatible Distribution ist nötig.
  Datei ausführbar machen und starten. Falls FUSE fehlt, ist auch
  `APPIMAGE_EXTRACT_AND_RUN=1 ./Arboretum-2.1.1-Linux-x64.AppImage` möglich.
- `Arboretum-2.1.1-Windows-x64.zip`: Windows, 64-Bit Intel/AMD. Vollständig
  entpacken und `Arboretum.bat` starten. `Diagnose.bat` und `Dateitest.bat`
  helfen bei Problemen.
- `Arboretum-2.1.1-macOS-x64.zip`: macOS, Intel. Enthält eine ad-hoc-signierte,
  nicht von Apple notarisierte App. Dies ist kein nativer Apple-Silicon-Build.
- `Source code`: Quelltext exakt des Release-Tags, automatisch von GitHub.

Windows verwendet vorläufig die einfache GTK-Eingabemethode. Native
IME-Komposition (z. B. für ostasiatische Eingabesysteme) ist damit eingeschränkt.
Automatische Tests ersetzen keine vollständigen Tests auf allen Zielrechnern.
