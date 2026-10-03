# Arboretum 3.1.0

Arboretum 3.1.0 verbessert die Lesbarkeit von Wahrscheinlichkeitsbäumen mit
einer wählbaren Beschriftungsseite und einer optionalen automatischen
Kollisionsvermeidung.

## Neue Funktionen

- Unter **Darstellung → Form** lassen sich Wahrscheinlichkeiten oberhalb,
  unterhalb oder nach oberer/unterer Hälfte der ausgehenden Zweige platzieren.
  Standard ist die gemischte Platzierung. Bei einer ungeraden Zahl von Zweigen
  ist die Seite des mittleren Zweigs separat einstellbar; standardmäßig oben.
- Die optionale Automatik verschiebt einzelne Wahrscheinlichkeiten zunächst
  geringfügig nach rechts. Reicht das nicht, erweitert sie die betroffenen
  Astabstände. Die ausgewählte Zweigseite bleibt erhalten. Auch Überlappungen
  zwischen Wahrscheinlichkeitsbeschriftungen werden berücksichtigt.
- Bei vertikalen Bäumen gelten die Seitenoptionen entsprechend links/rechts.
  Die Automatik verschiebt Beschriftungen nach unten und erweitert bei Bedarf
  die horizontalen Astabstände.
- Die automatisch berechneten Abstände gelten nur für die fixierte Ansicht.
  Beim Ausschalten der Automatik gelten wieder die manuell eingestellten Werte.
- Anzeige und Export verwenden dieselbe Geometrie, auch bei gestapelten
  Brüchen. Die Optionen werden in der Baumdatei und im Undo gespeichert.

## Weitere Verbesserungen

- Knotenbreiten werden je Baumstufe an den Inhalt angepasst.
- Die Standardschrift wird auch auf die Eingabefelder angewendet;
  Schriftgrößen werden einheitlich in Punkt angegeben.
- Die fixierte Ansicht wird beim Laden und Rückgängigmachen korrekt neu
  aufgebaut. Bei verbleibenden Layoutkollisionen zeigt die Automatik einen
  Hinweis an.

## Dateiformat

Ältere Baumdateien bleiben lesbar. Neu gespeicherte Dateien enthalten
zusätzliche Darstellungseinstellungen und benötigen zum Öffnen Version 3.1.0
oder eine spätere kompatible Version.

## Prüfungen

Die Layouttests decken beide Baumrichtungen, alle Seitenoptionen, die
Mittelauswahl, Bruchdarstellungen sowie Speichern und Undo ab. Hinzu kommen
die vorhandenen Tests für Tastaturbedienung, Dateiparser und Exportformate.

## Downloads

- `Arboretum-Linux-x64.AppImage`: Linux für Intel/AMD 64-Bit, gebaut auf
  Ubuntu 24.04 (glibc 2.39). Datei ausführbar machen und starten. Ohne FUSE
  ist der Start mit `APPIMAGE_EXTRACT_AND_RUN=1` möglich.
- `Arboretum-Windows-x64.zip`: Windows für Intel/AMD 64-Bit. Vollständig
  entpacken und `Arboretum.bat` starten.
- `Arboretum-macOS-x64.zip`: macOS für Intel. Enthält eine ad-hoc-signierte,
  nicht von Apple notarisierte App; kein nativer Apple-Silicon-Build.

Alle drei Pakete werden aus dem Tag `v3.1.0` gebaut. Die enthaltene
`BUILD-INFO.txt` nennt den vollständigen Quell-Commit. GitHub stellt außerdem
den Quelltext des Tags als ZIP und TAR.GZ bereit.
