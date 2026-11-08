.TH DOWN M WEGA
.\" @[$]down.M	1.2
.ad
.fi
.SH NAME
down \- Abfahren des Systems
.SH SYNTAX
.B down
.I [dtime btime]
.SH BESCHREIBUNG
.B Down
ist ein Shell-Skript zum ordnungsgemaessen Abfahren des Systems.
.LP
Die Argumente
.I dtime
und
.I btime,
koennen zur Angabe der Zeit in Minuten bis zum Abfahren des
Systems sowie zur Angabe der Zeit zwischen der Aussendung von
zwei Warnungen an die Benutzer, angegeben werden. Fehlen die
Argumente, erfolgt eine entsprechende Anfrage durch das Programm.
.LP
Im angegebenen Zeitabstand wird an alle angemeldeten Benutzer
eine Nachricht gesendet, die mitteilt, in wieviel Minuten das
System abgefahren wird. Nach Ablauf der Zeit werden alle Puffer
rueckgeschrieben und ein
.B kill -1 1
ausgefuehrt.
Dieses wird durch 
.BR halt (M)
vollzogen.
.LP
In die motd-Datei (message-of-the-day) wird eine Zeile aufgenommen,
die mitteilt, dass das System abgefahren wird. Die Originaldatei
wird zuvor gerettet und nach dem Abfahren rueckgeschrieben.
.SH RESTRIKTIONEN
Das Kommando kann nur vom Superuser ausgefuehrt werden.
Die minimale Wartezeit bis zum Abfahren betraegt 5 Minuten.
Bei Angabe einer geringeren Zeit, wird diese auf 5 Minuten
erhoeht. Soll das System sofort ordnungsgemaess abgefahren
werden, kann
.BR halt (M)
benutzt werden.
.SH DATEIEN
/etc/down		dieses Programm
.br
/etc/motd		message of the day
.br
/etc/motd.save	gerettete motd
.SH SIEHE AUCH
kill(1),
halt(M).
