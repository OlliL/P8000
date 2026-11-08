.TH MLF M WEGA
.\" @[$]boot.M	1.2
.ad
.fi
.SH NAME
mlf  \- Directorys lost+found anlegen
.SH SYNTAX
.B mlf filesystems
.SH BESCHREIBUNG
Die Shell-Prozedur
.B mlf
ermoeglicht dem Superuser, die Directorys
.I lost+found
in den Dateisystemen
.I filesystems
anzulegen.
Fuer
.I filesystems
werden die Namen der Dateisysteme  angegeben
(z.B. mlf / /usr). Die Dateisysteme muessen zuvor mit
.BR mount (M)
verbunden worden sein. Das Directory
.I lost+found
wird einmalig in jedem Dateisystem nach dessen Einrichtung mit
.BR mkfs (M)
erstellt.
.PP
In jedem Dateisystem sollte das Directory
.I lost+found
vorhanden sein. Es reserviert Eintraege fuer maximal 158
Dateien. Das Programm
.BR fsck (M)
traegt in dieses Directory alle Dateien ein, die keinem
Directory zugeordnet sind.
.SH DATEIEN
.sp
/etc/mlf	dieses Programm
.br
lost+found
.SH SIEHE AUCH
WEGA-Systemhandbuch.
