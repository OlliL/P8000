.th DB \*(RR 3.10.89 4
.sh AUFRUF
db2 [flags] d_b_i d_b_d d_b {r_e_l}
.br
db3 [flags] d_b_i d_b_d d_b {r_e_l}
.sh FLAGS
.lp +5 5
.in 0
-f	Ausgabe der  \*(RR Felddefinitionen
(fields)
.i0
.s1
Die Ausf\*uhrung des Kommandos
.lp +8 8
%	db2 -f dbi pers.dbd \*(rr personal
.i0
w\*urde folgende Ausgabe erzeugen:
.s1
.nf
--- no --- off --- field --- type --- length --- dec ---
|    0       0     #            I          4
|    1       0     *            C          1
|    2       1     NUM          N          5
|    3       6     NAME         C         20
--------------------------------------------------------
.fi
.s1
Es bedeuten:
.lp +20 20
no	Feldnummer
.lp +20 20
off	Offset des Feldes im \*(RR Datensatz
.lp +20 20
field	Feldname
.lp +20 20
type	Feldtyp
.lp +20 20
length	Feldl\*ange
.lp +20 20
dec	Anzahl der Dezimalstellen
(nur bei Feld\%typ N)
.i0
.s1
Das Feld # mit der Nummer 0
ist die \*(RR Satznummer,
ein Feld das weder im \*(RR Datensatz
noch in den \*(RR Feld\%be\%schreibungen erscheint.
F\*ur dieses Feld wird der in\%ter\%ne Da\%ten\%typ I4
(32-bit Integer)
vorgegeben.
Das Feld * mit der Nummer 1
ist die \*(RR L\*oschmarkierung,
ein Feld das im \*(RR Datensatz,
nicht aber in den \*(RR Feld\%be\%schreibungen erscheint.
F\*ur dieses Feld wird der Da\%ten\%typ C1 vorgegeben.
Beide Felder sind imagin\*are Felder,
die aber in beiden Richtungen \*ubernommen werden k\*onnen.
.s1
.lp +5 5
.in 0
-h	Ausgabe der \*(RR Da\%ten\%bankdefinitionen
(header)
.i0
.s1
Die Ausf\*uhrung des Kommandos
.lp +8 8
%	db2 -h dbi pers.dbd \*(rr personal
.i0
w\*urde folgende Ausgabe erzeugen:
.s1
.nf
---- \*(RR -- 2 ----
| database from file |	pers.dbd
| length of header   |	521
| number of records  |	7
| length of record   |	26
| last modification  |	3.10.1989
| number of fields   |	4
----------------------
.fi
.s1
Es bedeuten:
.lp +20 20
database\ from\ file	\*(UU Filename der \*(RR Da\%ten\%bank
.lp +20 20
length\ of\ header	\*(RR Kopfl\*ange,
der Kopf enth\*alt die Da\%ten\%bank-
und die Felddefinitionen.
Nach dem Kopf folgen die Datens\*atze.
.lp +20 20
number\ of\ records	Anzahl der \*(RR Datens\*atze,
dabei sind auch die zum L\*oschen markierten Da\%ten\%s\*atze eingeschlossen.
.lp +20 20
length\ of\ record	L\*ange eines \*(RR Datensatzes
(in By\%tes),
in der L\*ange ist das Feld zur Mar\%kie\%rung
gel\*oschter Daens\*atze ent\%hal\%ten.
.lp +20 20
last\ modification	Datum der letzten \*Anderung
in der \*(RR Da\%ten\%bank
.lp +20 20
number\ of\ fields	Anzahl der \*(RR Felder,
die Anzahl ist gegen\*uber \*(RR um 2 erh\*oht,
da die beiden in \*(RR direkt nicht vor\%kom\%menden Felder
Satznummer und L\*osch\%mar\%kierung
automatisch zur Feld\%be\%schreibung hinzugef\*ugt werden.
.i0
.s1
.lp +5 5
.in 0
-i	\*Ubernahme der \*(RR Da\%ten\%bank
im \*(II Format,
das
.i0
hei\*st,
die imagin\*aren Felder Satznummer
und L\*osch\%mar\%kie\%rung werden nicht \*ubernommen.
.s1
.lp +5 5
.in 0
-l	das DBI-file soll nicht erzeugt werden,
d.h. die \*(RR
.i0
Da\%ten\%bank soll nur gelistet werden
(list only)
.s1
.lp +5 5
.in 0
-r	Ausgabe aller \*(RR Datens\*atze
(records)
.i0
.s1
Die Ausf\*uhrung des Kommandos
.lp +8 8
%	db2 -r dbi pers.dbd \*(rr personal
.i0
w\*urde folgende Ausgabe erzeugen:
.s1
.nf
----no-*------------------------------------------------
|    1| |   35|Loda, Dieter        
|    2| |   55|Malter, Juergen     
|    3| |   98|Weise, Juergen      
|    4| |   32|Heine, Rolf         
|    5| |   33|Ernst, Eva          
|    6| |   37|Neumann, Inge       
|    7| |   10|Rose, Peter         
--------------------------------------------------------
.fi
.s1
Es bedeuten:
.lp +20 20
no	\*(RR Satznummer
.lp +20 20
*	\*(RR L\*oschmarkierung,
ein Leer\%zei\%chen kennzeichnet einen g\*ultigen Da\%ten\%satz,
ein * einen gel\*oschten Datensatz.
.lp +20 20
-----	\*(RR Datensatz,
die Felder werden durch das Zeichen | separiert.
Eine bi\%n\*a\%re Null wird als * dargestellt.
.i0
.s1
.lp +5 5
.in 0
-rd	Ausgabe nur der gel\*oschten \*(RR Datens\*atze
.i0
(deleted records)
.s1
.lp +5 5
.in 0
-rv	Ausgabe nur der g\*ultigen \*(RR Datens\*atze
.i0
(valid records)
.sh ARGUMENTE
.lp +8 8
d_b_i	\*(UU-file-name des DBI-files
.lp +8 8
d_b_d	\*(UU-file-name der \*(RR Da\%ten\%bank
.lp +8 8
d_b	Name der \*(II Da\%ten\%bank
.lp +8 8
{r_e_l}	Namen von \*(II Relationen
aus der Da\%ten\%bank d_b
(max. 5, durch Leerzeichen getrennt)
.i0
.sh FUNKTION
db2, db3 erstellt zu der existierenden
\*(RR Da\%ten\%bank d_b_d das DBI-
(Data Bases Interface)
Textfile d_b_i
in CL mit einer vollst\*andigen Zuordnung der
\*(RR Da\%ten\%bank zu den Relationen {r_e_l} der
\*(II Da\%ten\%bank d_b.
Das Programm kann zus\*atzlich
zum Auslisten der \*(RR Da\%ten\%bank verwendet werden.
.br
Das Textfile d_b_i kann anschlie\*send
mit einem \*(UU Text\%editor ge\*andert werden.
Bei der Angabe mehrere Re\%la\%tio\%nen
wer\%den alle \*(RR Felder jeder \*(II Relation zu\%ge\%ordnet,
so da\*s eine Bearbeitung
des Textfile d_b_i un\%er\%l\*a\*s\%lich ist.
Die Programme db2, db3 stellen
also nur ein Ger\*ust f\*ur die Konvertierung
\*(RR/\*(II bereit,
grunds\*atzlich kann das DBI-Textfile
auch vollst\*andig mit der Hand erstellt werden.
Das kann notwendig sein,
wenn kei\%ne \*(RR Da\%ten\%bank vorhanden ist,
weil sie erst generiert werden soll.
.s1
Was kann in d_b_i alles ge\*andert werden
(vergleiche dazu das DBI-Textfile aus BEISPIEL(\*(RR)) ?
.s1
Nach der Kommentarzeile
.lp +8 8
:	command  part before conversion
.i0
k\*onnen \*(UU Kommandos eingetragen wer\%den,
die vor dem Auslesen (Read)
bzw. Schrei\%ben (Write)
von \*(RR Da\%ten\%banken
ausgef\*uhrt werden sollen.
Alle Zeilen der Form
.lp +8 8
R	\*(uu command
.i0
werden von rdb2, rdb3
(Read \*(rr Data Base)
und alle Zei\%len der Form
.lp +8 8
W	\*(uu command
.i0
werden von wdb2, wdb3
(Write \*(rr Data Base)
ausgef\*uhrt.
Es ist zul\*assig,
mehrere Kommandos anzugeben.
.bp
Im folgenden werden einige sinnvolle Kommandos diskutiert.
.s1
.lp +8 8
R	\*(ms xb d_b_d
.lp +8 8
R	\*(cp xb d_b_d
.i0
Mit den Programmen
\*(ms (\*(MS(\*(RR)),
\*(cp (\*(CP(\*(RR))
kann die \*(RR Da\%ten\%bank
von einem \*(MS-, \*(CP-Da\%ten\%tr\*ager
ins \*(UU Filesystem
unter dem Namen d_b_d eingespielt werden.
Es k\*on\%nen auch beliebige andere Programme aufgrufen werden,
die diese Funktion
(in der Regel langsamer)
erf\*ullen.
.s1
.lp +8 8
R	cdb d_b d_b_i
.i0
Das Kommando cdb(\*(RR) erzeugt vor der Konvertierung
die n\*otigen \*(II Relationen,
die vorher gel\*oscht werden.
.s1
.lp +8 8
R	modifyr d_b r_e_l truncated
.i0
Das Kommando l\*oscht die Daten der
Relation r_e_l in der \*(II Da\%ten\%bank d_b,
um sie anschlie\*send mit den Daten
der \*(RR Da\%ten\%bank zu f\*ullen.
.s1
.lp +8 8
R	modifyr d_b r_e_l heap
.i0
Das Kommando setzt die
Relation r_e_l in der \*(II Da\%ten\%bank d_b zur\*uck,
um ein schnelles Laden
der \*(RR Da\%ten\%bank zu garantieren.
.s1
.lp +8 8
R	echo `infomativer Text'
.lp +8 8
W	echo `infomativer Text'
.i0
Zu Protokollzwecken k\*onnen Texte
\*uber echo(1)
oder andere Kommandos ausgegeben werden.
.s1
.lp +8 8
W	rm -f d_b_d
.i0
Das Kommando l\*oscht die alte \*(RR Da\%ten\%bank d_b_d.
.s1
.lp +8 8
W	modifyr d_b r_e_l
.i0
Das Kommando reorganisiert die Relation r_e_l
der \*(II Da\%ten\%bank d_b,
um die richtige Satzreihenfolge
in der \*(RR Da\%ten\%bank zu erzeugen.
.s1
.lp +8 8
W	modifyr d_b r_e_l heapsort ...
.i0
Das Kommando sortiert die Relation r_e_l
der \*(II Da\%ten\%bank d_b,
um eine gew\*unschte Satzreihenfolge
der \*(RR Da\%ten\%bank zu erzeugen.
.s1
\*Anderungen in der Beschreibung
der \*(RR Datenbank,
des \*(II Datenbanknamens
und der \*(II Relationsnamen
sind in der Regel nicht angebracht.
.s1
In den Definitionen der Dom\*anen
einer \*(II Relation
k\*onnen folgende \*Anderungen durchgef\*uhrt werden.
.s1
.lp +8 8
-	Streichen von Zeilen
.i0
d.h. es sollen nicht alle
\*(RR Felder in die
\*(II Relation aufgenommen werden.
.s1
.lp +8 8
-	Hinzuf\*ugenen von Zeilen
.i0
d.h. es gibt in der
\*(II Relation Dom\*anen,
die nicht aus
\*(RR Feldern gef\*ullt werden.
.s1
.lp +8 8
-	\*Andern des Namens der Dom\*ane
.i0
.s1
.lp +8 8
-	\*Andern des Typs der Dom\*ane
.i0
.s1
Nachdem alle \*Anderungen in den Definitionen der Dom\*anen
einer \*(II Relation ausgef\*uhrt sind,
mu\*s sicher\%gestellt werden,
da\*s die Dom\*anen von 1 bis n
l\*uckenlos durchnummeriert sind.
Eine aufsteigende Sortierung ist nicht notwendig,
sollte aber aus Gr\*unden der Lesbarkeit
an\%ge\%strebt werden
.s1
Im Zuordnungsteil wird definiert,
welches \*(RR Feld in welche
\*(II Dom\*ane konvertiert werden soll.
\*An\%de\%run\%gen im Definitionsteil
der Dom\*anen m\*ussen sinngem\*a\*s
im Zuordnungsteil wiederholt werden.
Folgende Typ\%zu\%ord\%nun\%gen wer\%den unterst\*utzt:
.s1
.in 13
.nf
\*(RR                   \*(II
----------------------------------------------------
Cl   (0 < l < 255)        cn (0 < n < 256)
D                         c8
L                         c1
Nl.d (0 <= d < l < 21)    i1 || i2 || i4 || f4 || f8
M    (nur \*(R3/4)    keine Zuordnung m\*oglich
Satznummer                i4
Loeschmarkierung          c1
.fi
.i0
.s1
.s1
Nach der Kommentarzeile
.lp +8 8
:	command  part after  conversion
.i0
k\*onnen \*(UU Kommandos eingetragen wer\%den,
die nach dem Auslesen (Read)
bzw. Schrei\%ben (Write)
von \*(RR Da\%ten\%banken
ausgef\*uhrt werden sollen.
Alle Zeilen der Form
.lp +8 8
R	\*(uu command
.i0
werden von rdb2, rdb3
(Read \*(rr Data Base)
und alle Zei\%len der Form
.lp +8 8
W	\*(uu command
.i0
werden von wdb2, wdb3
(Write \*(rr Data Base)
ausgef\*uhrt.
Es ist zul\*assig,
mehrere Kommandos anzugeben.
.bp
Im folgenden werden einige sinnvolle Kommandos diskutiert.
.s1
.lp +8 8
W	\*(ms c d_b_d
.lp +8 8
W	\*(cp c d_b_d
.i0
Mit den Programmen
\*(ms (\*(MS(\*(RR)),
\*(cp (\*(CP(\*(RR))
kann das \*(UU File d_b_d als \*(RR Da\%ten\%bank
auf einem \*(MS-, \*(CP-Da\%ten\%tr\*ager gebracht werden.
Es k\*on\%nen auch beliebige andere Programme aufgrufen werden,
die diese Funktion
(in der Regel langsamer)
erf\*ullen.
.s1
.lp +8 8
R	rm -f d_b_d
.lp +8 8
W	rm -f d_b_d
.i0
Wenn die \*(RR Datenbank
nicht mehr ben\*otigt wird,
kann sie aus dem \*(UU Filesystem gel\*oscht werden.
.s1
.lp +8 8
R	modifyr d_b r_e_l isam ...
.lp +8 8
W	modifyr d_b r_e_l isam ...
.i0
Das Kommando organisiert die Relation r_e_l
der \*(II Datenbank d_b
entsprechend dem vorgesehenen Datenmodell um.
.s1
.lp +8 8
R	indexr d_b r_e_l indexrel ...
.lp +8 8
W	indexr d_b r_e_l indexrel ...
.i0
Zu dem Prim\*arschl\*ussel
k\*onnen Indexschl\*ussel hin\%zu\%ge\%f\*ugt werden.
.s1
.lp +8 8
R	echo `infomativer Text'
.lp +8 8
W	echo `infomativer Text'
.i0
Zu Protokollzwecken k\*onnen Texte
\*uber echo(1)
oder andere Kommandos ausgegeben werden.
.sh BEISPIEL\ 1
Mit dem Kommando
.lp +8 8
%	rdb2 dbi
.i0
sollen die Daten der \*(R2 Datenbank pers.dbd
an die Re\%la\%tion personal der \*(II Datenbank \*(rr angef\*ugt
und mit dem Kommando
.lp +8 8
%	wdb2 dbi
.i0
soll die \*(R2 Datenbank pers.dbd
komplett neu erstellt wer\%den.
Die Relation personal soll den
Prim\*arschl\*ussel num
und den Indexschl\*ussel name haben.
Folgendes DBI-Text\%file realisiert die Aufgabenstellung.
.s1
.nf
  1    : command  part before conversion
  2    R       \*(ms xb pers.dbd
  3    R       modifyr \*(rr personal cheap
  4    W       modifyr \*(rr personal cheapsort rec_no
  5    
  6    : \*(R2 part
  7    
  8    2       pers.dbd
  9    
 10    F         0      #               I4  
 11    F         1      *               C1  
 12    F         2      NUM             N5
 13    F         3      NAME            C20
 14    
 15    : \*(ii 16.3/4 part
 16    
 17    I       \*(rr
 18    
 19    : relation personal part
 20    T       personal
 21    
 22    D         1      rec_no          i4
 23    D         2      rec_mark        c1
 24    D         3      num             i2
 25    D         4      name            c20
 26    
 27    @         0        1
 28    @         1        2
 29    @         2        3
 30    @         3        4
 31    
 32    : command  part after  conversion
 33    R       modifyr \*(rr personal cisam num
 34    R       indexr \*(rr personal pers_index name
 35    R       rm -f pers.dbd
 36    W       modifyr \*(rr personal cisam num
 37    W       indexr \*(rr personal pers_index name
 38    W       \*(ms c pers.dbd
 39    W       rm -f pers.dbd
.fi
.s1
Diskussion f\*ur rdb2/3:
.lp +10 10
Zeile\ \ 2	Holt die \*(RR Datenbank pers.dbd
von einem \*(MS Datentr\*ager.
.lp +10 10
Zeile\ \ 3	Modifiziert die Relation personal
der \*(II Datenbank \*(rr,
um ein schnelles Anf\*ugen der Daten der
\*(RR Datenbank zu gew\*ahrleisten.
.i0
Jetzt erfolgt die Konvertierung der \*(RR Daten.
.lp +10 10
Zeile\ 33	Die Relation personal
der \*(II Datenbank \*(rr
wird auf die Dom\*ane num geschl\*usselt.
.lp +10 10
Zeile\ 34	Die Dom\*ane num wird indiziert.
.lp +10 10
Zeile\ 35	Die \*(RR Datenbank pers.dbd
wird aus dem \*(UU Filesystem gel\*oscht.
.i0
.s1
Diskussion f\*ur wdb2/3:
.lp +10 10
Zeile\ \ 4	Sortiert tempor\*ar die Relation personal
ent\%sprechend der Dom\*ane rec_no
der \*(II Da\%ten\%bank \*(rr,
um ein satzgerechtes Anf\*ugen der Daten an die
\*(RR Datenbank zu gew\*ahrleisten.
.i0
Jetzt erfolgt die Konvertierung der \*(II Daten.
.lp +10 10
Zeile\ 36	Die Relation personal
der \*(II Datenbank \*(rr
wird auf die Dom\*ane num geschl\*usselt.
.lp +10 10
Zeile\ 37	Die Dom\*ane num wird indiziert.
.lp +10 10
Zeile\ 38	Schreibt die \*(RR Datenbank pers.dbd
auf einen \*(MS Datentr\*ager.
.lp +10 10
Zeile\ 39	Die \*(RR Datenbank pers.dbd
wird aus dem \*(UU Filesystem gel\*oscht.
.i0
.sh BEISPIEL\ 2
Es soll die \*(R2 Datenbank pers.dbd
in die zwei Re\%la\%tionen num und name
der \*(II Datenbank \*(rr kon\%ver\%tiert werden,
wobei die \*(RR Satznummer die Ver\%bin\%dungs\%in\%for\%ma\%tion
zwischen den Relationen sein soll.
Um die Ar\%beit zu beschleunigen,
wird das Kommando
.lp +8 8
%	db2 bsp pers.dbd \*(rr num name
.i0
ausgef\*uhrt.
.lp +8 8
%	cat bsp
.i0
liefert folgende Ausgabe
(die Zeilennummerierung ist nur f\*ur
Dokumentationsbez\*uge eingef\*ugt worden)
.s1
.nf
  1    : command  part before conversion
  2    
  3    : \*(RR 2 part
  4    2       pers.dbd
  5    
  6    F         0     #               I4  
  7    F         1     *               C1  
  8    F         2     NUM             N5  
  9    F         3     NAME            C20 
 10    
 11    : \*(ii 16.3/4 part
 12    I       \*(rr
 13    
 14    : relation num part
 15    T       num
 16    
 17    D         1     rec_no          i4
 18    D         2     rec_mark        c1
 19    D         3     num             i2
 20    D         4     name            c20 
 21    
 22    @         0       1
 23    @         1       2
 24    @         2       3
 25    @         3       4
 26    
 27    : relation name part
 28    T       name
 29    
 30    D         1     rec_no          i4
 31    D         2     rec_mark        c1
 32    D         3     num             i2
 33    D         4     name            c20 
 34    
 35    @         0       1
 36    @         1       2
 37    @         2       3
 38    @         3       4
 39    
 40    : command  part after  conversion
.fi
.i0
.s1
Anschlie\*send wird das DBI-Textfile bsp
mit einem \*(UU Texteditor ge\*andert
(alle Zeilenangaben beziehen sich ab\%so\%lut
auf vorstehende Liste):
.s1
.lp +10 10
Zeile 18	streichen,
L\*oschmarkierung rec_mark wird nicht ben\*otigt
.lp +10 10
Zeile 19	\*andern Nummer der Dom\*ane von 3 auf 2
.lp +10 10
Zeile 20	streichen,
das \*(RR Feld NAME soll in die Re\%la\%tion name
.lp +10 10
Zeile 23	streichen
(siehe Zeile 18)
.lp +10 10
Zeile 24	\*andern Nummer der Dom\*ane von 3 auf 2
.lp +10 10
Zeile 25	streichen
(siehe Zeile 20)
.lp +10 10
Zeile 31	streichen,
L\*oschmarkierung rec_mark wird nicht ben\*otigt
.lp +10 10
Zeile 32	streichen,
das \*(RR Feld NUM soll in die Re\%la\%tion num
.lp +10 10
Zeile 33	\*andern Nummer der Dom\*ane von 4 auf 2
.lp +10 10
Zeile 36	streichen
(siehe Zeile 31)
.lp +10 10
Zeile 37	streichen
(siehe Zeile 32)
.lp +10 10
Zeile 38	\*andern Nummer der Dom\*ane von 4 auf 2
.i0
.s1
Nachdem 8 Zeilen gestrichen
und in 4 Zeilen eine Ziffer
durch eine andere ersetzt wurde,
ist folgendes der Auf\%ga\%ben\%stel\%lung
entsprechende DBI-Textfile entstanden.
.s1
.nf
  1    : command  part before conversion
  2    
  3    : \*(RR 2 part
  4    2       pers.dbd
  5    
  6    F         0     #               I4  
  7    F         1     *               C1  
  8    F         2     NUM             N5  
  9    F         3     NAME            C20 
 10    
 11    : \*(ii 16.3/4 part
 12    I       \*(rr
 13    
 14    : relation num part
 15    T       num
 16    
 17    D         1     rec_no          i4
 18    D         2     num             i2
 19    
 20    @         0       1
 21    @         2       2
 22    
 23    : relation name part
 24    T       name
 25    
 26    D         1     rec_no          i4
 27    D         2     name            c20 
 28    
 29    @         0       1
 30    @         3       2
 31    
 32    : command  part after  conversion
.fi
.i0
.sh FEHLER
.lp +8 8
\*(RR\ Sie\ sind\ kein\ eingetragener\ \*(ii-Nutzer	
.lp +8 8
===>	Nur eingetragene Nutzer des \*(II Systems
k\*on\%nen dieses Kommando ausf\*uhren.
.i0
.s1
.lp +8 8
\*(RR\ Unbekanntes\ Flag\ 'ff'	
.lp +8 8
usage:\ \ DB2\ [flags]\ dbi\ dbd\ db\ {rel}	
.lp +8 8
flags:\ \ -f\ \ list\ field\ descriptions	
.lp +8 8
\ \ \ \ \ \ \ \ -h\ \ list\ header	
.lp +8 8
\ \ \ \ \ \ \ \ -l\ \ dont't\ create\ DBI-file	
.lp +8 8
\ \ \ \ \ \ \ \ -r\ \ list\ \ \ all\ \ \ records	
.lp +8 8
\ \ \ \ \ \ \ \ -rd\ list\ deleted\ records	
.lp +8 8
\ \ \ \ \ \ \ \ -rv\ list\ \ valid\ \ records	
.lp +8 8
===>	Das \*ubergeben Flag ff
ist dem Kommando nicht be\%kannt.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ access\ DBD-file\ xx	
.lp +8 8
\*(RR\ syntax\ error	
.lp +8 8
===>	Die \*(RR Da\%ten\%bank xx ist nicht vorhanden
oder kann von \*(II nicht zum Lesen
er\*offnet werden.
.i0
.s1
.lp +8 8
\*(RR\ can't\ access\ DBI-file\ xx	
.lp +8 8
===>	Das DBI-Textfile xx kann von \*(II
nicht zum Schreiben er\*offnet werden,
m\*oglicherweise ist die Schreiberlaubnis
f\*ur den Nutzer \*(ii in dem ak\%tu\%el\%lem Verzeichnis nicht gegeben.
.i0
.s1
.lp +8 8
\*(RR\ format\ error\ on\ DBD-file\ xx	
.lp +8 8
===>	Das angebotene DBD-file xx ist keine \*(RR Da\%ten\%bank,
der Kopfteil ist zu kurz.
.i0
.s1
.lp +8 8
\*(RR\ record\ count\ error	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ r1\ records	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ r2\ records	
.lp +8 8
===>	Beim Lesen der \*(RR Da\%ten\%bank wurden r1 Da\%ten\%s\*atze erwartet
und r2 Da\%ten\%s\*atze gefunden
(nur bei Verwendung eines der -r Flags m\*oglich).
.i0
.s1
.lp +8 8
\*(RR\ record\ length\ error	
.lp +8 8
\ \ \ \ \ \ \ \ on\ record\ rr	
.lp +8 8
===>	Beim Lesen der \*(RR Da\%ten\%bank
trat in dem Da\%ten\%satz rr ein Lesefehler oder EOF ein.
Ist die Satz\%num\%mer rr==0,
ist der Kopfteil der \*(RR Da\%ten\%bank zu kurz,
d.h. das Kommando konnte nicht
auf den ersten Da\%ten\%satz positionieren
(nur bei Verwendung eines der -r Flags m\*oglich).
.i0
.s1
.lp +8 8
\*(RR\ unknown\ type\ tt\ on\ field ff/nn	
.lp +8 8
===>	Das Feld mit der Nummer ff und dem Namen nn
der \*(RR Da\%ten\%bank hat den unbekannten Datentyp tt.
.i0
.s1
.lp +8 8
\*(RR\ version\ error\ on\ DBD-file\ xx	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ \*(RR\ v1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ \*(RR\ v2	
.lp +8 8
===>	Es wurde eine \*(RR Version v1 Da\%ten\%bank erwartet
und eine \*(RR Version v2 Da\%ten\%bank gefunden.
.i0
