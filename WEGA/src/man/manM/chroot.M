.TH CHROOT M WEGA
.\" @[$]chroot.M	1.2
.ad
.fi
.SH NAME
chroot \- Wechseln des root-Directorys fuer ein Kommando
.SH SYNTAX
.B chroot
.I newroot
.I command
.SH BESCHREIBUNG
Das Kommando
.I command
wird relativ zur neuen Wurzel (root) ausgefuehrt. Die Bedeutung
jedes ersten Schraegstriches (\fB/\fR)
im Pfadnamen wird fuer das Kommando
.I command 
und dessen Sohnprozesse in
.I newroot
geaendert. Das initialisierte Arbeitsdirectory ist
.I newroot\fR.
Der neue root-Pfadname ist immer relativ zur augenblicklichen
Wurzel. Auch wenn gerade ein
.B chroot
wirksam ist, ist das Argument
.I newroot
relativ zur augenblicklichen Wurzel des laufenden Prozesses.
.PP
Dieses Kommando ist nur vom Superuser nutzbar.
.SH BEISPIEL
Das Kommando:
.PP
.RS
\fBchroot \fInewroot command > newfile\fR
.RE
.PP
erzeugt die Datei
.I newfile
relativ zur originalen Wurzel.
.SH DATEIEN
/etc/chroot	dieses Programm
.SH SIEHE AUCH
chdir(2).
.SH EINSCHRAENKUNGEN
Vermeide extreme Bedingungen bei der Benutzung von Geraetedateien
im neuen root-Dateisystem.
