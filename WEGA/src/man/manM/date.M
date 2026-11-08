.TH DATE M WEGA
.\" @[$]date.M	1.2
.ad
.fi
.SH NAME
date \- Datum anzeigen und setzen
.SH SYNTAX
.B date
[\fB-u\fR] [\fImmddhhmm[yy]\fR] 
[\fB+\fIformat\fR]
.SH BESCHREIBUNG
Mit der Option
.B -u 
wird die Zeit in GMT ausgegeben.
.PP
Ist kein Argument angegeben oder beginnt das Argument mit dem
Zeichen \fB+\fP,
werden das aktuelle Datum und die Zeit ausgegeben,
anderenfalls werden sie gesetzt.
Das erste
.I mm
ist der Monat als Zahl;\fR
.I dd
ist der Tag;\fR
.I hh
ist die Stunde (24 Stundenanzeige);\fR
das zweite
.I mm
ist die Anzahl der Minuten;\fR
.I yy
sind die letzten zwei Stellen des Jahres. Diese Angabe ist
optional.
Zum Beispiel:
.PP
.RS
date\| 10081245
.RE
.PP
setzt das Datum auf den 8.Oktober 12:45 Uhr. Das Jahr bleibt
unveraendert.
Das System WEGA arbeitet mit der Zeit in \s-1GMT\s0.
.B Date
konvertiert die Zeit von und zur oertlichen Standard- bzw.
Sommerzeit.
.PP
Beginnt das Argument mit \fB+\fP,
wird das Ausgabeformat von
.B date
durch den Benutzer festgelegt. Das Format der Ausgabe entspricht
dem ersten Argument fuer
.BR printf (3).
Alle Ausgabefelder besitzen eine feste Laenge. Jeder Felddeskriptor
beginnt mit dem Zeichen \fB%\fP
und wird waehrend der Ausgabe durch seinen korrespondierenden
Wert ersetzt. Ein einzelnes Zeichen
\fB%\fP wird durch \fB%%\fP dargestellt.
Alle anderen Zeichen werden unveraendert ausgegeben.
Die Zeichenkette wird stets mit dem Newline-Zeichen abgeschlossen.
.PP
Felddeskriptoren:
.RS
.PD 0
.TP 5
.B  n
Newline-Zeichen einfuegen
.TP 5
.B  t
Tab einfuegen
.TP 5
.B  m
Monat \- 01 bis 12
.TP 5
.B  d
Tag \- 01 bis 31
.TP 5
.B  y
letzte 2 Stellen des Jahres \- 00 bis 99
.TP 5
.B D
Datum als mm/dd/yy
.TP 5
.B  H
Stunde \- 00 bis 23
.TP 5
.B  M
Minute \- 00 bis 59
.TP 5
.B  S
Sekunde \- 00 bis 59
.TP 5
.B T
Zeit als \s-1HH:MM:SS\s+1
.TP 5
.B  j
Julianisches Datum \- 001 bis 366
.TP 5
.B  w
Wochentag \- Sunday = 0
.TP 5
.B  a
abgekuerzter Wochentag \- Sun bis Sat
.TP 5
.B  h
abgekuerzter Monat \- Jan bis Dec
.TP 5
.B  r
Zeit in
.SM AM/PM 
Notation
.RE
.ne 8
.SH BEISPIEL
.RS
date\| \(fm+DATE: %m/%d/%y%nTIME: %H:%M:%S\(fm
.RE

.PP
wuerde eine Ausgabe wie folgt erzeugen:

.PP
.RS
DATE: 08/01/86
.br
TIME: 14:45:05

.RE
.SH DATEIEN
/bin/date       dieses Programm
.br
/usr/adm/wtmp   zum Vermerken der Zeitsetzung
.br
/dev/kmem.

.SH SIEHE AUCH
date(1),
time(2),
ctime(3),
utmp(5)

.SH DIAGNOSTIK
\fIno permition\fR: Nur der Superuser hat die Erlaubnis
Datum und Zeit zu setzen.
.sp
\fIbad conversion\fR: Datum ist syntaktisch falsch angegeben.
.sp
\fIbad format character\fR: Felddeskriptor ist nicht deutbar.
