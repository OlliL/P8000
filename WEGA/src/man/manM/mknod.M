.TH MKNOD M WEGA
.\" @[$]mknod.M	1.2
.ad
.fi
.SH NAME
mknod \- Erstellen einer Geraetedatei (special file)
.SH SYNTAX
.B mknod
.I name
[\fBc\fR] [\fBb\fR]
.I major minor
.br
.B mknod
.I name
.B p
.SH BESCHREIBUNG
.B Mknod
erstellt einen Directory-Eintrag und das korrespondierende
Inode fuer eine Geraetedatei.
.PP
.B Mknod
kann auch benutzt werden, um FIFOs (benannte Pipes)
anzulegen (2. Teil der Syntax). Die erste Funktion kann nur
von einem Benutzer genutzt werden, der Mitglied der Systemgruppe
ist.
.PP
Das erste Argument
.I name
ist der Name des Eintrags, der zweite ist
.B b
fuer ein Blockgeraet oder
.B c
fuer ein Charactergeraet.
Die letzten beiden Argumente
.I major
und
.I minor
bezeichnen die Major- und Minor-Geraetenummer. Diese koennen
dezimal oder oktal angegeben werden. Eine fuehrende 0 kennzeichnet
eine oktale Zahl.
.PP
Die Zuordnung der Major-Nummer haengt von der Position des
Eintrags im Sprungverteiler des Kerns (conf.o) ab. Die
Major-Nummern der vorhandenen Treiber lauten wie folgt:

.nf
E/A-Geraet           Zeichengeraet       Blockgeraet 
                     Major Nummer        Major Nummer

md                        0                    0
fd                        1                    1
ud                        2                    -
mem                       6                    -
tty                       7                    -
sio                       8                    -
lp                        9                    -
udev1 (Anwendergeraet)   10                    2
udev2 (Anwendergeraet)   11                    3
udev3 (Anwendergeraet)   12                    4
udev4 (Anwendergeraet)   13                    5
udev5 (Anwendergeraet)   14                    6
udev6 (Anwendergeraet)   15                    7
.fi
.PP
Die Minor-Nummer ist vom E/A-Geraet abhaengig. Fuer die Hard-Disk
ist die Minor-Nummer die Nummer des Dateisystems. Die erste
Ziffer (dezimal) gibt die Laufwerknummer an, die zweite Ziffer
(dezimal) das n-te Dateisystem auf diesem Laufwerk. Fuer die
tty-Geraete kennzeichnet die Minor-Nummer den Kanal.
.SH DATEIEN
/etc/mknod	dieses Programm
.SH SIEHE AUCH
mknod(1),
mknod(2).
