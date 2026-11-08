.TH FSCK M WEGA
.\" @[$]fsck.M	1.2
.ad
.fi
.SH NAME
fsck \- Konsistenzpruefung und Reparatur von Dateisystemen
.SH SYNTAX
.B fsck
.I [[\fB-fnsSty\fI] filesystem]\fR
.SH BESCHREIBUNG
.B  Fsck
prueft und repariert inkonsistente Bedingungen der angegebenen
Dateisysteme
.IR filesystems.
Ist ein Dateisystem konsistent (fehlerfrei), werden die Anzahl
der Dateien, die Anzahl der benutzten Bloecke und die Anzahl
der freien Bloecke ausgegeben. Ist das Dateisystem inkonsistent,
wird der Benutzer um Einverstaendnis gefragt, bevor jede Korrektur
durchgefuehrt wird. Die meisten Korrekturen haben Datenverluste
zur Folge, alle Verluste werden berichtet. Das Programm wartet
bei Korrekturen auf die Eingabe von `yes' oder `no'. Besteht
keine Schreiberlaubnis fuer das Dateisystem, stellt
.B fsck
die Option
.B -n
ein.
.PP
Sind dem Programm
.B fsck
keine Dateisysteme angegeben, benutzt es eine Liste von Dateisystemen,
die der Datei
.I /etc/checklist
entnommen werden.
.PP
.ne 10
Die Ueberpruefungen wird nach folgenden Fehlern durchgefuehrt:
.TP
1.
Bloecke, die von mehr als einem Inode oder der Freiblockliste
beansprucht werden.
.TP
2.
Bloecke, die von einem Inode oder der Freiblockliste beansprucht
werden und ausserhalb des Dateisystems liegen.
.TP
3.
Fehlerhafte Link-Zaehler.
.TP
4.
Ueberpruefung der Groessen:
.RS
Fehlerhafte Anzahl von Dateibloecken.
.br
Laenge des Directorys ist nicht ein vielfaches von 16 Bytes.
.RE
.TP
5.
Fehlerhaftes Inode-Format.
.TP
6.
Bloecke, die nirgendwo enthalten sind.
.TP
7.
Ueberpruefung der Directorys:
.RS
Diskrepanzen zwischen der Anzahl der Directory-Eintraege und
den Link-Zaehlern der Inodes.
.br
Inode-Nummern ausserhalb des zulaessigen Bereichs.
.RE
.TP
8.
Ueberpruefungen des Superblocks:
.RS
Mehr als 65536 Inodes.
.br
Mehr Inode-Bloecke als Bloecke im Dateisystem.
.RE
.TP
9.
Fehlerhaftes Format der Freiblockliste.
.TP
10.
Fehlerhafte Gesamtanzahl der Freibloecke und/oder des Zaehlers
der freien Inodes.
.PP
Verwaiste Dateien und Directorys (angelegte Inodes aber in
keinem Directory eingetragen) werden mit Einverstaendnis des
Benutzers, durch Eintragen in das Directory "lost+found", neu
verbunden.
Sie erhalten als Namen ihre Inode-Nummer.
Die einzige Einschraenkung ist, dass das Directory "lost+found"
bereits vor der Ueberpruefung des Dateisystems mit leeren Eintraegen
im root-Directory dieses Dateisystems vorhanden sein muss.
Leere Eintraege werden durch kopieren einer
grossen Anzahl von Dateien in das Directory und anschliessendem
Loeschen der Dateien erzeugt (siehe
.BR mlf (1)
und WEGA-Systemhandbuch fuer weitere Informationen ueber das Anlegen
der Directorys "lost+found").
.PP
Die Ueberpruefung des raw-Geraetes ist meistens schneller.
.SH OPTIONEN
.TP
.B  \-f
Es erfolgt ein Echo der Ausgaben des Kommandos zur Datei
.I /tmp.fsck.err.
Die Option wird nicht befolgt, wenn das Dateisystem, das ueberprueft
wird, /tmp ist oder /tmp nicht mit mount eingebunden ist.
.TP
.B  \-n
Alle Fragen werden selbstaendig mit "no" beantwortet.
.TP
.B  \-s
Ignoriere die aktuelle Freiblockliste und erstelle (unabhaengig)
eine neue Liste durch Neuschreiben des Superblocks des Dateisystems.
Das Dateisystem sollte fuer diese Aktion nicht mit mount eingebunden
sein, ansonsten ist dafuer zu sorgen, dass keine Aktivitaeten
im Dateisystem ablaufen und das System unmittelbar nach der
Ueberpruefung neu gestartet wird. Diese Vorbedingungen sind
notwendig, damit der alte fehlerhafte Superblock, der sich
im Kern befindet, nicht weiterhin benutzt oder zum Dateisystem
rueckgeschrieben wird.
.IP
Die Freiblockliste kann mit optimalem Interleaving-Faktor (Blockversatz)
durch folgende Angaben angelegt werden:
.RS
.IP
.BI \-s c : s
.sp
Versatz von
.I s
Bloecken im Zylinder, je Zylinder existieren
.I c
Bloecke.
.RE
.IP
Wenn
.I c:s 
nicht angegeben ist, werden die Werte verwendet, die beim Anlegen
des Dateisystems benutzt wurden. Fehlen diese Angaben, wird
.IR c =72,
.IR s =9
gesetzt. Beim P8000 betraegt s=1 und c=72, d.h. es wird kein
Versatz verwendet.
.TP
.B  \-S
Abhaengige Neuerstellung der Freiblockliste. Diese Option entspricht
.BR \-s
mit der Ausnahme, dass die Freiblockliste nur neu erstellt
wird, wenn keine Diskrepanzen im Dateisystem entdeckt wurden.
Diese Option ist nuetzlich, um die Freiblockliste eines
intakten Dateisystems fuer optimale Zugriffe neu zu organisieren.
.B \-S
setzt automatisch die Option
.BR \-n .
.TP
.B \-t
Kann
.B fsck
nicht genuegend Speicher fuer seine Tabellen reservieren, benutzt
es temporaere Dateien. Ist die Option
.B -t
angegeben, wird das naechste Argument als Name der temporaeren
Datei verwendet. Ohne die Option
.B \-t
fragt
.B fsck
danach, wenn es eine temporaere Datei benoetigt.
Diese Datei sollte nicht im zu testenden Dateisystem liegen.
Die Datei wird, falls sie nicht schon vorher existierte oder
es sich um eine Geraetedatei handelt, wieder geloescht, wenn
.B fsck
endet.
.TP
.B  \-y
Alle Fragen werden selbstaendig mit "yes" beantwortet.
.SH DATEIEN
/etc/fsck		dieses Programm
.br
/etc/checklist	enthaelt die Liste der zu ueberpruefenden Dateisysteme
.SH SIEHE AUCH
dcheck(M),
icheck(M),
filesys(5).
.SH EINSCHRAENKUNGEN
Die Inode-Nummern des Directorys "dot" "\ \fB.\fR\ " und des
Directorys "parent" "\ \fB..\ \fR" sollten in jedem Directory
auf Gueltigkeit ueberprueft werden.
.br
Die Option
.B \-b
des Kommandos
.BR icheck (M)
sollte verfuegbar sein.
