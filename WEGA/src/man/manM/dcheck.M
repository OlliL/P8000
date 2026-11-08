.TH DCHECK M WEGA
.\" @[$]dcheck.M	1.2
.ad
.fi
.SH NAME
dcheck \- Directory-Konsistenz eines Dateisystems pruefen
.SH SYNTAX
.B dcheck
[\fB\-i\fR
.I numbers]
.I [filesystem]
.SH BESCHREIBUNG
.B Dcheck
liest die Directorys eines Dateisystems und vergleicht den
Link-Zaehler in jedem Inode mit der Anzahl der Directory-Eintraege,
die auf ihn Bezug nehmen. Ist das Dateisystem nicht angegeben,
wird ein Standardsatz von Dateisystemen ueberprueft.
.PP
Der Option
.B \-i
folgt eine Liste von Inode-Nummern. Tritt eine dieser Nummern
in einem Directory auf, werden diese Nummer, die Inode-Nummer des
Directorys und der Name des Eintrags angezeigt.
.PP
Das Programm arbeitet am schnellsten, wenn das raw-Geraet benutzt
wird. In diesem Fall wird die i-Liste in groesseren Abschnitten
eingelesen.
.SH DATEIEN
/etc/dcheck	dieses Programm
.SH SIEHE AUCH
filsys(5),
clri(M),
icheck(M),
ncheck(M).
.SH DIAGNOSTIK
Es werden alle Dateien angezeigt, bei denen der Link-Zaehler
und die Anzahl der Directory-Eintraege nicht uebereinstimmen
oder beide Werte 0 sind. Dabei ist nur der Fall kritisch, bei
dem die Anzahl der Eintraege groesser als der Link-Zaehler
ist; erreicht der Link-Zaehler beim Loeschen von Eintraegen
den Wert 0, fehlt den verbleibenden Directory-Eintraegen der
Bezug auf den Datenbereich. Derartige Zu\%staende sollten durch
Loeschen des Inodes und der Directory-Eintraege beseitigt werden.
In den beiden anderen Faellen kann Speicherbereich auf der
Platte verloren gehen, jedoch kein kritischer Zustand entstehen.
.SH EINSCHRAENKUNGEN
.B Dcheck
arbeitet in zwei Durchlaeufen. Treten zwischen den beiden Phasen
Veraenderungen im Dateisystem auf, kann dies zu Fehlermitteilungen
fuehren. Deshalb ist
.B dcheck
nicht auf aktive Dateisysteme anzuwenden.
