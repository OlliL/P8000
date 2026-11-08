.TH MAKEKEY M WEGA
.\" @[$]makekey.M	1.2
.ad
.fi
.SH NAME
makekey \- Verschluesselungskode generieren
.SH SYNTAX
.B makekey
.SH BESCHREIBUNG
.B Makekey
stellt ein sicheres Verschlusselungsprinzip bereit. Der Aufwand
fuer die Entschluesselung erhoeht sich staendig mit der Anzahl der Bytes.
.B Makekey
liest 10 Bytes von der Standardeingabe und schreibt 13 Bytes
zur Standardausgabe. Die Ausgabe haengt in einer Weise von
der Eingabe ab, die schwierig zu berechnen ist.
.PP
Die ersten 8 Eingabebytes bilden den Eingabeschluessel
.IR "input key" .
Es koennen beliebige ASCII-Zeichen sein. Die letzten beiden Zeichen
(\fIsalt\fR)
werden am besten aus einem Satz von Ziffern, Gross-/Kleinbuchstaben,
`.' und `/' ausgewaehlt.
Die "salt"-Zeichen werden als die ersten beiden Zeichen der
Ausgabe wiederholt. Die restlichen 11 Ausgabezeichen werden
dem Satz der "salt"-Zeichen  entnommen. Sie bilden den
Ausgabeschluessel
.I "output key."
.PP
Das "salt" wird verwendet, um eine der 4096 Verschluesselungsmechanismen
auszuwaehlen.
.PP
.B Makekey
ist fuer Programme bestimmt, die eine Verschluesselung ausfuehren
(z.B.
.BR ed (1)
und
.BR crypt (1)).
Im allgemeinen wird das Programm innerhalb einer
Pipe verwendet.
.SH DATEIEN
/usr/lib/makekey	dieses Programm
.SH SIEHE AUCH
crypt(1),
ed(1).
