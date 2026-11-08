.TH GETTY M WEGA
.\" @[$]getty.M	1.2
.ad
.fi
.SH NAME
GETTY \- Setzen des Terminal-Modus
.SH SYNTAX
.B GETTY
name type delay
.SH BESCHREIBUNG
.B GETTY
wird normalerweise von
.BR INIT (M)
als erster Schritt fuer ein Login der Benutzer
aufgerufen.
Die Zeilen in
.I /etc/inittab
teilen
.B INIT
mit, mit welchen Angaben
.B GETTY
aufgerufen wird.
.PP
.I Name
ist der Name eines Terminals in dem Directory
.I /dev
(z.B.
.BR tty3 ).
.I Type
ist ein einzelnes Zeichen, ausgewaehlt aus
.BR \- ,
.BR 0 ,
.BR 1 ,
.BR 2 ,
.BR 3 ,
.BR 4 ,
.BR 5 ,
.BR 6 ,
.B 7
oder
.BR 8 ,
(kann mit der Implementation stark variieren),
das die Geschwindigkeitstabelle in
.B GETTY
auswaehlt. Es kann aber auch das Zeichen
.B !
sein, das
.B GETTY
veranlasst, die Datei
.I /etc/utmp
zu aktualisieren und ein Exit auszufuehren.
.I Delay
ist nur fuer "dial-up"-Kanaele von Bedeutung. Es spezifiziert
die Zeit in Sekunden, die gewartet wird, bevor der Kanal
deaktiviert wird, falls der Benutzer nicht auf das
\fBWEGA login:\fP reagiert.
.PP
Zuerst gibt
.B GETTY
die Ausschrift \fBWEGA login:\fP aus.
Das
.B "WEGA login:"
haengt von der benutzten Geschwindigkeitstabelle ab und kann
Zeichen enthalten, die den Modus bestimmter Terminaltypen einstellen.
Existiert fuer den Terminaltyp, der durch
.I /etc/ttytype
gegeben ist,
eine Zeichenfolge zum Loeschen des Bildschirms, enthaelt auch die
Ausschrift
.B WEGA login:
diese Zeichen.
Die Ausschrift
.B WEGA
ist dabei abhaengig vom Node-Namensfeld
.BR uname (2).
.PP
Anschliessend wird der Login-Name des Benutzers als Einzelzeichen
eingelesen.
.PP
Beim Einlesen versucht
.B GETTY
die Geschwindigkeit und den Modus dem Terminal anzupassen.
Wird ein Null-Zeichen empfangen, vermutet es, dass das Zeichen
das Ergebnis einer Break-Bedingung ist. Die Geschwindigkeit
wird dann entsprechend der von
.B GETTY
benutzten Geschwindigkeitstabelle gewechselt und ein neues
.B WEGA login:
angefordert. Nachfolgende Breaks bewirken eine zyklische Benutzung
der Geschwindigkeiten in der Geschwindigkeitstabelle.
.PP
Der Login-Name des Benutzers wird mit einem Newline- oder
Carriage-Return-Zeichen abgeschlossen. Das letztere Zeichen
bewirkt, dass Carriage-Return-Zeichen im System entsprechend behandelt
werden. Enthaelt der Login-Name nur Grossbuchstaben, wandelt
das System alle folgenden Grossbuchstaben in die entsprechenden
Kleinbuchstaben um.
.PP
Zum Schluss wird
.BR login (1)
mit dem Login-Namen des Benutzers als Argument aufgerufen.
.PP
Geschwindigkeitsfolgen fuer die Geschwindigkeitstabellen:
.PP
.PD 0
.RS
.TP
.B \-
B110; fuer 110 Baud Konsole
.SM TTY\*S.
.TP
.B 0
B300\-B1200\-B150\-B110; 
normale "dial-up"-Geschwindigkeitsfolge beginnend ab B300.
Nuetzlich fuer "deal-up"-Verbindungen mit unterschiedlichsten
Terminals.
.TP
.B 1
B150;  
feste Geschwingigkeit.
.TP
.B 2
B9600;  
feste Geschwindigkeit, fuer 9600 Baud Terminals.
.TP
.B 3
B1200\-B300;  
normale "dial-up"-Geschwindigkeitsfolge beginnend ab B1200.
Nuetzlich, wenn die meisten Terminals mit 1200 Baud arbeiten.
.TP
.B 4
B300;  fuer 300 Baud Terminals.
.TP
.B 5
B300\-B1200;  normale "dial-up"-Geschwindigkeitsfolge beginnend
ab 300 Baud.
.TP
.B 6
B2400;  feste Geschwindigkeit.  
.TP
.B 7
EXTA (19200); feste Geschwindigkeit.
.TP
.B !
Aktualisierung der Accounting-Dateien und Beendigung von GETTY.
.RE
.PD
.SH DATEIEN
/etc/GETTY	dieses Programm
.SH SIEHE AUCH
login(1), tty(4), inittab(5), utmp(5), INIT(M), uname(2), ttytype(5).
.SH BEKANNTE FEHLER
Besser waere es, wenn die Geschwindigkeitstabellen von einer
Datei eingelesen werden und nicht im Programm
.B GETTY
enthalten waeren.
