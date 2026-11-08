.TH UPKEEP M WEGA
.\" @[$]upkeep.M	1.2
.ad
.fi
.SH NAME
upkeep \- Verwaltung der Directorys
.SH SYNTAX
.B upkeep
[\fB-mdfsli\fR]
.I [directory]
.SH BESCHREIBUNG
.B Upkeep
verwaltet eine Datei .contents und das korrespondierende Directory.
Die Datei .contents wird als Master-Datei benutzt. Sie enthaelt
Informationen, welche Dateien (mit welchen Zugriffsrechten,
Benutzer-Id, Gruppen-Id)
in dem Directory vorhanden sein sollten. Die Flags haben folgende
Bedeutung:
.TP
.BR -i
Initialisiere die Datei .contents mit dem angegebenem Directory.
.TP
.BR -l
Auslisten der Datei .contents.
.TP
.BR -d
Differenzen zwischen der Datei .contents und dem Directory
werden angezeigt.
.TP
.BR -f
"Schnelle" Aktualisierung des Directorys, aendert das Directory
entsprechend der Datei .contents und erstellt einen Bericht
der Aenderungen.
.TP
.BR -s
"Langsame" Aktualisierung des Directorys, Aenderungen erfolgen
analog der "schnellen" Aktualisierung. Jede Aenderung wird jedoch
auf dem Terminal angezeigt und muss vom Benutzer bestaetigt
werden.
.TP
.BR -m
Modifikation der Datei .contents und des Directorys. In diesem
Modus akzeptiert
.B upkeep
den vom Benutzer angegebenen Dateinamen und fuehrt eine der
folgenden moeglichen Aktionen aus:
.RS +4
.TP
.BR add
.br
- Aufnahme einer Datei in die .contents. Die Datei muss bereits
in dem Directory vorhanden sein, bevor die Aktion ausgefuehrt
werden kann.
.TP
.BR delete
- Loeschen einer Datei aus der .contents und dem Directory.
.TP
.BR change
- Anpassen der Zugriffsrechte, uid und/oder gid des Dateieintrages in
der .contents an die Werte in dem Directory.
.s1
.TP
.BR correct
- Korrektur der Zugriffsrechte, uid und/oder gid der Datei in dem Directory
nach den Angaben in der Datei .contents.
.RE
.PP
Die Antwort ctl-D auf eine interaktive Anfrage des Programms,
beendet das Programm. Nur der Eigner und der Superuser koennen
Aenderungen der Datei .contents und des Directorys ausfuehren.
.SH DATEIEN
/etc/upkeep		dieses Programm
.br
\&.contents
.br
/tmp/upk.aXXXXX
