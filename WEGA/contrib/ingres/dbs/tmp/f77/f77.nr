.pl 66
.ll 65
.ns
.po 5
.nr in 5
.de i0
.in \n(in
..
.de lp
.i0
.if !\n(.V .ta \\$2+1
.if \n(.V .ta \\$2
.in \\$1
.ti -\\$2
..
.de s1
.sp 1
.ne 4
..
.de s2
.sp 1
..
.de fo
'sp 2
'tl ''- % -''
'bp
..
.de th
.de x1
.rs
'sp 4
'tl '\\*(II/\\$3''\\$1(\\$2)'
'sp 2
\\..
.wh -7 fo
.wh 0 x1
.pn 1
'fi
'ad
.bp
.in \n(in
.dt
..
.de sh
.i0
.s1
.ne 5
.ti 0
\\$1
.br
..
.de dt
.ta 8 16 24 32 40 48 56 64
..
.ds II ING_DB 6.3
.ds UU UNIX
.ds AA AMX
.ds FF FORTRAN77
.if !\n(.V .ds +- +-
.if \n(.V .ds +- \(+-
.ds ** *
.ds - -
.ds _ _
.ds v |
.hy 0
.so f77.um
.nf
 
.fi
.sp 5
.nf
             @    @     @ @     @
            @ @   @@   @@  @   @
           @   @  @ @ @ @   @ @
          @     @ @  @  @    @
          @@@@@@@ @     @   @ @
          @     @ @     @  @   @
          @     @ @     @ @     @
.fi
.sp 5
.nf
           @    @  @    @  @@@@@
           @    @  @@   @  @    @
           @    @  @ @  @  @    @
           @    @  @  @ @  @    @
           @    @  @   @@  @    @
            @@@@   @    @  @@@@@
.fi
.sp 5
.nf
          @@@@@@@ @@@@@@@ @@@@@@@
          @       @    @  @    @
          @           @       @
          @@@@@      @       @
          @         @       @
          @         @       @
          @         @       @

.fi
.sp 10
.nf
           J .   W e n z l a f f

           Z K I   d e r   A d W
            K u r s t r .   3 3
          -----------------------
          B E R L I N ,   1 0 8 6
.fi
.th \*(AA \*(FF 4
.sh EINLEITUNG
F\*ur die Ein\%bin\%dung von Da\%ten\%bank\%aktio\%nen in
\*(FF unter
\*(UU bsd 2.9 stehen die
\*(AA\*-Compiler `amxf' und `oamxf' zur Ver\%f\*ugung.
`oamxf' ist die overlay\*-Ver\%sion von `amxf'.
.br
Die Da\%ten\%bank\%aktio\%nen wer\%den in
\*(AA/C pro\%gram\%miert und mit `amxf' \*uber\%setzt,
alle anderen Pro\%gramm\%tei\%le k\*on\%nen in
\*(FF ge\%schrie\%ben wer\%den.
Sie wer\%den mit `f77' \*uber\%setzt
und mit `amxf' bzw. `oamxf' gelinkt (ver\%bunden).
.br
Die Ver\%wen\%dung von `amxf' zur Pro\%gramm\%er\%stel\%lung
erfordert keine An\%ga\%be einer speziellen
\*(AA\*-Bib\%lio\%thek
(analog zu `amx').
Die Ver\%wen\%dung von `oamxf' erfordert vom Nutzer
.s2
.lp +10 5
1.	die An\%ga\%be des Flags `\*-V' beim
\*Uber\%setzen der zu \*uber\%lagernden Moduln
.s2
.lp +10 5
2.	die An\%ga\%be der \*uber\%la\%ger\%baren
\*(AA\*-Bib\%lio\%theken
.br
\*-lova1 bzw. `.../lib/libova1.a'
.br
\*-lova2 bzw. `.../lib/libova2.a'
.br
\*-lova3 bzw. `.../lib/libova3.a'
.br
\*-lova4 bzw. `.../lib/libova4.a'
.br
wobei der Pfad `.../' das home\*-directory
des Da\%ten\%bank\%sys\%tems \*(II an\%ge\%ben soll.
.i0
.s1
Jede Bib\%lio\%thek belegt ein Text\%seg\%ment
(8 KByte).
Ent\%sprech\%end der \*Uber\%la\%ge\%rungs\%struk\%tur k\*on\%nen
die Bib\%lio\%theken ver\%teilt wer\%den.
Die Num\%me\%rie\%rung schreibt aber
die Reihenfolge der An\%ga\%be
beim Ver\%bin\%den vor.
.s1
.sh "EIN\*-/AUSGABEN"
So\%lan\%ge in \*(FF und C jeweils ver\%schie\%dene
Da\%tei\%en an\%ge\%sprochen wer\%den,
k\*on\%nen ohne Sprach\%mix\%pro\%ble\%me
beliebige Ein\*-/Aus\%ga\%be\%ope\%ra\%tionen
ausgef\*uhrt wer\%den.
Glei\%che Da\%tei\%en k\*on\%nen nur dann
in bei\%den Spra\%chen an\%ge\%sprochen wer\%den,
wenn es sich um die Stan\%dard\*-Ein\*-
oder \*-Aus\%ga\%be\%da\%tei han\%delt.
.s2
.lp +25 20
\*(FF	C
.br
.lp +25 20
\*-\*-\*-\*-\*-\*-\*-\*-\*-	\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-
.br
.lp +25 20
Einheit 5	stdin\ \ (Des\%krip\%tor 0)
.br
.lp +25 20
Einheit 6	stdout (Des\%krip\%tor 1)
.br
.lp +25 20
Einheit 0	stderr (Des\%krip\%tor 2)
.i0
.s1
Einheit 5, 6 oder 0 in \*(FF darf dabei keiner
anderen Da\%tei zugewiesen wer\%den,
wie auch stdin, stdout oder stderr
bzw. Des\%krip\%tor 0, 1 oder 2 in C.
In C soll\%ten die Stan\%dard\%da\%tei\%en auch
nicht ge\%puf\%fert wer\%den (s. SETBUF(III)).
.s1
.sh FELDER
In \*(FF beginnen Indizes von Fel\%der (Vek\%to\%ren)
immer mit 1,
in C dagegen mit 0.
.s1
Beispiel
.lp +20 15
\*(FF:	array(n)\ \ \ hat die Indizes 1 .. n
.lp +20 15
C	array[n]\ \ \ hat die Indizes 0 .. (n \*- 1)
.i0
.s1
Pro\%ble\%me k\*on\%nen immer dann auftreten,
wenn \*uber Ar\%gumente ein Index \*ubermittelt wird,
der den Be\%ar\%bei\%tungs\%stand des Fel\%des aus\%dr\*ucken soll.
Die \*Uber\%mitt\%lung der Kar\%di\%na\%li\%t\*at
des Fel\%des (Anzahl der Feld\%ele\%men\%te)
bringt keine Pro\%ble\%me.
.br
In mehr\%di\%men\%siona\%len Fel\%dern hat in \*(FF
Spalte vor Zei\%le,
in C Zei\%le vor Spalte Vorrang.
.s1
.nf
\*(FF
                 +\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-+
        a(2,3)   | 1,1 | 1,2 | 1,3 | 2,1 | 2,2 | 2,3 |
        a(3,2)   | 1,1 | 1,2 | 2,1 | 2,2 | 3,1 | 3,2 |
        a(6)     |  1  |  2  |  3  |  4  |  5  |  6  |
                 +\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-+
C
                 +\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-+
        a[6]     |  0  |  1  |  2  |  3  |  4  |  5  |
        a[2,3]   | 0,0 | 1,0 | 0,1 | 1,1 | 0,2 | 1,2 |
        a[3,2]   | 0,0 | 1,0 | 2,0 | 0,1 | 1,1 | 2,1 |
                 +\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-\*-+
.fi
.s1
.sh "\*(FF KONVENTIONEN"
Der Ein\%tritts\%punkt in das
\*(FF\*-Pro\%gramm ist
.s1
.in 15
subroutine amx()
.i0
.s2
Damit wird eine ordnungsgem\*a\*se
Da\%ten\%bank\%ini\%tia\%li\%sie\%rung und
\*-beendigung gesichert.
.br
F\*ur \*(FF gibt es keine weiteren Konventionen
zu beachten,
fol\%gen\%de Hin\%wei\%se sol\%len die Ar\%beit etwas er\%leich\%tern:
.s1
.lp +10 5
\*-	Ver\%meiden Sie die Ver\%wen\%dung von Funk\%tio\%nen,
die in C ge\%schrie\%ben sind
und von \*(FF aufgerufen wer\%den,
so\%lan\%ge Sie den Me\%cha\%nis\%mus der
Ar\%gu\%ment\*uber\%ga\%be nicht voll\%st\*an\%dig ver\%stan\%den haben.
.s1
.lp +10 5
\*-	Stringvariablen in \*(FF,
die Zei\%chen\%ket\%ten\%dom\*anen aufnehmen sol\%len,
m\*ussen ein Zei\%chen l\*anger sein,
als die L\*ange der Zei\%chen\%ket\%ten\%dom\*ane
in der Da\%ten\%bank,
auch wenn \*(FF das Abschlu\*szeichen `\e0'
f\*ur die Zei\%chen\%ket\%te nicht ben\*otigt.
.s2
.lp +10 5
\*-	Schon sehr kleine
\*(AA/\*(FF\*-Pro\%gram\%me belegen
den vol\%len Adre\*s\%raum.
Benutzen Sie von vorn\%he\%rein die
\*Uber\%la\%ge\%rungs\%struk\%tu\%ren.
.s2
.lp +10 5
\*-	Be\%nutzen Sie f\*ur \*(AA/\*FF\*-Aus\%tausch\%ob\%jek\%te
vom Typ `integer' immer `integer*2'
bzw. `integer*4'.
.s1
.sh "C KONVENTIONEN"
Vom C\*-Teil der C/\*(FF\*-Pro\%gram\%men
k\*on\%nen alle \*(FF\*-Objekte
(Rou\%ti\%nen, Da\%ten),
vom \*(FF\*-Teil k\*on\%nen nur bestimmte
C\*-Objekte benutzt wer\%den.
.s1
.sh "NAMEN"
\*(FF\*-Namen (Symbole)
d\*urfen maximal 6 Zei\%chen lang sein.
In C mu\*s an den \*(FF\*-Namen
ein zu\%s\*atz\%lich\%er Unterstrich `_'
an\%ge\%h\*angt wer\%den,
um \*(FF\*-Objekte in C zu erreichen.
.s1
.nf
     z.B.   \*(FF     name   string   func
            C             name_  string_  func_
.fi
.s1
Normalerweise wird in \*(FF
Gro\*s\*- und Kleinschreibung
nicht unterschieden,
d.h. der \*(FF\*-Compiler \*uber\%setzt
alle Gro\*s\*- in Klein\%buch\%sta\%ben.
Das `f77'\*-Flag `\*-U' gestattet
die Ver\%wen\%dung von Gro\*s\*- und Klein\%buch\%sta\%ben.
.s1
.sh TYPKONVERSIONEN
F\*ur die Typen in \*(FF und C
gel\%ten fol\%gen\%de Zu\%ord\%nun\%gen:
.s1
.lp +30 25
integer*2	short bzw. short int
.lp +30 25
integer	long bzw. long int
.br
oder
.br
short bzw. short int
.br
(siehe `f77'\*-Flag `\*-i2')
.lp +30 25
integer*4	long bzw. long int
.lp +30 25
logical	long bzw. long int
.br
oder
.br
short bzw. short int
.br
(siehe `f77'\*-Flag `\*-i2')
.lp +30 25
real	float
.lp +30 25
double precision	double
.lp +30 25
complex	struct { float r, i; }
.lp +30 25
double complex	struct { double r, i; }
.lp +30 25
character*n	char [n]
.i0
.s1
.sh ARGUMENTKONVERSIONEN
In \*(FF wer\%den Pa\%ra\%me\%ter grund\%s\*atz\%lich
durch Adre\*sver\%weise
(Call By Reference) \*ubergeben,
einige Pa\%ra\%me\%ter
(Funk\%tio\%nen, Pa\%ra\%me\%ter vom Typ character*n)
er\%zeu\%gen einen zu\%s\*atz\%lichen L\*an\%gen\%pa\%ra\%me\%ter
vom Typ integer*4 bzw. long.
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
integer*2 x	short *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
integer x	long *x;
.br
oder
.br
short *x;
.br
(siehe `f77'\*-Flag `\*-i2')
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
integer*4 x	long *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
logical x	long *x;
.br
oder
.br
short *x;
.br
(siehe `f77'\*-Flag `\*-i2')
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
real x	float *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
double precision x	double *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
complex x	struct {float r, i; } *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ...)
.lp +30 30
double complex x	struct {double r, i; } *x;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ..., xlen)
.lp +30 30
character*(*) x	char x[];
.br
long xlen;
.s1
.lp +30 30
subroutine f(..., x, ...)	f_(..., x, ..., xnull)
.lp +30 30
subroutine x	int (*x)();
.br
long xnull; /* == 0L */
.s1
.i0
.s1
.sh FUNKTIONSKONVERSIONEN
F\*ur die \*Uber\%ga\%be von Funk\%tions\%wer\%ten
an aufru\%fen\%de Rou\%ti\%nen
gel\%ten fol\%gen\%de Zu\%ord\%nungen:
.s1
.lp +35 35
integer*2 function f(...)	short f_(...)
.s1
.lp +35 35
integer function f(...)	long f_(...)
.br
oder
.br
short f_(...)
.br
(siehe `f77'\*-Flag `\*-i2')
.s1
.lp +35 35
integer*4 function f(...)	long f_(...)
.s1
.lp +35 35
logical function f(...)	long f_(...)
.br
oder
.br
short f_(...)
.br
(siehe `f77'\*-Flag `\*-i2')
.s1
.lp +35 35
real function f(...)	nicht darstellbar,
weil float in Aus\%dr\*ucken immer in double
um\%ge\%wan\%delt wird
.s1
.lp +35 35
double precision function f(...)	double f_(...)
.s1
.lp +35 35
complex function f(...)	f_(r, ...)
.br
struct {float r, i;} *r;
.s1
.lp +35 35
double complex function f(...)	f_(r, ...)
.br
struct {double r, i;} *r;
.s1
.lp +35 35
character*n function f(...)	f_(r, l...)
.br
char r[];
.br
long l;
.i0
.s1
