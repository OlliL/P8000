.TH WALL M WEGA
.\" @[$]wall.M	1.2
.ad
.fi
.SH NAME
wall  \- Schreiben an alle Benutzer
.SH SYNTAX
.B wall
.SH BESCHREIBUNG
.B Wall
liest die Standardeingabe bis zum EOF. Es sendet dann die Nachricht
mit dem vorangestellten Text `Broadcast Message ...'
zu allen angemeldeten Benutzern.
.PP
Der Aussender der Nachricht sollte Superuser sein, um jeden
vom Benutzer gesetzten Schutz zu ueberschreiben.
.SH DATEIEN
/etc/wall 	dieses Programm
.br
/dev/tty?
.br
/etc/utmp
.SH SIEHE AUCH
mesg(1),
write(1).
.SH DIAGNOSTIK
\fICannot send to name\fR:
wenn das Terminal schreibgeschuetzt ist (siehe
.BR mesg (1)).
.SH EINSCHRAENKUNGEN
Es ist moeglich das Terminal zu schuetzen, um Stoerungen durch
Ausgaben an den Terminalkanal zu vermeiden.

