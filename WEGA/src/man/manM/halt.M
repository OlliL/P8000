.TH HALT M WEGA
.\" @[$]halt.M	1.2
.ad
.fi
.SH NAME
halt \- schnelles Abfahren des Systems
.SH SYNTAX
.B halt
.SH BESCHREIBUNG
.B Halt
ist ein Shell-Skript zum schnellen Abfahren des \%Systems. 30
Sekunden vor dem Wechsel in den Single-User-Mode wird an alle
angemeldeten Benutzer eine Warnung ausgegeben. Nach Ablauf
der Zeit, werden die Puffer rueckgeschrieben und ein
.B kill -1 1
ausgefuehrt. Danach ersetzt
.B halt
die Datei
.I /etc/motd
durch die Datei
.I /etc/motd.save,
falls diese \%existiert.
.LP
Das Kommando funktioniert nur, wenn es durch den Superuser
benutzt wird.
.SH DATEIEN
/etc/halt		dieses Programm
.br
/etc/motd		message of the day
.br
/etc/motd.save	urspruegliche motd
.SH SIEHE AUCH
down(M),
wall(M),
kill(1).
