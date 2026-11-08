.TH DQUEUER M WEGA
.\" @[$]dqueuer.M	1.2
.ad
.fi
.SH NAME
dqueuer \- Ausfuehren der Warteschlangenauftraege von
.BR nq
.SH SYNTAX
.B dqueuer 
[\fB\-rnk\fR]
.SH BESCHREIBUNG
.B Dqueuer
erhaelt von
.BR nq (1)
und
.BR xq (M)
Auftraege und druckt Dateien auf geteilt benutzten E/A-Geraeten
aus. Das Programm kommuniziert mit
.B nq
und
.B xq
ueber Signale und einen Satz von Steuerdateien.
.B Dqueuer
laeuft staendig und wird gewoehnlich durch
.I /etc/rc
gestartet.
.PP
Alle Geraete, die unter Steuerung von
.B dqueuer
laufen, werden vom Systemverwalter in der Datei
.I /usr/spool/queuer/config
definiert (siehe WEGA-Systemhandbuch, Abschnitt 7.1).
.PP
Das Programm
.B dqueuer
ueberprueft, ob die aktive Konfigurationsdatei
.I /usr/spool/queuer/activeconfig
existiert, oder ob die Option
.B -r 
gesetzt ist. In diesem Fall wird die Konfigurationsdatei
.I /usr/spool/queuer/config
eingelesen und die aktive Konfigurationsdatei angelegt.
.PP
Die aktive Konfigurationsdatei enthaelt die gleichen Informationen
wie die Konfigurationsdatei im binaeren Format und zusaetzlich
die Prozessidentifikation pid des laufenden Programms
.BR dqueuer .
.B Nq
und
.B xq
benutzen die aktive Konfigurationsdatei fuer ihre Auftraege.
.PP
Laeuft das Programm
.B dqueuer
bereits, wenn die Option -r angegeben wird, signalisiert
dies der neue Prozess dem alten Prozess dqueuer und fuehrt
unverzueglich ein Exit aus. Das Signal bewirkt, dass der augenblickliche
dqueuer-Prozess die aktive Konfigurationsdatei neu erstellt.
.PP
Die Option
.B -n
(no execute) kann benutzt werden, um eine Zusammenfassung der
Konfigurationsdatei auszugeben, die ohne Aenderung dieser Datei
und ohne Aktivierung des dqueuer-Prozesses auf Fehler ueberprueft
werden kann.
.PP
Die Option
.B -k
beendet den dqueuer-Prozess, trotzdem setzen aber die Backend-Programme
die Ausgabe ihrer aktuellen Dateien bis zum Ende fort.
.PP
.B Dqueuer
erhaelt von
.B nq
und
.B xq
seine Auftraege. Diese Programme erzeugen Dateien im Directory
.I /usr/spool/queuer/requestdir
mit bestimmten Informationen, die zur Ausfuehrung der Auftraege
erforderlich sind, und senden ein Signal an
.B dqueuer.
.PP
Durch das Signal von
.B nq
oder
.B xq
wacht
.B dqueuer
auf und fuehrt alle ausstehenden Auftraege, die es in dem
Auftrags-Directory findet, aus.
.PP
Zum Ausdrucken der Dateien muss
.B dqueuer
mit einem Satz von "Backend"-Programmen ausgestattet sein,
die speziell fuer jede Geraeteart geschrieben werden muessen.
Die Namen dieser Backend-Programme sind in der Konfigurationsdatei
enthalten. Zwei Backend-Programme sind z.Z. verfuegbar:
.I /usr/lib/lp
fuer normale Drucker und
.I /usr/lib/text
fuer Drucker mit Textqualitaet.
.SH DATEIEN
/etc/dqueuer				diese Programm
.br
/usr/spool/queuer/config
.br
/usr/spool/queuer/activeconfig
.br
/usr/spool/queuer/statusdir 
.br
/usr/spool/queuer/requestdir
.br
/tmp/queuer        
.br
/usr/spool/queuer/logfile
.fi
.SH SIEHE AUCH
nq(1),
xq(1),
backend(M),
spool(7),
xq(M).
