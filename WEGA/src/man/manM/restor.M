.TH RESTOR M WEGA
.\" @[$]restor.M	1.2
.ad
.fi
.SH NAME
restor \- Rueckspeichern eines Dateisystemabzugs
.SH SYNTAX
.B restor
.I key [argument ...]
.SH BESCHREIBUNG
.B Restor
wird zum Lesen eines Dateisystemabzuges, der mit dem Kommando
.B dump
erstellt wurde, benutzt. Das Standard-E/A-Geraet ist \fI/dev/dumpdev\fR.
Existiert
.I /dev/dumpdev
nicht, wird
.I /dev/rfd1
benutzt.
.I Key
spezifiziert, was zu tun ist.
.SH KEYS
.I key
ist eines der Zeichen
.B rRtx
optional kombiniert mit \fBf\fR. 
.PP
.TP
.B  f
Das erste Argument nach dem Feld
.I key
wird als Dateinamen benutzt (Standard:
.IR /dev/dumpdev ).
.TP
\fBr\fR oder \fBR\fR
Der Dateisystemabzug wird gelesen und in das mit
.I argument
bezeichnete Dateisystem geladen.
Dies sollte nicht leichtsinnig (siehe unten) erfolgen.
Ist key das Zeichen
.BR R ,
fragt
.BR restor ,
mit welchem Teil des Backup-Speichermediums (welcher Diskette) des Abzugssatzes
gestartet werden soll. Dadurch kann
.B restor
unterbrochen und anschliessend neu gestartet werden (ein
.B icheck \-s
muss vor dem Neustart ausgefuehrt werden).
.TP
.B  x
Jede Datei, die mit
.I argument
bezeichnet wird, wird ausgewaehlt. `mount'-Vorsilben entfallen,
zum Beispiel besitzt
.I /usr/bin/lpr
im Abzug den Namen
.I /bin/lpr.
Die ausgewaehlte Datei wird in eine Datei mit dem selben Namen
kopiert. Alle notwendigen Directorys werden automatisch mit
der Referenz auf das gegenwaertige Arbeitsdirectory angelegt.
.sp
Soll eine Hierarchie von Abzuegen rueckgespeichert werden,
kann mit Hilfe des Programm
.BR  dumpdir (M)
eine Namensliste und ein Shell-Skript zum Kopieren der
Dateien in ihre Directory erstellt werden.
.TP
.B t
Das Erstellungsdatum des Dateisystemabzugs sowie das Datum,
von welchem an die geaenderten Dateien aus dem Dateisystem
abgezogen wurden, wird ausgegeben.
.PP
Die Option
.B r
sollte nur benutzt werden, um einen kompletten Dump oder einen
inkrementalen Dump in ein leeres Dateisystem rueckzuspeichern.
.PP
	/etc/mkfs /dev/md0 13000
.br
	restor r /dev/md0
.PP
ist eine typische Kommandofolge zum Rueckspeichern eines kompletten
Dump. Nachfolgende Aufrufe des Programms
.B restor
koennen zum Rueckspeichern weiterer inkrementaler Dumps benutzt werden.
.PP
Das Kommando
.B dump
gefolgt vom Kommando
.B mkfs
und dem Kommando
.B restor
kann zum Aendern der Groesse eines Dateisystems benutzt
werden.
.SH DATEIEN
/etc/restor	dieses Programm
.br
/tmp/rst*
.SH SIEHE AUCH
dump(M),
dumpdir(M),
mkfs(M),
dump(5).
.SH DIAGNOSTIK
Verschiedene Fehlernachrichten  koennen im Zusammenhang mit Lesefehlern
auf dem Backup-Medium und Schreibfehlern der Platte ausgegeben
werden. Fehler werden auch ausgegeben, wenn die i-Liste oder
die Freiblockliste des Dateisystems nicht ausreicht, um den
Dump aufzunehmen.
.SH EINSCHRAENKUNGEN
Der Dump enthaelt redundante Informationen, die benutzt werden
koennten, wenn Leseprobleme auftreten.
.B Restor
nutzt diese Informationen nicht.
