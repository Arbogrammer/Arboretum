# BDG-Testdateien

Dieser Ordner enthält bewusst unterschiedliche Eingaben für Parser-, GUI- und
Robustheitstests. Dateien mit `gueltig_` müssen sich öffnen lassen; Dateien mit
`ungueltig_` müssen kontrolliert abgelehnt werden, ohne dass Arboretum abstürzt
oder den bereits geöffneten Baum verändert.

Die Fälle decken Minimaldateien, Unicode, Verzweigungen, einen tiefen Baum,
Feld- und Positionsgrenzen sowie beschädigte Trennzeichen, Nullbytes,
ungültiges UTF-8, abgeschnittene Daten und fehlerhafte Baumstrukturen ab.

Die binären Steuerzeichen des BDG-Formats sind in einem Texteditor nicht gut
sichtbar. Die Dateien lassen sich reproduzierbar neu erzeugen mit:

```sh
make bdg-testdata
```
