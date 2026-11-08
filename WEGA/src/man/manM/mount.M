.TH MOUNT M WEGA
.\" @[$]mount.M	1.2
.ad
.fi
.SH NAME
mount, umount \- Einbinden und Loesen eines Dateisystems
.SH SYNTAX
.B mount
.I [special directory
[\fB\-r\fR]]
.br
.B umount
.I special
.SH BESCHREIBUNG
.B Mount
meldet dem System, dass ein Dateisystem auf dem E/A-Geraet
.I special
verfuegbar ist und eingebunden werden soll.
Das Directory
.I directory
muss bereits existieren, es wird dem root-Directory
des neu verbundenen Dateisystems zugeordnet.
.PP
Die Kommandos
.B mount
und
.B umount
verwalten eine Tabelle mit den verbundenen Geraeten.
Ohne  angegebene Argumente gibt
.B mount
diese Tabelle aus.
.PP
Das letzte optionale Argument zeigt an, dass ein
Nur-Lese-Dateisystem eingebunden werden soll.
Physisch schreibgeschuetzte Geraete und Magnetbanddateisysteme
(im P8000 nicht vorhanden) muessen mit dieser Option eingebunden
werden, sonst treten Fehler beim aktualisieren der Zugriffszeiten
auf, auch wenn kein expliziter Schreibzugriff erfolgt.
.PP
.B Umount
meldet dem System, dass die mount-Verbindung eines Dateisystems
auf dem Geraet
.I special
geloest werden soll.
.SH DATEIEN
/etc/mount	dieses Programm
.br
/etc/umount	dieses Programm
.br
/etc/mnttab	mount-Tabelle
.SH SIEHE AUCH
mount(2),
mnttab(5).
.SH DIAGNOSTIK
.B Mount
sendet eine Warnung aus, wenn das zu verbindende Dateisystem
bereits unter einem anderen Namen eingebunden ist.
.PP
.B Umount
gibt einen Fehler aus, wenn die Geraetedatei nicht verbunden
ist oder noch benutzt wird (d.h. noch "busy" ist). Das Dateisystem
ist "busy", wenn noch eroeffnete Dateien oder Arbeitsdirectorys
der Benutzer vorliegen.
.SH EINSCHRAENKUNGEN
Es werden einige Ueberpruefungen auf Gueltigkeit des Dateisystems
durchgefuehrt. Im allgemeinen ist es nicht moeglich, zerstoerte
und ungueltige Dateisysteme einzubinden.
