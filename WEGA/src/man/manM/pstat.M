.TH PSTAT M WEGA
.\" @[$]pstat.M	1.2
.ad
.fi
.SH NAME
pstat \- Ausgabe von Systemfakten
.SH SYNTAX
.B pstat
[\fB\-aixptuf\fR]
.I [suboptions] [file]
.SH BESCHREIBUNG
.B Pstat
interpretiert den Inhalt verschiedener Systemtabellen.
Ist
.I file
angegeben, werden die Tabellen der Datei entnommen, anderenfalls
von
.I /dev/mem
und
.I /dev/kmem
gelesen.
(Systemkern-Abzuege (cores) werden gegenwaertig nicht unterstuetzt).
Die erforderliche Namensliste wird
.I /wega
entnommen.
Optionen sind:
.TP \w'WCHAN\ 'u
.B \-a
Unter
.BR \-p ,
beschreibe alle Prozess-Slots,
auch die, die gerade nicht aktiv sind.
.TP
.B \-i
Ausgabe der Inode-Tabelle mit diesen Ueberschriften:
.IP LOC
Die Kernadresse dieses Tabelleneintrags.
.PD 0
.IP FLAGS
Verschiedene Statusvariablen, die wie folgt kodiert sind:
.RS
.IP L
verriegelt (locked)
.IP U
Aktualisierungszeit (update time)
.BR (filsys (5))
muss korrigiert werden.
.IP A
Zugriffszeit (access time) muss korrigiert werden.
.IP M
Dateisystem ist hier mit mount verbunden.
.IP W
benoetigt von einem anderen Prozess (Flag L ist ein)
.IP T
enthaelt eine Text-Datei
.IP C
Aenderungszeit (changed time) muss korrigiert werden.
.RE
.IP CNT
Anzahl der Eintraege in der Tabelle der eroeffneten Dateien
fuer dieses Inode.
.IP DEV
Major- und Minor-Geraetenummer des Dateisystems, in dem das
Inode liegt.
.IP INO
Inode-Nummer im E/A-Geraet.
.IP MODE
Zugriffsrechte-Bits, siehe
.BR chmod (2).
.IP NLK
Anzahl der Links zu diesem Inode.
.IP UId
Benutzer-Id des Eigners.
.IP SIZ/DEV
Anzahl der Bytes in einer Datei oder Major- und Minor-Nummer
einer Geraetedatei.
.PD
.TP
.B \-x
Ausgabe der naechsten Texttabelle mit diesen Ueberschriften:
.IP LOC
Die Kernadresse dieses Tabelleneintrags.
.PD 0
.IP FLAGS
Verschiedene Statusvariablen, die wie folgt kodiert sind:
.RS
.IP T
.BR ptrace (2)
aktiv
.IP W
Text ist gerade nicht zum swap-Geraet geschrieben.
.IP L
Das Laden befindet sich in der Ausfuehrung.
.IP K
verriegelt
.IP w
angefordert (Flag L ist ein)
.RE
.PD
.IP DADDR
Diskadresse im swap-Bereich, angegeben als Vielfaches von 512
Byte.
.IP CADDR
Kernadresse, angegeben als Vielfaches von 256 Byte.
.IP SIZE
Laenge des Textsegments, angegeben als Vielfaches von 256
Byte.
.IP IPTR
Kernadresse des korrespondierenden Inode.
.IP CNT
Anzahl der Prozesse, die dieses Textsegment benutzen.
.IP CCNT
Anzahl der Prozesse, die sich im RAM befinden und dieses Textsegment
benutzen.
.PD
.TP
.B \-p
Ausgabe der Prozesstabelle fuer aktive Prozesse mit diesen
Ueberschriften:
.IP LOC
Die Kern-Adresse dieses Tabelleneintrags.
.PD 0
.IP S
Run-Status, kodiert wie folgt:
.RS
.IP 0
kein Prozess
.IP 1
Prozess wartet auf Ereignisse
.IP 3
Prozess ist lauffaehig
.IP 4
Prozess wurde erzeugt
.IP 5
Prozess wurde beendet
.IP 6
Prozess wurde unter Trace angehalten
.RE
.IP F
Verschiedene Statusvariablen, die verknuepft sein koennen:
.RS
.IP 001
geladen
.IP 002
Scheduler-Prozess
.IP 004
verriegelt
.IP 008
mit swap ausgelagert
.IP 010
Trace aktiv
.IP 020
benutzt in Trace
.IP 040
verriegelt durch
.BR lock (2).
.RE
.IP PR
Scheduling-Prioritaet, siehe
.BR nice (2).
.IP SG
Signale, die empfangen wurden (Signale 1-16 sind in den Bits
0-15 kodiert).
.IP UId
reales Benutzer-Id.
.IP TIM
Gesamtzeit in Sekunden; Zeiten ueber 127 sind als 127 dargestellt.
.IP CPU
aufsummierte CPU-Zeit, fuer Scheduler.
.IP NI
Nice level, siehe
.BR nice (2).
.IP PGRP
Prozessnummer der root der Prozessgruppe
(des Eroeffners des steuernden Terminals).
.IP PId
Prozess-Id-Nummer.
.IP PPId
Prozess-Id des Vaterprozesses.
.IP ADDR
Wenn im RAM, die physische Adresse der User-Struktur des Prozesses,
angegeben als Vielfaches von 256 Byte.
Wenn mit swap ausgelagert, die Position im swap-Bereich, angegeben
als Vielfaches von 512 Byte.
.IP SZE
Groesse des Prozessabbildes als Vielfaches von 256 Byte.
.IP WCHN
Nummer des Wait-Kanals des wartenden Prozesses.
.IP LINK
Link-Zeiger in der Liste der lauffaehigen Prozesse.
.IP TEXTP
Wenn Text getrennt, Zeiger zum Texttabelleneintrag.
.IP CLKT
Countdown-Zaehler fuer
.BR alarm (2),
gemessen in Sekunden.
.PD
.TP
.B \-t
Print-Tabelle fuer Terminals (SIO-Ports) mit diesen Ueberschriften:
.IP RAW
Anzahl der Zeichen in der raw-Eingabewarteschlange.
.PD 0
.IP CAN
Anzahl der Zeichen in der vorschriftsmaessigen Eingabewarteschlange.
.IP OUT
Anzahl der Zeichen in der Ausgabewarteschlange.
.IP IMODE
Eingabe-Modus-Flags
.IP OMODE
Ausgabe-Modus-Flags
.IP CMODE
Steuer-Modus-Flags
.IP LDMODE
"Line discipline"-Modus-Flags
.IP IOADDR
E/A-Adresse
.IP CTCADDR
CTC-Adresse
.IP DEL
Anzahl der Begrenzer (Newline-Zeichen) in der
vorschriftsmaessigen Eingabewarteschlange.
.IP COL
Berechnete Spaltenposition des Terminals.
.IP STATE
Verschiedene Status-Variablen, die wie folgt kodiert sind:
.RS
.IP W
Warten auf Beendigung des Eroeffnens.
.IP O
eroeffnet (open)
.IP S
besitzt eine spezielle Startroutine (fuer die Ausgabe).
.IP C
carrier ist ein
.IP B
beschaeftigt (busy) mit der Ausgabe.
.IP A
Prozess erwartet eine Ausgabe.
.IP X
eroeffnet fuer exklusiven Gebrauch.
.IP H
hangup beim Schliessen (close).
.RE
.IP PGRP
Prozessgruppe, die dieses Terminal steuert.
.PD
.TP
.B \-u
Ausgabe von Informationen ueber einen Benutzerprozess;
das naechste Argument ist seine Adresse so wie sie von
.BR ps (1)
angegeben wird. Der Prozess muss sich im Hauptspeicher befinden
oder die benutzte Datei kann ein Benutzerkernabbild (user
core image) und die Adresse 0 sein.
.TP
.B \-f
Ausgabe der Tabelle der eroeffneten Dateien mit diesen
Ueberschriften:
.IP LOC
Die Kernadresse dieses Tabelleneintrags.
.PD 0
.IP FLG
Verschiedene Status-Variablen, die wie folgt kodiert sind:
.RS
.IP R
eroeffnet zum Lesen
.IP W
eroeffnet zum Schreiben
.IP P
Pipe
.RE
.IP CNT
Anzahl der Prozesse, die diese eroeffnete Datei kennen.
.IP IPTR
Adresse des Inode-Tabelleneintrags fuer diese Datei.
.IP OFFS
Datei-Offset, siehe
.BR lseek (2).
.PD
.PP
.SH DATEIEN
/bin/pstat	dieses Programm
.br
/wega		Namensliste
.br
/dev/mem		Standardquelle fuer die User-Struktur
.br
/dev/kmem		Standardquelle fuer die Datentabellen im Kern
.SH SIEHE AUCH
ps(1), stat(2), filsys(5)
