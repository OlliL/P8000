.TH ACCTPRC M WEGA
.\" @[$]acctprc.M	1.2
.ad
.fi
.SH NAME
acctprc \- Prozessabrechnung
.SH SYNTAX
\fBacctprc1\fR [\fBctmp\fR]
.PP
.B acctprc2
.SH BESCHREIBUNG
.B Acctprc1
liest die Eingabe in einer Form, die in
.BR acct (5)
beschrieben ist, fuegt die mit den Benutzer-Ids korrespondierenden
Benutzernamen ein und schreibt fuer jeden Prozess eine ASCII-Zeile,
die das Benutzer-Id, den Login-Namen, "prime CPU time"
(in tics), "non-prime CPU time" (in tics) und "mean memory
size" (in 64 Byte Einheiten) angibt.
Ist
.B ctmp
angegeben, erwartet das Programm, dass die Datei eine Liste
von Login-Sitzungen in einer Form, die in
.BR acctcon (M)
beschrieben ist, enthaelt, die nach dem Benutzer-Id und dem
Login-Namen sortiert ist. Wenn diese Datei nicht zur Verfuegung
steht, erhaelt das Programm die Login-Namen aus der Passwortdatei.
Die Informationen in
.B ctmp
helfen dem Programm zwischen verschiedenen Login-Namen, die
sich das gleiche Benutzer-Id teilen, zu unterscheiden.
.PP
.B Acctprc2
liest Datensaetze im Format, das von
.B acctprc1
geschrieben wird, summiert diese nach Benutzer-Id und Namen
und schreibt die sortierte Zusammenfassung als Gesamtabrechnungsdatensaetze
zur Standardausgabe.
.SH BEISPIELE
Eine typische Benutzung dieser Kommandos ist im folgenden Beispiel
gezeigt:
.PP
.RS
acctprc1 \|ctmp \|</usr/adm/pacct \|\(bv \|acctprc2 \|>ptacct
.RE
.SH DATEIEN
/etc/passwd
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctcon(M),
acctmerg(M),
acctsh(M),
fwtmp(M),
runacct(M).
.SH EINSCHRAENKUNGEN
Obwohl es bei normal laufenden Kommandos moeglich ist,
zwischen Login-Namen, die sich das
gleiche Benutzer-Id teilen, zu unterscheiden, ist es schwierig,
diese Unterscheidung fuer Kommandos zu treffen, die z.B. von
.BR cron (M)
gestartet wurden.
Eine praezisere Umwandlung kann durch Aufbereitung der Login-Sitzungen
auf der Konsole mit dem Programm
.B acctwtmp in
.BR acct (M)
erfolgen.
