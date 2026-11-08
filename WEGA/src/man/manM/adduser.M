.TH ADDUSER M WEGA
.\"@[$]adduser.M	1.2
.ad
.fi
.SH NAME
adduser \- Benutzer im System einrichten
.SH SYNTAX
.B adduser
.SH BESCHREIBUNG
Diese Shell-Prozedur hilft dem Systemverwalter, neue Benutzer
in das System aufzunehmen. Sie aktualisiert die Dateien
.I /etc/passwd 
und 
.I /etc/group
und legt verschiedene Directorys und Dateien fuer den neuen
Benutzer an.
.B Adduser
ist interaktiv und fordert alle benoetigten Informationen an.
Dem neuen Benutzer wird eine uid zugeordnet, die um eins
hoeher als die zuletzt benutzte Nummer in der Datei
.I /etc/passwd
ist.
.PP
.B Adduser
fragt nach dem neuen Benutzernamen, dem Home-Directory,
der Login-Gruppe und weiteren Gruppennummern. Die Liste
der zusaetzlichen Gruppen wird von
.BR newgrp (1)
benutzt.
Ein Beispiel fuer
.B adduser
ist:
.sp 2
.nf
.na
.in +6
New user's name: hugo
Scanning for a uid number.
Home directory: /z/hugo
Login shell (/bin/csh is the default): <cr>
Default group at login ('other' is the default): <cr>
.fi
.ad
.sp
Enter groups OTHER THAN the default group that `hugo' should
belong to. Enter one group name per line, with a control-d to end.
If the user should belong only to her/his default group, just
type control-d to the question.
Group: END
.nf
.na
.sp
User:            hugo  
Uid:             17
Gid:             1
Dir:             /z/hugo
Login shell:     /bin/csh

Okay to add (yes or no)? yes
hugo: Added
New password:
Retype new password:
.in -6
.fi
.ad
.PP
Auf die Frage "Okay to add (yes or no)? "
werden alle Eingaben ausser "yes" und "y" als nein (no)
interpretiert.
.SH DATEIEN
/etc/adduser	dieses Programm
.br
/etc/group	Datei fuer die Gruppenzuordnung
.br
/etc/passwd	Passwortdatei
.br
/usr/spool/mail	Directory fuer die Mailbox
.SH SIEHE AUCH
mkdir(1),
passwd(1),
rmuser(M).
.SH DIAGNOSTIK
Ungueltige Eingaben werden zurueckgewiesen und neu angefordert.
.SH EINSCHRAENKUNGEN
Es existiert ein Zeitfenster, in dem sich zwei simultan laufende
Kopien von
.B adduser
gegenseitig stoeren koennen. Es wird keine Ueberpruefung
des Passwortes durchgefuehrt. Tritt ein Fehler auf (z.B. die
zwei Anfragen von
.BR passwd (1)
werden unterschiedlich beantwortet), wird der neue Benutzer
ohne ein Passwort installiert und die Sicherheit seiner Dateien
ist nicht gewaehrleistet.
.LP
.B Adduser
ist eine C-Shell-Prozedur und damit langsam.
