.TH BACKEND M WEGA
.\"@[$]backend.M	1.3
.ad
.fi
.SH NAME
lp, lp_nf, text, text_nf \- Ausgabeprogramme fuer den Spooler 
.SH SYNTAX
\fBlp\fR [\fIoptions\fR]
.br
\fBtext\fR [\fIoptions\fR]
.SH BESCHREIBUNG
Dieses Programm wird vom Spooler zur Ausgabe der aktuellen
Kopie der Datei an den Drucker benutzt.
.B Lp
wird fuer normale Drucker und
.B text
fuer Drucker mit Textqualitaet verwendet.
Beide Programme filtern ESC-Folgen aus dem Zeichenstrom heraus.
Die analogen Treiber
.B lp_nf
und
.B text_nf
tun dies nicht.
.PP
Jedes Backend-Programm wird durch
.BR dqueuer (M)
gestartet. Die Steuerung des Aufrufs erfolgt mit Hilfe der
Konfigurationsdatei (siehe WEGA-Systemhandbuch, Abschnitt 7.1).
Das Backend-Programm erhaelt von
.B dqueuer
drei eroeffnete Dateideskriptoren: RFFD (read file), SFFD (device)
und STATUSFD (status log).
Die aktuelle Definition dieser Variablen ist in der Datei
/usr/include/spool.h enthalten.
Die Backend-Programme 
.B lp
und
.B text
kopieren die Daten von der Datei RFFD zur Datei SFFD.
Statusinformationen werden in
der Datei STATUSFD abgelegt.
.PP
Zusaetzlich zur Ausgabe realisieren die Backend-Programme Funktionen,
die vom speziellen E/A-Geraet benoetigt werden.
Das Programm
.B lp
filtert die Backspace-Zeichen heraus und
.B text
ruft vor der Ausgabe der Daten
.BR stty (1)
auf. Sie erzeugen auch
Titelseiten mit nuetzlichen Informationen.
.PP
Beim Aufruf wird dem Backend-Programm eine von
.BR dqueuer (M)
erstellte Liste uebergeben. Sie enthaelt Optionen, die von
.BR dqueuer (M)
generiert oder der Konfigurationsdatei entnommen wurden.
Optionen, die von
.B dqueuer
generiert und von beiden Backend-Programmen
akzeptiert werden sind:
.TP 3
.B \-B
Zeigt an, dass die Datei, die von dqueuer
ausgedruckt werden sollte, nicht existiert.
Es wird eine Titelseite ausgegeben, die diese Nachricht
enthaelt.
.TP 3
.B \-c
.I n
.br
Die Option gibt an, dass die Datei mehrmals ausgedruckt werden
soll. Das Backend-Programm erzeugt
.I n 
Kopien.
.TP 3
.B \-d 
.I dest
.br
Die Zeichenkette 
.I dest
erscheint auf der Titelseite.
.TP 3
.B \-f 
.I filename
.br
Dateiname, erscheint auf der Titelseite.

.TP 3
.B \-F 
.I from
.br
Die 'from' Zeichenkette erscheint auf der Titelseite.
.TP 3
.B \-s 
.I spool_time
.br
Zeichenkette im
.BR ctime (3)-Format,
gibt auf der Titelseite die Uhrzeit an.
.TP 3
.B \-t 
.I title
.br
Die Zeichenkette gibt den Titel auf der Titelseite an.
.PP
Das Backend-Programm druckt auch die Zeichenkette in der Datei
/etc/sitefile an der Seitengrenze der Titelseite.
Existiert die Datei nicht, wird ein Standardtext benutzt.
.PP
Das Programm
.B text
akzeptiert einen zusaetzlichen Parameter,
der nur in der Konfigurationsdatei angegeben werden kann:
.TP 3
.B \-T
[\fIstty parameters\fR]
.br
Setzen der Terminal-Parameter, wird ein TTY-Kanal fuer die
Ausgabe benutzt, muss er in der Datei
.I /etc/inittab
deaktiviert werden. Der Kanal wird geschlossen (ausser falls noch Ausgaben
erfolgen). Alle Terminal-Parameter werden rueckgesetzt. Die
Option -T setzt diese Parameter ueber
.BR stty (1)
neu.  Die Zeile

.br
.nf
         D1,R,/dev/tty0,/usr/lib/text -T "1200"

.fi
in der Konfigurationsdatei setzt das E/A-Geraet /dev/tty2 auf
1200 Baud bevor die Ausgaben erfolgen.
.SH DATEIEN
/usr/lib/lp		dieses Programm
.br
/usr/lib/lp_nf		dieses Programm
.br
/usr/lib/text		dieses Programm
.br
/usr/lib/text_nf	dieses Programm
.SH SIEHE AUCH
dqueuer(M)
