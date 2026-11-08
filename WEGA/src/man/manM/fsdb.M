.TH FSDB M WEGA
.\" @[$]fsdb.M	1.2
.ad
.fi
.SH NAME
fsdb \- Dateisystem-Debugger
.SH SYNTAX
.B fsdb
.I special-file
[\fB\-\fR]
.SH BESCHREIBUNG
Mit
.B fsdb\^
kann ein Dateisystem nach einem Absturz repariert werden. 
.B Fsck\^
kann Blocknummern
und Inode-Nummern in die korrespodierenden Diskadressen uebersetzen;
es kann auf die verschiedenen Teile eines Inode zugreifen.
Die Korrektur der Steuerblockeintraege oder das Verfolgen der
Baumstruktur des Dateisystems sind relativ einfach.
.PP
.B Fsdb\^
enthaelt verschiedene Routinen zur Fehlererkennung in Inodes
und Blockadressen. Als Basis dieser Ueberpruefungen liest 
.B fsdb\^
i-size und f-size vom Superblock des Dateisystems. Wenn notwendig,
koennen diese Ueberpruefungen durch Aufruf von
.B fsdb\^
mit dem optionalen Argument
.B \-
oder durch Benutzung des Symbols
.B O
unterdrueckt werden.
.PP
.B Fsdb\^
akzeptiert Zahlen im Oktal- (mit fuehrender Null) und Dezimalformat;
Standard ist das Dezimalformat. Waehrend jeder Operationszuweisung
werden die Zahlen auf Gueltigkeit ueberprueft.
.PP
Da
.B fsdb\^
die Bloecke einzeln einliest, arbeitet es sowohl mit dem raw-
als auch mit dem Block-E/A-Geraet. Zur Reduzierung der Anzahl
der Systemrufe zum Lesen, haelt eine Pufferverwaltungsroutine
die am meisten benutzten Datenbloecke fest. Alle Zuweisungsoperationen
bewirken ein sofortiges Schreiben des korrespondierenden Blocks.
.PP
Die fuer
.B fsdb\^
moeglichen Symbole sind:
.RS
.TP 13
.B #
absolute Adresse
.br
.ns
.TP 13
.B i
konvertiert Inode-Nummer in Inode-Adresse
.br
.ns
.TP 13
.B b
konvertiert zur Blockadresse
.br
.ns
.TP 13
.B d 
Directory-Slot-Offset
.br
.ns
.TP 13
.BR + , \-
Adressarithmetik
.br
.ns
.TP 13
.B q
quitt
.br
.ns
.TP 13
.BR > , <
Adresse speichern, rueckspeichern
.br
.ns
.TP 13
.B =
numerische Zuweisung
.br
.ns
.TP 13
.B =+
inkrementale Zuweisung
.br
.ns
.TP 13
.B =\-
dekrementale Zuweisung
.br
.ns
.TP 13
.B ="
Zeichenkettenzuweisung
.br
.ns
.TP 13
.B O
Fehlertest-Flip-Flop
.br
.ns
.TP 13
.B p
allgemeine Druckmoeglichkeit
.br
.ns
.TP 13
.B f
Dateidruckmoeglichkeit
.br
.ns
.TP 13
.B B
Byte-Mode
.br
.ns
.TP 13
.B W
Word-Mode
.br
.ns
.TP 13
.B D
Double-Word-Mode
.br
.ns
.TP 13
.B !
Aufruf der Shell
.RE
.PP
Die Druckmoeglichkeit generiert eine formatierte Ausgabe in
verschiedenen Arten. Bevor die Ausgabe beginnt, wird die
aktuelle Adresse auf einen passenden Wert gerundet.
Die Adresse erhoeht sich mit der Ausgabe; die letzte Adresse
ist die Adresse des letzten ausgegebenen Wertes. Die Ausgabe
kann zu jeder Zeit durch Druecken der Delete-Taste beendet
werden. Eine Zahl, die dem Symbol
.B p
folgt, gibt die Anzahl der auszugebenden Eintraege an.
Da logisch sequentielle Bloecke im allgemeinen nicht physisch
sequentiell angeordnet sind, wird auf Ueberlauf der Blockgrenzen
getestet. Wird als Zaehler Null angegeben, werden alle Eintraege
bis zum Ende des aktuellen Blocks ausgegeben.
Die Print-Optionen sind:
.RS
.TP 13
.B i
Ausgeben als Inodes
.br
.ns
.TP 13
.B d
Ausgeben als Directorys
.br
.ns
.TP 13
.B o
Ausgeben als oktale Words
.br
.ns
.TP 13
.B e
Ausgeben als dezimale Words
.br
.ns
.TP 13
.B c
Ausgeben als Zeichen
.br
.ns
.TP 13
.B b
Ausgeben als oktale Bytes
.RE,
.PP
Das Symbol
.B f
wird zur Ausgabe von Datenbloecken, die mit dem aktuellen
Inode assoziieren, benutzt. Wenn dem Symbol eine Zahl folgt,
wird der entsprechende Block der Datei ausgegeben. Bloecke
werden ab Null numeriert. Das Zeichen fuer die gewuenschte Druckoption
folgt der Blocknummer, wenn angegeben, oder dem Symbol
.BR f .
Diese Druckmoeglichkeit funktioniert fuer kleine und grosse
Dateien. Das Programm prueft auf Geraetedateien und ueberprueft,
ob die benutzten Blockzeiger auf die Daten ungleich Null sind.
.PP
Die Zeichen Punkt, Tab und das Leerzeichen koennen zum Trennen
der Funktionen benutzt werden, sie sind aber nicht erforderlich.
Eine Zeile, die nur das Newline-Zeichen enthaelt, inkrementiert
die aktuelle Adresse um die Groesse des letzten ausgegebenen
Datentyps. Die Adresse wird auf das naechste Byte, Word, Directory-Eintrag
oder Inode gesetzt, um dem Benutzer die Moeglichkeit zu geben,
sich schrittweise durch eine Region des Dateisystems zu bewegen.
Informationen werden in einem zum Datentyp passenden Format
ausgegeben. Bytes, Words und Double-Words werden mit den oktalen
Adressen, gefolgt vom oktalen und dezimalen Wert, dargestellt.
Die Zeichen
.B .B
oder
.B .D
werden der Adresse fuer Byte- und Double-Word-Werte angehaengt.
Directorys werden als ein
Directory-Slot-Offset, gefolgt von der dezimalen Inode-Nummer und
dem Namen des Eintrags, dargestellt.
Inodes werden mit bezeichneten Feldern, die jedes Element
beschreiben, ausgegeben.
.PP
Die folgende Mnemonik wird fuer die Ueberpruefung der Inodes
benutzt, und nimmt auf das aktuelle Inode Bezug:
.RS
.TP 13
.B md
Zugriffsrechte (Mode)
.br
.ns
.TP 13
.B ln
Link-Zaehler
.br
.ns
.TP 13
.B uid
Benutzer-Id
.br
.ns
.TP 13
.B gid
Gruppen-Id
.br
.ns
.TP 13
.B sz
Dateigroesse (size)
.br
.ns
.TP 13
.B a#
Nummer des Datenblocks (0 \-\| 12)
.br
.ns
.TP 13
.B at
Zugriffszeit (access time)
.br
.ns
.TP 13
.B mt
Modifikationszeit (modification time)
.br
.ns
.TP 13
.B maj
Major-Geraetenummer
.br
.ns
.TP 13
.B min
Minor-Geraete-Nummer
.RE
.SH BEISPIELE
.TP 16
386i
Ausgeben des Inode 386 im Inode-Format. Dieses wird das aktuelle
Arbeits-Inode.
.TP 16
ln=4
Aendern des Link-Zaehlers fuer das aktuelle Inode auf 4.
.TP 16
ln=+1
Inkrementieren des Link-Zaehlers um 1.
.TP 16
fc
Block 0 der Datei, die mit dem aktuellen Inode assoziiert,
in \s-1ASCII\s0 ausgeben.
.TP 16
2i.fd
Ausgabe der ersten 32 Directory-Eintraege fuer das Inode der
Wurzel (root) des Dateisystems.
.TP 16
d5i.fc
wechselt das aktuelle Inode zum Inode, das mit dem 5. Directory-Eintrag,
der mit dem vorigen Kommando gefunden wurde, assoziiert.
Die ersten 512 Byte der Datei werden in
\s-1ASCII\s0 ausgegeben.
.TP 16
1b.p0o
Ausgabe des Superblocks des Dateisystems im Oktalformat.
.TP 16
2i.a0b.d7=3
aendert die Inode-Nummer fuer den 7. Directory-Eintrag in dem root-Directory
auf 3. Dieses Beispiel zeigt auch, wie verschiedene Operanden
in einer Kommandozeile kombiniert werden koennen.
.ne 3
.TP 16
d7.nm="name"
Aendern des Namensfeldes im Directory-Slot zur angegebenen
Zeichenkette. Ist das erste Zeichen ein Buchstabe, sind 
die Anfuehrungsstriche bei
.B nm
optional.
.SH DATEIEN
/etc/fsdb		dieses Programm
.SH SIEHE AUCH
fsck(M), dir(5), filesys(5).
