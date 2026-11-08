.TH FWTMP M WEGA
.\" @[$]fwtmp.M	1.2
.ad
.fi
.SH NAME
fwtmp, wtmpfix \- Manipulieren von wtmp-Datensaetzen
.SH SYNTAX
\fBfwtmp\fR [\fB\-ic\fR]
.br
\fBwtmpfix\fR [\fIfiles\fR]
.SH BESCHREIBUNG
.B Fwtmp
liest von der Standardeingabe und schreibt zur Standardausgabe
und konvertiert dabei binaere Datensaetze (records), wie sie in der Datei
.I wtmp
vorkommen, in formatierte ASCII-Datensaetze.
Die ASCII-Version ermoeglicht das Editieren fehlerhafter Datensaetze
oder das Editieren zur allgemeinen Verwaltung der Datei.
.B Wtmpfix
ueberprueft die Standardeingabe oder die angegebenen Dateien
im
.BR wtmp -Format,
korrigiert die Zeit/Datum-Kennzeichen um die Eintraege
konsistent zu machen und schreibt zur Standardausgabe.
Ein \fB\-\fR kann an Stelle von \fIfiles\fR benutzt werden,
um die Standard\%eingabe zu bezeichnen. Werden Zeit/Datum-Korrekturen
nicht ausgefuehrt, funktioniert das Programm
.B acctcon1
nicht, wenn Datensaetze mit Datumsaenderungen erkannt werden.
.PP
Jedesmal, wenn das Datum im Multi-User-Mode gesetzt wird, wird
ein Datensatzpaar mit Datumsaenderungen zur Datei
.I /usr/adm/wtmp
geschrieben. Der erste Datensatz enthaelt das alte Datum und wird
durch \fB|\fR im Namensfeld gekennzeichnet.
Der zweite Datensatz gibt das neue Datum an und wird mit
\fB{\fR im Namensfeld gekennzeichnet.
.B Wtmpfix
benutzt diese Datensaetze zur Synchronisation aller Zeitkennzeichen
in der Datei.
.SH OPTIONEN
.TP
.B \-ic
kennzeichnet, dass die Eingabe im ASCII-Format ist und die Ausgabe
im binaeren Format erfolgen soll.
.SH DATEIEN
/usr/lib/acct/fwtmp		dieses Programm
.br
/usr/lib/acct/wtmpfix	dieses Programm
.br
/usr/adm/wtmp
.br
/usr/include/utmp.h
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctcon(M),
acctmerg(M),
acctprc(M),
acctsh(M),
runacct(M).
