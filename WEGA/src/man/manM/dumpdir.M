.TH DUMPDIR M WEGA
.\" @[$]dumpdir.M	1.2
.ad
.fi
.SH NAME
dumpdir \- Auflistung eines Dump-Abzuges
.SH SYNTAX
.B dumpdir
[\fBf\fI filename\fR]
.SH BESCHREIBUNG
.B Dumpdir
erstellt eine Liste der Namen und Inode-Nummern aller Dateien
und Directorys eines Dump-Abzuges.
.br
Das Standard-E/A-Geraet ist \fI/dev/dumpdev\fR.
Existiert
.I /dev/dumpdev
nicht, wird
.I /dev/rfd1
benutzt. Die Option
.B f
bewirkt das Einlesen ueber das Archivmedium
.IR filename .
.SH DATEIEN
/etc/dumpdir	dieses Programm
.SH SIEHE AUCH
dump(M),
restor(M),
dump(5).
.SH EINSCHRAENKUNGEN
Der Dump enthaelt redundante Informationen, die benutzt werden
koennten, wenn Leseprobleme auftreten.
.B Dumpdir
nutzt diese Informationen nicht.
