.TH MKFS M WEGA
.\" @[$]mkfs.M	1.2
.ad
.fi
.SH NAME
mkfs \- Konstruieren eines Dateisystems
.SH SYNTAX
.B mkfs
.I special proto
.br
.B mkfs
.I special blocks interleave sectors
.SH BESCHREIBUNG
.B Mkfs
konstruiert durch Schreiben auf die Geraetedatei
.I special
ein Dateisystem, gemaess den Anweisungen in der Kommandozeile.
Ist das zweite Argument
.IR proto ,
konstruiert
.B mkfs
ein Dateisystem, das mit der Prototypdatei
.I proto
uebereinstimmt.
Die Prototypdatei enthaelt Angaben, die durch Leerzeichen oder
Newline-Zeichen getrennt sind.
.PP
Die erste Angabe ist der Name einer Datei, die als Boot-Block
auf den ersten Block kopiert wird. Die zweite Angabe ist eine
Zahl, die die Groesse des zu erstellenden Dateisystems darstellt.
.PP
Die naechste Angabe ist die Anzahl der Inodes in der i-Liste.
Der naechste Satz von Angaben enthaelt die Spezifikation fuer
die root-Datei. Dateispezifikationen bestehen aus Angaben,
die die Zugriffsrechte, das Benutzer-Id, das Gruppen-Id und den
Anfangsinhalt der Datei beinhalten. Die Syntax des Inhaltfeldes
haengt von den Zugriffsrechten ab.
.PP
Die Angabe der Zugriffsrechte fuer eine Datei besteht aus einer
6 Zeichen langen Zeichenkette. Das erste Zeichen spezifiziert
den Typ der Datei. (Die Zeichen
.B \-bcd
spezifizieren regulaere, blockorientierte, zeichenorientierte
bzw. Directory-Dateien.)
Das zweite Zeichen ist entweder
.B u
oder
.BR \- ,
um das "set-user-id"-Zugriffsrecht zu setzen oder nicht.
Das dritte Zeichen ist
.B g
oder
.B \-
fuer das "set-group-id"-Zugriffsrecht.
Der Rest der Zugriffsrechte, ist eine dreistellige Oktalzahl,
welche fuer Eigner, Gruppe und die anderen Benutzer die Lese-,
Schreib- und Ausfuehrungsrechte festlegt, siehe
.BR chmod (1).
.PP
Den Angaben der Zugriffsrechte folgen zwei dezimale Zahlen.
Sie spezifizieren das Benutzer- und Gruppen-Id des Eigners der
Datei.
.PP
Ist die Datei eine regulaere Datei, dann ist die naechste Angabe
ein Pfadname, der angibt, von wo sie kopiert
wird.
.PP
Wenn die Datei eine block- oder zeichenorientierte Geraetedatei
ist, dann folgen zwei dezimale Zahlen als Angaben fuer die
Major- und Minor-Geraetenummer.
.PP
Ist die Datei ein Directory, erstellt
.B mkfs
die Eintraege
.BR . ""
und
.B  ..
und liest dann eine Liste von Namen und (rekursiv) Dateispezifikationen
fuer die Eintraege ins Directory. Jede Angabe
wird mit
.B $ 
abgeschlossen.
.PP
Ist das zweite Argument
.IR blocks ,
erstellt
.B mkfs
ein Dateisystem mit einem einzigen leeren Directory. Die Groesse
des Dateisystems ist der Wert von
.I blocks
und wird als dezimale Zahl interpretiert.
Die Anzahl der Inodes wird als Funktion der Groesse des Dateisystems
berechnet.
.PP
Die optionalen Angaben
.I interleave
und
.I sectors
geben den Interleaving-Faktor (Sektorversatz) und die Anzahl
der Bloecke pro Zylinder an. Die Standardwerte fuer das P8000
sind 1 und 72.
.PP
Eine einfache Prototypdatei sieht wie folgt aus:
.RS
.sp
.nf
/usr/diag/uboot
4872 55
d\-\-777 3 1
usr	d\-\-777 3 1
	sh	\-\-\-755 3 1 /bin/sh
	ken	d\-\-755 6 1
		$
	b0	b\-\-644 3 1 0 0
	c0	c\-\-644 3 1 0 0
	$
$
.fi
.RE
.sp
.SH DATEIEN
/etc/mkfs	dieses Programm
.SH SIEHE AUCH
dir(5),
filsys(5).
.SH EINSCHRAENKUNGEN
Es muessten noch einige Moeglichkeiten existieren, um Links
anzulegen.
