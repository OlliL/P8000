.TH XQ M WEGA
.\" @[$]xq.M	1.2
.ad
.fi
.SH NAME
xq  \- Ausfuehren und Loeschen von Auftraegen des Druckerspoolers
.SH SYNTAX
.B xq
.I [modifiers] [options]
.SH BESCHREIBUNG
.B Xq
ist Teil des allgemeinen Warteschlangensystems, das einzelne
Eintraege in der Warteschlange ueberprueft und loescht. Der
Superuser und die Systemgruppe koennen alle Kommandos ausfuehren,
die in \fBxq\fR(1) beschrieben sind.
Mit der Angabe des Modifikators
\fB-o\fR als erstes Argument, koennen sie alle nicht eigenen
Dateien ueberpruefen.
.sp
\fBXq\fR akzeptiert zwei Arten von Argumenten, Modifikatoren und
Optionen. Pro
\fBxq\fR-Aufruf kann nur eine Option angegeben werden, aber
die Option kann mit einem oder beiden der zwei moeglichen Modifikatoren
angegeben werden. Die Modifikatoren muessen vor den Optionen, der
Modifikator \fB-o\fR vor dem Modifikator \fB-q\fR angegeben sein. 
.sp 
Ohne Argumente gibt
.B xq
eine Statusliste aller Warteschlangen und E/A-Geraete aus.
.sp
Zusaetzliche Optionen koennen zur Steuerung der Abarbeitung
von
.BR dqueuer (M),
der Warteschlangen und E/A-Geraete verwendet werden.
.SH MODIFIKATOREN
.TP 3
.B \-o
Obwohl der Benutzer nicht Eigner der Datei ist, kann er den
Auftrag ausfuehren lassen. Er muss aber Superuser oder Mitglied
der Systemgruppe sein. Dieser Modifikator muss das erste Argument
von \fBxq\fR sein.  
.TP 3
.BI \-q " queue:device"  
Gibt die Warteschlange und das E/A-Geraet an, an die der Auftrag
gerichtet wird.
.SH OPTIONEN
.TP 3
.B  \-b
Backup-E/A-Geraet, veranlasst ein E/A-Geraet zum Sichern der
aktuellen Seite seiner Ausgabe. Dies wird fuer den Fall verwendet,
dass der Drucker ausfaellt bzw. neues Papier benoetigt.
.TP 3
.BI \-d " SEQ#" 
Loeschen des noch nicht ausgedruckten Auftrags mit der laufenden
Nummer
.I SEQ#
aus der Warteschlange.
Jedem Auftrag wir eine laufende Nummer fuer spaetere Referenzen
bis zum Beginn des Ausdrucks zugeordnet. Die Ausfuehrung eines
.BR xq -Kommandos
ohne Optionen gibt diese Nummern in der ersten Spalte der Ausgabe
aus.
.TP 3
.B  \-Dd
Down device.
Setzt ein E/A-Geraet in den Zustand "DOWN". Diese Option kann
benutzt werden, um ein Geraet zeitweilig ausser Betrieb zu
setzen. Die Ausgabe der aktuellen Datei wird zu Ende gebracht,
neue Ausgaben aber nicht mehr ausgefuehrt.
.TP 3
.B  \-Dq
Down queue.
Setzt eine Warteschlange in den Zustand "DOWN". Auftraege an
die Warteschlange werden in diesem Zustand nicht angenommen,
die E/A-Geraete der Warteschlange sind nicht zugaenglich. Dieses
Kommando sollte nur ausgefuehrt werden, wenn alle Geraete der
Warteschlange fuer eine laengere Zeit ausser Betrieb gesetzt
werden sollen.
.TP 3
.B  \-r
Restart \fIqueue:device\fR.
Bewirkt einen Neustart der Ausgabe der aktuellen Datei
von Beginn an. Dieses Kommando wird benutzt, wenn der Drucker
ausgefallen war.
.TP 3
.BR  \-s kd
Stop des Ausdruckens.
Dies bewirkt, dass die Ausgabe des angegebenen Geraetes unverzueglich
gestoppt wird. Ist
.I k 
angegeben, bleibt die aktuelle Datei in der Warteschlange,
anderenfalls wird sie geloescht. Ist
.I d 
angegeben, wird das E/A-Geraet in den Zustand "DOWN" versetzt,
um zeitweilig die Ausgabe weiterer Dateien zu verhindern.
.TP 3
.B  \-Ud
Up device.
Ein E/A-Geraet wird wieder fuer Ausgaben zugelassen (siehe \fB-Dd\fR).
.TP 3
.B  \-Uq
Up queue.
Eine Warteschlange wird wieder fuer Druckerauftraege zugelassen
( siehe \fB-Dq\fR).
.SH BEISPIELE
.nf
                       \fB xq -d\fR SEQ# 
.fi
.ad
.sp
Das obere Beispiel loescht einen noch nicht ausgefuehrten Auftrag
mit der laufenden Nummer SEQ# aus der Warteschlange.
.sp
.nf
                       \fB xq -q\fR lpr:1 \fB-sk\fR 
.ad
.fi
.sp
Dieses Beispiel besagt: Die Ausgabe, der augenblicklich der
lpr-Warteschlange, Geraet 1 zugeordneten Datei, wird gestoppt, der Auftrag
bleibt aber in der Warteschlange bestehen.
.bp
Die Kommandos fuer das Sperren und Zulassen der E/A-Geraete
lauten:
.nf
.sp
                  \fBxq -q \fIque:dev \fB-Dd\fR

                  \fBxq -q \fIque:dev \fB-Ud\fR
.fi
.sp
Die Kommandos fuer das Sperren und Zulassen einer Warteschlange
lauten:
.nf
.sp
                   \fBxq -q \fIque \fB-Dq\fR 
   
                   \fBxq -q \fIque \fB-Uq\fR
.fi
.SH DATEIEN
/usr/bin/xq				dieses Programm
.br
/usr/spool/queuer/activeconfig	aktive Konfigurationsdatei
.br
/usr/spool/queuer/logfile
.br
/usr/spool/queuer/statusdir
.br
/usr/spool/queuer/requestdir
.br
/tmp/queuer
.SH SIEHE AUCH
nq(1),
xq(1),
backend(M),
dqueuer(M).
.SH DIAGNOSTIK
Hat ein Bestandteil des Spooler-Systems den Anschein, dass
er inkonsistent ist, gibt
.BR xq
eine Fehlernachricht aus und traegt den Fehler in die Error-Log-Datei
des Spoolers ein.
