.TH ACCT M WEGA
.\"@[$]acct.M	1.2
.ad
.fi
.SH NAME
acct \- Ueberblick ueber die Systembuchhaltung und deren Kommandos
.SH SYNTAX
.B acctdisk
.PP
.B acctdusg
[\fB\-p\fR \fIfile\fR] [\fB\-u \fIfile\fR] > \fIdtmp-file\fR
.PP
\fBaccton\fR [\fIfile\fR]
.PP
\fBacctwtmp\fR [\fIname [line]]\fR \fB>> \fI/usr/adm/wtmp
.SH BESCHREIBUNG
Die Abrechnungssoftware (accountig software) besteht aus Werkzeugen
(C- und Shell- Programme),
die zum Aufbau eines Abrechnungssystems verwendet werden koennen.
.BR Acctsh (M)
beschreibt die Shell-Programme.
.PP
Die Login-Zeitabrechnung wird durch verschiedene Programme,
die Datensaetze in die Datei
.I /usr/adm/wtmp
schreiben (siehe
.BR utmp (5)
), realisiert.
Die in
.BR acctcon (M)
beschriebenen Programme konvertieren diese Datei in Datensaetze ueber
bestimmte Benutzungsperioden und Kosten, die anschliessend mit
.BR acctmerg (M)
aufsummiert werden koennen. 
.PP
Die Prozessabrechnung wird vom WEGA-Kern ausgefuehrt.
Nach Beendigung eines Prozesses, wird ein Datensatz pro Prozess
zu einer Datei (normalerweise
.IR /usr/adm/pacct )
geschrieben. Die Programme in
.BR acctprc (M)
summieren diese Daten.
.BR Acctcms (M)
wird zur Summierung der Kommandobenutzung verwendet.
Die aktuellen Prozessdaten koennen mit
.BR acctcom (M)
bearbeitet werden.
.PP
Prozessabrechnung oder Login-Zeitabrechnung (oder auch jeder
Abrechnungsdatensatz, der in
.BR acct (5)
beschrieben ist), kann mit
.B acctmerg
(siehe \fItacct\fP-Format in
.BR acct (5))
zu Gesamtabrechnungsdatensaetzen verknuepft werden.
.B Prtacct
(siehe
.BR acctsh (M))
kann zum Formatieren von Abrechnungsdatensaetzen
benutzt werden.
.PP
.B Acctdisk
liest Zeilen, die Benutzer-Id, Login-Namen und die Anzahl der
Diskbloecke enthalten, und konvertiert diese zu Gesamtabrechnungsdatensaetzen,
die dann mit anderen Abrechnungsdatensaetzen kombiniert werden koennen.
.PP
.B Acctdusg
liest von der Standardeingabe (gewoehnlich von
.BR "find / \-print" )
und berechnet die Diskbenutzung. 
.PP
.B Accton
allein schaltet die Prozessabrechnung aus.
Ist
.I file
angegeben, muss es der Name einer existierenden Datei sein.
An diese haengt der Kern Abrechnungsdatensaetze (siehe
.BR acct (2)
und
.BR acct (5)).
.PP
.B Acctwtmp
schreibt einen Datensatz
.BR wtmp (5)
zur Standardausgabe. Der Datensatz enthaelt die aktuelle
Zeit, Name
.I name
und
.IR line .
Ist
.I line
weggelassen, wird ein Wert ausgegeben, der von anderen Programmen
als Neustart interpretiert wird. 
Fuer eine genauere Abrechnung, seien die folgenden Kommandos
fuer die Benutzung
in den Start- und Abfahrprozeduren empfohlen:
.PP
.RS
acctwtmp \|\*`uname\*` \|>>/usr/adm/wtmp
.br
acctwtmp \|reason \|>>/usr/adm/wtmp
.RE
.SH OPTIONEN
.TP
.B \-p
.I file
ist der Name der Passwortdatei. Die Option wird nicht benoetigt,
wenn die Passwortdatei
.I /etc/passwd
ist.
.TP
.B \-u
Datensaetze, die aus solchen Dateinamen bestehen, fuer die
.I acctdusg
keine Kosten erhebt, werden in
.I file
abgelegt.
(eine potentielle Moeglichkeit Benutzer herauszufinden, die
die Disk-Kosten sparen wollen).
.SH DATEIEN
.ta \w'/usr/adm/wtmp\ \ \ \ 'u +.5i
.nf
/etc/passwd	fuer Konvertierung Benutzernamen in Benutzer-Id
.br
/usr/lib/acct	enthaelt alle Abrechnungskommandos, die in
.br
	Sektion M beschrieben sind.
/usr/adm/pacct	aktuelle Prozessabrechnungsdatei
/usr/adm/wtmp	login/logoff-history-Datei
.fi
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acctcms(M),
acctcon(M),
acctmerg(M),
acctprc(M),
acctsh(M),
fwtmp(M),
runacct(M).
.br
.I "Das PWB/WEGA-Abrechnungssystem"
in WEGA Dienstprogramme
.DT
