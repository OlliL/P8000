.TH NCHECK M WEGA
.\" @[$]ncheck.M	1.2
.ad
.fi
.SH NAME
ncheck  \- Dateinamen aus Inode-Nummer ermitteln
.SH SYNTAX
.B ncheck
[\fB\-i\fR
.I numbers]
[\fB\-as\fR]
.I [filesystem]
.SH BESCHREIBUNG
.B Ncheck
ohne Optionen gibt eine Liste aller Dateien des Dateisystems
.I filesystem
aus, in der die Pfadnamen den Inode-Nummern zugeordnet sind.
Directorys sind durch den Anhang
`/\fB.\fR'
gekennzeichnet. Die Option
.B \-i
begrenzt die Liste auf die Dateien, deren Inode-Nummern folgen.
Die Option
.B \-a
bewirkt, dass auch die Namen
.RB ` . '
und
.RB ` .. ',
die normalerweise unterdrueckt werden, in der Liste erscheinen.
Die Option
.B \-s
beschraenkt die Liste auf Geraetedateien und Dateien mit
"set-user-Id"-Zugriffsrecht. Sie dient dazu, verborgene Verletzungen
der Sicherheitsregeln zu ermitteln.
.PP
Die Liste erscheint ungeordnet. Fuer eine sinnvolle Nutzung
sollte sie sortiert werden.
.SH DATEIEN
/etc/ncheck	dieses Programm
.SH SIEHE AUCH
sort(1),
dcheck(M),
icheck(M).
.SH DIAGNOSTIK
Ist die Struktur des Dateisystems nicht in Ordnung, koennen
folgende Ausgaben erfolgen:
.sp
`\fI??\fR' bedeutet, dass eine Datei kein Directory
besitzt.
.sp
`\fI...\fR' am Anfang eines Pfadnamens weist auf eine Schleife hin.
