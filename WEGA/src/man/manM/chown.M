.TH CHOWN M WEGA
.\"@[$]chown.M	1.2
.ad
.fi
.SH NAME
chown, chgrp \- Eigner / Gruppe einer Datei aendern
.SH SYNTAX
.B chown
.I owner file 
.PP
.B chgrp
.I group file
.SH BESCHREIBUNG
.B Chown
aendert das Eignerkennzeichen der Datei
.I files
zu
.IR owner .
Die Angabe kann mit einer dezimalen uid oder mit dem Login-Namen,
der in der Passwortdatei gefunden wird, erfolgen.
.PP
.B Chgrp
aendert die Gruppenzugehoerigkeit gid der Datei
.I files
zu
.IR group .
Die Angabe kann mit einer dezimalen gid oder mit dem Gruppennamen,
der in der Gruppendatei gefunden wird, erfolgen.
.SH DATEIEN
/etc/chown	dieses Programm
.br
/etc/chgrp	dieses Programm
.br
/etc/passwd	Benutzernamen und Benutzer-Id (uid)
.br
/etc/group	Gruppennamen und Gruppen-Id (gid)
.SH SIEHE AUCH
chown(2),
passwd(5),
group(5),
chmog(M).
