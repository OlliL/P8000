.TH ACCTCON M WEGA
.\" @[$]acctcon.M	1.2
.ad
.fi
.SH NAME
acctcon \- Login-Zeitabrechnung
.SH SYNTAX
\fBacctcon1\fR [\fIoptions\fR]
.PP
.B acctcon2
.SH BESCHREIBUNG
.B Acctcon1
konvertiert eine Folge von Login/Logoff-Datensaetzen, die es
von der Standardeingabe liest, in eine Folge von neuen Datensaetzen,
je einen pro Login-Sitzung. Die Eingabe sollte normalerweise
von
.I /usr/adm/wtmp
erfolgen. Die Ausgabe des Programms ist im ASCII-Format und gibt das
E/A-Geraet, das Benutzer-Id, den Login-Namen, "prime connect
time" (in Sekunden), "non-prime connect time" (in Sekunden),
die Startzeit der Sitzung (im numerischem Format) und Startdatum und -zeit
an.
.PP
.B Acctcon2
akzeptiert als Eingabe eine Folge von Login-Sitzungsdatensaetzen
und konvertiert diese in Gesamtabrechnungsdatensaetze
(siehe Format
.B tacct
in
.BR acct (5)).
.SH OPTIONEN
.TP
.BI \-l " file"
Die Datei
.I file
wird angelegt und enthaelt eine Aufsummierung ueber die Benutzung
der seriellen Kanaele. Diese enthaelt den Namen des Kanals,
die Zeit in Minuten, die dieser Kanal benutzt wurde, die Prozentzahl
zur Gesamtzeit, die Anzahl der Sitzungen und die Anzahl der
Logins und Logoffs. Die Datei hilft, defekte Kanaele zu identifizieren
und ein "unnormales" Verhalten von Software und Hardware herauszufinden.
Sowohl ein "hang-up" als auch die Beendigung der Login-Shell
generieren einen Logoff-Datensatz, sodass die Anzahl der Logoffs
oftmals doppelt so gross ist, als die Anzahl der Sitzungen.
.TP
.BI \-o " file"
Die Datei
.I file
wird mit einem Datensatz "ueber alles" fuer die Abrechnungsperiode
gefuellt, der die Startzeit, die Beendigungszeit, die Anzahl
der Systemstarts und die Anzahl der Datumsaenderungen enthaelt.
.TP
.B \-p
Druckt nur die Eingabe aus, zeigt den Namen des Kanals, den
Login-Namen und die Zeit (im numerischen und im date/time-Format)
an.
.TP
.B \-t
.B Acctcon1
verwaltet eine Liste der Kanaele, ueber die sich Benutzer im
System anmelden (login)
koennen. Wenn das Programm das Ende seiner Eingabe feststellt,
gibt es fuer jeden scheinbar aktiven seriellen Kanal einen Sitzungsdatensatz
aus. Das Programm setzt normalerweise voraus, das seine Eingabe
die aktuelle Datei ist, sodass es die aktuelle Zeit als Beendigungszeit
jeder Sitzung, die in Ausfuehrung ist, benutzt.
Das Flag
.B \-t
bewirkt, dass das Programm die letzte in seiner Eingabe gefundene
Zeit benutzt, um vernuenftige und wiederholbare Werte fuer
nicht aktuelle Dateien zu erhalten.
.SH BEISPIELE
Die typische Benutzung der Kommandos wird im folgenden Beispiel
gezeigt.  Die Datei
.B ctmp
wird nur fuer die Benutzung von
.BR acctprc (M)-Kommandos
erstellt:
.PP
acctcon1 \-t \-l lineuse \-o reboots <wtmp \(bv sort +1n +2 >ctmp
.br
acctcon2 <ctmp \(bv acctmerg >ctacct
.SH DATEIEN
/usr/adm/wtmp
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctmerg(M),
acctprc(M),
acctsh(M),
fwtmp(M),
runacct(M).
.SH EINSCHRAENKUNGEN
Der Bericht ueber die Kanalbenutzung wird durch Datumsaenderungen
verfaelscht. Benutze \fBwtmpfix\fR (siehe
.BR fwtmp (M))
zur Korrektur dieser Situation.
