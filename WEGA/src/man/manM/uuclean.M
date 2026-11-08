.TH UUCLEAN M WEGA
.\" @[$]uuclean.M	1.2
.ad
.SH NAME
uuclean \- uucp-Spooler-Directory saeubern
.SH SYNTAX
.B uuclean
.I [options] ...
.SH BESCHREIBUNG
.B Uuclean
ueberprueft das Spooler-Directory auf Dateien mit dem angegebenen
Praefix und loescht alle Dateien, die aelter als die angegebene
Anzahl von Stunden sind.
.SH OPTIONEN
.TP 3 
.BI \-d directory
Saeubert das Directory
.I directory
anstelle des Spooler-Directorys.
.TP 
.B \-m
Sendet eine Nachricht an die Eigner der Dateien, die geloescht
werden.
.TP
.BI \-n time
Dateien, die aelter als
.I time
Stunden sind werden geloescht, wenn sie den entsprechenden
Praefix besitzen (die Standardeinstellung betraegt 72 Stunden).
.TP
.BI \-p pre
Ueberprueft Dateien mit dem als
.I pre
angegebenen Praefix.
Bis zu 10 Argumente
.B \-p
koennen angegeben sein.
Ein
.B \-p
ohne
.I pre
loescht alle Dateien, die aelter als die angegebene Zeit sind.
.TP
.BI \-x num
Angabe der gewuenschten Debugging-Ebene.
.PP
Dieses Programm wird typisch von \fBcron\fR(M) gestartet.
.SH DATEIEN
/usr/lib/uucp/uuclean   dieses Programm
.br
/usr/lib/uucp/*         von \fBuuclean\fR intern benutzte Kommandos
.br
/usr/spool/uucp         Spooler-Directory
.SH SIEHE AUCH
uucp(1), uux(1).
