.TH DEVNM M WEGA
.\" @[$]devnm.M	1.2
.ad
.fi
.SH NAME
devnm \- Geraetenamen ermitteln
.SH SYNTAX
.B devnm
.I [names]
.SH BESCHREIBUNG
.B Devnm
identifiziert die Geraetedatei (spezial file), die dem
mit mount verbundenen Dateisystem mit dem Namen des Arguments
\fIname\fP zugeordnet ist.
.PP
Dieses Kommando wird meist in
.I /etc/rc_csh
zum Erzeugen eines mount-Tabelleneintrags fuer das Geraet
.I root
benutzt.
.SH BEISPIEL
Das Kommando:
.sp
.RS
/etc/devnm /
.RE
.sp
erzeugt
.sp
.RS
md2 /
.RE
.sp
falls
.B /
mit
.I /dev/md2
verbunden ist.
.SH DATEIEN
/etc/devnm	dieses Programm
.br
/dev/md\(**
.br
/etc/mnttab
.SH SIEHE AUCH
setmnt(M).
