.th CL \*(RR 3.10.89 4
.sh UEBERSICHT
Die Abbildung von \*(RR Da\%ten\%banken
auf Relationen in \*(II Da\%ten\%banken
und umgekehrt wird formal  durch
die Da\%ten\%bank\%ver\%bindungssprache
CL (Connection Language) definiert.
Die CL Syntax ist einfach
und schnell zu erlernen.
Die Da\%ten\%bankverbindungssprache CL
ist eine zei\%len\%orientierte Spra\%che,
die formal (Syntax)
und informal (Semantik) ein\%ge\%f\*uhrt wird.
.br
CL Pro\%gram\%me werden in DBI
(Data Bases Interface)
Textfiles ab\%ge\%legt,
die interpretierend von
CDB(\*(RR),
RDB(\*(RR) und
WDB(\*(RR) abgearbeitet werden.
.br
DBI-Textfiles k\*onnen mit einem
\*(UU Texteditor erstellt bzw. ge\*andert werden.
Bei der Erstellung kann
DB(\*(RR) eine zeitersparende Hilfe sein.
.sh SYNTAX
Die formale Beschreibung erfolgt in Form von Regeln (R).
Die rechte Seite der Regel gibt den Regelname an
und die linke Seite die Regelaufl\*osung.
Beide Seiten werden durch das Zeichen : getrennt.
Regelnamen werden immer mit klei\%nen,
nicht weiter aufgel\*oste Konstanten
immer mit gro\*sen Buch\%sta\%ben geschrieben.
Alle Regeln (und Alternativen) wer\%den durchnummeriert,
um einfache Bez\*uge herstellen zu k\*on\%nen.
.s1
.lp +8 8
(R\ 1)	CL\ \ \ \ \ \ \ \ \ \ \ :\ {line}
.i0
.s1
R1 besagt,
da\*s die CL aus einer nicht leeren Menge ({})
von Zeilen (line) besteht.
Die geschweiften Klammern sind syn\%tak\%tische Metasymbole,
die eine 1 bis n-malige Wie\%der\%holung angeben.
Der Name line ist ein Regelname,
also durch wei\%te\%re Regeln noch aufzul\*osen.
.s1
.lp +8 8
(R\ 2)	line\ \ \ \ \ \ \ \ \ :\ NL
.lp +8 8
(R\ 3)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ dbi_line
.i0
.s1
Eine Zeile kann leer sein
(R2: nur aus dem Zeichen NL be\%ste\%hend;
NL ist eine Konstante,
die das Zeilenendezeichen (newline) repr\*asentiert)
oder eine Schnitt\%stel\%len\%de\%fi\%ni\%tions\%zeile
(R3: dbi_line) sein.
Die Alternative wird durch das syntaktische Metazeichen |
am Anfang von R3 dargestellt.
Leerzeilen k\*onnen \*uberall stehen
und sollen daher nicht weiter betrachtet werden.
Schnittstellendefinitioszeilen wer\%den der Einfachheit halber
im folgendem Zeilen genannt.
Alle Zeilen haben einen allgemeinen Aufbau.
Sie d\*urfen maximal 255 Zeichen lang sein
und beginnen mit einem Zei\%len\%kennzeichen
gefolgt von einer Menge von Fel\%dern,
die durch ei\%nen Seperator (sep)
voneinander getrennt sind.
.s1
.lp +8 8
(R\ 4)	sep\ \ \ \ \ \ \ \ \ \ :\ {space}
.lp +8 8
(R\ 5)	space\ \ \ \ \ \ \ \ :\ TAB\ |\ BLANK
.i0
.s1
Ein Seperator kann aus vielen (R4)
Tabulatoren oder Leer\%zei\%chen (R5)
in beliebiger Reihenfolge gebildet werden.
.s1
Es werden 8 verschiedene Zeilentypen unterschieden:
.s1
.lp +8 8
(R\ 6)	dbi_line\ \ \ \ \ :\ comment_line
.lp +8 8
(R\ 7)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ dbd_line
.lp +8 8
(R\ 8)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ field_line
.lp +8 8
(R\ 9)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ db_line
.lp +8 8
(R10)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ tab_line
.lp +8 8
(R11)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ dom_line
.lp +8 8
(R12)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ ref_line
.lp +8 8
(R13)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ cmd_line
.i0
.s1
.lp +8 8
(R14)	comment_line\ :\ ':'\ sep\ COMMENT\ NL
.i0
.s1
Die Konstante ':' stellt das Zeichen : dar,
sie ist das Zei\%len\%kenn\%zei\%chen der Kommentarzeile
(R14: comment_line).
Die Konstante COM\%MENT ist ein beliebiger Kommentartext,
der durch den Seperator sep vom Zeilenkennzeichen zu trennen ist
und durch das Zeilenendezeichen NL beendet wird.
Kom\%men\%tar\%zeilen k\*onnen \*uberall stehen
und sollen daher nicht weiter betrachtet werden.
.s1
.lp +8 8
(R15)	dbd_line\ \ \ \ \ :\ '2'\ |\ '3'\ sep\ DBD_NAME\ NL
.i0
.s1
Eine \*(RR Da\%ten\%bank\%zeile
(R15: dbd_line)
wird durch die \*(RR Versionsnummer
(2 f\*ur \*(R2
bzw. 3 f\*ur \*(R3/4)
als Zeilenkennzeichen eingeleitet,
sie hat ein Feld.
.br
Das Feld DBD_NAME ist der
\*(UU Filename der \*(RR Da\%ten\%bank.
.s1
Die angegebene Da\%ten\%bank mu\*s
zur Abarbeitungszeit von DB(\*(RR) und RDB(\*(RR)
im \*(UU Filesystem existieren.
.s1
.lp +8 8
(R16)	field_line\ \ \ :\ 'F'\ sep\ F_NO\ sep\ F_NAME\ sep\ F_TYP
.lp +8 8
	\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ [sep\ F_DEC]\ NL
.i0
.s1
Eine (\*(RR) Feldzeile
(R16: field_line)
wird durch das Zeilenkennzeichen F eingeleitet,
sie hat 3 oder 4 Felder.
.br
Das Feld F_NO ist die Nummer des \*(RR Feldes,
sie darf zwischen 0 .. Feldanzahl-1 der Da\%ten\%bank liegen.
.br
Das Feld F_NAME ist der Name des \*(RR Feldes,
er darf aus dem Zei\%chen\%satz [A-Z:0-9] gebildet werden
und maximal 10 Zeichen lang sein.
.br
Das Feld F_TYP ist die Typ-/L\*angenangabe des \*(RR Feldes.
.br
Das Feld F_DEC ist die Anzahl der Dezimalstellen des \*(RR Feldes.
Ist F_DEC==0,
kann die Angabe des Feldes entfallen
(die Metazeichen [] geben die wahlweise Verwendung
der syn\%tak\%tischen Konstruktion an).
.s1
Folgende Wer\%te f\*ur F_TYP und F_DEC sind zul\*assig:
.s1
.in 13
.nf
F_TYP   F_DEC
-------------------------------------------
Cl      0       0 < l < 255
D8      0
I4      0       nur f\*ur \*(RR Satznummer
L1      0
M10     0       nur \*(R3/4
Nl      d       0 <= d < l < 21
.fi
.i0
.s1
Das Feld F_NO==0 ist die \*(RR Satznummer,
ein Feld das we\%der im \*(RR Datensatz
noch in den \*(RR Feld\%de\%fi\%ni\%tionen erscheint.
Dieses Feld mu\*s den Namen F_NAME==#
und den in\%ter\%nen Da\%ten\%typ F_TYP==I4
(32-bit Integer) erhalten.
.br
Das Feld F_NO==1 ist die \*(RR L\*oschmarkierung,
ein Feld das im \*(RR Datensatz,
nicht aber in den \*(RR Feld\%de\%fi\%ni\%tionen erscheint.
Dieses Feld mu\*s den Namen F_NAME==*
und den Da\%ten\%typ F_TYP==C1 erhalten.
.br
Beide Felder sind imagin\*are Felder,
die aber in beiden Rich\%tun\%gen \*ubernommen werden k\*onnen.
Die eigentlichen \*(RR Felder beginnen
also mit der Nummer F_NO==2.
.s1
Das angegebene Feld mu\*s
zur Abarbeitungszeit von DB(\*(RR) und RDB(\*(RR)
in der \*(RR Da\%ten\%bank
mit den gleichen Werten f\*ur F_NO, F_TYP und F_DEC existieren.
.s1
.lp +8 8
(R17)	db_line\ \ \ \ \ \ :\ 'I'\ sep\ DB_NAME\ NL
.i0
.s1
Eine \*(II Da\%ten\%bank\%zeile
(R17: db_line)
wird durch das Zeilenkennzeichen I eingeleitet,
sie hat ein Feld.
.br
Das Feld DB_NAME ist der Name der \*(II Da\%ten\%bank,
er darf aus dem Zei\%chen\%satz [a-z_0-9] gebildet werden
und maximal 12 Zeichen lang sein.
.s1
Die angegebene Da\%ten\%bank mu\*s
zur Abarbeitungszeit von CDB(\*(RR),
RDB(\*(RR) und WDB(\*(RR)
im \*(II System existieren.
.s1
.lp +8 8
(R18)	tab_line\ \ \ \ \ :\ 'T'\ sep\ TAB_NAME\ NL
.i0
.s1
Eine (\*(II) Tabellenzeile
(R18: tab_line)
wird durch das Zeilenkennzeichen T eingeleitet,
sie hat ein Feld.
.br
Das Feld TAB_NAME ist der Name der \*(II Tabelle (Re\%la\%tion),
er darf aus dem Zei\%chen\%satz [a-z_0-9] gebildet werden
und maximal 12 Zeichen lang sein.
.s1
Die angegebene Relation mu\*s
zur Abarbeitungszeit von RDB(\*(RR) und WDB(\*(RR)
in der \*(II Da\%ten\%bank existieren.
.s1
.lp +8 8
(R19)	dom_line\ \ \ \ \ :\ 'D'\ sep\ D_NO\ sep\ D_NAME\ sep\ D_TYP\ NL
.i0
.s1
Eine (\*(II) Dom\*anzeile
(R19: dom_line)
wird durch das Zeilenkennzeichen D eingeleitet,
sie hat 3 Felder.
.br
Das Feld D_NO ist die Nummer der \*(II Dom\*ane,
sie darf zwischen 1 .. Dom\*ananzahl der Relation liegen.
.br
Das Feld D_NAME ist der Name der \*(II Dom\*ane,
er darf aus dem Zei\%chen\%satz [a-z_0-9] gebildet werden
und maximal 12 Zeichen lang sein.
.br
Das Feld D_TYP ist die Typ-/L\*angenangabe der \*(II Dom\*ane.
.br
.s1
Folgende Wer\%te f\*ur D_TYP sind zul\*assig:
.s1
.in 13
.nf
D_TYP
-------------------------------------------
cn      0 < n < 256
fn      n==4 || n==8
in      n==1 || n==2 || n==4
.fi
.i0
.s1
Die angegebene Dom\*ane mu\*s
zur Abarbeitungszeit von RDB(\*(RR) und WDB(\*(RR)
in der \*(II Relation
mit den gleichen Werten f\*ur D_NO und D_TYP existieren.
.s1
.lp +8 8
(R20)	ref_line\ \ \ \ \ :\ '@'\ sep\ F_NO\ sep\ D_NO\ NL
.i0
.s1
Eine Zuordnungszeile
(R20: ref_line)
wird durch das Zei\%len\%kennzeichen @ eingeleitet,
sie hat 2 Felder.
.br
Das Feld F_NO ist die Nummer des \*(RR Feldes,
sie darf zwischen 0 .. Feldanzahl-1 der Da\%ten\%bank liegen.
.br
Das Feld D_NO ist die Nummer der \*(II Dom\*ane,
sie darf zwischen 1 .. Dom\*ananzahl der Relation liegen.
.br
Die Zuordnungszeile definiert,
welches \*(RR Feld
in welche \*(II Dom\*ane geladen werden soll
und umgekehrt.
Hinter den \*(RR Feldern und \*(II Dom\*anen
verbergen sich Werte, die umgeladen werden sollen.
Diese Werte sind allgemein als Typen
(F_TYP/F_DEC und D_TYP) definiert,
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
I4 (Satznummer)           i4
C1 (Loeschmarkierung)     c1
.fi
.i0
.s1
Bei der Zuordnung von \*(RR Feldern
zu \*(II Dom\*anen gilt,
da\*s jeweils nur ein Feld einer Dom\*ane zugewiesen wer\%den kann.
.s1
.lp +8 8
(R21)	cmd_line\ \ \ \ \ :\ 'R'\ sep\ COMMAND\ NL
.lp +8 8
(R22)	\ \ \ \ \ \ \ \ \ \ \ \ \ |\ 'W'\ sep\ COMMAND\ NL
.i0
.s1
Eine Kom\%man\%do\%zei\%le
(R21|R22: cmd_line)
wird entweder durch das Zei\%len\%kennzeichen R
oder W eingeleitet,
sie hat 1 Feld.
.br
Das Feld COMMAND ist der Kom\%man\%do\%text,
des auszuf\*uhrenden Kom\%man\%dos.
.br
Die R-Kom\%man\%do\%zei\%len (R21)
werden durch RDB(\*(RR)
und die W-Kom\%man\%do\%zei\%len (R22)
durch WDB(\*(RR) ausgef\*uhrt.
.sh SEMANTIK
Die bisherige Syntax mu\*s
um die Regeln der Reihenfolge
der verschiedenen Zeilen erweitert werden.
.br
Die informale Beschreibung erfolgt in Form von Regeln (S).
.br
Wie schon bekannt,
k\*onnen Leer- und Kommentarzeilen
an al\%len Stellen des DBI-Textfiles eingef\*ugt werden,
wo Zeilen stehen d\*urfen.
Sie sollen bei der Darstellung der se\%man\%tischen Regeln
nicht mehr erw\*ahnt werden.
.s1
.lp +8 8
(S\ 1)	DBI\ \ \ \ \ \ \ \ \ \ :\ [before]\ \*(rr\ \*(ii\ [after]
.i0
.s1
Ein DBI-Text gliedert sich in 4 Abschnitte:
.lp +16 8
before	Kom\%man\%do\%teil vor der Konvertierung (S2)
.lp +16 8
\*(rr	\*(RR Da\%ten\%bank\%teil (S3)
.lp +16 8
\*(ii	\*(II Da\%ten\%bank\%teil (S5)
.lp +16 8
after	Kom\%man\%do\%teil nach der Konvertierung (S9)
.i0
Die Kom\%man\%do\%teile vor bzw. nach der Konvertierung sind wahl\%weise,
d.h. sie k\*onnen entfallen.
.s1
.lp +8 8
(S\ 2)	before\ \ \ \ \ \ \ :\ {cmd_line}
.i0
.s1
Der Kom\%man\%do\%teil vor der Konvertierung
besteht aus einer be\%liebigen Menge
von R-Kom\%man\%do\%zei\%len (R21) bzw.
W-Kom\%man\%do\%zei\%len (R22).
.br
Die R-Kom\%man\%do\%zei\%len (R21)
werden durch RDB(\*(RR)
und die W-Kom\%man\%do\%zei\%len (R22)
durch WDB(\*(RR) ausgef\*uhrt.
Ein Kommando kann ein Programm mit Argumenten
oder eine shell-Prozedur mit Argumenten sein.
.br
In DB(\*(RR) sind Beispiele zur Verwendung
des Kom\%man\%do\%teils angegeben.
.s1
.lp +8 8
(S\ 3)	\*(rr\ \ \ \ \ \ :\ dbd_line\ fields
.i0
.s1
Der \*(RR Da\%ten\%bank\%teil beschreibt vollst\*andig
die \*(RR Da\%ten\%bank.
Sie wird durch eine \*(RR Da\%ten\%bank\%zei\%le (R15: dbd_line)
und den Feldteil (S4: fields) definiert.
.s1
.lp +8 8
(S\ 4)	fields\ \ \ \ \ :\ {field_line}
.i0
.s1
Die \*(RR Felder werden durch
eine Menge von Feldzeilen (R16: field_line) definiert.
.s1
.lp +8 8
(S\ 5)	\*(ii\ \ \ \ \ \ \ \ \ :\ db_line\ {relation}
.i0
.s1
Der \*(II Da\%ten\%bank\%teil beschreibt vollst\*andig
die \*(II Da\%ten\%bank.
Sie wird durch eine \*(II Da\%ten\%bank\%zei\%le (R17: db_line)
und eine Menge von (\*(II) Ta\%bel\%len\%tei\%len (S6: relation) definiert.
Jeder Ta\%bel\%len\%tei\%l be\%schreibt genau eine \*(II Relation.
Es sind maximal 5 Ta\%bel\%len\%tei\%le zul\*assig.
.s1
.lp +8 8
(S\ 6)	relation\ \ \ \ \ :\ tab_line\ doms\ refs
.i0
.s1
Der (\*(II) Ta\%bel\%len\%tei\%l beschreibt vollst\*andig
eine \*(II Relation.
Sie wird durch eine Tabellenzeile (R18: tab_line),
den Dom\*anteil(S7: doms)
und den Zuordnungsteil (S8: refs) definiert.
.s1
.lp +8 8
(S\ 7)	doms\ \ \ \ \ \ \ \ \ :\ {dom_line}
.i0
.s1
Die \*(II Dom\*anen werden durch
eine Menge von Dom\*anzeilen (R19: dom_line) definiert.
.s1
.lp +8 8
(S\ 8)	refs\ \ \ \ \ \ \ \ \ :\ {ref_line}
.i0
.s1
Die Zuordnungen werden durch
eine Menge von Zuordnungszeilen (R20: ref_line) definiert.
.s1
.lp +8 8
(S\ 9)	after\ \ \ \ \ \ \ \ :\ {cmd_line}
.i0
.s1
Der Kom\%man\%do\%teil nach der Konvertierung
besteht aus einer be\%liebigen Menge
von R-Kom\%man\%do\%zei\%len (R21) bzw.
W-Kom\%man\%do\%zei\%len (R22).
.br
Die R-Kom\%man\%do\%zei\%len (R21)
werden durch RDB(\*(RR)
und die W-Kom\%man\%do\%zei\%len (R22)
durch WDB(\*(RR) ausgef\*uhrt.
Ein Kommando kann ein Programm mit Argumenten
oder eine shell-Prozedur mit Argumenten sein.
.br
In DB(\*(RR) sind Beispiele zur Verwendung
des Kom\%man\%do\%teils angegeben.
.bp
.sh BEISPIEL
Folgender DBI-Text soll die syntaktischen
und semantischen Regeln erl\*autern.
.s1
.nf
 1   R echo START \*(R2 ==> \*(II
 2   W echo START \*(II ==> \*(R2
 3   2   pers.dbd
 4   F       0       #       I4  
 5   F       1       *       C1  
 6   F       2       NUM     N5
 7   F       3       NAME    C20
 8   I   \*(rr
 9   T     personal
10   D       1       num     i2
11   D       2       name    c20
12   @       2       1
13   @       3       2
14   R echo ENDE \*(R2 ==> \*(II
15   W echo ENDE \*(II ==> \*(R2
.fi
.s1
Folgend sollen alle Zeilen den
syntaktischen und semantischen Regeln zugeordnet werden:
.s1
.lp +12 12
Zeile\ \ 1-15	(S1) DBI-text
.s1
.lp +12 12
Zeile\ \ 1-\ 2	(S2) Kommandoteil vor der Konvertierung
.s1
.lp +12 12
Zeile\ \ \ \ \ 1	(R21) Kommando f\*ur RDB(\*(RR),
.br
gibt den Text "START \*(R2 ==> \*(II"
nach dem Start der Konvertierung aus.
.lp +12 12
Zeile\ \ \ \ \ 2	(R22) Kommando f\*ur WDB(\*(RR),
.br
gibt den Text "START \*(II ==> \*(R2"
nach dem Start der Konvertierung aus.
.s1
.lp +12 12
Zeile\ \ 3-\ 7	(S3) \*(RR Da\%ten\%bank\%teil
.s1
.lp +12 12
Zeile\ \ \ \ \ 3	(R15) \*(RR Da\%ten\%bank\%zeile,
f\*ur die \*(RR Da\%ten\%bank,
die sich in dem \*(UU file pers.dbd be\%fin\%det.
.s1
.lp +12 12
Zeile\ \ 4-\ 7	(S4) Feld\%teil
.s1
.lp +12 12
Zeile\ \ \ \ \ 4	(R16) Feldzeile,
f\*ur das Feld 0/# mit dem Typ I4
(\*(RR Satznummer).
.lp +12 12
Zeile\ \ \ \ \ 5	(R16) Feldzeile,
f\*ur das Feld 1/* mit dem Typ C1
(\*(RR L\*oschmarkierung).
.lp +12 12
Zeile\ \ \ \ \ 6	(R16) Feldzeile,
f\*ur das Feld 2/NUM mit dem Typ N5
(Angestelltennummer).
.lp +12 12
Zeile\ \ \ \ \ 7	(R16) Feldzeile,
f\*ur das Feld 3/NAME mit dem Typ C20
(Angestelltenname).
.s1
.lp +12 12
Zeile\ \ 8-13	(S5) \*(II Da\%ten\%bank\%teil
.s1
.lp +12 12
Zeile\ \ \ \ \ 8	(R17) \*(II Da\%ten\%bank\%zeile,
w\*ahlt die Da\%ten\%bank \*(rr aus.
.s1
.lp +12 12
Zeile\ \ 9-13	(S6) Tabellen\%teil
.s1
.lp +12 12
Zeile\ \ \ \ \ 9	(R18) Tabellen\%zeile
f\*ur die Relation personal der Da\%ten\%bank \*(rr.
.s1
.lp +12 12
Zeile\ 10-11	(S7) Dom\*an\%teil
.s1
.lp +12 12
Zeile\ \ \ \ 10	(R19) Dom\*anzeile,
f\*ur die Dom\*ane 1/num mit dem Typ i2
(Angestelltennummer).
.lp +12 12
Zeile\ \ \ \ 11	(R19) Dom\*anzeile,
f\*ur die Dom\*ane 2/name mit dem Typ c20
(Angestelltenname).
.s1
.lp +12 12
Zeile\ 12-13	(S8) Zuordnungs\%teil
.s1
.lp +12 12
Zeile\ \ \ \ 12	(R20) Zuordnungszeile,
korreliert das Feld 2 (NUM/N5)
mit der \*(II Dom\*ane 1 (num/i2).
.lp +12 12
Zeile\ \ \ \ 13	(R20) Zuordnungszeile,
korreliert das Feld 3 (NAME/C20)
mit der \*(II Dom\*ane 2 (name/c20).
.s1
.lp +12 12
Zeile\ 14-15	(S9) Kommandoteil nach der Konvertierung
.s1
.lp +12 12
Zeile\ \ \ \ 14	(R21) Kommando f\*ur RDB(\*(RR),
.br
gibt den Text "ENDE \*(R2 ==> \*(II"
nach dem Ende der Konvertierung aus.
.lp +12 12
Zeile\ \ \ \ 15	(R22) Kommando f\*ur WDB(\*(RR),
.br
gibt den Text "ENDE \*(II ==> \*(R2"
nach dem Ende der Konvertierung aus.
.i0
