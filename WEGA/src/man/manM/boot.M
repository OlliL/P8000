.TH BOOT M WEGA
.\" @[$]boot.M	1.2
.ad
.fi
.SH NAME
boot  \- sekundaeres Bootstrap-Programm
.SH SYNTAX
.B boot
.SH BESCHREIBUNG
.B Boot
wird vom primaeren Bootstrap-Programm aufgerufen. Das primaere
Bootstrap-Programm befindet sich auf Block 0 des ersten Dateisystems
der Hard-Disk (die Datei
.I pb.image
in dem root-Directory entspricht dem Block 0)
oder unter den Namen
.I boot0.ud
oder
.I boot0.rm
auf der WEGA-Startdiskette (UDOS-Format).
Auf dieser Diskette befindet sich unter dem Namen
.I boot0.fd
auch der Block 0 fuer eine Diskette im WEGA-Dateisystemformat.
.PP
Die Hauptaufgabe von
.B boot
ist, den WEGA-Kern in den RAM zu laden und zu starten. Das
Programm
.B boot
ist aber auch in der Lage, unterschiedliche Stand-alone-Programme
von der Hard-Disk oder Floppy-Disk zu laden und ausfuehren
zu lassen.
.PP
.B Boot
muss in dem root-Directory des ersten Dateisystems der Systemplatte
vorhanden sein. In der augenblicklichen Konfiguration
der Platte ist das das Dateisystem
.I /usr
auf der logischen Platte
.I /dev/usr
bzw.
.I /dev/md0 .
Der Pfadname des sekundaeren Bootstrap-Programms ist damit
.I /usr/boot.
Das Programm befindet sich aber auch unter den Namen
.I boot
auf der WEGA-Startdiskette.
.PP
Mit
.BR sysgen (M).
kann ein neues sekundaeren Bootstrap-Programm generiert werden.
.SH DATEIEN
/usr/boot	dieses Programm
.SH SIEHE AUCH
init(M).
