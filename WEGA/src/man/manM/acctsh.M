.TH ACCTSH M WEGA
.\" @[$]acctsh.M	1.2
.ad
.fi
.SH NAME
acctsh \- Shell-Prozeduren fuer die Abrechnung
.SH SYNTAX
.B chargefee
.I login-name number
.PP
.B ckpacct
.I [blocks]
.PP
.B dodisk
.PP
.B lastlogin
.PP
.B monacct
.I number
.PP
.B nulladm
.I file
.PP
.B prctmp
.PP
.B prdaily
.PP
.B prtacct
.I file [heading]
.PP
.B runacct
.I [mmdd] [mmdd state]
.PP
.B shutacct
.I [reason]
.PP
.B startup
.PP
.B turnacct
[\fBon\fR
|
.B off
|
.BI switch ]
]
.SH BESCHREIBUNG
.B Chargefee
wird zum Anrechnen der Kosten
.I number
zu Lasten von
.I login-name
aufgerufen. Ein Datensatz wird zu
.I /usr/adm/fee
geschrieben. Dieser kann mit anderen Abrechnungsdatensaetzen
kombiniert werden.
.PP
.B Ckpacct
wird ueber
.B cron
initialisiert. Es ueberprueft periodisch die Groesse von
.IR /usr/adm/pacct .
Ueberschreitet die Groesse \fIblocks\fP, der Standardwert ist
1000, wird 
.B turnacct
mit dem Argument
.I switch
aufgerufen.
.PP
.B Dodisk
wird von
.B cron
aufgerufen. Es fuehrt Disk-Abrechnungsfunktionen aus.
.PP
.B Lastlogin
wird von
.B runacct
zum Aktualisieren der Datei
.IR /usr/adm/acct/sum/loginlog ,
die das Datum des letzten Logins eines jeden Benutzers enthaelt,
aufgerufen.
.PP
.B Monacct
sollte einmal pro Monat oder Abrechnungsperiode aufgerufen
werden.
.I Number
zeigt an, um welchen Monat oder welche Abrechnungsperiode es sich
handelt. Das Programm erzeugt Summendateien in
.I /usr/adm/acct/fiscal
und startet die Summendatei in
.I /usr/adm/acct/sum
neu.
.PP
.B Nulladm
erzeugt
.I file
mit den Zugriffsrechten 644 und dem Eigner
.BR adm .
Das Programm wird von
.BR lastlogin ,
.BR runacct ,
und
.B turnacct
aufgerufen.
.PP
.B Prctmp
kann zum Ausdrucken der Sitzungsdatensatzdatei (standardmaessig
.IR /usr/adm/acct/nite/ctmp ),
die von
.B acctcon1
erzeugt wurde, benutzt werden (siehe
.BR acctcon (M)).
.PP
.B Prdaily
wird von
.B runacct
zum Erstellen eines Berichts ueber fruehere Tagesabrechnungen
aufgerufen. Der Bericht befindet sich in
.I /usr/adm/acct/sum/rprtxxxx,
dabei ist
.I xxxx
der Monat und Tag des Berichts.
Die Tagesabrechnungsberichte koennen sooft wie gewuenscht
mit folgendem Kommando ausgegeben werden:
.sp
.RS
\fBcat /usr/adm/acct/sum/rprt\(**\fR
.RE
.sp
Sie muessen explizit geloescht werden, wenn sie nicht mehr
benoetigt werden.
.PP
.B Prtacct
kann zum Formatieren und zur Ausgabe jeder Abrechnungsdatei
benutzt werden.
.PP
.B Runacct
fuehrt die Ansammlung der Login-, Prozess-, Gebuehren-,
und Disk-Abrechnung auf Tagesbasis aus. Es erzeugt auch eine
Summierung der Kommandobenutzung. Fuer weitere Informationen
siehe \fIrunacct\fP(M).
.PP
.B Shutacct
sollte beim Abfahren des Systems zum Abschalten der Prozessabrechnung
und zum Anfuegen eines "Grund-" ("reason") Datensatzes an
.IR /usr/adm/wtmp ,
benutzt werden.
.PP
.B Startup
sollte durch
.BR rc (M)
zum Einschalten der Abrechnung benutzt werden, wenn das System
in den Multi-User-Mode gefahren wird.
.PP
.B Turnacct
ist ein Interface zu
.B accton
(siehe
.BR acct (M))
zum Setzen der Prozessabrechnung auf \fBon\fP (ein) oder \fBoff\fP
(aus).
Das Argument
.B switch
setzt die aktuelle 
.I /usr/adm/pacct
auf den naechsten freien Namen in
.IR /usr/adm/pacct[1-9] ,
schaltet die Abrechnung aus und anschliessend wieder ein.
Diese Prozedur wird von
.B ckpacct
ueber \fBcron\fP 
zum Verkleinern der Dateigroesse von \fIpacct\fP aufgerufen.
.SH DATEIEN
.ta \w'/usr/adm/pacct[1-9]\ \ \ \ 'u +.5i
.nf
/usr/adm/fee	Akkumulator fuer Gebuehren
.sp
/usr/adm/pacct	aktuelle Datei fuer Prozessabrechnung
.sp
/usr/adm/pacct[1-9]	benutzt, wenn pacct zu gross wird und
	waehrend der Tagesabrechnungsprozedur
.sp
/usr/adm/wtmp	Login/Logoff-Summierung
.sp
/usr/adm/wtmp[1-9]	benutzt waehrend der Tagesabrechnungsprozedur
.sp
/usr/adm/acct/nite	Arbeitsdirectory
.sp
/usr/lib/acct	enthaelt alle Abrechnungskommandos,
	die in der Sektion M beschrieben sind
/usr/adm/acct/sum	Summendirectory, sollte geschuetzt werden
.fi
.DT
.SH SIEHE AUCH
acct(M),
acctcms(M),
acctcom(M),
acctcon(M),
acctmerg(M),
acctprc(M),
fwtmp(M),
runacct(M),
acct(2),
acct(5),
utmp(5).
