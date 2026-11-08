.TH QUOT M WEGA
.\" @[$]quot.M	1.2
.ad
.fi
.SH NAME
quot \- Summieren der Dateisystembelegung durch die Benutzer
.SH SYNTAX
.B quot
.I [\fB-ncf\fI] filesystems\fR
.SH BESCHREIBUNG
.B Quot
gibt die Anzahl der von den einzelnen Benutzern belegten
Bloecke der angegebenen Dateisysteme
.I filesystems
aus.
.I Filesystems
kann mit den Geraetenamen oder den Namen der mount-Directorys
angegeben werden.
.SH OPTIONEN
.TP
.B \-n
Zum Erstellen einer Liste aller Dateien und ihrer Eigner benutze
die Pipeline:
.br
.sp
ncheck filesystem | sort +0n | quot \-n filesystem
.br
.sp
Beachte, dass die Option
.B \-n
ohne diese Kommandokette bedeutungslos
ist.
.TP
.B \-c
Ausgabe von drei Spalten, die die Dateigroesse in Bloecken, die Anzahl
der Dateien dieser Groesse und die aufsummierte Anzahl der
Bloecke angeben.
.TP
.B \-f
Ausgabe der Anzahl der Dateien jedes Benutzers und der Speicherbelegung
durch jeden Benutzer.
.SH DATEIEN
/usr/bin/quot	dieses Programm
.br
/etc/passwd	um die Benutzernamen zu ermitteln
.SH SIEHE AUCH
ls(1),
du(1).
.SH EINSCHRAENKUNGEN
Luecken in Dateien werden so gezaehlt, als wenn sie tatsaechlich
Speicherplatz belegen.
.sp
.B Quot
ignoriert uid-Nummern groesser 1000.
