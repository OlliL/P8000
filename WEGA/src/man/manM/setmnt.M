.TH SETMNT M WEGA
.\" @[$]setmnt.M	1.2
.ad
.fi
.SH NAME
setmnt \- Tabelle mnttab einrichten
.SH SYNTAX
.B setmnt
.SH BESCHREIBUNG
.B Setmnt
erzeugt die Tabelle
.I /etc/mnttab
(siehe
.BR mnttab (5)),
die fuer die beiden Kommandos
.BR mount (M)
und
.BR umount (M)
erforderlich ist.
.B Setmnt
liest die Standardeingabe und legt fuer jede Eingabezeile 
einen Eintrag in der Datei
.B mnttab
an.
Eingabezeilen besitzen folgendes Format:
.PP
.RS
.I filesys node
.RE
.PP
wobei
.I filesys
der Name des Dateisystems
.I special file
(z.B.
.IR /dev/rm?? )
und
.I node
der root-Name dieses Dateisystems sind.
.I Filesys
und
.I node
sind die ersten beiden Zeichenketten im
.BR mnttab (5)-Eintrag.
.SH DATEIEN
/etc/setmnt	dieses Programm
.br
/etc/mnttab	mount-Tabelle
.SH SIEHE AUCH
mnttab(5).
.SH EINSCHRAENKUNGEN
Die Namen fuer
.I filesys
oder
.I node
duerfen nicht laenger als 32 Zeichen sein.
.br
.B Setmnt
ueberwacht die maximale Anzahl der Eintraege. Die Anzahl ist vom
Parameter
.B Nmount
im WEGA-Kern abhaengig.
