.TH STR M WEGA
.\" @[$]str.M	1.2
.ad
.fi
.SH NAME
str \- Eingabeprogramm fuer Software-Fehlerberichte
.SH SYNTAX
\fBstr\fR [\fB-l\fR]
.SH BESCHREIBUNG
Dieses Programm wird fuer die Registrierung von Software-Fehlern
benutzt. Das Programm fragt den Benutzer nach allen benoetigten
Informationen. Der Benutzer hat auch die Moeglichkeit, den
Editor
.BR vi (1)
aufzurufen. Der Benutzer kann den Editor zu Beginn des
Fehlerbeschreibungsfeldes oder kurz vor dem Einordnen des
Berichts aufrufen.
.PP
Jedem  eingeordneten Fehlerbericht wird eine laufende Nummer zugeordnet
und es erfolgt eine Eintragung in die "master tracking"-Datei.
.SH OPTIONEN
.TP
.B \-l
erzeugt nach Abschluss des Fehlerberichtes ein Druckerlisting.
.SH DATEIEN
/usr/bin/str
.br
	dieses Programm
.sp
/usr/lib/problems/strfile 
.br
	Datenbasis zum Verfolgen der Fehlerberichte
.br
	("master tracking"-Datei)
.sp
/usr/lib/problems/seqno	
.br
	enthaelt letzte benutzte laufende Nummer
.sp
/usr/lib/problems	
.br
	Directory fuer die Fehlerberichtsdateien
.sp
/usr/lib/problems/scrb_date	
.br
	enthaelt Datum des naechsten Treffens zur Fehlerauswertung
.SH SIEHE AUCH
strfile(5),
strprint(M).
