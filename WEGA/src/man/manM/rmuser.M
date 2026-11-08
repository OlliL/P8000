.TH RMUSER M WEGA
.\"@ [$]rmuser.M	1.2
.ad
.fi
.SH NAME
rmuser \- Loeschen eines Benutzers aus dem System
.SH SYNTAX
.B rmuser
.I name
.SH BESCHREIBUNG
Diese Shell-Prozedur hilft dem Systemverwalter, Benutzer aus
dem System zu streichen. Es aktualisiert die Dateien
.I /etc/passwd 
und 
.I /etc/group
und erstellt eine Liste aller Dateien des ehemaligen Benutzers.
Geloescht wird nur die Mailbox des Benutzers. Die Prozedur
ist interaktiv und fragt nach allen benoetigten Informationen.
.SH DATEIEN
/etc/group	Gruppendatei,
.I name
wird in allen Gruppen geloescht
.br
/etc/passwd	Passwortdatei,
.IR name 's
Passwort wird geaendert
.br
.IR files.name	Liste
aller Dateien des Benutzers
.br
/etc/rmuser	dieses Programm
.SH SIEHE AUCH
passwd(1).
.SH DIAGNOSTIK
Wenn der Benutzer
.I name
in der Datei
.I /etc/passwd,
nicht gefunden wird, endet die Prozedur mit einer entsprechenden
Mitteilung.
.SH EINSCHRAENKUNGEN
Hatte der Benutzer Zugriff auf weitere Passworte (z.B. root,
bin), sollten diese auch geaendert werden.
.sp
Dateien des Benutzers
.I name
in nicht verbundenen Dateisystemen, werden nicht in der Datei
.RI files .name
erfasst.
.sp
Es wird nicht ueberprueft, ob dass Passwort erfolgreich geaendert
wurde.
