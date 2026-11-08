.TH SETLP M WEGA
.\" @[$]setlp.M	1.2
.ad
.fi
.SH NAME
setlp \- Druckparameter setzen und anzeigen
.SH SYNTAX
.B setlp
[\fB \-l\fI number\fR
\fB\-w\fI number
\fB\-s\fI number
\fB\-d\fI devname\fR]
.SH BESCHREIBUNG
.B Setlp
gestattet das Anzeigen und Setzen der
Druckparameter: Anzahl der Zeilen pro Seite, Anzahl
der Zeichen pro Zeile und linker Rand (Anzahl der Leerzeichen
vor der ersten Zeichenposition).
Sind keine Parameter angegeben, zeigt
.B setlp
die aktuelle Einstellung an.
.LP
Die Optionen sind:
.TP 
.B \-l \fInumber\fR
Die Anzahl der Zeilen wird auf den angegebenen dezimalen Wert
gesetzt.
.TP 
.B \-w \fInumber\fR
Die Anzahl der Zeichen (Spalten) wird auf den angegebenen dezimalen
Wert gesetzt.
.br
.TP 
.B \-s \fInumber\fR
Die Anfangsspalte (nummeriert ab 0) wird auf den angegebenen
dezimalen Wert gesetzt.
.TP 
.B \-d \fIdevname\fR
Name der Geraetedatei, fehlt diese Option, wird
.I /dev/lp
benutzt.
.SH DATEIEN
/etc/setlp	dieses Programm
.SH SIEHE AUCH
ioctl(2),
stty(1).
