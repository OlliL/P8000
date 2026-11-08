.TH DF M WEGA
.\" @[$]df.M	1.2
.ad
.fi
.SH NAME
df \- Anzahl der freien Diskbloecke ermitteln
.SH SYNTAX
.B df
[\fB\-t\fR] [\fB\-f\fR]
.I [file-systems]
.SH BESCHREIBUNG
.B Df
gibt die Anzahl der freien Bloecke und der freien Inodes von
Online-Dateisystemen
durch Ueberpruefen der Zaehler im Superblock aus.
Die Dateisysteme
.I file-systems\^
koennen mit dem Geraetenamen (z.B.
.IR /dev/md0 )
oder mit dem mount-Directory-Namen (z.B.
.IR /usr )
spezifiziert werden. Fehlt
die Namensangabe, wird der freie Speicherplatz aller mit mount
verbundenen Dateisysteme ausgegeben.
.PP
Das Flag
.B \-t
bewirkt, dass zusaetzlich die Anzahl, der in den Dateisystemen insgesamt
verfuegbaren Bloecke, ausgegeben wird.
.PP
Ist das Flag
.B \-f
angegeben, wird nur die Anzahl der freien Bloecke, die in der
Freispeicherliste enthalten sind, ausgegeben. (Freie Inodes
werden nicht angegeben.) Mit dieser Option benutzt 
.B df\^
das raw-Geraet.
.SH DATEIEN
/bin/df		dieses Programm
.br
/dev/md\(**
.br
/etc/mnttab
.SH SIEHE AUCH
fsck(1),
filesys(5),
mnttab(5).
