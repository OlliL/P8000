.TH CRON M WEGA
.\" @[$]cron.M	1.2
.ad
.fi
.SH NAME
cron \- Uhr-Daemon
.SH SYNTAX
.B cron
.SH BESCHREIBUNG
.B Cron
fuehrt Kommandos zu angegebenen Tagen und Zeiten, entsprechend
den Anweisungen in der Datei
.IR /usr/lib/crontab ,
aus.
.B Cron
sollte nur einmal gestartet werden, da das Programm niemals
endet.
.B Cron
wird am besten vom Initialisierungsprozess durch die Datei
.I /etc/rc
(siehe
.BR init (M))
gestartet.
.PP
Die Datei
.I crontab
enthaelt Zeilen mit jeweils 6 Feldern. Die Felder werden durch
Leerzeichen oder Tabs getrennt. Die ersten 5 Felder enthalten
Integerwerte, die die Minute (0-59), die Stunde (0-23), den
Tag (1-31), den Monat (1-12) und den Wochentag (0-6, 0 = Sonntag),
angeben.
Jedes dieser Felder kann enthalten:
.PP
.RS
- einen Wert im oben angegebenen Bereich;
.br
- zwei durch ein Minuszeichen voneinander getrennte Werte zur
Angabe eines Bereiches (einschliesslich dieser beiden Werte);
.br
- eine Liste von durch Komma voneinander getrennter Werte oder
.br
- einen Stern zur Angabe aller moeglichen Werte.
.RE
.PP
Im sechsten Feld steht eine Zeichenkette, die von der Shell
zu den angegebenen Zeiten abgearbeitet wird. Ein Zeichen
.B %
in diesem Feld wird in das Newline-Zeichen uebersetzt. Nur
die erste Zeile (bis zum
.B %
oder dem Zeilenende) des Kommandofeldes wird von der Shell
als Kommando gewertet. Die anderen Zeilen stehen dem Kommando
als Standardeingabe zur Verfuegung.
.PP
.B Cron
fragt die Datei
.I crontab
jede Minute ab, um Aenderungen festzustellen.
.B Cron
liest diese dann ein. Es dauert daher nur maximal eine Minute bis die
Eintraege wirksam werden.
.SH DATEIEN
/etc/cron			dieses Programm
.br
/usr/lib/crontab
.br
/usr/lib/cronlog
.SH SIEHE AUCH
sh(1),
init(M).
.SH DIAGNOSTIK
Alle Aktionen von
.B cron
werden in der Datei
.I /usr/lib/cronlog
aufgezeichnet.
.SH EINSCHRAENKUNGEN
.B Cron
liest die Datei
.I crontab
nur, wenn sich die Datei geaendert hat. Es liest aber die Kernversion
dieser Tabelle jede Minute. Ein effizienterer Algorithmus kann
nicht geschaffen werden. Trotzdem liegt die CPU-Belastung
ueber ein Prozent, die von
.B cron
ausgefuehrten Kommandos sind dabei nicht beruecksichtigt.
