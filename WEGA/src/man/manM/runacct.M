.TH RUNACCT M WEGA
.\" @[$]runacct.M	1.2
.ad
.fi
.SH NAME
runacct \- Ausfuehren der Tagesabrechnung
.SH SYNTAX
.B runacct
.I [mmdd [state]]
.SH BESCHREIBUNG
.B Runacct
ist die hauptsaechliche Tagesabrechnungs-Shell-Prozedur. Sie
wird normalerweise ueber
.BR cron (M)
initialisiert.
.B Runacct
bearbeitet Login-, Gebuehren-, Disk- und Prozessabrechnungsdateien.
Es bereitet auch Summendateien fuer
.B prdaily
oder Rechnungen auf.
.PP
.B Runacct
passt auf, um aktive Abrechnungs- oder Summendateien
im Fehlerfall nicht zu zerstoeren.
Es zeichnet seinen Programmablauf durch entsprechende Mitteilungen,
die in die Datei
.I active
geschrieben werden, auf.
Wird ein Fehler festgestellt, wird eine Mitteilung  zu
.IR /dev/console ,
eine Nachricht (siehe 
.BR mail (1))
zu
.BR wega \ und \ adm
geschickt und
.B runacct
endet.
.B Runacct
benutzt eine Reihe von Verriegelungsdateien, um sich vor einem
Neuaufruf zu schuetzen.
Die Dateien
.I lock
und 
.I lock1
werden zum Verhindern von simultanen Aufrufen, die
Datei
.I lastdate
zur Verhinderung von mehr als einem Aufruf pro Tag benutzt.
.PP
.B Runacct
teilt seine Verarbeitung in separate, restartfaehige
.I states
und benutzt
.IR statefile ,
um sich an den letzten beendeten
.I state
zu erinnern. Dies geschieht durch Schreiben des Namens von
.I state
nach
.BR statefile .
.PP
.B Runacct
schaut in
.I statefile
nach, um zu ermitteln, was es getan hat und
was es als naechstes zu verarbeiten hat.
.I States
werden in folgender Reihenfolge ausgefuehrt:
.LP
.TP 12
.B SETUP
Kopieren der aktiven Abrechnungsdateien in Arbeitsdateien.
.TP 12
.B WTMPFIX
Vergleichen der Integritaet der Datei
.B wtmp
und, wenn notwendig, Korrektur der Datumsaenderungen.
.TP 12
.B CONNECT1
Erzeugt Login-Datensaetze im
.IR ctmp.h -Format.
.TP 12
.B CONNECT2
Konvertiert
.IR ctmp.h -Datensaetze
ins
.IR tacct.h -Format.
.TP 12
.B PROCESS
Konvertiert Prozessabrechnungsdatensaetze ins
.IR tacct.h -Format.
.TP 12
.B MERGE
Kombinieren von Login- und Prozessabrechnungsdatensaetzen.
.TP 12
.B FEES
Konvertiert die Ausgabe von
.B chargefee
ins
.IR tacct.h -Format
und kombiniert mit Login- und Prozessabrechnungsdatensaetzen.
.TP 12
.B DISK
Kombinieren von Diskabrechnungsdatensaetzen mit Login-,
Prozess- und Gebuehrenabrechnungsdatensaetzen.
.TP 12
.B MERGETACCT
Kombiniert die Gesamttagesabrechnungsdatensaetze der Datei
.I daytacct
mit den Gesamtsummenabrechnungsdatensaetzen in
.IR /usr/adm/acct/sum/tacct .
.TP 12
.B CMS
Erzeugt eine Zusammenfassung aller Kommandos.
.TP 12
.B USEREXIT
Jedes beliebige installationsabhaengige Abrechnungsprogramm
kann hier eingefuegt werden.
.TP 12
.B CLEANUP
Saeubern der temporaeren Dateien und Exit.
.PP
Um 
.B runacct
nach einem Fehlerabbruch neu starten zu koennen, muss zuerst
die Datei
.B active
zur Fehlerdiagnose ueberprueft werden. Anschliessend muessen
alle fehlerhaften Datendateien, wie
.IR pacct \ oder \ wtmp ,
repariert werden. Die Dateien
.B lock
und
.B lastdate
muessen geloescht werden, bevor
.B runacct
neu gestartet werden kann.
Das Argument
.I mmdd
ist notwendig, wenn
.B runacct
neu gestartet wird. Es spezifiziert den Monat und den Tag,
fuer den
.B runacct
die Abrechnung neu ausfuehrt. Der Eintrittspunkt fuer die Verarbeitung
wird auf Basis des Inhalts der Datei
.I statefile
ermittelt. Um diesen zu ueberschreiben, fuege den gewueschten
.I state
in die Kommandozeile ein, um den Beginn der Abarbeitung zu
kennzeichnen.
.SH BEISPIELE
Zum Starten von
.BR runacct :
.RS 5
nohup runacct 2> /usr/adm/acct/nite/fd2log &
.RE
.PP
Zum Neustart (Restart) von
.BR runacct :
.RS 5
nohup runacct 0601 2>> /usr/adm/acct/nite/fd2log &
.RE
.PP
Zum Neustart von
.B runacct
an einem spezifischen
.IR state :
.RS 5
nohup runacct 0601 MERGE 2>> /usr/adm/acct/nite/fd2log &
.RE
.SH DATEIEN
/usr/lib/acct/runacct		dieses Programm
.br
/usr/adm/wtmp
.br
/usr/adm/pacct[1-9]
.br
/usr/src/cmd/acct/tacct.h
.br
/usr/src/cmd/acct/ctmp.h
.br
/usr/adm/acct/nite/active
.br
/usr/adm/acct/nite/daytacct
.br
/usr/adm/acct/nite/lock
.br
/usr/adm/acct/nite/lock1
.br
/usr/adm/acct/nite/lastdate
.br
/usr/adm/acct/nite/statefile
.br
/usr/adm/acct/nite/ptacct[1-9].\fImmdd\fR
.SH SIEHE AUCH
acctcom(M),
acct(2),
acct(5),
utmp(5),
acct(M),
acctcms(M),
acctcon(M),
acctmerg(M),
acctprc(M),
acctsh(M),
cron(M),
fwtmp(M).
.br
.I PWB/WEGA-Abrechnungssystem
in WEGA-Dienstprogramme
.SH DIAGNOSTIK
Selbsterklaerend.
.SH EINSCHRAENKUNGEN
Normalerweise sollte ein Neustart von
.B runacct
in den
.B SETUP
.I state
vermieden werden. Lasse
.B SETUP
manuell laufen und vollziehe den Neustart ueber:
.PP
.RS 5
.B runacct
.I mmdd
.B WTMPFIX
.RE
.PP
Erfolgt ein Fehlerabbruch von
.B runacct
im
.B PROCESS
.IR state,
loesche die letzte Datei
.BR ptacct ,
da sie nicht vollstaendig ist.
