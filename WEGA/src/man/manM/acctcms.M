.TH ACCTCMS M WEGA
.\" @[$]acctcms.M	1.2
.ad
.fi
.SH NAME
acctcms \- Summierung der Kommandos aus den Prozessdatensaetzen
.SH SYNTAX
\fBacctcms\fR [\fIoptions\fR] \fIfiles\fR
.SH BESCHREIBUNG
.B Acctcms
liest eine oder mehrere Dateien
.IR files ,
normalerweise mit dem in
.BR acct (5)
beschriebenen Format, ein.
Es summiert alle Datensaetze fuer Prozesse, die mit gleichen
Kommandonamen ausgefuehrt wurden, sortiert diese und schreibt
sie zur Standardausgabe und benutzt dabei standardmaessig das
interne Summenformat.
.SH OPTIONEN
.TP
.B \-a
Ausgabe im ASCII-Format, anstelle des internen Summenformats.
Die Ausgabe enthaelt den Kommandonamen, die Anzahl der
Kommandoausfuehrungen, die Gesamt-kcore-Minuten,
die Gesamt-CPU-Minuten, die Gesamt-real-Minuten,
mean size (in K), mean-CPU-Minuten pro Aufruf und den
"hog"-Faktor", der in 
.BR acctcom (M)
beschrieben ist.
Die Ausgabe wird standardmaessig nach den Gesamt-kcore-Minuten
sortiert.
.TP
.B \-c
Sortiert nach der Gesamt-CPU-Zeit, anstelle der Gesamt-kcore-Minuten.
.TP
.B \-j
Kombiniert nur alle Kommandos, die von "***other" aufgerufen
wurden.
.TP
.B \-n
Sortiert nach der Anzahl der Kommandoaufrufe.
.TP
.B \-s
Alle folgenden Dateinamen sind bereits im internen Summenformat.
.SH BEISPIELE
Eine typische Kommandofolge fuer die Tagesabrechnung und fuer
eine Gesamtuebersicht aller ausgefuehrten Kommandos ist:
.PP
.RS
.nf
acctcms \|file \|.\|.\|. \|>today
cp \|total \|previoustotal
acctcms \|\-s \|today \|previoustotal \|>total
acctcms \|\-a \|\-s \|today
.fi
.RE
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcon(M),
acctmerg(M),
acctprc(M),
acctsh(M),
fwtmp(M),
runacct(M).
