.TH DUMP M WEGA
.\" @[$]dump.M	1.2
.ad
.fi
.SH NAME
dump \- inkrementaler Dateisystemabzug
.SH SYNTAX
.B dump
.I [key [argument]
.I [key [argument]]
.I filesystem]
.SH BESCHREIBUNG
.B Dump
kopiert alle Dateien zu einem Backup-Geraet, die sich im Dateisystem
.I filesystem
nach einem bestimmten Datum geaendert haben. Im P8000 wird
die Floppy-Disk als Backup-Medium verwendet.
.PP
Das Standard-E/A-Geraet ist
.I /dev/dumpdev.
Existiert 
.I /dev/dumpdev
nicht, wird
.I /dev/rfd1
benutzt.
Das Standard-\fIfilesystem\rR ist
.BR /dev/rz .
.PP
Dumps duerfen nur von ruhenden Dateisystemen ausgefuehrt werden.
(Der Abzug eines aktiven Dateisystems erzeugt einen nichtbrauchbaren
Dump.)
.SH KEYS
.I key
bezeichnet das Datum und andere Optionen fuer den Dump:
.TP 5
.B  f
Der Dump wird zum Medium, das mit dem naechstfolgenden Argument
angegeben wird, geschrieben (nicht nach
.IR /dev/dumpdev ).
.TP 5
.B s
Das naechste Argument gibt die Speicherkapazitaet des Backup-Mediums
an.
Wird diese Grenze erreicht, unterbricht das Programm seine
Arbeit, um dem Benutzer die Moeglichkeit zu geben, das Speichermedium
zu wechseln. Die Diskettenkapazitaet wird ueber den Treiber
ermittelt und muss nicht spezifiziert werden.
.TP 5
.B  u
Konnte der Dump erfolgreich durchgefuehrt werden, wird der
Zeitpunkt (Datum und Uhrzeit) des Beginns in die Datei
`/etc/ddate' eingetragen. Diese Datei fuehrt fuer jedes Dateisystem
und jedes Dump-Niveau ein separates Datum.
.TP 5
.B  0\-9
Diese Zahl gibt das Dump-Niveau an. Abgezogen werden alle Dateien
aus dem angegebenen Dateisystem, die seit dem Datum
geaendert wurden, das fuer den letzten Abzug dieses Dateisystems
auf einem niedrigerem Niveau als dem angegebenen in
`/etc/ddate' eingetragen ist. Falls kein solches Datum ermittelt
werden kann, wird der Beginn der Systemzeitrechnung als Datum
angenommen. Daraus ergibt sich, dass ein 
.B dump 0
in jedem Falle den Abzug eines vollstaendigen Dateisystems
bewirkt.
.PP
.DT
Sind keine Argumente angegeben, wird
.I key
mit
.B 9u
angenommen.
.SH BEISPIELE
Als naechstes soll die Vorgehensweise fuer das periodische
Abziehen eines Dateisystems erlaeutert werden, die bei relativ
geringem zeitlichen und materiellen Aufwand eine recht hohe
Datensicherheit gewaehrleistet. Zu Beginn ist mit
.PP
	dump 0u \fI/dev/filesys\fR
.PP
ein vollstaendiger Abzug des gesamten Dateisystems anzufertigen.
Dann werden periodisch Abzuege auf dem Niveau 9 ausgefuehrt.
Diese enthalten jeweils alle Dateien, die seit dem Niveau-0-Abzug
geaendert wurden. Fuer die Auswahl der Backup-Mediumsaetze
sollte die "Turm von Hanoi"-Strategie
(1 2 1 3 1 2 1 4 ...) benutzt werden. Wenn die Anzahl der Dumps der Form
.PP
	dump 9u \fI/dev/filesys\fR
.PP
zu umfangreich werden, sollte ein Abzug
.PP
	dump 1u \fI/dev/filesys\fR
.PP
von diesem Dateisystem angefertigt werden. Dieser basiert auf
dem ursprueglichen Niveau-0-Abzug. Anschliessend wird mit den
Abzuegen auf dem Niveau 9 fortgefahren, die nun allerdings
auf dem Niveau-1-Abzug basieren. Die naechste Stufe waere ein
Niveau-2-Abzug des Dateisystems und die weitere Fortsetzung
des Verfahrens in der angegebenen Weise, solange wie gewuenscht.
.SH DATEIEN
/etc/dump	dieses Programm
.br
/etc/ddate 
.SH SIEHE AUCH
dumpdir(M),
restor(M),
dump(5).
.SH EINSCHRAENKUNGEN
Lesefehler im Dateisystem werden ignoriert. Schreibfehler des
Backup-Geraetes sind gewoehnlich fatal. Fuer den Abzug eines
Dateisystems sollte stets der gleiche Dateisystemname verwendet werden.
Wenn
.B dump
die Datei
.I /etc/ddate
zum Bestimmen des Datums des letzten Dumps einliest,
wertet es z.B.
\'/dev/rmd0\' und
\'/dev/md0\' als zwei verschiedene Dateisysteme.
