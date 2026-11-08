.TH INSTALL M WEGA
.\" @[$]install.M	1.2
.ad
.fi
.SH NAME
install \- Installieren von Kommandos
.SH SYNTAX
\fBinstall\fR
.br
	[\fB\-c\fI dira\fR]
.br
	[\fB\-f\fI dirb\fR]
.br
	[\fB\-i\fR]
.br
	[\fB\-n\fI dirc\fR]
.br
	[\fB\-o\fR]
.br
	[\fB\-s\fR]
.br
	\fIfile\fR
.br
	[\fIdirx\fR \.\.\.]
.SH BESCHREIBUNG
.B Install
wird meist in "makefiles" (siehe
.BR make (1))
zum Installieren einer Datei
.I file
(aktualisierte Zieldatei) an einem spezifischen Platz im Dateisystem
benutzt.
Jede Datei
.I file
wird durch Kopieren in das entsprechende Directory unter Beibehaltung
der Zugriffsrechte und des Eigners des Originalkommandos installiert.
Das Programm gibt Nachrichten, die exakt mitteilen, welche
Datei in welchem Directory erzeugt oder ersetzt wurde, aus.
.PP
Sind keine Optionen oder Directorys
.RI ( dirx " .\|.\|.)"
angegeben, sucht
.B install
(mit
.BR find (1))
in verschiedenen Directorys (
.I  /bin, /usr/bin, /etc, /lib,
und
.I /usr/lib ,
in dieser Reihenfolge)
nach einer Datei mit dem selben Namen wie
.IR file .
Ist die erste Datei gefunden, gibt
.B install
eine Nachricht aus, die besagt, dass es diese Datei mit
.I file
ueberschreibt und fuehrt dieses dann aus.
Wird die Datei nicht gefunden, endet das Programm ohne weitere
Aktionen, nachdem es eine entsprechende Mitteilung ausgegeben
hat.
.PP
Sind eine oder mehrere Directorys
.RI ( dirx " \.\.\.)"
hinter
.I file
angegeben, werden diese vor den Directorys durchsucht, die
in der Standardliste enthalten sind.
.SH OPTIONEN
.TP 15
.BI \-c "  dira"
installiert ein neues Kommando im Directory
.IR dira .
Es sucht die Datei
.I file
in
.I dira
und installiert das Kommando nur, wenn die Datei noch nicht vorhanden
ist. Existiert die Datei bereits, wird dies mitgeteilt und
.B install
endet ohne die Datei zu ueberschreiben.
Die Option kann fuer sich allein oder mit der Option
.B \-s
benutzt werden.
.TP 15
.BI \-f "  dirb"
installiert die Datei
.I file
im angegebenen Directory unabhaengig davon, ob die Datei bereits
existiert. Existiert sie noch nicht, werden die Zugriffsrechte auf
\fB751\fR und der Eigner zu \fBbin\fR gesetzt.
Existiert die Datei bereits, werden die Zugriffsrechte und der
Eigner von dieser
Datei uebernommen. Die Option kann fuer sich allein oder mit
den Optionen
.B \-o
oder
.B \-s
benutzt werden.
.TP 15
.B \-i
ignoriert die Liste der Standard-Directorys und sucht die Datei
nur in den angegebenen Directorys
.RI ( dirx " \.\.\.)."
Die Option kann fuer sich allein oder mit anderen Optionen,
ausser
.B \-c
und
.BR \-f ,
benutzt werden.
.TP 15
.BI \-n "  dirc"
Wenn die Datei
.I file
in keiner der durchsuchten Directorys gefunden wird, wird sie
in dem Directory
.I dirc
installiert.
Die Zugriffsrechte und der Eigner der neuen Datei werden auf
\fB755\fR und \fBbin\fR gesetzt.
Die Option kann fuer sich allein oder mit anderen Optionen,
ausser
.B \-c
und
.BR \-f ,
benutzt werden.
.TP 15
.B \-o
Wird die Datei
.I file
gefunden, so wird diese durch Kopieren in die Datei
.BI OLD file
des gleichen Directorys gesichert.
Die Option kann fuer sich allein oder mit anderen Optionen,
ausser
.BR \-c ,
benutzt werden.
.TP 15
.B \-s
Nachrichten, ausser Fehlermeldungen, werden unterdrueckt. Die
Option kann fuer sich allein oder in Verbindung mit allen anderen
Optionen benutzt werden.
.SH DATEIEN
/etc/install	dieses Programm
.SH WARNUNG
.B Install
ist ein
Bourne-Shell-Skript. Ist die Login-Shell die
C-Shell, muss
.B install
von
.I makefile
wie folgt aufgerufen werden:
.sp
.RS
/bin/sh /etc/install file
.RE
.sp
Dies kann auch leicht durch eine Makrodefinition fuer 
.B make
erfolgen. Zum Beispiel
.sp
.RS
INS=/bin/sh /etc/install
.RE
.sp
und 
.sp
.RS
$(INS) file
.RE
.sp
zum Ausfuehren von
.BR install .
.sp
Dies alles ist nicht notwendig, wenn die Login-Shell die
.I Bourne-Shell
ist, wenn
.B install 
vom Kommandoniveau dieser Shell benutzt wird oder
.B install
von einem Kommandoskript gestartet wird.
