.TH ACCTMERG M WEGA
.\" @[$]acctmerg.M	1.2
.ad
.fi
.SH NAME
acctmerg \- Kombinieren oder Addieren von Gesamtabrechnungsdatensaetzen
.SH SYNTAX
.B acctmerg
[\fB-aiptuv\fR] [\fIfile\fR]
.SH BESCHREIBUNG
.B Acctmerg
liest seine Standardeingabe und bis zu neun zusaetzliche Dateien,
die alle im Format
.B tacct
(siehe
.BR acct (5))
oder in der ASCII-Version vorliegen.
Es kombiniert diese Eingaben durch Addition der Datensaetze,
deren Schluessel (standardmaessig Benutzer-Id und Name) identisch
sind und erwartet, dass die Eingabe nach diesem Schluessel
sortiert ist.
.SH OPTIONEN
.TP
.B \-a
Erzeugt die Ausgabe in der ASCII-Version von
.BR tacct .
.TP
.B \-i
Eingabedateien sind in der ASCII-Version von
.BR tacct .
.TP
.B \-p
Ausgeben der unverarbeiteten Eingabe.
.TP
.B \-t
Erzeugt fuer alle Eingaben einen einzigen Datensatz.
.TP
.B \-u
Summieren nach dem Benutzer-Id, anstelle von Benutzer-Id und
Namen.
.TP
.B \-v
Erzeuge die Ausgabe im ausfuehrlichen ASCII-Format mit genauerer
Notation fuer Gleitkommazahlen.
.SH BEISPIELE
Die folgende Kommandofolge ist fuer die "Reparatur" jeder beliebigen Datei
in diesem Format nuetzlich:
.PP
.RS
.nf
acctmerg \|\-v \|<file1 \|>file2
	\f2Editiere file2 wie gewuenscht\fP .\|.\|.
acctmerg \|\-i \|<file2 \|>file1
.fi
.RE
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctcon(M),
acctprc(M),
acctsh(M),
fwtmp(M),
runacct(M).
