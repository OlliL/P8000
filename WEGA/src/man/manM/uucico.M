.TH UUCP M WEGA
.\" @[$]uucico.M	1.2
.ad
.fi
.SH NAME
uucico \- uucp-Auftraege ausfuehren
.SH SYNTAX
.B uucico
.I [options] ...
.SH BESCHREIBUNG
.B Uucico
fuehrt die Uebertragung von mit
.B uucp
oder
.B uux
erzeugten C-, D- und X-Dateien an den entfernten Rechner durch
und wird im allgemeinen durch die Programme
.BR uucp ,
.B uux
oder
.B uuxqt
automatisch gestartet. Vom Benutzer kann es jedoch auch direkt
gestartet werden, um ein nachtraegliches Abarbeiten von in
den Spool-Directorys der lokalen oder entfernten Rechner verbliebenen
Auftraegen durchzufuehren. Darueber hinaus dient es fuer Testzwecke.
.B Uucico
bewahrt die Ausfuehrungserlaubnis waehrend einer Dateiuebertragung
und gibt die Zugriffsrechte 666 (siehe
.BR chmod (2)).
.SH OPTIONEN
.TP 6
.B \-r1
Start des Programms im Master-Modus.
.br
Diese Option muss bei Aufruf durch den Benutzer immer angegeben
werden.
.TP
.BI \-s sys
Pruefen des Spool-Directorys auf Auftraege nur bezueglich des
Systems
.I sys
und Durchfuehrung der Uebertragung eventueller Auftraege. Ein
Ruf an das spezifizierte System wird sogar aufgesetzt, wenn
kein Auftrag fuer diesen Rechner vorliegt.
.TP
.BI \-d dir
Nutzung des Directory
.I dir
als Spool-Directory.
.TP
.BI \-x num
Angabe der gewuenschten Debugging-Ebene.
.SH DATEIEN
/usr/lib/uucp/uucico	dieses Programm
.br
/usr/spool/uucp		Spool-Directory
.br
/usr/lib/uucp/\(**		andere Daten- und Programmdateien
.SH SIEHE AUCH
uucp(1),
uux(1).
.SH ACHTUNG
Die Option
.B \-r1
(Start im Master-Modus) muss bei Aufruf des Programms
.B uucico
durch einen Benutzer stets angegeben werden.
.br
Eine Abarbeitung von
.B uucico
kann nicht erfolgen, wenn von vorausgegangenen fehlerhaften
Uebertragungen noch STST- bzw. LCK-Dateien im Spool-Directory
enthalten sind. In diesem Fall muessen die Dateien geloescht
werden.
.SH EINSCHRAENKUNGEN
Bei der Option
.B \-d
kann nur ein Directory angegeben werden, das sich auf dem gleichen
physischen Geraet befindet wie
.IR /usr/spool/uucp .
