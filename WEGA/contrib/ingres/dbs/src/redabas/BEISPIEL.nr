.th BEISPIEL \*(RR 3.10.89 4
Alle Erl\*auterungen beziehen sich auf
die Relation personal der \*(II Datenbank \*(rr
mit folgender Struktur:
.s1
.in 12
.nf
Relation:		personal
---------
Besitzer:		\*(ii
Domaenanzahl:		4
Tupellaenge:		27
Tupelanzahl:		7

 Nummer Domaenname        Typ   Laenge  Schluessel
---------------------------------------------------
    1   rec_no      	    i       4
    2   rec_mark    	    c       1
    3   num         	    i       2
    4   name        	    c      20
---------------------------------------------------
.fi
.i0
.s1
und folgenden Daten:
.s1
.in 12
.nf
Relation: personal

|rec_no     |rec_mark   |num   |name                |
|---------------------------------------------------|
|          1|           |    35|Loda, Dieter        |
|          2|           |    55|Malter, Juergen     |
|          3|           |    98|Weise, Juergen      |
|          4|           |    32|Heine, Rolf         |
|          5|           |    33|Ernst, Eva          |
|          6|           |    37|Neumann, Inge       |
|          7|           |    10|Rose, Peter         |
|---------------------------------------------------|
.fi
.i0
.s1
Die Dom\*ane rec_no ist vergleichbar
der in \*(RR ver\%f\*ugbaren Satznummer.
Die Dom\*ane rec_mark
stellt die \*(RR L\*oschmarkierung dar.
Die Dom\*ane num ist die An\%ge\%stelltennummer
und die Dom\*ane name der Angestelltenname.
Die Relation personal ist bewu\*st einfach gehalten,
um  alle Beispiele \*ubersichtlich
und pr\*agnant zu halten.
.bp
Zu der Beispielrelation personal
der \*(II Datenbank \*(rr wurde
das folgende DBI-file dbi
mit einem \*(UU Texteditor in CL erstellt,
die Zeilennummerierung ist nicht Bestandteil
des DBI-Textfiles:
.s1
.nf
   1   : command  part before conversion
   2   
   3   : \*(RR 2 part
   4   
   5   2	pers.dbd
   6   
   7   F	  0	#		I4  
   8   F	  1	*		C1  
   9   F	  2	NUM		N5
  10   F	  3	NAME		C20
  11   
  12   : \*(ii 16.3/4 part
  13   
  14   I	\*(rr
  15   
  16   : relation personal part
  17   T	personal
  18   
  19   D	  1	rec_no   	i4
  20   D	  2	rec_mark   	c1
  21   D	  3	num		i2
  22   D	  4	name		c20
  23   
  24   @	  0	  1
  25   @	  1	  2
  26   @	  2	  3
  27   @	  3	  4
  28   
  29   : command  part after  conversion
.fi
.s1
Mit diesem DBI-file wurde die \*(R2 Datenbank
pers.dbd erstellt.
