 !*******************************************************
 *							*
 *		T E R M I N A L				* 
 *							*
 *   mit Einchip-Mikrorechner U 882 und CRTC 8275	*
 *   ============================================	*
 *							*
 *  HAUPT Module; Version 5.0:				*
 *    - Anpassung an IBM (XT) Tastatur			*
 *    - 24 Zeilen / 80 Zeichen				*
 *  Inhalt:						*
 *    - MAIN Procedure mit Steuerschleife		*
 *    - UP's: PUTA, TGETCHAR, SGETCHAR, START_DISPLAY	*
 *	      SIO_OUT, BREAK				*
 *							*
 ********************************************************
 
 Bearbeiter:	Heller/Kuehle / WAT/WAE
 Stand:		20.01.88
 
 Name:		P8T.MAIN !
 
 
 
HAUPT MODULE
 
 
CONSTANT
! 'Z8T.VEREINBARUNGEN' !
$LISTOFF
 
! Werte !
P01EXT	:=	%96	! P0:=A8...A14, P1:=AD0...AD7, Stack intern !
P01TRI	:=	%9E	! tri state: P0,P1,AS,DS,R/W !
SBUFL	:=	45	! SIO-Puffer-Laenge !
 
! Bit Definitionen !
B0	:=  %1		! bit 0 !
B1	:=  %2		! bit 1 !
B2	:=  %4		! bit 2 !
B3	:=  %8		! bit 3 !
B4	:= %10		! bit 4 !
B5	:= %20		! bit 5 !
B6	:= %40		! bit 6 !
B7	:= %80		! bit 7 !
NB0	:= %FE		! negiertes bit 0 !
NB1	:= %FD		! negiertes bit 1 !
NB2	:= %FB		! negiertes bit 2 !
NB3	:= %F7		! negiertes bit 3 !
NB4	:= %EF		! negiertes bit 4 !
NB5	:= %DF		! negiertes bit 5 !
NB6	:= %BF		! negiertes bit 6 !
NB7	:= %7F		! negiertes bit 7 !
 
! Bildparameter !
ZEILENZAHL	:=	24
ZEICHENZAHL	:=	80
 
! Adressen !
BRAM		:=	%1000	! Bildwiederholspeicher !
ZADR_TAB	:=	%1780	! Tabelle der Zeilenanfangsadr. !
CRT_PAR		:=	%1C00	!Parameter lesen / schreiben !
CRT_COM		:=	%1C01	! Commands schreiben !
CRTL_STAT	:=	%1C01	! Status lesen !
ZG0		:=	%4000	! ASCII-Zeichensatz !
ZG1		:=	%2000	! Alternativ-Zeichensatz !
BELL_ADR	:=	%8000	! Akustischer Signalgeber !
 
! Display Commands !
RES_DISPL	:=	%00
START_DISPL	:=	%20
STOP_DISPL	:=	%40
READ_LPEN	:=	%60
LD_CURSOR	:=	%80
ENABLE_INT	:=	%A0
DISABLE_INT	:=	%C0
PRES_COUNT	:=	%E0
 
! Parameter fuer Display-Reset-Command !
RES1		:=	%4F	! 80 characters/row !
RES2		:=	%97	! !
RES3		:=	%0B	! !
RES4		:=	%6C	! !
 
! Field Attribute Code !
FAC	:=	%80	! Field Attribute: Normal Mode !
FACH	:=	%01	! Highlight !
FACB	:=	%02	! Blink !
FACR	:=	%10	! Reverse video !
FACU	:=	%20	! Underline !
 
! ASCII Control Characters entsprechend ADM 31 !
NUL	:=	%00	! Null			^@ !
SOH	:=	%01	! Start of Heading	^A !
STX	:=	%02	! Start of Text		^B !
ETX	:=	%03	! End of Text		^C !
EOT	:=	%04	! End of Transmission	^D !
ENQ	:=	%05	! Inquiry (who are you)	^E !
ACK	:=	%06	! Acknewledge		^F !
BEL	:=	%07	! Bell			^G !
BS	:=	%08	! Backspace		^H !
HT	:=	%09	! Horizontal Tabulator	^I !
LF	:=	%0A	! Line Feed		^J !
VT	:=	%0B	! Vertical Tabulator	^K !
FF	:=	%0C	! Forspace		^L !
CR	:=	%0D	! Carriage Return	^M !
SO	:=	%0E	! Shift out		^N !
SI	:=	%0F	! Shift in		^O !
 
DLE	:=	%10	! Data Link Escape	^P !
DC1	:=	%11	! Device Control	^Q !
DC2	:=	%12	! 			^R !
DC3	:=	%13	! 			^S !
DC4	:=	%14	! 			^T !
NAK	:=	%15	! Not Acknewl.		^U !
SYN	:=	%16	! Synchronous Idle	^V !
ETB	:=	%17	! End of Trans.-Block	^W !
CAN	:=	%18	! Cancel		^X !
EM	:=	%19	! End of Medium		^Y !
SBS	:=	%1A	! Substitute		^Z !
ESC	:=	%1B	! Escape Sequenz	^[ !
FS	:=	%1C	! File Separator	!		
GS	:=	%1D	! Group Separator	^] !
RS	:=	%1E	! Cursor HOME		^^ !
US	:=	%1F	! New Line		^~ !		
$LISTON
	ZESCF	:=26		!Zaehler ESC-Folge!
	PARA	:=254		!Parameter fuer n;row;column!
$PAGE
EXTERNAL
	CHAR_ADRESSE				PROCEDURE
	ADR_DEC					PROCEDURE
	READ_ADR				PROCEDURE
	WR_ADR					PROCEDURE
	WRITE_CHAR				PROCEDURE
	CURSOR_POS				PROCEDURE
	LOAD_CURSOR				PROCEDURE
	ROLL					PROCEDURE
	CLEAR_LINE				PROCEDURE
	CLEAR_SCREEN				PROCEDURE
	ESC_SEQUENZ				PROCEDURE
	ESC_OUT_SEQ				PROCEDURE
	BREAK					PROCEDURE
	ESC_VT100				PROCEDURE
	C_left					LABEL
	C_rigth					LABEL
	C_ft					LABEL
	P0, P1, P2, P3, STAT0, STAT1, STAT2	BYTE
	CHAR_ADR, LPOS_ADR			WORD
	SPALTZ, ZEILZ, CHAR, BDR, STAT3		BYTE
	VBYTE, ZBYTE				BYTE
	SINPTR, SOUTPTR, SCOUNT			BYTE
	SWERT					ARRAY [SBUFL BYTE]
 
 
GLOBAL
	LINE_FEED CARRIAGE_RETURN		LABEL
	TAB		LABEL
	BACK_TAB	LABEL
 
 
 
$SECTION SEC1 PROGRAM
 
MAIN PROCEDURE
 !*******************************************************
 *
 * Terminal - Steuerschleife
 * Input:	Tastatur (TGETCHAR), bzw. SIO
 * Output:	Bildschirm (PUTA), bzw. SIO
 *
 *******************************************************!
ENTRY
 
 CALL	CLEAR_SCREEN	! loeschen + FAC am Zeilenende setzen !
 CALL	LOAD_CURSOR	! Cursor auf Anfangsposition setzen !
 LD	IMR,#%01	! IRP32 -> VTRC freigeben !
 CALL	START_DISPLAY	! CRTC starten !
 EI
 LD	SIO,#%00	! Scheinausgabe SIO !
 
! Ausgabe Terminalgenerierung ADM31 oder VT100 umschaltbar ueber F1
  Ausgabe Generierung Video Attribute umschaltbar ueber F2!
TOUT1:
	tm	STAT0,#B3	!online/offline ?!
	jr	nz,TOUT2	!online!
	and	STAT2,#NB5	!set war offline!
	jr	TOUT3
TOUT2:
	or	STAT2,#B5	!set war online!
TOUT3:
	and	STAT0,#NB3	!set offline!
	call	CURSOR_HOME
	call	CLEAR_SCREEN
	tm	STAT2,#B3	!ADM31/VT100 ?!
	jr	nz,TOUT4	!ADM31!
	ld	r6,#HI VT1
	ld	r7,#LO VT1
	jr	TOUT5
TOUT4:
	ld	r6,#HI ADM
	ld	r7,#LO ADM
TOUT5:
	ldc	r8,@rr6
	ld	CHAR,r8
	cp	CHAR,#'='
	jr	z,TOUT6
	cp	CHAR,#'#'
	jr	z,TOUT7
	call	PUTA
	incw	rr6
	jr	TOUT5
TOUT6:
	tm	STAT2,#B4	!Video Attribute on/off ?!
	jr	nz,TOUT8	!off!
	ld	r6,#HI VAN
	ld	r7,#LO VAN
	jr	TOUT5
TOUT8:
	ld	r6,#HI VAF	!on!
	ld	r7,#LO VAF
	jr	TOUT5
TOUT7:
	tm	STAT2,#B5	!war on/off ?!
	jr	nz,TOUT9
	and	STAT0,#NB3	!set offline!
	jr	MAIN0
TOUT9:
	or	STAT0,#B3	!set online!
MAIN0:
 TM	STAT0,#B4	! Zeichen im TPUFFER? !
 JR	NZ,MAIN1	! wenn ja, Sprung !
 TM	STAT0,#B3	! on_line? !
 JR	Z,MAIN0		! wenn nein, Warteschleife !
 TM	STAT0,#B6	! Zeichen im SPUFFER? !
 JR	Z,MAIN0		! wenn nein, Warteschleife !
 
 ! Input ueber SIO im on_line Mode !
 CALL	SGETCHAR	! Zeichen aus SPUFFER in CHAR !
 CALL	PUTA		! Auswertung des Zeichens !
 JR	MAIN0		! Warteschleife !
 
MAIN1:
 ! Input ueber Tastatur !
 CALL	TGETCHAR	! Zeichen aus TPUFFER in CHAR
			  dabei Tastaturanpassung !
 CP	CHAR,#%F0	! on/off Taste? !
 JR	NZ,MAIN10
 
 ! Umschaltung on / off !
 CLR	CHAR		! neutralen Wert in CHAR !
 TM	STAT0,#B3
 JR	NZ,OFFLINE
 OR	STAT0,#B3	! on_line ein !
 JR	MAIN0
OFFLINE:
 AND	STAT0,#NB3	! off_line ein !
 JR	MAIN0
 
MAIN10:
 CP	CHAR,#%F1	! Break-Taste? !
 JR	NZ,MAIN11
 CALL	BREAK		! Breakfunktion ausfuehren !
 JR	MAIN0
 
! Umschaltung VT100 <---> ADM31 ueber Taste MOD
  ********************************************* !
MAIN11:
	cp	CHAR,#%90
	jr	nz,MAIN14
	tm	STAT2,#B3	!VT100/ADM31 ?!
	jr	nz,ADM31
! Umschaltung VT100 ---> ADM31 !
	or	STAT2,#B3
	jp	TOUT1
! Umschaltung ADM31 ---> VT100 !
ADM31:
	and	STAT2,#NB3
	jp	TOUT1
 
! Umschaltung Video Attribute on <---> off ueber Taste VIDEO
  ********************************************************** !
MAIN14:
	cp	CHAR,#%A0
	jr	nz,MAIN13
	tm	STAT2,#B4	!Video Attribute on/off ?!
	jr	nz,V_off
	or	STAT2,#B4	!Video attributes off!
	jp	TOUT1
V_off:
	and	STAT2,#NB4	!Video attributes on!
	jp	TOUT1
MAIN13:
 TCM	CHAR,#B7	! CHAR_7=1? !
 JR	NZ,MAIN12	! wenn ja, Escape-Sequenz ausgeben !
 CALL	ESC_OUT_SEQ
 JR	MAIN0
 
MAIN12:
 ! on_line oder off_line Mode? !
 TM	STAT0,#B3	! on_line? !
 JR	NZ,MAIN2	! wenn ja, SIO_out !
 
 ! Output auf Bildschirm im off_line Mode !
 CALL	PUTA
 JR	MAIN0		! Warteschleife !
 
MAIN2:
 ! Output ueber SIO im on_line Mode !
 CALL	SIO_OUT
 JR	MAIN0		! Warteschleife !
 
END MAIN
 
 
ADM	array [* byte] := 'ADM31/9600 baud/='
VT1	array [* byte] := 'VT100/9600 baud/='
VAN	array [* byte] := 'Video Attr. on%R%L#'
VAF	array [* byte] := 'Video Attr. off%R%L#'
$PAGE
 
PUTA PROCEDURE
 !*******************************************************
 *
 * Zeichen in CHAR auswerten
 * Cursor setzen, Bildram beschreiben
 *
 *******************************************************!
ENTRY
 
 ! Abfrage auf Program Mode (PGM) !
 !------------------------------------------------------!
 ! Im PGM werden alle Zeichen direkt ohne interne Verarbeitung
   angezeigt. Einschalten mit ESC U, aus mit ESC u bzw. ESC X !
 TCM	STAT1,#B3	! PGM-Flag setzen !
 JR	NZ,PUTA1
 CP	CHAR,#ESC	! Escape-Zeichen !
 JR	NZ,PUTA2
 OR	STAT1,#B4	! Escape_Flag setzen !
 JR	PUTA0
 
PUTA2:
 TCM	STAT1,#B4	! Escape_Flag gesetzt? !
 jr	NZ,PUTA0
 and	STAT1,#NB4	! ESC-Flag ruecksetzen !
 cp	CHAR,#'X'	! clear PGM mit ESC X !
 jr	z,CLEAR_PGM
 cp	CHAR,#'u'	! clear PGM mit ESC u !
 jr	nz,PUTA0
 
CLEAR_PGM:
 and	STAT1,#NB3	! PGM-Flag ruecksetzen !
 jr	PUTA0
 !------------------------------------------------------!
 
PUTA1:
 ! Normal Mode !
 TCM	STAT1,#B4	! Escape-Flag gesetzt ? !
 JP	Z,ESC_FLG
 
 CP	CHAR,#%20	! Control oder sichtbares Zeichen? !
 JR	ULT,PUTA_CONTROL
 !------------------------------------------------------!
 
PUTA0:
 ! Behandlung sichtbarer Zeichen !
 CALL	CHAR_ADRESSE	! aktuelle Bildramadr. berechnen !
 LD	CHAR_ADR,R12	! Berechnete Adresse ablegen !
 LD	CHAR_ADR+1,R13
 CALL	CURSOR_POS	! Cursor berechnen und setzen !
 CALL	ROLL		! Rollen, falls Rollflag gesetzt !
 LD	R8,CHAR_ADR
 LD	R9,CHAR_ADR+1
 LD	R13,CHAR
 CALL	WRITE_CHAR	! Zeichen in Bildram eintragen !
 RET
 !------------------------------------------------------!
 
PUTA_CONTROL:
 ! Abfrage der CTRL-Zeichen !
 CP	CHAR,#CR	! CTRL M !
 JR	Z,CARRIAGE_RETURN
 CP	CHAR,#LF	! CTRL J !
 JR	Z,LINE_FEED
 CP	CHAR,#BS	! CTRL H !
 JR	Z,BACK_SPACE
 CP	CHAR,#VT	! CTRL K !
 JR	Z,VERT_TAB
 CP	CHAR,#FF	! CTRL L !
 JR	Z,FOR_SPACE
 CP	CHAR,#ESC	! CTRL [ !
 JP	Z,ESC_SEQ
 CP	CHAR,#RS	! CTRL ^ !
 JP	Z,CURSOR_HOME
 cp	CHAR,#HT	! CTRL I !
 jr	z,TAB
 CP	CHAR,#BEL	! CTRL G !
 JP	Z,BELL
 RET
 !------------------------------------------------------!
 
 
 ! Bearbeitung der Steuerzeichen !
 
CARRIAGE_RETURN:
! *** Cursor auf linken Rand *** !
 
 CLR	SPALTZ
 CALL	LOAD_CURSOR
 RET
 
 
LINE_FEED:
! *** Cursor eine Zeile nach unten *** !
 
 INC	ZEILZ
 CP	ZEILZ,#ZEILENZAHL
 JR	NZ,NEUE_ZEILE
 LD	ZEILZ,#ZEILENZAHL-1	! Cursor bleibt nach Rollen
				in der letzen Zeile !
 OR	STAT0,#B2	! Rollflag setzen !
 CALL	ROLL		! Rollen, falls Rollflag gesetzt !

NEUE_ZEILE:
 CALL	LOAD_CURSOR
 RET
 
 
BACK_SPACE:
! *** Cursor ein Zeichen nach links *** !
	tm	STAT2,#B3	!ADM31-/VT100-Terminal ?!
	jr	nz,BACK_SPACE_ADM31
! VT100-Terminal !
	ld	PARA,#1
	call	C_left
	ret
! ADM31-Terminal !
BACK_SPACE_ADM31:
 
 DEC	SPALTZ
 
BACK_SPACE1:		! Entry fuer BACK-TAB !
 cp	SPALTZ,#ZEICHENZAHL
 jr	ult,BSP0
 LD	SPALTZ,#ZEICHENZAHL-1
 DEC	ZEILZ
 JR	PL,BSP0
 CLR	ZEILZ		! Cursor bleibt unveraendert !
 CLR	SPALTZ		! in der linken oberen Ecke !
 
BSP0:
 CALL	LOAD_CURSOR
 RET
 
 
 
BACK_TAB:
! *** ESC I: Cursor eine Tab-Pos. zurueck *** !
 dec	SPALTZ
 and	SPALTZ,#%F8	! Tab-Position !
 jr	BACK_SPACE1
 
 
TAB:
! *** Cursor auf Tab-Position *** !
	tm	STAT2,#B3	!ADM31-/VT100-Terminal ?!
	jr	nz,TAB_ADM31
! VT100-Terminal !
	ld	PARA,#1
	call	C_ft
	ret
! ADM31-Terminal !
TAB_ADM31:
 add	SPALTZ,#%08
 and	SPALTZ,#%F8	! Tab-Position !
 jr	FOR_SPACE1
$PAGE
VERT_TAB:
! *** Cursor eine Zeile nach oben *** !
 
 DEC	ZEILZ
 JR	PL,VT0
 CLR	ZEILZ		! Cursor bleibt in der 1.Zeile,
			  wenn er sie erreicht hat !
VT0:
 CALL	LOAD_CURSOR
 RET
 
 
FOR_SPACE:
! *** Cursor ein Zeichen nach rechts *** !
	tm	STAT2,#B3	!ADM31-/VT100-Terminal ?!
	jr	nz,FOR_SPACE_ADM31
! VT100-Terminal !
	ld	PARA,#1
	call	C_rigth
	ret
! ADM31-Terminal!
FOR_SPACE_ADM31:
 
 INC	SPALTZ
 
FOR_SPACE1:		! Entry fuer TAB-Funktion !
 CP	SPALTZ,#ZEICHENZAHL
 JR	ULT,FF0
 CLR	SPALTZ
 JR	LINE_FEED	! ZEILZ erhoehen, rollen !
 
FF0:
 CALL	LOAD_CURSOR
 RET
 
 
CURSOR_HOME:
! *** Cursor wird auf Pos. 1,1 gesetzt *** !
 
 LD	ZEILZ,#0
 LD	SPALTZ,#0
 CALL	LOAD_CURSOR
 RET
 
 
ESC_SEQ:
! *** ESC-Flag setzen *** !
 
 OR	STAT1,#B4
	clr	ZESCF		!ZESCF=0!
 RET
 
 
BELL:
 ! *** akustisches Signal *** !
 PUSH	RP
 SRP	#%20
 LD	R8,#HI BELL_ADR
 LD	R9,#LO BELL_ADR
 LDC	@RR8,R9		! Scheinausgabe zur Impulserzeug. !
 POP	RP
 RET
 
! Behandlung ESC-Folgen
  Bit 3 in STAT2:	0 ---> VT100-Terminal
			1 ---> ADM31-Terminal !
ESC_FLG:
	tm	STAT2,#B3
	jp	nz,ESC_SEQUENZ	!ADM31-Terminal!
	jp	ESC_VT100	!VT100-Terminal!
 
END PUTA
$PAGE
TGETCHAR PROCEDURE
 !*******************************************************
 *
 * Spezielle Anpassung an IBM (XT) Tastatur
 * Input:	ZBYTE		Abtastcode
 * Output:	CHAR		ASCII-Zeichen
 *
 *******************************************************!
 
ENTRY
 
	push	rp
	srp	#%20
	tcm	r1,#B7		!Taste mit Vorbyte E1 ?!
	jr	z,TGET0
	tcm	r0,#B0
	jr	z,TGET2
	jr	TGET3
TGET0:
	ld	r6,r1
	clr	r1
	tcm	r0,#B0
	jr	z,TGET3
	ld	r1,r6
	sub	r1,#B7
	add	r3,r1
	jr	nc,TGET1
	inc	r2
TGET1:
	ldc	r1,@rr2		!Wert aus SHIFT_Tab!
! NORMAL-STATUS !
	ld	r2,#HI NORMAL_Tab
	ld	r3,#LO NORMAL_Tab
	jr	TGET3
TGET2
	ld	r6,r1
	clr	r1
	tcm	r6,#%40
	jr	nz,TGET3
	cp	r6,#%7B
	jr	gt,TGET3
	and	r6,#%1F
	ld	r1,r6
	jr	TGET3
TGET3:
	tcm	r0,#%08
	jr	nz,TGET7
	tcm	r0,#%02
	jr	nz,TGET3
	cp	r1,#%41
	jr	lt,TGET3
	cp	r1,#%5A
	jr	gt,TGET3
	add	r1,#%20
TGET3:
	ld	CHAR,r1
	tcm	r0,#%04
	jr	nz,TGET5
	tcm	STAT1,#%01
	jr	z,TGET6
	or	STAT1,#%01
	ld	r4,#%020
	ld	r5,#%00
	ldc	@rr4,r5
	jr	TGET6
TGET5:
	tcm	STAT1,#%01
	jr	nz,TGET6
! Zeichensatz 1 (amerikanisch) !
	and	STAT1,#NB0
	ld	r4,#HI ZG0
	ld	r5,#LO ZG0
	ldc	@rr4,r5
TGET6:
	and	STAT0,#NB4 land NB5
	pop	rp
	ret
TGET7:
	tm	STAT1,#%01
	jr	z,TGET3
	tm	r0,#%02
	jr	z,TGET8
	cp	r1,#%7A
	jr	ule,TGET3
	cp	r1,#%7E
	jr	nc,TGET3
	sub	r1,#%20
	jr	TGET3
TGET8:
	cp	r1,#%5A
	jr	ule,TGET3
	cp	r1,#%5E
	jr	nc,TGET3
	add	r1,#%20
	jr	TGET3
END TGETCHAR
 
$PAGE
!
Abtastcode	Normal	ASCII	SHIFT	ASCII	
Dez/Hex								
---------------------------------------------
 
 
0   00		+	2B	+	2B
1   01		ESC	1B	ESC	1B
2   02		1	31	Ausr.Z. 21
3   03		2	32	"	22
4   04		3	33	@ Par*	40
5   05		4	34	$	24
6   06		5	35	%	25
7   07		6	36	&	26
8   08		7	37	/	2F
9   09		8	38	(	28
10  0A		9	39	)	29
11  0B		0	30	=	3D
12  0C		~  sz*	7E	? 	3F
13  0D		'	27	`	60
14  0E		DEL	7F	DEL	7F
15  0F		BTAB	09	BTAB	09
 
16  10		q	71	Q	51
17  11		w	77	W	57
18  12		e	65	E	45	
19  13		r	72	R	52
20  14		t	74	T	54
21  15		z	7A	Z	5A
22  16		u	75	U	55
23  17		i	69	I	49
24  18		o	6F	O	4F
25  19		p	70	P	50
26  1A		] ue*	5D	} Ue*	7D
27  1B		+	2B	*	2A
28  1C		CR	0D	CR	0D
29  1D		TAB	82	TAB	82
30  1E		a	61	A	41	
31  1F		s	73	S	53	
 
32  20		d	64	D	44	
33  21		f	66	F	46
34  22		g	67	G	47
35  23		h	68	H	48
36  24		j	6A	J	4A
37  25		k	6B	K	4B
38  26		l	6C	L	4C
39  27		 oe*	5C	| Oe*	7C
40  28		[ ae*	5B	{ Ae*	7B
41  29		BS	08	BS	08
42  2A		SHIFT	80	SHIFT	80
43  2B		#	23	^	5E
44  2C		y	79	Y	59
45  2D		x	78	X	58
46  2E		c	63	C	43
47  2F		v	76	V	56
 
48  30		b	62	B	42
49  31		n	6E	N	4E
50  32		m	6D	M	4D
51  33		,	2C	;	3B
52  34		.	2E	:	3A
53  35		-	2D	_	5F
54  36		SHIFT	80
55  37		*	2A
56  38		CTRL	80
57  39		SP	20
58  3A		CAPS L.	80
59  3B		SI/S0	F2
60  3C		MOD	F3
61  3D		VIDEO	F4
62  3E		BREAK	F1
63  3F		F1	80
 
64  40		F2	80
65  41		F3	80
66  32		F4	80
67  43		F5	80
68  44		F6	80
69  45		CE	08
70  46		F10	80
71  47		7	37
72  48		8	38
73  49		9	39
74  4A		-	5F
75  4B		4	34
76  4C		5	35
77  4D		6	36
78  4E		=	3D
79  4F		1	31
 
80  50		2	32
81  51		3	33
82  52		0	30
83  53		,	2C
84  54		OFF	F0
85  55			80
86  56		<	3C	>	3E
87  57		F7	80
88  58		F8	80
 
* Zeichengenerator ZG1 (Umlaute)
!
NORMAL_Tab	array [85 byte] :=
	[%00 %00 %81 %F1 %00 %00 %86 %87 %88 %89 %8A %8B %8C %09 %F0 %00
	 %00 %00 %00 %00 %00 %00 %00 %00 %00 %00 %00 %1B %00 %9D %0D %00
	 %80 %82 %81 %85 %00 %00 %00 %00 %00 %00 %00 %00 %2C %00 %00 %00
	 %30 %31 %32 %33 %34 %35 %36 %37 %38 %39 %00 %2E %00 %2D %00 %00
	 %83 %80 %84 %00 %00 %00 %00 %00 %00 %00 %00 %00 %00 %8F %08 %80
	 %2B %90 %A0 %80 %80]
 
 
$PAGE
START_DISPLAY PROCEDURE
 !*******************************************************
 *
 * CRTC starten
 *
 *******************************************************!
ENTRY
 
 LD	R8,#HI CRT_COM
 LD	R9,#LO CRT_COM
 LD	R10,#START_DISPL
 LDE	@RR8,R10
 RET
 
 
END START_DISPLAY
 
 
 
SGETCHAR PROCEDURE
 !*******************************************************
 *
 * Zeichen wird aus Sio-Puffer gelesen
 * und in Zelle CHAR abgelegt
 *
 *******************************************************!
ENTRY
 
 dec	SCOUNT		! Counter erniedrigen !
 cp	SCOUNT,#0
 jr	nz,SGET1	! Sprung wenn Counter > 0 !
 tcm	STAT1,#B2	! Wurde CTRL_S gesendet? !
 jr	nz,SGET2
 ld	CHAR,#DC1	! CTRL_Q !
 call	SIO_OUT		! CTRL_Q senden !
 and	STAT1,#NB2	! CTRL_S-Flag ruecksetzen !
 
SGET2:
 AND	STAT0,#NB6
 AND	STAT0,#NB7
 
SGET1:
 LD	CHAR,@SOUTPTR	! Zeichen lesen !
 INC	SOUTPTR		! Outpointer erhoehen !
 CP	SOUTPTR,#SWERT+SBUFL	! Endwert ueberschritten? !
 JR	NZ,SGET0
 LD	SOUTPTR,#SWERT	! Init-Wert !
 
SGET0:
 RET
 
 
END SGETCHAR
 
 
 
SIO_OUT PROCEDURE
 !*******************************************************
 *
 * Ausgabe ueber SIO in on_line Mode
 * Es werden nur ASCII-Zeichen %01...%7F ausgegeben
 * Das NUL-Zeichen %00 wird unterdrueckt.
 *
 *******************************************************!
ENTRY
 
 CP	CHAR,#%00	! NUL-Zeichen? !
 JR	Z,SIO0		! wenn ja, Aussprung !
 
SIO1:
 TM	IRQ,#B4		! Sender bereit !
 JR	Z,SIO1		! warten auf Sendebereitschaft !
 LD	SIO,CHAR	! Zeichen senden !
 AND	IRQ,#NB4	! TX-Flag ruecksetzen !
 
SIO0:
 RET
 
 
END SIO_OUT
 
 
 
BREAK PROCEDURE
 !*******************************************************
 *
 *
 * Break-Funktion ausfuehren:
 * Senden %00 fuer die Dauer der halbierten Baudrate
 *
 *******************************************************!
ENTRY
 
 TCM	STAT0,#B3	! nur bei on_line wirksam !
 JR	NZ,BREAK0
 CP	CHAR,#%F1	! nochmalige Pruefung !
 JR	NZ,BREAK0
 CLR	CHAR		! NUL in CHAR !
 PUSH	RP
 SRP	#%20		! Reg-Gr. 2 !
 LD	R7,BDR		! aktuelle Baudrate holen !
 ADD	R7,R7		! Baudrate halbieren !
 LD	T0,R7		! neue Zeitkonst. in T0 !
 OR	TMR,#%03	! T0 laden und starten !
 
BREAK2:
 TM	IRQ,#B4		! Sender bereit !
 JR	Z,BREAK2	! warten auf Sendebereitschaft !
 LD	SIO,CHAR	! Zeichen senden !
 AND	IRQ,#NB4	! TX-Flag ruecksetzen !
 
BREAK1:
 TM	IRQ,#B4		! Senden beendet? !
 JR	Z,BREAK1
 LD	T0,BDR		! aktuelle Baudrate wieder laden !
 OR	TMR,#%03	! T0 laden und starten !
 POP	RP
 
BREAK0:
 RET
 
 
END BREAK
 
END HAUPT
