.th RDB \*(RR 3.10.89 4
.sh AUFRUF
rdb2 [flags] d_b_i
.br
rdb3 [flags] d_b_i
.sh FLAGS
.lp +8 8
-2d_b_d	nur rdb2: \*Andern des \*(RR Datenbankfilenamens
aus dem DBI-file in d_b_d
.lp +8 8
-3d_b_d	nur rdb3: \*Andern des \*(RR Datenbankfilenamens
aus dem DBI-file in d_b_d
.lp +8 8
-id_b	\*Andern des \*(II Datenbanknamens
aus dem DBI-file in d_b
.lp +8 8
-rd	\*Ubernahme nur der gel\*oschten Datens\atze
aus der \*(RR Datenbank in die \*(II Relationen
.br
(deleted records)
.lp +8 8
-rv	\*Ubernahme nur der g\*ultigen Datens\*atze
aus der \*(RR Datenbank in die \*(II Relationen
.br
(valid records)
.i0
.sh ARGUMENTE
.lp +8 8
d_b_i	\*(UU-file-name des DBI-files
.i0
.sh FUNKTION
rdb2, rdb3 liest auf der Basis des gegebenen DBI-Textfiles d_b_i
die \*(RR Da\%ten\%bank,
konvertiert die Da\%ten\%s\*atze
und speichert sie in 1 bis 5
\*(II Relationen ab.
.s1
Vor der Kon\%ver\%tierung werden die
im DBI-Textfile stehenden Kommandos
mit der Meldung
.s1
.in 13
\*(RR\ EXECUTING\ \ ...\ \ cmd	
.i0
.s1
ausgef\*uhrt.
Bei der Konvertierung wird der Stand
der Arbeit auf einer relativen Skala protokolliert.
Zu der Skala wird der absolute Ma\*sstab
(n Datens\*atze entsprechen 2% bzw. einem "." auf der Anzeige)
ausgegeben.
.br
Die Ausf\*uhrung des Kommandos
.lp +8 8
%	rdb2 dbi
.i0
w\*urde folgende Ausgabe erzeugen:
.s1
.nf
|--------------------------------------------------|
|                   1 records per "."              |
|--------------------------------------------------|
 .......
|__________________________________________________|
.fi
.s1
Nach der Kon\%ver\%tierung werden die
im DBI-Textfile stehenden Kommandos
mit der Meldung
.s1
.in 13
\*(RR\ EXECUTING\ \ ...\ \ cmd	
.i0
.s1
ausgef\*uhrt.
.s1
Die durch rdb2, rdb3 angesprochenen Relationen
d\*urfen keine Kataloge,
Sichten oder Indizes sein,
d\*urfen keine Indizes haben
und m\*ussen dem das Kommando
ausf\*uhrenden Nutzer an\%ge\%geignet sein
bzw. m\*ussen das ALL-TO-ALL-Zugriffsrecht ha\%ben.
.sh FEHLER
.lp +8 8
\*(RR\ cmd\ FAILED	
.lp +8 8
===>	Das \*(UU Kommando cmd wurde fehlerhaft beendet
(exit-Status != 0).
.br
Die Arbeit von rdb2/3 wird beendet,
alle bisher ausgef\*uhrten Aktionen bleiben erhalten.
.i0
.s1
.lp +8 8
\*(RR\ DBI-file:\ unexpected\ eof\ on\ line\ ll	
.lp +8 8
===>	Das DBI-file ist auf Zeile ll vorzeitig zu Ende,
ob\%wohl noch einige \*(RR/\*(II Zuordnungen feh\%len.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ RELATION\ rel\ has\ indexes	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die auf Zeile ll des DBI-files
angegebene Relation rel hat Indizes,
die durch das Kommando nicht un\%ter\%st\*utzt werden.
.i0
.s1
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
usage:\ \ rdb2\ [flags]\ dbi	
.lp +8 8
\ \ \ \ \ \ \ \ -2dbd\ \ change\ DBD-file-name
.lp +8 8
\ \ \ \ \ \ \ \ -idb\ \ \ change\ DB-name
.lp +8 8
\ \ \ \ \ \ \ \ -rd\ transfer\ deleted\ records	
.lp +8 8
\ \ \ \ \ \ \ \ -rv\ transfer\ \ valid\ \ records	
.lp +8 8
===>	Das \*ubergeben Flag ff
ist dem Kommando nicht be\%kannt (nur rdb2).
.i0
.s1
.lp +8 8
\*(RR\ Unbekanntes\ Flag\ 'ff'	
.lp +8 8
usage:\ \ rdb3\ [flags]\ dbi	
.lp +8 8
\ \ \ \ \ \ \ \ -3dbd\ \ change\ DBD-file-name
.lp +8 8
\ \ \ \ \ \ \ \ -idb\ \ \ change\ DB-name
.lp +8 8
\ \ \ \ \ \ \ \ -rd\ transfer\ deleted\ records	
.lp +8 8
\ \ \ \ \ \ \ \ -rv\ transfer\ \ valid\ \ records	
.lp +8 8
===>	Das \*ubergeben Flag ff
ist dem Kommando nicht be\%kannt (nur rdb3).
.i0
.s1
.lp +8 8
\*(RR\ can't\ access\ DB\ xx	
.lp +8 8
===>	Die \*(II Datenbank xx existiert nicht
oder der das Kommando ausf\*uhrende Nutzer
hat nicht das Recht,
auf sie zuzugreifen.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ access\ DBD-file\ xx	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(RR Da\%ten\%bank xx ist nicht vorhanden
oder kann von \*(II nicht zum Lesen
er\*offnet werden.
.i0
.s1
.lp +8 8
\*(RR\ can't\ access\ DBI-file\ xx	
.lp +8 8
===>	Das DBI-file xx existiert nicht
oder kann von \*(II nicht zum Lesen er\*offnet werden.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ access\ RELATION\ rel	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(II Relation rel existiert nicht
oder der das Kommando ausf\*uhrende Nutzer
hat nicht das Recht,
auf sie zuzugreifen.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ access\ VIEW\ rel	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(II Relation rel ist eine Sicht,
die durch das Kommando nicht unterst\*utzt wird.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ convert\ type\ t1l1\ to\ type\ t2l2	
.lp +8 8
\*(RR\ on\ FIELD\ ff\ /\ DOMAIN\ dd	
.lp +8 8
===>	Das \*(RR Feld ff mit dem Typ t1l1
kann nicht in den Typ t2l2
der \*(II Dom\*ane dd konvertiert wer\%den.
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
.s1
Alle bisher vollst\*andig richtig konvertierten \*(RR Datens\*atze
sind in die \*(II Datenbank \*ubertragen worden.
.s1
.lp +8 8
\ \ \ \ \ \ \ \ can't\ lock\ RELATION\ rel	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Bei dem Versuch die \*(II Relation rel
f\*ur das Kommando zu sperren,
trat ein Parallelit\*atsfehler auf.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ duplicate\ DOMAIN-number\ dd	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer der \*(II Dom\*ane dd,
die auf Zeile ll steht,
ist bereits vorher an eine
andere Dom\*ane vergeben worden.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ duplicate\ FIELD-number\ ff	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer des \*(RR Feldes ff,
das auf Zeile ll steht,
ist bereits vorher an ein
anderes Feld ver\%ge\%ben worden.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ duplicate\ REFERENCE\ to\ domain\ dd	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer der \*(II Dom\*ane dd,
die im Zu\%ord\%nungsteil auf Zeile ll steht,
ist bereits vorher ei\%nem
anderen \*(RR Feld zugeordnet worden.
Eine Do\%m\*ane kann nicht aus mehreren
Feldern geladen wer\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ duplicate\ REFERENCE\ to\ field\ ff	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer des \*(RR Feldes ff,
das im Zu\%ord\%nungsteil auf Zeile ll steht,
ist bereits vorher ei\%ner
anderen \*(II Dom\*ane zugeordnet worden.
Ein Feld darf nur in eine Dom\*ane geladen werden.
.i0
.s1
.lp +8 8
\*(RR\ exec\ error	
.lp +8 8
===>	Das mit
"\*(RR EXECUTING  ...  cmd"
protokollierte Kommando
kann nicht geladen werden,
da es nicht existiert
oder keine Zugriffsberechtigung besteht.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ extra\ lines	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Ab Zeile ll des DBI-files
stehen \*uberfl\*ussige Zeilen,
die durch das Kommando nicht interpretierbar sind.
.i0
.s1
.lp +8 8
\*(RR\ fork\ error	
.lp +8 8
===>	Das mit
"\*(RR EXECUTING  ...  cmd"
protokollierte Kommando
kann nicht zur Ausf\*uhrung gebracht werden,
da wahrscheinlich die Anzahl der Prozesse aus\%ge\%sch\*opft ist.
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
\ \ \ \ \ \ \ \ illegal\ DOMAIN-number\ dd	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer dd der \*(II Dom\*ane
auf Zeile ll ist falsch,
sie darf zwischen 1 .. Dom\*ananzahl der Re\%la\%tion liegen;
.br
vergleiche dazu Feld "Domaenanzahl:" von
.lp +16 8
%	helpr d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ illegal\ FIELD-number\ ff	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer des \*(RR Feldes
auf Zeile ll ist falsch,
sie darf zwischen 0 .. Feldanzahl-1 der Da\%ten\%bank liegen;
.br
vergleiche dazu Feld "number of fields" von
.lp +16 8
%	db2 oder db3 -l -h temp d_b_d d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ illegal\ REFERENCE-domain-number\ dd	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer dd der \*(II Dom\*ane
auf Zeile ll im Zuordnungsteil ist falsch,
sie darf zwischen 1 .. Dom\*ananzahl der Re\%la\%tion liegen;
.br
vergleiche dazu Feld "Domaenanzahl:" von
.lp +16 8
%	helpr d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ illegal\ REFERENCE-field-number\ ff	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Nummer des \*(RR Feldes
auf Zeile ll im Zu\%ord\%nungsteil ist falsch,
sie darf zwischen 0 .. Feld\%an\%zahl-1 der Da\%ten\%bank liegen;
.br
vergleiche dazu Feld "number of fields" von
.lp +16 8
%	db2 oder db3 -l -h temp d_b_d d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ length\ error\ on\ DOMAIN\ dd	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ l1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ l2	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die L\*ange der \*(II Dom\*ane dd
ist in der Re\%la\%tion der Datenbank l1 Bytes,
im DBI-file ist auf Zeile ll
die L\*ange aber mit l2 Bytes angegeben;
.br
vergleiche dazu
.lp +16 8
%	helpr d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ length\ error\ on\ FIELD\ ff	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ l1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ l2	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die L\*ange des \*(RR Feldes ff
ist in der Da\%ten\%bank l1 Bytes,
im DBI-file ist auf Zeile ll
die L\*an\%ge aber mit l2 Bytes angegeben;
.br
vergleiche dazu
.lp +16 8
%	db2 oder db3 -l -h temp d_b_d d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DB-line\ [I\ ...]	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(II Datenbankzeile
wird im DBI-file
auf der Zeile ll erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DB-name	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(II Datenbankzeile
wird im DBI-file
auf der Zeile ll der Name der Datenbank erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DBD-file-name	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(RR Datenbankzeile
wird im DBI-file
auf der Zeile ll der Name der Datenbank erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DBD-line\ [2\ ...]	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(R2 Datenbankzeile
wird im DBI-file
auf der Zeile ll erwartet,
aber nicht ge\%fun\%den (nur rdb2).
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DBD-line\ [3\ ...]	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(R3 Datenbankzeile
wird im DBI-file
auf der Zeile ll erwartet,
aber nicht ge\%fun\%den (nur rdb3).
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DOMAIN-name	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(II Dom\*anzeile
wird im DBI-file
auf der Zeile ll der Name der Dom\*ane erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DOMAIN-number	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(II Dom\*anzeile
wird im DBI-file
auf der Zeile ll die Nummer der Dom\*ane erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DOMAIN-type\ &\ length	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(II Dom\*anzeile
wird im DBI-file
auf der Zeile ll die Typ-/L\*angenangabe der Dom\*ane erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ DOMAIN\ with\ number\ dd	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	F\*ur die \*(II Dom\*ane dd
wird im DBI-file
auf der Zeile ll eine Dom\*anzeile erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ FIELD-name	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(RR Feldzeile
wird im DBI-file
auf der Zeile ll der Name des Feldes erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ FIELD-number	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(RR Feldzeile
wird im DBI-file
auf der Zeile ll die Nummer des Feldes erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ FIELD-type\ &\ length	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(RR Feldzeile
wird im DBI-file
auf der Zeile ll die Typ-/L\*angenangabe des Feldes erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ FIELD\ with\ number\ ff	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	F\*ur das \*(RR Feld ff
wird im DBI-file
auf der Zeile ll eine Feldzeile erwartet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ REFERENCES	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	F\*ur die \*(II Dom\*anen
werden im DBI-file auf Zeile ll
Zuordnungen zu \*(RR Feldern er\%war\%tet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ REFERENCE-domain-number	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der Zuordnungszeile
wird im DBI-file
auf der Zeile ll die Nummer der \*(II Dom\*ane er\%war\%tet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ REFERENCE-field-number	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der Zuordnungszeile
wird im DBI-file
auf der Zeile ll die Nummer des \*(RR Feldes er\%war\%tet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ RELATION-line\ [T\ ...]	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(II Tabellenzeile
wird im DBI-file
auf der Zeile ll er\%war\%tet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ missing\ RELATION-name	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	In der \*(II Tabellenzeile
wird im DBI-file
auf der Zeile ll der Name der Tabelle (Relation) er\%war\%tet,
aber nicht ge\%fun\%den.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ point\ error\ on\ FIELD\ ff	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ d1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ d2	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die Anzahl der Dezimalstellen des \*(RR Feldes ff
ist in der Da\%ten\%bank d1 Bytes,
im DBI-file ist auf Zeile ll
die L\*an\%ge aber mit d2 Bytes angegeben;
.br
vergleiche dazu
.lp +16 8
%	db2 oder db3 -l -h temp d_b_d d_b r_e_l
.i0
.s1
.lp +8 8
\*(RR\ record\ count\ error	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ r1\ records	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ r2\ records	
.lp +8 8
===>	Beim Lesen der \*(RR Da\%ten\%bank wurden r1 Da\%ten\%s\*atze er\%war\%tet
und r2 Da\%ten\%s\*atze ge\%fun\%den.
Trotzdem sind alle \*(RR Datens\*atze
in die \*(II Datenbank \*ubertragen worden.
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
auf den ersten Da\%ten\%satz positionieren.
Alle bisher vollst\*andig richtig gelesenen \*(RR Datens\*atze
sind in die \*(II Datenbank \*ubertragen worden.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ too\ many\ RELATION-lines\ [T\ ...]	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Die \*(II Tabellenzeile
im DBI-file auf Zeile ll
leitet die Definition der 6. Relation ein,
es sind aber nur 5 Relationen zul\*assig.
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ type\ error\ on\ DOMAIN\ dd	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ t1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ t2	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Der Typ der \*(II Dom\*ane dd
ist in der Re\%la\%tion der Datenbank t1,
im DBI-file ist auf Zeile ll
der Typ aber mit t2 angegeben;
.br
vergleiche dazu
.lp +16 8
%	helpr d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ type\ error\ on\ FIELD\ ff	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ t1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ t2	
.lp +8 8
\*(RR\ DBI-file:\ syntax\ error\ on\ line\ ll	
.lp +8 8
===>	Der Typ des \*(RR Feldes ff
ist in der Da\%ten\%bank t1,
im DBI-file ist auf Zeile ll
der Typ aber mit t2 angegeben;
.br
vergleiche dazu
.lp +16 8
%	db2 oder db3 -l -h temp d_b_d d_b r_e_l
.i0
.s1
.lp +8 8
\ \ \ \ \ \ \ \ unknown\ type\ ttll	
.lp +8 8
\*(RR\ on\ FIELD\ ff\ /\ DOMAIN\ dd	
.lp +8 8
===>	Der Typ tt in Verbindung mit der L\*ange ll
ist als \*(RR Datentyp unbekannt,
daher kann das \*(RR Feld ff
nicht in die \*(II Dom\*ane dd kon\%ver\%tiert werden.
.i0
.s1
.lp +8 8
\*(RR\ version\ error\ on\ DBD-file\ xx	
.lp +8 8
\ \ \ \ \ \ \ \ expected\ \*(RR\ v1	
.lp +8 8
\ \ \ \ \ \ \ \ \ \ \ found\ \*(RR\ v2	
.lp +8 8
===>	Es wurde eine \*(RR Version v1 Da\%ten\%bank er\%war\%tet
und eine \*(RR Version v2 Da\%ten\%bank ge\%fun\%den.
.i0
