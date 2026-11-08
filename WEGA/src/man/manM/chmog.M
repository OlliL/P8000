.TH CHMOG M WEGA
.\"@[$]chmog.M	1.2
.ad
.fi
.SH NAME
chmog, chog \- Zugriffsrechte / Eigner / Gruppe einer Datei aendern
.SH SYNTAX
.B chmog
.I octal-mode user group file
.sp
.B chog
.I user group file
.SH BESCHREIBUNG
.B Chmog
aendert die Zugriffsrechte, den Eigner und die Gruppenzugehoerigkeit
der angegebenen Dateien
.I files
zu
.I octal-mode, 
.I user,
und
.I group.
.B Chog
aendert den Eigner und die Gruppenzugehoerigkeit der angegebenen
Dateien
.I files.
Die Argumente
.I user
und
.I group
koennen durch das Benutzer-Id (uid), Gruppen-Id (gid) oder
durch den Benutzer- oder Gruppennamen angegeben werden.
.SH DATEIEN
/etc/chmog	dieses Programm
.br
/etc/chog		dieses Programm
.br
/etc/passwd
.br
/etc/group
.SH SIEHE AUCH
chmod(1),
chmod(2),
chgrp(1),
chown(M).
