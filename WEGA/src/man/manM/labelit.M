.TH LABELIT M WEGA
.\" @[$]labelit.M	1.2
.ad
.fi
.SH NAME
labelit \- Dateisystem mit Namen versehen
.SH SYNTAX
.B labelit
.I special [fsname volume
[\fB\-n\fR]]
.SH BESCHREIBUNG
.PP
.B Labelit
kann benutzt werden, um Namen fuer Dateisysteme, die nicht mit
.BR mount (M)
eingebunden sind,
zu vergeben. Ohne optionale Argumente gibt
.B labelit
diese Namen aus.
Die Option
.B \-n
kennzeichnet ein Magnetbandgeraet (im P8000 nicht benutzt).
Das Argument
.I fsname
entspricht dem Namen des mit 
.BR mount (M)
verbundenen Dateisystems (
.BR root ,
.BR /usr ,
usw.).
.PP
Das Argument
.I special
ist der Name des Diskbereichs (z.B.
.IR /dev/rmd0 ,
usw.)
.PP
.I fsname
und
.I volname
sind in den letzten 12 Byte des Superblocks gespeichert,
(char fsname[6], volname[6]).
.PP
.SH DATEIEN
/etc/labelit		dieses Programm
.br
/etc/log/filesave	Datei mit den Kopien der Namen
.SH SIEHE AUCH
filesys(5).
