 !*******************************************************
 *							*
 *		T E R M I N A L				* 
 *							*
 *   mit Einchip-Mikrorechner U 882 und CRTC 8275	*
 *   ============================================	*
 *							*
 *  ESCAPE_SEQUENZ Module; Version 3.1:			*
 *    - 24 Zeilen / 80 Zeichen				*
 *  Inhalt:						*
 *    - Auswertung der Escape-Sequenzen			*
 *							*
 ********************************************************
 
 Bearbeiter:	Heller/Kuehle / WAT/WAE
 Stand:		26.01.88
 
 Name:		P8T.ESCAPE !
 
 
 
ESCAPE_SEQUENZ MODULE
 
 
CONSTANT
 
! 'Z8T.VEREINBARUNGEN' !
PARA	:=254
$LISTOFF
 
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
$LISTON
$PAGE
EXTERNAL
	PUTA		PROCEDURE
	SIO_OUT		PROCEDURE
	WRITE_CHAR	PROCEDURE
	READ_CHAR	PROCEDURE
	CHAR_ADRESSE	PROCEDURE
	BRAM_ADR	PROCEDURE
	LOAD_CURSOR	PROCEDURE
	CLEAR_SCREEN	PROCEDURE
	CLEAR_LINE	PROCEDURE
	ROLL_DOWN	PROCEDURE
	CLEAR_L		LABEL
	ROLL_U2		LABEL
	ROLL_U3		LABEL
	TAB		LABEL
	BACK_TAB	LABEL
	CH_del		LABEL
	CH_ins		LABEL
 
	P0, P1, P2, P3, STAT0, STAT1, STAT2	BYTE
	CHAR_ADR				WORD
	SPALTZ, ZEILZ, CHAR, ESCH0		BYTE
 
 
GLOBAL
	CLEAR_SC PAGE_ERASE LINE_ERASE		LABEL
	ESC_AUS2				LABEL
 
$SECTION SEC3 PROGRAM
 
 
 
ESC_SEQUENZ PROCEDURE
 !*******************************************************
 *
 * Auswertung der ESC_Sequenzen
 *
 *******************************************************!
ENTRY
 
 TCM	STAT1,#B5	! Cursor-Steuerung !
 JP	Z,CURSOR_CONTROL
 TCM	STAT1,#B7	! Video-Atribute !
 JP	Z,VIDEO_CONTROL
 CP	CHAR,#'='
 JR	Z,CURSOR_FLAG
 CP	CHAR,#'G'
 JR	Z,VIDEO_FLAG
 
 and	STAT1,#NB4	! ESC-Flag ruecksetzen !
 CP	CHAR,#':'
 JR	Z,CLEAR_SC
 CP	CHAR,#'*'
 JR	Z,CLEAR_SC
 CP	CHAR,#'U'	! set PGM !
 JR	Z,SET_PGM
 cp	CHAR,#'i'	! ESC i = TAB !
 jp	z,TAB		! in PUTA !
 cp	CHAR,#'I'	! ESC I = Back TAB !
 jp	z,BACK_TAB	! in PUTA !
 CP	CHAR,#'T'	! line erase? !
 JR	Z,LINE_ERASE
 CP	CHAR,#'Y'	! page erase? !
 JR	Z,PAGE_ERASE
 CP	CHAR,#'R'	! line delete? !
 JR	Z,LINE_DELETE
 cp	CHAR,#'Q'	! char_insert !
 jr	z,CHAR_INSERT
 cp	CHAR,#'W'	! char_delete? !
 jp	z,CHAR_DEL
 cp	CHAR,#'E'	! line_insert? !
 jp	z,LINE_INSERT
 
 AND	STAT1,#NB4	! Escape_Flag ruecksetzen !
 RET
 
 
CURSOR_FLAG:
! *** Flag setzen *** !
 OR	STAT1,#B5
 RET
 
 
VIDEO_FLAG:
! *** Flag setzen *** !
 OR	STAT1,#B7
 RET
 
 
CLEAR_SC:
! *** Bildschirm loeschen, Cursor unveraendert *** !
 
 CALL	CLEAR_SCREEN
 RET
 
 
SET_PGM:
! *** set program mode *** !
 OR	STAT1,#B3	! PGM-Flag setzen !
 RET
 
 
LINE_ERASE:
! *** 'space' in Cursor-Zeile ab Cursor-Pos bis Ende *** !
 ld	r8,#HI ZADR_TAB
 ld	r9,#LO ZADR_TAB
 add	r9,ZEILZ	! Cursor-Zeile !
 ld	r15,SPALTZ
 call	CLEAR_L		! loeschen !
 RET
 
 
PAGE_ERASE:
! *** 'space' in Cursor-Zeile ab Curs.-Pos. bis Schirm-Ende !
 call	LINE_ERASE	! 'space' ab Cursor bis Zeilen-Ende !
			! rr8 enthaelt Adr. der Cursor-Zeile !
 ld	r7,ZEILZ	! Berechnung der Zahl der Zeilen !
 inc	r7
 ld	r6,#ZEILENZAHL
 sub	r6,r7		! Anzahl !
 jr	ULE,PAGE_E0	! Cursor steht in der letzen Zeile !
 
PAGE_E1:
 incw	rr8		! naechste Zeile !
 call	CLEAR_LINE	! Zeile loeschen !
 djnz	r6,PAGE_E1
 
PAGE_E0:
 ret			! Escape-Flag in LINE_ERASE rueckgesetzt !
 
 
LINE_DELETE:
! *** Cursor-Zeile loeschen, Cursor an Zeilenanfang *** !
 clr	SPALTZ		! Cursor auf Zeilenanfang !
 call	LOAD_CURSOR
 call	LINE_ERASE	! Zeile loeschen, rr8 laden !
 ld	r14,#ZEILENZAHL-1
 sub	r14,ZEILZ	! zu rollende Zeilen berechnen !
 jr	z,LINE_D0
 call	ROLL_U3		! Eintrittspunkt ohne Zeil.-loeschen !
 
LINE_D0:
 ret
 
 
 
CHAR_INSERT:
! *** Zeichen unter Cursor nach rechts verschieben *** !
	ld	PARA,#1
	call	CH_ins
	ret
 
 
 
CHAR_DEL:
! *** Cursor-Zeichen loeschen, die anderen nachschieben *** !
 
	ld	PARA,#1
	call	CH_del
	ret
 
 
LINE_INSERT:
! *** Zeile einschieben auf Cursor-Position *** !
 clr	SPALTZ		! Cursor auf Zeilenanfang !
 call	LOAD_CURSOR
 ld	r15,ZEILZ
 call	ROLL_DOWN
 ret
 
 
 
END ESC_SEQUENZ
$PAGE
CURSOR_CONTROL PROCEDURE
 !*******************************************************
 *
 * Positionierung des Cursors mittels
 * Escape-Seq: 'ESC=YX', Y = row, X = column
 *
 *******************************************************!
ENTRY
 
 LD	R8,CHAR		! Positionswert lesen (ASCII) !
 SUB	R8,#%20		! Position in Hex !
 JR	PL,CUCO0	! Fehler, wenn negativ !
 LD	R8,#0		! bei Fehler Wert:=0 !
 
CUCO0:
 TCM	STAT1,#B6	! Zeile oder Spalte? !
 JR	Z,CUCO1		! Flag=1: Spalte !
 CP	R8,#ZEILENZAHL-1
 JR	ULT,CUCO2
 LD	R8,#ZEILENZAHL-1
 
CUCO2:
 LD	ZEILZ,R8	! Zeilenkoordinate eintragen !
 OR	STAT1,#B6	! naechster Wert ist Spaltenkoordinate !
 RET
 
 
CUCO1:			! Spalte !
 CP	R8,#ZEICHENZAHL-1
 JR	ULT,CUCO3
 LD	R8,#ZEICHENZAHL-1
 
CUCO3:
 LD	SPALTZ,R8	! Spaltenkoordinate eintragen !
 AND	STAT1,#NB4 LAND NB5 LAND NB6
 CALL	LOAD_CURSOR	! Cursor positionieren !
 RET
 
 
END CURSOR_CONTROL
$PAGE
VIDEO_CONTROL PROCEDURE
 !*******************************************************
 *
 * Setzen und Ruecksetzen der 
 * Video Attribute Codes
 *
 *******************************************************!
ENTRY
 
	tm	STAT2,#B4	!Video attribute on/off ?!
	jr	nz,VIDEO1	!off!
 ! Abfrage der Parameter !
 CP	CHAR,#'0'
 JR	Z,ESCG0
 CP	CHAR,#'1'
 JP	Z,ESCG1
 CP	CHAR,#'2'
 JR	Z,ESCG2
 CP	CHAR,#'3'
 JR	Z,ESCG1
 CP	CHAR,#'4'
 JR	Z,ESCG4
 CP	CHAR,#'5'
 JR	Z,ESCG5
 CP	CHAR,#'6'
 JR	Z,ESCG6
 CP	CHAR,#'7'
 JR	Z,ESCG5
 CP	CHAR,#'8'
 JR	Z,ESCG8
 CP	CHAR,#'9'
 JR	Z,ESCG9
 CP	CHAR,#'A'
 JR	Z,ESCGA
 CP	CHAR,#'B'
 JR	Z,ESCGB
 CP	CHAR,#'C'
 JR	Z,ESCGC
 CP	CHAR,#'D'
 JR	Z,ESCGD
 CP	CHAR,#'E'
 JR	Z,ESCGE
 CP	CHAR,#'F'
 JR	Z,ESCGF
 CP	CHAR,#'('
 JR	Z,ESCG0
 CP	CHAR,#')'
 JR	z,ESCG8
VIDEO1:
 and	STAT1,#NB4 land NB7	!ESC- und Video-Flag ruecksetzen!
 RET
 
 
 
ESCG0:		! Reset to Standard Video !
 LD	CHAR,#FAC
 JR	VOUT
 
ESCG2:		! Blinken ein !
 LD	CHAR,#FAC+FACB
 JR	VOUT
 
ESCG4:		! Reverse ein !
 LD	CHAR,#FAC+FACR
 JR	VOUT
 
ESCG6:		! Reverse + Blinken ein !
 LD	CHAR,#FAC+FACB+FACR
 JR	VOUT
 
ESCG8:		! Highlight aus !
 LD	CHAR,#FAC+FACH
 JR	VOUT
 
ESCGA:		! Highlight aus + Reverse ein !
 LD	CHAR,#FAC+FACR+FACH
 JR	VOUT
 
ESCGC:		! Unterstrich ein !
 LD	CHAR,#FAC+FACU
 JR	VOUT
 
ESCGE:		! Unterstrich ein + Highlight aus !
 LD	CHAR,#FAC+FACU+FACH
 JR	VOUT
 
VOUT:
 AND	STAT1,#NB4 LAND NB7 ! ESC- und Video-Flag ruecksetzen !
 CALL	PUTA		! Ausgabe Video Attribut !
 RET
 
 
! *** Blank Routinen *** !
 
ESCG1:		! G1, G3 (blank area) !
 LD	CHAR,#%20	! Space !
 JR	VOUTB
 
ESCG5:		! G5, G7 (blank + reverse) !
 LD	CHAR,#FAC+FACR
 JR	VOUTB
 
ESCG9:		! blank + highlight aus !
 LD	CHAR,#FAC+FACH
 JR	VOUTB
 
ESCGB:		! blank + reverse + highlight (aus) !
 LD	CHAR,#FAC+FACR+FACH
 JR	VOUTB
 
ESCGD:		! blank + underline !
 LD	CHAR,#FAC+FACU
 JR	VOUTB
 
ESCGF:		! blank + underline + highlight (aus) !
 LD	CHAR,#FAC+FACU+FACH
 JR	VOUTB
 
VOUTB:
 AND	STAT1,#NB4 LAND NB7
 CALL	PUTA
 CALL	BLANK_AREA	! Feld loeschen !
 RET
 
 
 
 
END VIDEO_CONTROL
 
 
BLANK_AREA PROCEDURE
 !*******************************************************
 *
 * Loeschen eines bezeichneten Feldes
 * in einer Zeile (kein Rollen)
 * Input:	CHAR_ADR, SPALTZ
 *
 *******************************************************!
ENTRY
 
 LD	R8,CHAR_ADR
 LD	R9,CHAR_ADR+1
 LD	R10,#%20	! Space !
 LD	R12,SPALTZ
 OR	STAT0,#B1	! MCU-Zugriff !
 
BLANK0:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja warten !
 JR	Z,BLANK0
 
 INCW	RR8
 LDE	R11,@RR8	! lesen !
 TCM	R11,#B7		! Video Control Char ? !
 JR	Z,BLANK1
 
 LDE	@RR8,R10	! Blank ausgeben !
 INC	R12		! Naechste Spalte !
 CP	R12,#ZEICHENZAHL
 JR	NZ,BLANK0
 LD	R10,#FAC	! Reset to standard Video !
 LDE	@RR8,R10
 
BLANK1:
 AND	STAT0,#NB1	! MCU-Zugriff beendet !
 RET
END BLANK_AREA
 
 
 
ESC_OUT_SEQ PROCEDURE
 !*******************************************************
 *
 * Ausgabe der Escape_Sequenzen entspr. Funktionstaste
 * Es werden 1...3 Zeichen gesendet: 
 *
 *******************************************************!
ENTRY
 
 ! Ueberpruefung auf richtige Werte !
 TCM	CHAR,#%80	! ESC-Folge: CHAR>%80 !
 JP	NZ,ESC_OUT0
 CP	CHAR,#%80	! keine Wirkung !
 JR	Z,ESC_OUT0
 !------------------------------------------------------!
  
	cp	CHAR,#%C2	!00 ?!
	jr	nz,FKT_TAS	!nein!
	ld	CHAR,#'0'
	call	ESC_AUS2
	ld	ESCH0,#'0'
	jp	FKT_AUS
FKT_TAS:
	tm	STAT2,#B3	!VT100/ADM31 ?!
	jp	z,FKT_VT100
 ! Auswertung der Tasten-Codes ADM31 !
! Cursor Movement Tasten !
	ld	ESCH0,#FF	!Forspace!
	cp	CHAR,#%CD
	jp	z,FKT_AUS
	ld	ESCH0,#BS	!Backspace!
	cp	CHAR,#%CB
	jp	z,FKT_AUS
	ld	ESCH0,#LF	!Linefeed!
	cp	CHAR,#%D0
	jr	z,FKT_AUS
	ld	ESCH0,#VT	!Verticaltab!
	cp	CHAR,#%C8
	jr	z,FKT_AUS
	ld	ESCH0,#RS	!Cursor home!
	cp	CHAR,#%CF
	jr	z,FKT_AUS
! Funktionstasten !
 LD	ESCH0,#'W'	! char delete !
 CP	CHAR,#%D2
 JR	Z,ESC_SEQ_AUS
 LD	ESCH0,#'Q'	! char insert !
 CP	CHAR,#%C9
 JR	Z,ESC_SEQ_AUS
 LD	ESCH0,#'R'	! line delete !
 CP	CHAR,#%D3
 JR	Z,ESC_SEQ_AUS
 LD	ESCH0,#'E'	! line insert !
 CP	CHAR,#%D1
 JR	Z,ESC_SEQ_AUS
 LD	ESCH0,#'Y'	! page erase !
 CP	CHAR,#%C7
 JR	Z,ESC_SEQ_AUS
 ld	ESCH0,#'I'	! BACK_TAB !
 cp	CHAR,#%81
 jr	z,ESC_SEQ_AUS
 
 !------------------------------------------------------!
 
ESC_OUT0:
 CLR	CHAR
 RET
 !------------------------------------------------------!
 
 
ESC_SEQ_AUS:
 ! Senden der Escape_Sequenzen !
 LD	CHAR,#ESC	! Zeichen senden: ESC !
	call	ESC_AUS2
	tm	STAT2,#B3	!VT100/ADM31 ?!
	jr	nz,ESC_AUS3
	ld	CHAR,#'['
	call	ESC_AUS2
ESC_AUS3:
 LD	CHAR,ESCH0	! Zeichen senden: x !
	call	ESC_AUS2
	jr	ESC_OUT0
 
! Ausgabe Cursor Movement Tasten ADM31 !
FKT_AUS:
	ld	CHAR,ESCH0
	call	ESC_AUS2
	jr	ESC_OUT0
 
ESC_AUS2:
 TCM	STAT0,#B3	! on_line? !
 JR	Z,ESC_AUS1
 CALL	PUTA		! off_line !
	ret
 
ESC_AUS1:
 CALL	SIO_OUT		! on_line !
	ret
 
 
 
! Auswertung Funktionstasten VT100-Terminal !
FKT_VT100:
! Cursor Movement Tasten !
	ld	ESCH0,#'C'	!Cursor right!
	cp	CHAR,#%CD
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'D'	!Cursor left!
	cp	CHAR,#%CB
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'B'	!Cursor down!
	cp	CHAR,#%D0
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'A'	!Cursor up!
	cp	CHAR,#%C8
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'H'	!Cursor home!
	cp	CHAR,#%CF
	jr	z,ESC_SEQ_AUS
! Funktionstasten !
	ld	ESCH0,#'P'	!Delete character!
	cp	CHAR,#%D2
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'@'	!Insert character!
	cp	CHAR,#%C9
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'M'	!Delete line!
	cp	CHAR,#%D3
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'L'	!Insert line!
	cp	CHAR,#%D1
	jr	z,ESC_SEQ_AUS
	ld	ESCH0,#'J'	!Erasing from cursor to end of screen!
	cp	CHAR,#%C7
	jp	z,ESC_SEQ_AUS
	ld	ESCH0,#'Z'	!Cursor backward tab!
	cp	CHAR,#%81
	jp	z,ESC_SEQ_AUS
	jp	ESC_OUT0
 
END ESC_OUT_SEQ
 
 
 
END ESCAPE_SEQUENZ
