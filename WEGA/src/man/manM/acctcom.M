.TH ACCTCOM M WEGA
.\"@[$]acctcom.M	1.2
.ad
.fi
.SH NAME
acctcom \- Suchen und Ausgeben von Prozessabrechnungsdateien
.SH SYNTAX
\fBacctcom\fR [[\fIoptions\fR] [\fIfile\fR]] 
.SH BESCHREIBUNG
.B Acctcom
liest die Datei
.IR file ,
die Standardeingabe oder
.I /usr/adm/pacct 
in einem Format, das in
.BR acct (5)
beschrieben ist, und schreibt die selektierten Datensaetze zur
Standardausgabe. Jeder Datensatz beschreibt die Abarbeitung
eines Prozesses.
Die Ausgabe zeigt:
.sp
.RS
.B COMMAND NAME 
.br
.B USER
.br
.BR TTYNAME 
.br
.B START TIME
.br
.B END TIME
.br
.B REAL (SEC)
.br
.B CPU (SEC)
.br
.B MEAN SIZE (K)
.RE
.sp
und optional
.B F
(
.BR fork/exec- Flag:
\fB1\fR fuer \fBfork\fR ohne \fBexec\fR) und
.B STAT
(dem System-Exit-Status).
.PP
Dem Kommandonamen ist das Zeichen
.B #
angefuegt, wenn das Kommando mit den Privilegien des
.I Superusers
ausgefuehrt wurde.
Assoziiert ein Prozess mit einem unbekannten Terminal, wird
ein \fB?\fR im Feld
.B TTYNAME
ausgegeben.
.PP
Ist keine Datei
\fIfile\fR angegeben und assoziiert die Standardeingabe mit
einem Terminal oder mit
.I /dev/null
(z.B. wenn das Zeichen
.B &
in der Shell benutzt wurde), wird
.I /usr/adm/pacct
gelesen, im anderen Fall die Standardeingabe benutzt.
.PP
Sind
.IR file -Argumente
angegeben, werden sie in der angegebenen Reihenfolge gelesen.
Jede Datei wird normalerweise vorwaerts gelesen, z.B in chronologischer
Reihenfolge der Prozessbeendigungszeit.
Die Datei
.I /usr/adm/pacct
ist gewoehnlich die aktuelle Datei, die ueberprueft wird; ein
viel beschaeftigtes System kann verschiedene Dateien erfordern,
diese Dateien, ausser die aktuelle, sind in
.B /usr/adm/pacct?
aufzufinden.
.SH OPTIONEN
.TP 12
.B \-b
Liest rueckwaerts, zeigt die letzten Kommandos zuerst.
.TP
.BI \-C \ time
Zeigt nur solche Prozesse, die die Zeit
.I time
ueberschreiten, welche die Gesamt-CPU-Zeit angibt.
.TP
.BI \-d \ mm/dd
Alle Zeitargumente
.IR time ,
die diesem Flag folgen, bewirken die Benutzung des angegebenen
Monats und Tags, anstelle der letzten 24 Stunden. Dies wird
fuer das Nachschauen in alten Dateien benoetigt.
.TP
.BI \-e \ time
Zeigt nur solche Prozesse, die zur oder vor der Zeit
.I time
existierten. Die Benutzung der gleichen Zeit
.I time
fuer
.B \-s
und
.B \-e
zeigt die Prozesse, die nur zur Zeit
.I time
existierten.
.TP
.B \-f
Angabe der
.BR fork/exec- Flag-
und der System-Exit-Statusspalten in der Ausgabe.
.TP
.BI \-g \ group
Zeigt nur Prozesse, die zur Gruppe
.I group
gehoeren.
\fIGroup\fP kann mit dem Gruppen-Id oder mit dem Gruppennamen
bezeichnet werden.
.TP
.B \-h
Zeigt anstelle von mean memory size, den Teil der insgesamt
verfuegbaren CPU-Zeit, der von dem Prozess waehrend seiner
Ausfuehrung benoetigt wurde. Dieser "hog"-Faktor wird wie folgt
berechnet:
.br
	(Gesamt-CPU-Zeit) / (verstrichene Zeit).
.TP
.BI \-H \ factor
Zeigt nur solche Prozesse an, die
.I factor
ueberschreiten, wobei factor der "hog"-Faktor ist, wie in der
Option
.B -h
erlaeutert.
.TP
.B \-i
Angabe von Spalten, die die E/A-Zaehler in der Ausgabe enthalten.
.TP
.B \-k
Zeigt anstelle der Speichergroesse, die Gesamt-kcore-Minuten.
.TP
.BI \-l \ line
Zeigt nur Prozesse, die zu Terminal
.I /dev/line
gehoeren.
.TP
.B \-m
Zeigt nur mean core size (ist Standard).
.TP
.BI \-n \ pattern
Zeigt nur Kommandos, die in das Muster
.I pattern
passen, das ein regulaerer Ausdruck (wie in
.BR ed (1))
sein kann, mit der Ausnahme, dass \fB+\fP ein ein- oder mehrmaliges
Auftreten bedeutet.
.TP
.BI \-O \ time
Zeigt nur solche Prozesse, bei denen die CPU-Zeit des Operationssystems
.I time ueberschreitet.
.TP
.B \-r
Zeigt den CPU-Faktor (Userzeit/(Systemzeit+Userzeit).
.TP
.BI \-s \ time
Zeigt nur solche Prozesse, die zu oder nach
.I time
existierten.
.I time wird in folgender Form angegeben:
.IB hr : min : sec \fR.\fP
.BI : sec
oder
.BI : min : sec
koennen entfallen.
.TP
.B \-t
Zeigt separate System- und Userzeiten.
.TP
.BI \-u \ user
Zeigt nur Prozesse, die zum Benutzer 
.I user
gehoeren. Dieser wird mit dem Benutzer-Id, dem Login-Namen (der
dann in das Benutzer-Id konvertiert wird), einem \fB#\fP (zur
Kennzeichnung solcher Prozesse, die mit den Privilegien des
.I Superusers
ausgefuehrt wurden) oder mit
.B ?
(zur Kennzeichnung solcher Prozesse, die mit unbekannten
Benutzer-Ids assoziieren) angegeben.
.TP
.B \-v
Unterdrueckt die Spaltenueberschriften in der Ausgabe.
.PP
Mehrere Listing-Optionen zusammen, haben die Wirkung eines logischen
\fIund\fR.
.SH DATEIEN
/etc/passwd
.br
/usr/adm/pacct
.br
/etc/group
.SH SIEHE AUCH
ps(1),
su(1),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctcon(M),
acctmerg(M),
acctprc(M),
acctsh(M),
fwtmp(M),
runacct(M).
.SH EINSCHRAENKUNGEN
.B Acctcom
bearbeitet nur beendete Prozesse; benutze
.BR ps (1)
fuer aktive Prozesse.
.br
.DT
