.TH ICHECK M WEGA
.\" @[$]icheck.M	1.2
.ad
.fi
.SH NAME
icheck \- Speicherkonsistenz eines Dateisystems ueberpruefen
.SH SYNTAX
.B icheck
[\fB\-s\fR] [\fB\-b\fR
.I numbers] filesystem
.SH BESCHREIBUNG
.B Icheck
ueberprueft ein Dateisystem bezueglich seiner belegten Bloecke
und vergleicht das Ergebnis mit einer Liste freier Bloecke
(Freiblockliste),
die im Dateisystem gefuehrt wird.
Das Programm
.B icheck
liefert folgende Ausgaben:
.IP ""
Gesamtzahl der Dateien und Anzahl der "normalen" Dateien, Directorys
und Geraetedateien fuer blockorientierte und zeichenorientierte
Geraete.
.IP ""
Gesamtzahl der belegten Bloecke und Anzahl der einfach, doppelt
und dreifach indirekten Bloecke sowie die Anzahl der Directory-Bloecke.
.IP ""
Anzahl der freien Bloecke.
.IP ""
Anzahl der vermissten Bloecke, sie sind weder einer Datei zugeordnet
noch in der Freiblockliste enthalten.
.PP
Die
.B \-s
Option bewirkt, dass
.B icheck
die aktuelle Freiblockliste ignoriert und eine neue
Liste entsprechend der ermittelten Speicherbelegung erstellt
und den Superblock des Dateisystems neu schreibt.
Das Dateisystem sollte nicht mit mount verbunden sein, damit
keine anderen Aktivitaeten laufen. Ist das nicht moeglich,
wie z.B. beim root-Dateisystem, darf waehrend der Abarbeitung
von
.B icheck
keine andere Dateiarbeit erfolgen. Unmittelbar nach dem Ende
von
.B icheck
fuer das root-Dateisystem, muss das System neu gestartet werden
(Reset-Taste ohne vorheriges sync druecken !).
Dies verhindert, dass der alte Superblock im Hauptspeicher
benutzt wird. Die im Superblock
enthaltenen Werte fuer die Groesse der Freiblockliste und der
i-Liste werden von
.B icheck
nicht veraendert.
Die Option
.B \-s
unterdrueckt die normalen Ausschriften.
.PP
Die Option
.B \-b
bewirkt, dass jedes Auftreten der hinter -b angegebenen Blocknummern
in Dateien angezeigt wird.
.PP
.B Icheck
arbeitet am schnellsten, wenn das raw-Geraet benutzt wird.
In diesem Fall wird die i-Liste in groesseren Abschnitten gelesen.
.SH DATEIEN
/etc/icheck	dieses Programm
.sp
Die Dateisysteme variieren mit der Installation.
.SH SIEHE AUCH
filsys(5),
clri(M),
dcheck(M),
ncheck(M).
.SH DIAGNOSTIK
Gemeldet werden doppelt vergebene Bloecke (dup) und Bloecke,
deren Adressen ausserhalb des Dateisystems liegen (bad), mit
der Inode-Nummer und der Art Benutzung dieser Bloecke. Tritt ein
(Hardware-) Lesefehler auf, wird die Nummer des fehlerhaften
Blocks angezeigt und sein Inhalt fuer die Weiterarbeit mit
0 angenommen.
.PP
Die Ausschrift
\&`Bad freeblock' bedeutet, dass eine Blocknummer ausserhalb
des zur Verfuegung stehenden Bereichs in der Freiblockliste
entdeckt wurde.
.PP
Die Ausschrift
\&`\fIn\fR dups in free'
bedeutet, dass
.I n 
Bloecke in der Freiblockliste gefunden wurden, die auch einer
Datei zugeordnet oder nochmals in der Freiblockliste enthalten
sind.
.SH EINSCHRAENKUNGEN
.B Icheck
arbeitet mit zwei Durchlaeufen. Treten zwischen den beiden
Phasen Veraenderungen im Dateisystem auf, kann das zu Fehlermeldungen
fuehren. Deshalb ist
.B icheck
nicht in aktiven Dateisystemen anzuwenden.
.sp
.B Icheck
vertraut selbst widersinnigen Superbloecken.
