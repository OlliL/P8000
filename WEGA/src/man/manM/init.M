.TH INIT M WEGA
.\" @[$]init.M	1.2
.ad
.fi
.SH NAME
INIT \- Initialisierung der Prozesssteuerung
.SH SYNTAX
.B INIT
.I [state]
.SH BESCHREIBUNG
.B INIT
wird innerhalb von 
.SM WEGA
als letzter Schritt der Boot-Prozedur aufgerufen.
.B INIT
ist Prozess Nummer eins und der "Vorfahre" jedes anderen Prozesses
im System. Als dieser kann er zur Steuerung der Prozessstruktur
des Systems benutzt werden. Dies erfolgt durch den Superuser,
der
.B INIT
mit einem Argument aufruft.
.PP
.B INIT
besitzt 9 Zustaende, Status 1 bis 9. Status 1 wird bei der
manuellen, Status 2 bei der automatischen Boot-Prozedur benutzt.
.B INIT
fuehrt bei der Einnahme eines jeden Zustands die gleichen Funktionen aus.
.B
INIT
liest die Datei
.IR /etc/inittab ,
die den Uebergang in jeden der Zustaende definiert.
Die Zeilen in dieser Datei haben das Format:
.PP
.RS
state:id:flags:command
.PP
wobei
.PP
.I state
ein Integerwert von 1 bis 9,
.PP
.I id
eine Identifikation aus 2 Zeichen (z.B. co fuer Konsole, 00 fuer
tty0 usw.) ist, die fuer Prozesse, die in dem Status
aktiv sein koennen, steht,
.PP
.I flags
sind die Zeichen t, k, c oder o,
.PP
.I command
ist der Name einer ausfuehrbaren Datei mit ihren optionalen
Argumenten.
.RE
.PP
Alle Zeilen, in denen das Statusfeld zu dem augenblicklichen
Zustand von
.B INIT
passt, werden als gueltig anerkannt.
Ein Prozess, dessen 2 Byte
.I id
zu einer gueltigen Zeile passt, kann beendet (Signal 15) oder
abgebrochen (Signal 9) werden oder beides, wenn die Flags
.I flags
.B t
und
.B k
in dieser Reihenfolge stehen.
Das Signal wird zu allen Prozessen der Prozessgruppe
.I id
gesendet. Das Kommandofeld
.I command
wird fuer die spaetere Ausfuehrung gerettet.
.PP
Nach dem Einlesen von
.I /etc/inittab
und der Signalisierung an die laufenden Prozesse, aber vor
der Ausfuehrung aller Prozesse im neuen Status, ruft
.B INIT
.I /etc/rc
mit drei Argumenten auf.
.I /etc/rc
fuehrt solche Aktivitaeten aus, wie Loeschen von temporaeren
Dateien, Einbinden von Dateisystemen und den Start der "Daemonen".
Die drei Argument geben den augenblicklichen Zustand, die Anzahl
der frueheren Wechsel in diesen Zustand und den alten Zustand
an.
.PP
Nach Beendigung von
.I /etc/rc
fuehrt
.B INIT
alle Kommandos
.I commands
aus, die auf die Ausfuehrung warten.
Ein Kommando
.I command
wartet auf die Ausfuehrung, wenn augenblicklich kein Prozess
mit dem
.I id
des Kommandos, laeuft.
Das Flag
.B c
(continuous)
bewirkt, dass das Kommando
.I command
kontinuierlich neu gestartet wird, sobald der Prozess mit dem
gleichen
.I id
stirbt.
Das Flag
.B o
(off)
bewirkt, dass das Kommando
.I command
ignoriert wird. Das ist fuer die Deaktivierung von Zeilen ohne
extensives Editieren nuetzlich. Ansonsten wird
.I command
maximal einmal im augenblicklichem Status ausgefuehrt.
.PP
Die Kommandoausfuehrung erfolgt durch Eroeffnen von
.I /
zum Lesen und Schreiben zu den Dateideskriptoren 0, 1 und
2, Ruecksetzen aller Signale zu der Standardeinstellung im
System, Setzen einer neuen Prozessgruppe mit
.BR setpgrp (2)
und dem Start mit:
.PP
.RS
/bin/sh \|\-c \|exec\|
.I command
.RE
.SH BEISPIELE
Die Zeile
.PP
1:co:c:/bin/csh </dev/console > /dev/console 2>&1
.PP
in inittab erzeugt eine Benutzer-Umgebung im Single-User-Mode, wenn
.B INIT 1
vom Superuser ausgefuehrt wird. Die Zeile gibt an, dass im Status
1 (Single-User-Mode) ein Prozess mit der entscheidenden nur
einmal existierenden Identifikation
.I co
erzeugt werden soll. Das Programm, das fuer diesen Prozess
ausgefuehrt wird, soll die C-Shell sein, die ihre Eingaben
von der Konsole erhaelt und die Standard- und Fehlerausgaben zurueck zur
Konsole richtet.
Endet dieser Prozess (z.B. Logout), soll er neu gestartet
werden.
.PP
Bei Aufruf von
.B INIT 2,
bewirkt der Eintrag
.PP
2:co:c:/etc/GETTY console 2
.PP
in
.I inittab
im Multi-User-Mode (Status 2) einen Login-Prozess
und setzt die Baudrate auf 9600 Baud.
Der Eintrag
.PP
2:05:c:/etc/GETTY tty5 2
.PP
setzt den seriellen Kanal
.I /dev/tty5
zur Benutzung durch ein 9600 Baud Terminal,
.PP
1:05:k:/etc/GETTY !
.PP
sichert, dass dieser Kanal bei der Rueckkehr oder dem Eintritt
in den Single-User-Mode nicht aktiv ist.
.SH DIAGNOSTIK
Fehlt die Datei
.IR /etc/inittab
oder existiert dort kein Prozess
versucht
.B INIT
eine Shell fuer
.I /dev/console
auszufuehren. Ist das Problem beseitigt, muss dieser Zustand
durch Beenden der Shell behoben werden.
.SH EINSCHRAENKUNGEN
.B INIT
gibt keinen Fehler aus, wenn das Paar Status/Identifikation in 
.I /etc/inittab
nicht einmalig ist. Der letzte Eintrag ist
gueltig.
.SH DATEIEN
/etc/INIT		dieses Programm
.br
/etc/inittab
.br
/etc/rc
.br
/bin/sh
.br
/dev/console
.SH SIEHE AUCH
login(1),
sh(1),
exec(2),
setpgrp(2),
inittab(5),
getty(M).
