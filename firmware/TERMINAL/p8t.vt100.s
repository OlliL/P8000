 !*******************************************************
 *							*
 *		T E R M I N A L				* 
 *							*
 *   mit Einchip-Mikrorechner U 882 und CRTC 8275	*
 *   ============================================	*
 *							*
 *  ESCAPE_VT100 Module; Version 1.0:			*
 *    - 24 Zeilen / 80 Zeichen				*
 *  Inhalt:						*
*     - Auswertung der VT100-Escape-Sequenzen		*
*		       (ANSI-Standard 3.64)		*
 *							*
 ********************************************************
 
 Bearbeiter:	Kuehle / WAE
 Stand:		03.09.87
 
 Disk:		Kuehle (Mini)
 Name:		P8T.ESCVT100.S !
 
 
 
ESCAPE_VT100 MODULE
 
 
CONSTANT
 
! 'Z8T.VEREINBARUNGEN' !
 
! Werte !
P01EXT	:=	%96	! P0:=A8...A14, P1:=AD0...AD7, Stack intern !
P01TRI	:=	%9E	! tri state: P0,P1,AS,DS,R/W !
 
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
CRT_PAR		:=	%1C00	! Parameter lesen / schreiben !
CRT_COM		:=	%1C01	! Commands schreiben !
CRTL_STAT	:=	%1C01	! Status lesen !
ZG0		:=	%4000	! ADM31-ASCII-Zeichensatz !
ZG1		:=	%2000	! Alternativ-Zeichensatz !
BELL_ADR	:=	%8000	! Signalton !
 
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
! Temporaere Vereinbarungen Selektierung VT100-Escape-Sequenzen Gruppe 10 !
ADR	:=22
SIG1	:=23
SIG3	:=24
ZSEM	:=25		!Zaehler Semikolon ;!
ZESCF	:=26		!Zaehler ESC-Folge!
ZAHL1	:=27		!Parameter n;row;column BCD-codiert!
ZAHL2	:=28
ZAHL3	:=29
ZAHL4	:=30
ZAHL5	:=31
PARA	:=254		!Parameter fuer n;row;column binaer-codiert!
ZWSP1	:=14		!Zwischenspeicher row in CPR!
ZWSP2	:=15
S_ZEILZ	:=42		!Save und Restore Cursor attributes	ZEILZ!
S_SPALTZ:=43		!					SPALTZ!
$PAGE
EXTERNAL
	PUTA		PROCEDURE
	WRITE_CHAR	PROCEDURE
	READ_CHAR	PROCEDURE
	BRAM_ADR	PROCEDURE
	LOAD_CURSOR	PROCEDURE
	CLEAR_LINE	PROCEDURE
	ROLL_DOWN	PROCEDURE
	CLEAR_B		LABEL
	ROLL_U2		LABEL
	LINE_FEED	LABEL
	CARRIAGE_RETURN	LABEL
	CLEAR_SC	LABEL
	PAGE_ERASE	LABEL
	ESC_AUS2	LABEL
	LINE_ERASE	LABEL
 
	STAT0, STAT1, STAT2			BYTE
	CHAR_ADR				WORD
	SPALTZ, ZEILZ, CHAR			BYTE
 
 
 
GLOBAL
	C_left C_rigth C_ft			LABEL
	CH_del CH_ins				LABEL
 
$SECTION SEC5 PROGRAM
 
ESC_VT100 PROCEDURE
 
ENTRY
	inc	r10		!Zaehler ESCF erhoehen!
	cp	ZESCF,#10	!max. Laenge ESC-Folge ?!
	jr	ugt,ESC_ERROR	!Zeichen nach ESC>10!
	cp	ZESCF,#1	!1.Zeichen nach ESC ?!
	jp	nz,ESC11	!nein!
! Set Anfangsbedingungen Behandlung ESC-Folgen !
 
	clr	SIG1
	clr	SIG3
	clr	ZSEM
	clr	ZAHL1
	clr	ZAHL2
	clr	ZAHL3
	clr	ZAHL4
	clr	ZAHL5
 
!Behandlung 1.Zeichen nach ESC!
	cp	CHAR,#'['
	jr	z,Z1_1
	cp	CHAR,#'D'
	jr	z,INDEX_down
	cp	CHAR,#'E'
	jr	z,NEXT_line
	cp	CHAR,#'M'
	jr	z,INDEX_up
	cp	CHAR,#'7'
	jr	z,SAVE_c
	cp	CHAR,#'8'
	jr	z,RESTORE_c
	cp	CHAR,#'('
	jr	z,Z1_2
	cp	CHAR,#')'
	jr	z,Z1_3
	cp	CHAR,#'#'
	jr	z,Z1_4
 
!ESC-Folgen nicht behandeln (z.Zt.)
	TAB STOPS
	MODES
	SCROLLING REGION
	STATUS REPORT
	WHAT ARE YOU
	PRINTER SUPPORT COMMANDS
	CURSOR POSITION REPORT
	CHARACTER SETS (G0 and G1)
	MISCELLANEOUS
	PERSOFT VT100 EXTENSION !
ESC_NBH:
 
!Fehler in ESC-Folge
	Zeichen nach ESC>10
	Kein gueltiger Final-character
	n;row;column ungueltig
	u.a.m.!
ESC_ERROR:
 
!Ende ESC-Folge
	ESC-Folge okay und behandelt !
ESC_ENDE:
	and	STAT1,#NB4	!ESC-Flag reset!
	ret			!Ruecksprung!
 
!1.Zeichen nach ESC!
Z1_1:
	or	SIG1,#B0
	ret
Z1_2:
	or	SIG1,#B1
	ret
Z1_3:
	or	SIG1,#B2
	ret
Z1_4:
	or	SIG1,#B3
	ret
 
!   C U R S O R   M O V E M E N T   C O M M A N D S
    ***********************************************   !
 
! Index down, with scroll	ESC D
  *********************************** !
INDEX_down:
	call	LINE_FEED
	jr	ESC_ENDE
 
! Next line, with scroll	ESC E
  *********************************** !
NEXT_line:
	call	LINE_FEED
	call	CARRIAGE_RETURN
	jr	ESC_ENDE
 
! Index up, with scroll		ESC M
  *********************************** !
INDEX_up:
	dec	ZEILZ
	jr	pl,up1		!ZEILZ>=0!
	clr	ZEILZ
	ld	PARA,#1
	call	LI_ins
	jr	ESC_ENDE
up1:
	call	LOAD_CURSOR
	jr	ESC_ENDE
 
! Save cursor attributes	ESC 7
  *********************************** !
SAVE_c:
	ld	S_ZEILZ,ZEILZ
	ld	S_SPALTZ,SPALTZ
	jr	ESC_ENDE
 
! Restore cursor attributes	ESC 8
  *********************************** !
RESTORE_c:
	ld	ZEILZ,S_ZEILZ
	ld	SPALTZ,S_SPALTZ
	call	LOAD_CURSOR
	jr	ESC_ENDE
 
! 1.Zeichen war !
ESC11:
	tm	SIG1,#B0	!"[" ?!
	jr	z,ESC_NBH	!ESC-Folgen nicht behandeln!
	cp	ZESCF,#2	!2. Zeichen nach ESC ?!
	jr	nz,ESC21	!nein!
	cp	CHAR,#'?'	!2. Zeichen ist Fragezeichen ?!
	jr	nz,ESC21	!nein!
	or	SIG3,#B0	!set Kennz. fuer "?"!
	ret
 
ESC21:
	cp	CHAR,#%3f	!Final-Character ?!
	jr	ule,ESC41	!kein Buchstabe!
	tm	SIG1,#B5	!Fehler in ESC-Folge ?!
	jr	nz,ESC_ERROR	!ja!
 
! Final-Character Gross- und Kleinbuchstaben 
  ****************************************** !
 
	cp	CHAR,#'J'
	jp	z,C_screen	!Erasing screen!
	cp	CHAR,#'K'
	jp	z,C_line
	cp	CHAR,#'H'
	jp	z,C_da		!Direct cursor addressing!
	cp	CHAR,#'f'
	jp	z,C_da		!Direct cursor addressing!
	cp	CHAR,#'m'
	jp	z,CH_atb	!CHARACTER ATTRIBUTES!
	cp	CHAR,#'n'
	jp	z,CPR		!CURSOR POSITIONS REPORT!
 
! Umwandlung Parameter "n" von BCD-codiert in BINAER-codiert!
	tm	SIG3,#3		!War "?" und ";" in ESC-Folge ?!
	jp	nz,ESC_ERROR	!ja!
	clr	ZSEM
	call	BCD_BIN		!Umwandlung BCD in Binaer!
	cp	PARA,#0		!kein Parameter ?!
	jr	nz,ESC22	!nein!
	inc	PARA		!PARA=1!
ESC22:
! Selectierung Final-Character !
 
	cp	CHAR,#'L'
	jp	z,LI_ins	!Insert n lines!
	cp	CHAR,#'M'
	jp	z,LI_del	!Delete n lines!
	cp	CHAR,#'@'
	jp	z,CH_ins	!Insert n characters!
	cp	CHAR,#'P'
	jp	z,CH_del	!Delete n characters!
	cp	CHAR,#'A'
	jp	z,C_unl		!Cursor up n lines!
	cp	CHAR,#'B'
	jp	z,C_dnl		!Cursor down n lines!
	cp	CHAR,#'C'
	jp	z,C_rnc		!Cursor rigth n columns!
	cp	CHAR,#'D'
	jp	z,C_lnc		!Cursor left n columns!
	cp	CHAR,#'Z'
	jp	z,C_bt		!Cursor backwards tabs!
	cp	CHAR,#'I'
	jp	z,C_ft		!Cursor forwards tabs!
	jp	ESC_NBH
 
! Behandlung Parameter n;row;column (ASCII-Ziffern 0 ... 9 in ESC-Folge
  ********************************************************************  !
 
ESC41:
	cp	CHAR,#%30
	jp	ult,ESC43	!kein ASCII-Zeichen 0 ... 9!
	cp	CHAR,#%39
	jr	ugt,ESC42	!kein ASCII-Zeichen 0 ... 9!
! Behandlung ASCII-Zeichen 0 ... 9
  . Umwandlung in BCD-Zahl
  . begrenzt durch ";"
  . max. 2-stellig 0 ... 99 sonst Fehler in ESC-Folge!
	cp	CHAR,#%30	!"0" ?!
	jr	nz,ESC44	!nein!
	tm	SIG1,#B4	!Vornullenunterdrueckung ?!
	jr	z,ESC45		!ja!
	jr	ESC46		!nein!
ESC44:
	or	SIG1,#B4	!Vornullenunterdrueckung "aus"!
ESC46:
	or	SIG3,#B2	!Set "Zahl war"!
	and	CHAR,#%0F	!ASCII-Zahl ---> Binaerzahl!
	tm	SIG1,#B6	!L-Teil abspeichern ?!
	jr	z,ESC47		!ja!
	tm	SIG1,#B7	!H-Teil abspeichern ?!
	jr	nz,ESC48	!nein Fehler!
	or	SIG1,#B7	!Set "H-Teil"!
	ld	r6,ZAHL1(r9)	!r6=ZAHL1(ZSEM)!
	SWAP	ADR
	or	ADR,CHAR	!r6=r6 "or" CHAR!
	ld	ZAHL1(r9),r6	!ZAHL1(ZSEM)=r6!
ESC45:
	ret
ESC47:
	or	SIG1,#B6	!Set "L-Teil"!
	ld	r6,CHAR
	ld	ZAHL1(r9),r6	!ZAHL1(ZSEM)=r6!
	jr	ESC45
ESC42:
	cp	CHAR,#';'
	jr	nz,ESC48
	or	SIG3,#B1	!Set ";"!
	and	SIG1,#%2F	!SIG1(4,6,7)=0!
	inc	r9		!ZSEM=ZSEM+1!
	cp	ZSEM,#4		!Zaehler ";">4 ?!
	jr	ule,ESC45	!nein!
ESC48:
	or	SIG1,#B5	!Set "error" ESC-Folge!
	jr	ESC45
ESC43:
	cp	CHAR,#'-'
	jp	z,ESC_NBH	!PERSOFT VT100 EXTENSION!
	jr	ESC48
 
! Umwandlung BCD-Zahl in Binaerzahl
  Input-Parameter:	ZSEM	Zeiger ZAHL1 ... ZAHL5
  Output-Parameter:	PARA	Binaerzahl
  *************************************************** !
BCD_BIN:
	ld	r7,ZAHL1(r9)	!SIG1=ZAHL1(ZSEM)!
	and	SIG1,#%0F	!SIG1(4-7)=0!
	ld	PARA,r7		!PARA=SIG1!
	ld	r7,ZAHL1(r9)	!SIG1=ZAHL1(ZSEM)!
	and	SIG1,#%F0	!SIG1(0-3)=0!
	swap	SIG1
	ld	r8,#10		!SIG3=10!
BCD1:
	cp	SIG1,#0		!SIG1=0 ?!
	jr	z,BCD2		!ja!
	sra	SIG1		!c=1 ?!
	jr	nc,BCD3		!nein!
	add	PARA,SIG3	!PARA=PARA+SIG3!
BCD3:
	add	r8,r8		!SIG3=SIG3+SIG3!
	jr	BCD1
BCD2:
	ret
 
 
!  C U R S O R   M O V E M E N T   C O M M A N D S
   ***********************************************  !
 
! Cursor up n lines	E S C [ n A
  ********************************* !
C_unl:
	call	C_up
	jp	ESC_ENDE
C_up:
	sub	ZEILZ,PARA	!ZEILZ=ZEILZ-PARA!
	jr	pl,C_up1	!ZEILZ>=0!
	clr	ZEILZ		!ZEILZ=0!
C_up1:
	call	LOAD_CURSOR
	ret
 
! Cursor down n lines	E S C [ n B
  ******************************** !
C_dnl:
	call	C_down
	jp	ESC_ENDE
C_down:
	add	ZEILZ,PARA	!ZEILZ=ZEILZ+PARA!
	cp	ZEILZ,#ZEILENZAHL-1 !ZEILZ>(ZEILENZAHL-1) ?!
	jr	ule,down1	!nein!
	ld	ZEILZ,#ZEILENZAHL-1
down1:
	call	LOAD_CURSOR
	ret
 
! Cursor rigth n columns	E S C [ n C
  ********************** !
C_rnc:
	call	C_rigth
	jp	ESC_ENDE
C_rigth:
	add	SPALTZ,PARA	!SPALTZ=SPALZ+PARA!
	cp	SPALTZ,#ZEICHENZAHL-1 !SPALTZ>(ZEICHENZAHL-1) ?!
	jr	ule,rigth1	!nein!
	ld	SPALTZ,#ZEICHENZAHL-1
rigth1:
	call	LOAD_CURSOR
	ret
 
! Cursor left n columns	E S C [ n D
  ********************************* !
C_lnc:
	call	C_left
	jp	ESC_ENDE
C_left:
	sub	SPALTZ,PARA	!SPALTZ=SPALTZ-PARA!
	jr	pl,left1	!SPALTZ>=0!
	clr	SPALTZ
left1:
	call	LOAD_CURSOR
	ret
 
! Direct cursor addressing	E S C [ row;col H
				E S C [ row;col f 
  *********************************************** !
C_da:
	tm	SIG3,#B0	!"?" in ESC-Folge ?!
	jr	nz,da1		!ja!
	tm	SIG3,#B1	!";" in ESC-Folge ?!
	jr	z,da2		!nein!
	cp	ZSEM,#1		!nur 1 x ";" in ESC-Folge ?!
	jr	nz,da1		!nein!
	clr	ZSEM		!Zeiger Umwandlung ZAHL1!
	call	BCD_BIN
	cp	PARA,#0		!row-Parameter=0 ?!
	jr	z,da3		!ja!
	cp	PARA,#ZEILENZAHL !PARA>ZEILENZAHL ?!
	jr	ugt,da7		!ja!
	dec	PARA		!PARA=PARA-1!
	jr	da3
da7:
	ld	ZEILZ,#ZEILENZAHL-1
	jr	da4
da3:
	ld	ZEILZ,PARA
da4:
	ld	r9,#1		!ZSEM=1 Zeiger Umwandlung ZAHL2!
	call	BCD_BIN
	cp	PARA,#0		!col-Parmeter=0 ?!
	jr	z,da5		!ja!
	cp	PARA,#ZEICHENZAHL !PARA>ZEICHENZAHL ?!
	jr	ugt,da8		!ja!
	dec	PARA		!PARA=PARA-1!
da5:
	ld	SPALTZ,PARA	!SPALTZ=PARA!
	jr	da6
da8:
	ld	SPALTZ,#ZEICHENZAHL-1
da6:
	call	LOAD_CURSOR
da1:
	jp	ESC_ENDE
da2:
	tm	SIG3,#B2	!war Zahl ?!
	jr	nz,da1
!Cursor home !
	clr	SPALTZ
	clr	ZEILZ
	jr	da6
 
! Cursor backwards tabs		E S C [ n Z
  ***************************************** !
C_bt:
	cp	PARA,#10	!PARA>10 ?!
	jr	ugt,bt2		!ja!
bt1:
	sub	SPALTZ,#8	!SPALTZ=SPALTZ-8!
	dec	PARA
	jr	nz,bt1		!PARA=0 ?!
				!ja!
	and	SPALTZ,#%F8	!TAB-Position setzen!
	jr	pl,bt3		!SPALTZ>=0 ?!
				!ja!
bt2:
	clr	SPALTZ
bt3:
	call	LOAD_CURSOR
	jp	ESC_ENDE
 
! Cursor forward tabs		E S C [ n I
  ***************************************** !
C_ft:
	cp	PARA,#10	!PARA>10 ?!
	jr	ugt,ft2		!ja!
ft1:
	add	SPALTZ,#8	!SPALTZ=SPALTZ+8!
	dec	PARA
	jr	nz,ft1		!PARA=0 ?!
				!ja!
	and	SPALTZ,#%F8	!TAB-Position setzen!
	cp	SPALTZ,#ZEICHENZAHL-1 !SPALTZ>ZEICHENZAHL-1 ?!
	jr	ule,ft3		!nein!
ft2:
	ld	SPALTZ,#ZEICHENZAHL-1
ft3:
	call	LOAD_CURSOR
	jp	ESC_ENDE
 
 
!  E R A S I N G
   *************  !
 
! From cursor to end of screen	E S C [ J ; E S C [ 0 J
  From top of screen to cursor	E S C [ 1 J
  Entire screen			E S C [ 2 J
  ***************************************************** !
C_screen:
	tm	SIG3,#3		!War "?" oder ";" in ESC-Folge ?!
	jp	nz,ESC_ERROR	!ja!
	clr	ZSEM
	call	BCD_BIN		!Umwandlung BCD in Binaer!
	cp	PARA,#0		!PARA=... ?!
	jr	z,sc1
	cp	PARA,#1
	jr	z,sc2
	cp	PARA,#2
	jp	nz,ESC_ERROR
! Entire screen !
	call	CLEAR_SC
	jp	ESC_ENDE
! From cursor to end of screen !
sc1:
	call	PAGE_ERASE
	jp	ESC_ENDE
! From top of screen to cursor !
sc2:
	call	ELINE_CTB
	cp	ZEILZ,#0	!ZEILZ=0 ?!
	jp	z,ESC_ENDE
	ld	r6,ZEILZ	!Zaehler!
sc3:
	decw	rr8		!BRAM-Adr. erniedrigen!
	call	CLEAR_LINE	!Zeile loeschen!
	djnz	r6,sc3		!Zaehler=0 ?!
				!ja!
	jp	ESC_ENDE
 
! From cursor to end of line	E S C [ K ; E S C [ 0 K
  From begin of line to cursor	E S C [ 1 K
  Entire line containing cursor	E S C [ 2 K
  ****************************************************** !
C_line:
	tm	SIG3,#3		!War "?" oder ";" in ESC-Folge ?!
	jp	nz,ESC_ERROR	!ja!
	clr	ZSEM
	call	BCD_BIN		!Umwandlung BCD in Binaer!
	cp	PARA,#0		!PARA=... ?!
	jr	z,line1		
	cp	PARA,#1
	jr	z,line2
	cp	PARA,#2
	jp	nz,ESC_ERROR
! Entire line containing cursor !
	ld	r8,#HI ZADR_TAB
	ld	r9,#LO ZADR_TAB
	add	r9,ZEILZ
	call	CLEAR_LINE
	jp	ESC_ENDE
! From cursor to end of line !
line1:
	call	LINE_ERASE
	jp	ESC_ENDE
! From begin of line to cursor !
line2:
	call	ELINE_CTB
	jp	ESC_ENDE
 
ELINE_CTB:
	ld	r8,#HI ZADR_TAB	!BRAM-Adr. einstellen!
	ld	r9,#LO ZADR_TAB
	add	r9,ZEILZ
	call	CLEAR_B
	ret
 
!  C O M P U T E R   E D I T I N G
   *******************************  !
 
! Insert n characters		E S C [ n @
  ***************************************** !
CH_ins:
	ld	r8,#HI ZADR_TAB	!BRAM-Adr. einstellen!
	ld	r9,#LO ZADR_TAB
	add	r9,ZEILZ
	ld	r15,SPALTZ
	call	BRAM_ADR
	ld	r11,PARA	!Zaehler!
C_ins1:
	push	r8		!Store BRAM-Adr.!
	push	r9
	push	r15
	cp	ZAHL5,#ZEICHENZAHL-1 !r15=(ZEICHENZAHL-1) ?!
	jr	nz,C_ins2	!nein!
	ld	r13,#FAC	!letzte Spalte!
	call	WRITE_CHAR
	jr	C_ins0
C_ins2:
	call	READ_CHAR	!Zeichen lesen!
	ld	r13,#' '
	call	WRITE_CHAR	!Schreiben " "!
C_ins3:
	ld	r13,r12		!Zeichen umladen!
	add	r9,PARA		!BRAM-Adr. erhoehen!
	adc	r8,#0
	add	r15,PARA
	cp	ZAHL5,#ZEICHENZAHL-1 !r15>(ZEICHENZAHL-1) ?!
	jr	ugt,C_ins4	!ja!
	call	READ_CHAR	!Zeichen lesen!
	cp	ZAHL5,#ZEICHENZAHL-1 !r15=ZEICHENZAHL-1 ?!
	jr	nz,C_ins5	!nein!
	cp	ZAHL2,#FAC	!r12=FAC ?!
	jr	z,C_ins4	!ja!
	call	WRITE_CHAR	!vorher gelesenes Zeichen schreiben!
	jr	C_ins4
C_ins5:
	call	WRITE_CHAR	!vorher gelesenes Zeichen schreiben!
	JR	C_ins3
C_ins4:
	djnz	r11,C_ins6	!Zaehler=0 ?!
C_ins0:
	pop	r15		!Restore!
	pop	r9
	pop	r8
	jp	ESC_ENDE	!ja!
C_ins6:
	pop	r15		!Restore BRAM-Adr.!
	pop	r9
	pop	r8
	incw	rr8		!BRAM-Adr. erhoehen!
	inc	r15
	jr	C_ins1
 
! Delete n characters		E S C [ n P
  **************************************** !
CH_del:
	ld	r8,#HI ZADR_TAB	!BRAM-Adr. einstellen!
	ld	r9,#LO ZADR_TAB
	add	r9,ZEILZ
	ld	r15,SPALTZ
	call	BRAM_ADR
	jr	nz,C_del1
	ld	r13,#FAC
	call	WRITE_CHAR
	jp	ESC_ENDE
C_del1:
	push	r8		!Store BRAM-Adr.!
	push	r9
	push	r15
	add	r15,PARA	!r15=r15+PARA!
	cp	ZAHL5,#ZEICHENZAHL-1 !r15>=(ZEICHENZAHL-1) ?!
	jr	ugt,C_del2	!ja!
	add	r9,PARA		!BRAM-Adr. erhoehen!
	adc	r8,#0
	call	READ_CHAR	!Zeichen lesen!
	cp	ZAHL5,#ZEICHENZAHL-1 !r15=ZEICHENZAHL-1 ?!
	jr	nz,C_del5	!nein!
	cp	ZAHL2,#FAC	!r12=FAC ?!
	jr	z,C_del2	!ja!
	ld	r13,#FAC
	call	WRITE_CHAR
C_del5:
	ld	r13,r12		!Umladen gelesenes Zeichen!
	jr	C_del3
C_del2:
	ld	r13,#' '
C_del3:
	pop	r15		!Restore BRAM-Adr.!
	pop	r9
	pop	r8
	call	WRITE_CHAR	!Zeichen schreiben!
	djnz	r11,C_del4	!Zaehler=0 ?!
	jp	ESC_ENDE	!ja!
C_del4:
	inc	r15		!BRAM-Adr. erhoehen!
	incw	rr8
	jr	C_del1
 
! Delete n lines		E S C [ n M
  ***************************************** !
LI_del:
	ld	r14,#ZEILENZAHL
	sub	r14,ZEILZ	!Zaehler fuer Rollen!
	cp	PARA,ZAHL4	!PARA>r14 ?!
	jr	ule,L_del1	!nein!
	ld	PARA,r14	!PARA=r14!
L_del1:
	ld	r8,#HI ZADR_TAB	!BRAM-Adr. einstellen!
	ld	r9,#LO ZADR_TAB
	add	r9,ZEILZ
	djnz	r14,L_del2
	call	CLEAR_LINE	!letzte Zeile!
	jp	ESC_ENDE
L_del2:
	push	r8		!Store BRAM-Adr.!
	push	r9
	push	r14
	or	STAT0,#B2	!Rollbit set!
	call	ROLL_U2		!Zeile loeschen rollen up!
	pop	r14		!Restore!
	pop	r9
	pop	r8
	dec	PARA		!PARA=PARA-1!
	jp	z,ESC_ENDE
	jr	L_del2
 
! Insert n lines		E S C [ n L
  ***************************************** !
LI_ins:
	ld	r15,#ZEILENZAHL
	sub	r15,ZEILZ
	cp	PARA,ZAHL5	!PARA>r15 ?!
	jr	ule,L_ins1	!nein!
	ld	PARA,r15	!PARA=r15!
L_ins1:
	ld	r15,ZEILZ
	call	ROLL_DOWN	!Rollen 1 x down!
	dec	PARA		!PARA=PARA-1!
	jp	z,ESC_ENDE
	jr	L_ins1
 
 
!  C H A R A C T E R   A T T R I B U T E S
   ***************************************   !
CH_atb:
	tm	STAT2,#B4	!Video Attr. on/off ?!
	jp	nz,ESC_NBH	!off!
	tm	SIG3,#B0	!"?" in ESC-Folge ?!
	jp	nz,ESC_ERROR
	clr	CHAR
	ld	r7,#%FF		!SIG1=FF!
atb1:
	inc	r7		!SIG1=SIG1+1!
	ld	r8,ZAHL1(r7)	!SIG3=ZAHL1(SIG1)!
	cp	SIG3,#0
	jr	z,atb5		!all attributes off!
	cp	SIG3,#1
	jr	z,atb2
	cp	SIG3,#4
	jr	z,atb3
	cp	SIG3,#5
	jr	z,atb4
	cp	SIG3,#7
	jp	nz,ESC_ERROR
! Reverse video on !
	or	CHAR,#FACR
	jr	atb5
! Blink on !
atb4:
	or	CHAR,#FACB
	jr	atb5
! Underscore on !
atb3:
	or	CHAR,#FACU
	jr	atb5
! Bold on (Highlight !
atb2:
	or	CHAR,#FACH
atb5:
	dec	ZSEM		!ZSEM<0 ?!
	jr	pl,atb1		!nein!
	and	STAT1,#NB4	!reset ESC-Flag!
	or	CHAR,#FAC
	call	PUTA
	jp	ESC_ENDE
 
! CURSOR POSITIONS REPORT
  Inkoved by			ESC [ 6 n
  Response is row and col	ESC [ row ; col R
  *********************************************** !
CPR:
	tm	SIG3,#3		!"?" oder ";" in ESC-Folge ?!
	jp	nz,ESC_ERROR
	clr	ZSEM
	call	BCD_BIN
	cp	PARA,#6		!PARA=6 ?!
	jp	nz,ESC_ERROR
	ld	r6,ZEILZ
	inc	r6
	call	BIN_ASCII	!row!
	ld	ZWSP1,r7	!Zehner!
	ld	ZWSP2,r6	!Einer!
	ld	r6,SPALTZ
	inc	r6
	call	BIN_ASCII	!col!
	and	STAT1,#NB4	!Reset ESC-Flag!
	ld	CHAR,#ESC
	call	ESC_AUS2	!Ausgabe ESC!
	and	STAT1,#NB4	!Reset ESC-Flag!
	ld	CHAR,#'['
	call	ESC_AUS2	!Ausgabe "["!
! Ausgabe row !
	cp	ZWSP1,#'0'	!Zehner='0' ?!
	jr	z,CPR1
	ld	CHAR,ZWSP1
	call	ESC_AUS2
CPR1:
	ld	CHAR,ZWSP2
	call	ESC_AUS2
	ld	CHAR,#';'
	call	ESC_AUS2	!Ausgabe ";"!
! Ausgabe col !
	cp	SIG1,#'0'	!Zehner='0' ?!
	jr	z,CPR2
	ld	CHAR,r7
	call	ESC_AUS2
CPR2:
	ld	CHAR,r6
	call	ESC_AUS2
	ld	CHAR,#'R'
	call	ESC_AUS2	!Ausgabe R!
	jp	ESC_ENDE
 
! Umwandlung ZEILZ,SPALTZ in ASCII-Ziffern - Ausgabe row;col !
BIN_ASCII:
	ld	r7,#%FF		!SIG1=FF!
AS1:
	inc	r7
	sub	r6,#10		!ADR=ADR-10!
	jr	pl,AS1
	add	r6,#10		!Einer!
	ld	r8,#HI ASCII_ZI
	ld	r9,#LO ASCII_ZI
	add	r9,r7		!r9=r9+SIG1!
	adc	r8,#0
	ldc	r7,@rr8		!SIG1=(rr8)!
	ld	r8,#HI ASCII_ZI
	ld	r9,#LO ASCII_ZI
	add	r9,r6		!r9=r9+ADR!
	adc	r8,#0
	ldc	r6,@rr8		!ADR=(rr8)!
	ret
END ESC_VT100
 
ASCII_ZI	array [* byte] := '0123456789'
 
END ESCAPE_VT100
