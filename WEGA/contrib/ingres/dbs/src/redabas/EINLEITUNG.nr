.th EINLEITUNG \*(RR 3.10.89 4
.sh \*Ubersicht
Das \*(II Konvertierungspaket zu \*(RR
(down-load-component)
besteht aus der formalen Da\%ten\%bank\%ver\%bin\%dungs\%spra\%che
(CL = Connection Language)
und den Programmen CDB, DB, RDB und WDB.
.br
Bei allen Programmen wird vorausgesetzt,
da\*s sich die \*(RR Datenbank
in dem \*(UU Filesystem befindet.
F\*ur den physischen (off-line)
Transport werden die Programme
\*(MS und \*(CP bereitgestellt,
die \*(MS- und. \*(CP-Da\%ten\%tr\*ager
lesen bzw. schreiben k\*onnen.
Bei dem Transport ist darauf zu achten,
da\*s die \*(RR Datenbanken bin\*ar
\*ubertragen werden.
Mit dem Konvertierungspaket k\*onnen
\*(R2, \*(R3 und \*(R4 Datenbanken bearbeitet werden.
.s1
Die folgenden Anstriche geben eine alphabetische Zu\%sam\%men\%fas\%sung
\*uber die einzelenen Teile dieser Dokumentation.
.sh BEISPIEL
In diesem Dokumentationsteil wird
ein Beispiel vorgestellt,
da\*s in der gesamten Dokumentation verwendet wird.
.sh CDB
cdb ist ein Programm zur Erstellung
der \*(II Da\%ten\%bank\%struk\%tur
auf der Basis eines gegebenen DBI-files.
.sh CL
Die CL ist eine zeilenorientierte Sprache,
die die komplexe Zuordnung von \*(RR Da\%ten\%bank\%de\%fi\%ni\%tionen
zu \*(II Relationenschemas vornimmt.
.sh DB
db2, db3 erstellt zu einer existierenden
\*(RR Da\%ten\%bank das Textfile DBI
(Data Bases Interface)
in CL mit einer vollst\*andigen Zuordnung der
\*(RR Da\%ten\%bank zu \*(II Relationen.
Das Programm kann zus\*atzlich
zum Auslisten der \*(RR Da\%ten\%bank verwendet werden.
Es k\*onnen
.s1
.lp +8 8
-	die \*(RR Da\%ten\%bankbeschreibung
.lp +8 8
-	die \*(RR Felddefinitionen
.lp +8 8
-	die \*(RR Da\%ten\%s\*atze
.s1
.i0
ausgegeben werden.
.br
db2 verarbeitet \*(R2 Datenbanken,
db3 \*(R3 und \*(R4 Datenbanken.
.sh \*(MS
\*(ms liest/schreibt Dateien (files)
von/auf \*(MS-Da\%ten\%tr\*ager.
.sh RDB
rdb2, rdb3 liest auf der Basis eines gegebenen DBI-files
eine \*(RR Da\%ten\%bank,
konvertiert die Da\%ten\%s\*atze
und speichert sie in 1 bis 5
\*(II Relationen ab.
.nf

                                  +--------+
     \*(RR                      | record |
                                  +--------+
       |                           // || \\\\
       |                         //   ||   \\\\
     split                     //     ||     \\\\
       |                     //       ||       \\\\
       v                   //         ||         \\\\
                   +--------+     +--------+     +--------+
     \*(II     | tuple1 | ... | tuple3 | ... | tuple5 |
                   +--------+     +--------+     +--------+

.fi
.s1
Das Splitten von \*(RR Da\%ten\%s\*atzen
kann aus drei Gr\*unden angebracht sein:
.s1
.lp +8 8
-	\*(II unterst\*utzt in den 16-bit Ver\%sionen
geringere Satzl\*angen als \*(R3/4
.s1
.lp +8 8
-	\*(II unterst\*utzt in den 16-bit Ver\%sionen
weniger Felder als \*(R3/4
.s1
.lp +8 8
-	Aufgrund der geringen Geschwindigkeit von \*(RR
bei der Arbeit mit mehreren Da\%ten\%banken,
sind es \*(RR Programmierer gewohnt,
mehrere inhaltlich getrennte Da\%ten\%banken
in eine zu packen.
.i0
.s1
rdb2 verarbeitet \*(R2 Datenbanken,
rdb3 \*(R3 und \*(R4 Datenbanken.
.sh WDB
wdb2, wdb3 liest auf der Basis eines gegebenen DBI-files
aus 1 bis 5 \*(II Relationen,
konvertiert die Datens\*atze
und speichert sie in eine \*(RR Da\%ten\%bank.
.nf

                   +--------+     +--------+     +--------+
     \*(II     | tuple1 | ... | tuple3 | ... | tuple5 |
                   +--------+     +--------+     +--------+
       |                   \\\\         ||         //
       |                     \\\\       ||       //
     join                      \\\\     ||     //
       |                         \\\\   ||   //
       v                           \\\\ || //
                                  +--------+
     \*(RR                      | record |
                                  +--------+

.fi
.s1
wdb2 verarbeitet \*(R2 Datenbanken,
wdb3 \*(R3 und \*(R4 Datenbanken.
.sh \*(CP
\*(cp liest/schreibt Dateien (files)
von/auf \*(CP-Da\%ten\%tr\*ager.
