.TH TTYCONFIG M WEGA
.\" @[$]ttyconfig.M	1.2
.ad
.fi
.SH NAME
ttyconfig \- Konfigurieren der TTY-Kanaele fuer Terminal oder Modem
.SH SYNTAX
.B ttyconfig
[\fB-m\fR
.I ports]
[\fB-t\fR
.I ports]
.SH BESCHREIBUNG
.B Ttyconfig
wird vom Systemverwalter zur Konfiguration der TTY-Kanaele
fuer Terminal- oder Modembetrieb benutzt. Die typische
Anwendung von
.B ttyconfig
ist der Aufruf durch den Startskript
.I /etc/rc_csh.
.PP
.B Ttyconfig
ohne Argumente gibt die Einstellung der Kanaele aus. Die Optionen
haben folgende Bedeutung:
.TP 4
.B -m
Konfigurieren der angegebenen Kanaele fuer Modembetrieb.
.TP 4
.B -t
Konfigurieren der angegebenen Kanaele fuer Terminalbetrieb.
.PP
Die Angabe der Kanalnummern erfolgt als ein Bereich
von Kanaelen oder eine mit Komma getrennte Liste von Kanaelen.
Zum Beispiel werden die Kanaele 1 bis 3 und 5-7 fuer den Terminalbetrieb
eingestellt:
.sp
.in +6
.B ttyconfig
.B -t
1-3,5-7,
.in -6
.sp
Die Kanaele 0 und 4 werden fuer Modembetrieb eingestellt:
.sp
.in +6
.B ttyconfig
.B -m
0,4,
.in -6
.PP
Der Grundzustand fuer alle Kanaele beim Systemstart ist
Terminalmode.
.SH DATEIEN
/etc/ttyconfig		dieses Programm
.SH SIEHE AUCH
mdmctl(2).
.SH DIAGNOSTIK
Eine Ueberpruefung erfolgt auf ungueltige Optionen und ungueltige
Kanaele. Die Konfiguration der Kanaele muss lesbar und setzbar
sein.
