.th CDB \*(RR 3.10.89 4
.sh AUFRUF
cdb d_b d_b_i
.sh ARGUMENTE
.lp +8 8
d_b	Name der \*(II Datenbank
.lp +8 8
d_b_i	\*(UU-file-name des DBI-files
.i0
.sh FUNKTION
cdb ist ein Programm zur Erstellung
der \*(II Da\%ten\%bank\%struk\%tur
auf der Basis eines gegebenen DBI-files.
.br
Durch cdb wird f\*ur jede in d_b_i enthaltene
Relation rel die Kommandofolge:
.s1
.lp +8 8
-	destroyr d_b rel
.s1
.lp +8 8
-	creater d_b rel  . . .
.i0
.s1
ausgef\*uhrt.
Bei der Konvertierung \*(RR zu \*(II
sind zwei verschieden Wege zu unterst\*utzen:
.s1
.lp +8 8
1)	Neuerstellung der \*(II Relation
aus der \*(RR Datenbank
.s1
.lp +8 8
2)	Erweiterung existierender \*(II Relationen
um die Daten der \*(RR Datenbank
.i0
.s1
Aus diesem Grund werden die \*(II Relationen
nicht bei der Konvertierung automatisch angelegt,
sondern sind im Vor\%feld mit cdb zu erzeugen.
Die Erzeugung kann in die Aus\%f\*uh\%rungszeit der
Konvertierung verlagert
und damit au\%to\%matisiert werden
(siehe dazu CL(\*(RR)).
.br
Ob die Anzahl der Dom\*anen
und die Tupell\*ange im Sinne von \*(II
korrekt sind,
wird nur bei der Ausf\*uhrung von cdb \*uberpr\*uft.
.sh FEHLER
.lp +8 8
DBI-file-name\ fehlt	
.br
das zweite Argument d_b_i
wurde nicht mit an\%ge\%ge\%ben
.s1
.lp +8 8
Datenbankfehler	
.br
siehe CREATER(\*(UU), DESTROYR(\*(UU)
.i0
