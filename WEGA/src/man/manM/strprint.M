.TH STRPRINT M WEGA
.\" @[$]strprint.M	1.2
.ad
.fi
.SH NAME
strprint \- Listingprogramm fuer Software-Fehlerberichte
.SH SYNTAX
\fBstrprint\fR
.SH BESCHREIBUNG
Diese Shell-Prozedur wird zur Ausgabe einer Kopie aller Fehlerberichte
auf den Drucker benutzt. Jeder Fehlerbericht wird auf eine
separate Seite gedruckt. Fuer jede P8000-Installation sollte
periodisch eine Kopie zum KEAW Berlin-Treptow
geschickt werden. Das Listing sollte auch allen Benutzern des
Systems verfuegbar sein, damit sie Kenntnis ueber alle Software-Fehlerberichte
erhalten.
.SH DATEIEN
/usr/bin/strprint
.br
	dieses Programm
.sp
/etc/strfile
.br
	Datenbasis fuer das Verfolgen der Fehlerberichte
.sp
/etc/seqno
.br
	enthaelt die zuletzt benutzte laufende Nummer
.sp
/etc/problems
.br
	Directory fuer die Fehlerberichte
.sp
/etc/scrb_date
.br
	enthaelt das Datum fuer das naechste Treffen zur
.br
	Fehlerauswertung
.SH SIEHE AUCH
strfile(5),
str(M).
