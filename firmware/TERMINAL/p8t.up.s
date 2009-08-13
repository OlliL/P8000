 !*******************************************************
 *							*
 *		T E R M I N A L				* 
 *							*
 *   mit Einchip-Mikrorechner U 882 und CRTC 8275	*
 *   ============================================	*
 *							*
 *  UNTERPROG Module; Version 3.1:			*
 *    - 24 Zeilen / 80 Zeichen				*
 *  Inhalt:						*
 *    - Unterprogramme fuer die anderen Module		*
 *							*
 ********************************************************
 
 Bearbeiter:	Heller / WAT
 Stand:		09.03.87
 
 Disk:		Heller.8 (Mini)
 Name:		P8T.UP !
 
 
 
UP MODULE
 
 
CONSTANT
! 'Z8T.VEREINBARUNGEN' !
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
$PAGE
EXTERNAL
	ESC_SEQUENZ				PROCEDURE
	ESC_OUT_SEQ				PROCEDURE
	BREAK					PROCEDURE
	SIO_OUT					PROCEDURE
	P0, P1, P2, P3, STAT0, STAT1, STAT2	BYTE
	SPALTZ, ZEILZ, CHAR			BYTE
 
global
	CLEAR_B					LABEL
	CLEAR_L					label
	ROLL_U2					label
	ROLL_U3					label
 
$section SEC2 program
 
 
 
CHAR_ADRESSE PROCEDURE
 !*******************************************************
 *
 * Berechnung der aktuellen Zeichenadresse
 * Reg. Gruppe:	1
 * Input:	SPALTZ, ZEILZ, ZADR_TAB
 * Output:	RR12, akt. BRAM_ADR
 *
 *******************************************************!
ENTRY
 
 ld	r8,#HI ZADR_TAB
 ld	r9,#LO ZADR_TAB
 add	r9,ZEILZ	! Tabellenzeile, Ubertrag tritt nicht auf !
 call	READ_ADR	! codierte Adresse holen !
 call	ADR_DEC		! Adresse decodieren !
 add	r13,SPALTZ	! Spalte beruecksichtigen !
 jr	nc,CRA0		! Uebertrag? !
 inc	r12
 
CRA0:
 ret			! akt. BRAM_ADR in rr12 !
 
 
END CHAR_ADRESSE
 
 
 
ADR_DEC PROCEDURE
 !*******************************************************
 *
 * Umwandlung 1byte -> 2byte Wert
 * Input:	r12, codierter Adresswert aus Tabelle
 * Output:	rr12, dekodierte BRAM_Zeilen_Adresse
 *
 *******************************************************!
ENTRY
 
 ld	r13,#%F0
 and	r13,r12
 and	r12,#%0F
 or	r12,#%10
 
ret
 
END ADR_DEC
 
 
READ_ADR PROCEDURE
 !*******************************************************
 *
 * Lesen einer codierten Adresse aus ZADR_Tabelle (BRAM)
 * Input: 	rr8, Tabellen-Adresse
 * 		r12, Inhalt der Adresse
 *
 *******************************************************!
ENTRY
 
 OR	STAT0,#B1	! MCU-Flag setzen !
 
RAWAIT:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja, warten !
 JR	Z,RAWAIT
 DI
 lde	r12,@rr8	! lesen !
 EI
 AND	STAT0,#NB1	! MCU-Flag ruecksetzen !
 RET
 
 
END READ_ADR
 
 
 
WR_ADR PROCEDURE
 !*******************************************************
 *
 * Schreiben einer codierten Adresse in ZADR_Tabelle (BRAM)
 * Input: 	rr10, Tabellen-Adresse
 *  		r12, Inhalt der Adresse
 *
 *******************************************************!
ENTRY
 
 OR	STAT0,#B1	! MCU-Flag setzen !
 
WAWAIT:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja, warten !
 JR	Z,WAWAIT
 DI
 LDE	@RR10,R12	! schreiben !
 EI
 AND	STAT0,#NB1	! MCU-Flag ruecksetzen !
 RET
 
END WR_ADR
 
 
 
WRITE_CHAR PROCEDURE
 !*******************************************************
 *
 * Beschreiben des Bildram mit einem Zeichen
 * Reg. Gruppe:	1
 * Input:	RR8 := Zieladresse 
 *		R13 := Zeichen
 *
 *******************************************************!
ENTRY
 
 OR	STAT0,#B1	! MCU-Flag setzen !
 
WRITECH0:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja, warten !
 JR	Z,WRITECH0
 DI
 LDE	@RR8,R13
 EI
 AND	STAT0,#NB1	! MCU-Flag ruecksetzen !
 RET
 
END WRITE_CHAR
 
 
 
READ_CHAR PROCEDURE
 !*******************************************************
 *
 * Ein Zeichen aus dem Bildram lesen
 * Reg. Gruppe:	1
 * Input:	RR8 := Zieladresse
 * Output:	r12 := Zeichen
 *
 *******************************************************!
ENTRY
 
 OR	STAT0,#B1	! MCU-Flag setzen !
 
READCH0:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja, warten !
 JR	Z,READCH0
 DI
 lde	r12,@rr8	! Lesen !
 EI
 AND	STAT0,#NB1	! MCU-Flag ruecksetzen !
 RET
 
 
END READ_CHAR
 
 
 
CURSOR_POS PROCEDURE
 !*******************************************************
 *
 * Berechnung der neuen Cursor-Position
 * Input:	SPALTZ, ZEILZ
 * Output:	SPALTZ, ZEILZ (neue Werte)
 *
 *******************************************************!
ENTRY
 
 INC	SPALTZ
 CP	SPALTZ,#ZEICHENZAHL
 JR	NZ,CPOS0
 CLR	SPALTZ
 INC	ZEILZ
 CP	ZEILZ,#ZEILENZAHL
 JR	NZ,CPOS0
 LD	ZEILZ,#ZEILENZAHL-1	! Cursor bleibt nach Rollen
				in der letzen Zeile !
 OR	STAT0,#B2	! Rollflag setzen !
 
CPOS0:
 CALL	LOAD_CURSOR
 RET
 
 
END CURSOR_POS
 
 
 
LOAD_CURSOR PROCEDURE
 !*******************************************************
 *
 * Laden der berechneten Cursor-Position in
 * den CRTC
 *
 *******************************************************!
ENTRY
 
 LD	R8,#HI CRT_COM
 LD	R9,#LO CRT_COM
 LD	R10,#LD_CURSOR
 LD	R11,SPALTZ	! Spaltenposition 0...79 !
 LD	R12,ZEILZ	! Zeilenposition0...23 !
 OR	STAT0,#B1	! MCU-Zugriff !
 
LCURSOR0:
 TCM	STAT0,#B0	! DMA aktiv?, wenn ja warten !
 JR	Z,LCURSOR0
 
 DI
 LDE	@RR8,R10	! out command !
 DECW	RR8
 LDE	@RR8,R11	! out char. pos in row !
 LDE	@RR8,R12	! out row number !
 EI
 
 AND	STAT0,#NB1	! MCU-Zugriff beendet !
 RET
 
 
END LOAD_CURSOR
 
 
ROLL PROCEDURE
 !*******************************************************
 *
 * Umladen der Adressen-Tabelle im BRAM
 * Input:	rr8, Zeilenadr, init: ZADR_TAB
 *		r14, Zaehler, init: ZEILENZAHL-1
 * Reg:		rr8, rr9, r12, r13, r14
 *
 *******************************************************!
ENTRY
 
 ! init-Werte !
 ld	r8,#HI ZADR_TAB
 ld	r9,#LO ZADR_TAB
 ld	r14,#ZEILENZAHL-1	! Zaehler !
 
ROLL_U2:
 ! 2. Eintrittspunkt: rr8 und r14 laden !
 TM	STAT0,#B2	! Rollbit gesetzt? !
 jr	z,ROLL_U0
 call	CLEAR_LINE	! 1. Zeile loeschen vor Rollen !
 
ROLL_U3:
 ! 3. Eintrittspunkt: kein Zeilenloeschen, r14 laden !
 ld	r10,r8		! Schreibadresse laden !
 ld	r11,r9
 call	READ_ADR	! 1. Tabellen-Wert holen !
 ld	r13,r12		! 1. Wert ablegen !
 
ROLL_U1:
 inc	r9		! naechste Adresse !
 call	READ_ADR
 call	WR_ADR		! Wert zurueckschreiben !
 inc	r11		! Schreibadresse erhoehen !
 djnz	r14,ROLL_U1	! !
 ld	r12,r13		! 1. Wert rueckschreiben !
 call	WR_ADR
 
ROLL_U0:
 AND	STAT0,#NB2	! Rollflag zuruecksetzen !
 ret
 
 
END ROLL
 
 
 
ROLL_DOWN PROCEDURE
 !*******************************************************
 *
 * Umladen der Adresentabelle im BRAM (vorwaerts)
 * Input:	rr8, Zeilenadr, init: ZADR_TAB+ZEILENZAHL
 *		r14, Zaehler, init ZEILENZAHL-1
 *		r15, ZEILZ, init 00
 * Reg:		rr8, rr9, r12, r13, r14, r15
 *
 *******************************************************!
ENTRY
 
 ! init-Werte !
 ld	r8,#HI ZADR_TAB
 ld	r9,#LO ZADR_TAB
 ld	r14,#ZEILENZAHL-1
 add	r9,r14		! letzte Zeile !
 sub	r14,r15		! Zaehler laden !
 jr	nz,ROLL_D2
 call	CLEAR_LINE	! nur loeschen, falls schon letzte Zeile !
 jr	ROLL_D0
 
ROLL_D2:
 call	CLEAR_LINE	! letzte Zeile loeschen !
 
ROLL_D3:
 ! Eintrittspunkt: kein Zeilenloeschen, r14 laden !
 ld	r10,r8		! Schreibadresse laden !
 ld	r11,r9
 call	READ_ADR	! 1. Tabellenwert holen !
 ld	r13,r12		! 1. Wert ablegen !
 
ROLL_D1:
 dec	r9		! Lese-Adr. verringern !
 call	READ_ADR	! Lesen !
 call	WR_ADR		! Rueckschreiben !
 dec	r11		! Schreibadresse verrringern !
 djnz	r14,ROLL_D1
 ld	r12,r13		! 1. Wert zurueckschreiben !
 call	WR_ADR
 
ROLL_D0:
 ret
 
 
END ROLL_DOWN
 
 
 
CLEAR_LINE PROCEDURE
 !*******************************************************
 *
 * Loeschen einer Bildzeile
 * Zeilenende: 'Reset to Standard Video'
 * Input:	RR8:=Anfangsadresse
 *		r11 = ZEICHENZAHL-1
 *		r15, Spalte fuer Beginn, init: 0
 * Register:	rr8, r10, r11, rr12, r15
 *
 *******************************************************!
ENTRY
 
 ld	r15,#%00	! Initwert fuer Spalte !
 
CLEAR_L:		! 2. Eintrittspunkt: Parameter r15 !
	and	STAT2,#NB2
cl0:
 push	r8
 push	r9
 call	BRAM_ADR
	jr	cl1
! 3. Eintrittspunkt	Loeschen vom Beginn bis Cursor !
CLEAR_B:
	or	STAT2,#B2
	ld	r15,#0
	jr	cl0
cl1:
 jr	z,ZLOESCH0
 ld	r13,#' '	! Space !
 
ZLOESCH1:
 call	WRITE_CHAR	! Space schreiben !
 incw	rr8		! naechste Adr !
 djnz	r11,ZLOESCH1
 
ZLOESCH0:
	tm	STAT2,#B2
	jr	z,ZLOESCH2
	cp	SPALTZ,#ZEICHENZAHL-1
	jr	nz,ZLOESCH3
ZLOESCH2:
 LD	R13,#FAC	! Reset to Standard Video !
 call	WRITE_CHAR
ZLOESCH3:
 AND	STAT0,#NB1	! MCU-Zugriff beendet !
 pop	r9
 pop	r8
 RET
 
 
END CLEAR_LINE
 
 
 
CLEAR_SCREEN PROCEDURE
 !*******************************************************
 *
 * Loeschen des Bildschirms (24 Zeilen)
 * am Ende jeder Zeile wird das Video-Attribut
 * 'Reset to Standard Video' eingetragen
 *
 *******************************************************!
ENTRY
 
 LD	R8,#HI ZADR_TAB
 LD	R9,#LO ZADR_TAB
 LD	R6,#ZEILENZAHL	! Zaehler !
 
CLSC0:
 CALL	CLEAR_LINE
 inc	r9		! naechste Zeile, kein Hi-Teil !
 DJNZ	R6,CLSC0
 RET
 
 
END CLEAR_SCREEN
 
 
 
BRAM_ADR PROCEDURE
 !*******************************************************
 *
 * Berechnung einer BRAM-ADR
 * Input:	rr8 = Zeilenanfangsadr., r15 = SPALTZ 
 * Output:	rr12 = physische BRAM-ADR
 *		Z-Flag, wenn letzte Spalte
 * Register:	rr8, r11, rr12, r15
 *
 *******************************************************!
ENTRY
 ld	r11,#ZEICHENZAHL-1
 call	READ_ADR	! codierte BRAM-ADR !
 call	ADR_DEC		! physische BRAM-ADR !
 ld	r8,r12		! Adr fuer WRITE_CHAR !
 ld	r9,r13
 add	r9,r15		! Anfangsadr. !
 jr	nc,BRAM0
 inc	r8		! Uebertrag? !
 
BRAM0:
	tm	STAT2,#B2
	jr	z,BRAM1		!Zeile loeschen zum Zeilenende!
				!Zeile loeschen zum Cursor!
	ld	r11,SPALTZ
	inc	r11
	ret
BRAM1:
 sub	r11,r15		! Zaehler laden !
 ret
 
END BRAM_ADR
 
 
 
END UP
