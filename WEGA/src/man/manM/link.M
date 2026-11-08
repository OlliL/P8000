.TH LINK M WEGA
.\" @[$]link.M	1.2
.ad
.fi
.SH NAME
link, unlink \- Systemrufe link und unlink ausfuehren
.SH SYNTAX
\fBlink \fIfile1 file2\fR
.br
\fBunlink \fIfile\fR
.SH BESCHREIBUNG
.B Link
und
.B unlink
fuehren die entsprechenden Systemrufe ohne irgendwelche Fehlerueberpruefungen
aus. Die Kommandos kann nur der Superuser anwenden.
.SH DATEIEN
/etc/link		dieses Programm
.br
/etc/unlink	dieses Programm
.SH SIEHE AUCH
rm(1),
link(2),
unlink(2).
.SH EINSCHRAENKUNGEN
Das Programm kann die Dateisystemgrenzen nicht ueberschreiten.
