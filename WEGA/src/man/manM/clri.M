.TH CLRI M WEGA
.\"@[$]clri.M	1.2
.ad
.fi
.SH NAME
clri \- Loeschen eines Inodes
.SH SYNTAX
.B clri
.I filesystem i-number 
.SH BESCHREIBUNG
.B Clri
schreibt Nullen auf das Inode mit der dezimalen Nummer
.I i-number
im Dateisystem
.I filesystem.
Nach
.B clri
erscheint jeder Block der betroffenen Datei bei einem
.BR icheck (M)
im Dateisystem
als vermisst (missing).
.PP
Lese- und Schreibrechte sind fuer das angegebene Dateisystem
erforderlich. Das Inode wird wieder frei.
.PP
Der Hauptzweck dieses Programms ist das Loeschen einer Datei,
die aus irgendwelchen Gruenden in keinem Directory enthalten
ist. Wird es auf ein Inode angewandt, das in einem Directory
enthalten ist, muss dieser Directory-Eintrag entfernt werden.
Anderenfalls zeigt der alte Directory-Eintrag bei Neuvergabe
des Inodes auf eine neue Datei. Ein Loeschversuch fuer die
alte Datei fuehrt dann zum Loeschen der neuen Datei. Der Directory-Eintrag
der neuen Datei weist aber wiederum auf ein unbelegtes Inode.
Damit kann sich der beschriebene Effekt staendig wiederholen.
.SH DATEIEN
/etc/clri	dieses Programm
.SH SIEHE AUCH
icheck(M).
.SH EINSCHRAENKUNGEN
.B Clri
sollte nicht fuer eroeffnete Dateien ausgefuert werden.
