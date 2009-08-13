 !*******************************************************
 *							*
 *		T E R M I N A L				* 
 *							*
 *   mit Einchip-Mikrorechner U 882 und CRTC 8275	*
 *   ============================================	*
 *							*
 *  INIT Module; Version 5.0:				*
 *    - Anpassung an IBM (XT) Tastatur			*
 *    - 24 Zeilen / 80 Zeichen				*
 *  Inhalt:						*
 *    - Initialisierung Rechner, Bildspeicher, CRTC	*
 *    - Interrupt-Service-Routinen			*
 *							*
 ********************************************************
 
 Bearbeiter:	Heller/Kuehle / WAT/WAE
 Stand:		29.09.88
 
 Name:		P8T.INIT !
 
 
INIT MODULE
 
 
CONSTANT
 
! 'P8T.VEREINBARUNGEN' !
 
! Werte !
P01EXT	:=	%96	! P0:=A8...A14, P1:=AD0...AD7, Stack intern !
P01TRI	:=	%9E	! tri state: P0,P1,AS,DS,R/W !
SBUFL	:=	45	! SIO-Puffer-Laenge !
ZDMA	:=	26	! DMA ausschalten: 26*1.08=28 mys !
ZVSY	:=	192	! VSYN: 192*1.08=172.8 mys !
 
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
 
! spezielle Ausgabeadressen (nur High-Teil) !
ZG0		:=	%40	! ADM31-ASCII-Zeichensatz !
ZG1		:=	%20	! Alternativ-Zeichensatz !
BELL_ADR	:=	%80	! Signalton !
TRES		:=	%C0	! Schieberegister ruecksetzen !
 
! Display Commands !
RES_DISPL	:=	%00
START_DISPL	:=	%20
STOP_DISPL	:=	%40
READ_LPEN	:=	%60
LD_CURSOR	:=	%80
ENABLE_INT	:=	%A0
DISABLE_INT	:=	%C0
PRES_COUNT	:=	%E0
$PAGE
! Parameter fuer Display-Reset-Command !
RES1		:=	%4F	! 80 characters/row !
RES2		:=	%97	! !
RES3		:=	%CC	! 13. Linie Unterstrich, 13 Linien !
RES4		:=	%6A	! visible video control !
				!ruhender Cursor-Block!
RES5		:=	%4A	!volles Feld blinkend!
RES6		:=	%7A	!Unterstrich!
RES7		:=	%5A	!Unterstrich blinkend!
 
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
DEL	:=	%7F	! Delete		   !
$PAGE
EXTERNAL
 MAIN			PROCEDURE
 SIO_OUT		PROCEDURE
 
 
GLOBAL
$SECTION REGISTER
 
$REL	0
 ! Ports, Status- und Hilfsregister: Reg.-Grp. 0 !
 P0, P1, P2, P3, STAT0, STAT1, STAT2, BDR	BYTE
 CHAR_ADR					WORD
 SPALTZ, ZEILZ, CHAR, STAT3			BYTE
 
$REL	%20
 ! Tastatureingabe !
VBYTE, ZBYTE					BYTE
 
$REL	%2C
 ! Hilfregister fuer Escape_Sequenzen !
 LPOS_ADR			WORD
 ESCH0				BYTE
 
$REL	%30
 ! SIO-Input-Puffer: Reg.-Grp. 3, 4, 5 !
 SINPTR, SOUTPTR, SCOUNT	BYTE
 SWERT				ARRAY [SBUFL BYTE]
 
 
 
$SECTION PROGRAM
$REL	0
 
INTERRUPT ARRAY [6 WORD] := [IRP32 IRP33 IRP31 IRP30 IRT0 IRT1]
$PAGE
JP_MESSAGE PROCEDURE
ENTRY
 
 jr	INITIAL
 
END JP_MESSAGE
 
! Message: Copyright  Name  Version  Datum !
MESSAGE array [* byte] := '(C) ZFT/KEAW 5.0 Dec88'
 
 
INITIAL PROCEDURE
 !*******************************************************
 *
 * Initialisierung :
 * Ports, Register, SIO, Tastatur, CRTC
 *
 *******************************************************!
ENTRY
 
 
 ! Ports !
 LD	P01M,#P01EXT
 LD	P2M,#%FF	! P20...P27 Input !
 LD	P3,#%2F		! Anfangszustand, P35=1 !
 LD	P3M,#%51	! DM aktiv !
 
 ! Timer T0 !
 LD	PRE0,#%0D	! PRE0:=3, modulo n, int. Clock !
 
 ! Timer T1 !
 LD	PRE1,#%06	! PRE1:=1, single pass, int. Clock !
 ld	T1,#ZDMA	! Zeitkonstante fuer DMA-Dauer !
 
 ! Register !
 LD	SPL,#%80	! init Stackpointer !
 LD	SPH,#%00
 CLR	IRQ		! no Int.request !
 
 ! Register loeschen !
 SRP	#%00
 CLR	R4
 LD	R5,#6
 
REG0:
 LD	@5,4
 INC	5
 CP	5,#%80
 JR	NZ,REG0
 CLR	R5
 
! Zeilenadressen-Tabelle laden !
 srp	#%10
 ld	r0,#HI ZADR_TAB
 ld	r1,#LO ZADR_TAB
 ld	r2,#HI BRAM
 ld	r3,#LO BRAM
 ld	r5,#ZEILENZAHL
 
ZADR1:
 ld	r6,r2
 and	r6,#%0F		! BRAM_ADR codieren !
 or	r6,r3		! r6 enthaelt codierte ADR !
 lde	@rr0,r6		! codierte ADR in Tabelle !
 incw	rr0
 add	r3,#ZEICHENZAHL	! naechste Adr. in rr2 !
 jr	nc,ZADR0	! Uebertrag? !
 inc	r2
 
ZADR0:
 djnz	r5,ZADR1	! Schleifenzaehler !
 
 ! SIO Vorbereiten !
 LD	SINPTR,#SWERT	! Input-Pointer laden !
 LD	SOUTPTR,#SWERT	! Output-Pointer laden !
 LD	BDR,#%02	! Baudratenregister laden, 9600 Baud !
 LD	T0,BDR		! Baudrate in T0 !
 OR	TMR,#%03	! T0 laden und starten !
			! Scheinausgabe SIO in MAIN1 !
 or	STAT0,#B3	! on_line ein !
 or	STAT2,#B3	!ADM31-Mode!
 
 ! IBM (XT) Tastatur und Zeichengenerator vorbereiten !
 SRP	#%20
 LD	R4,#ZG0		! nur High-Adr interessant !
 LDC	@RR4,R5		! Scheinausgabe zum Einschalten ZG0 !
 ld	r4,#TRES
 ldc	@rr4,r5		! Schieberegister ruecksetzen !
 
 ! Vorbereitung Zeilenrefresh !
 LD	CHAR_ADR,#HI BRAM
 SRP	#%10
 ld	r0,#HI ZADR_TAB
 ld	r1,#LO ZADR_TAB
 LD	R5,#ZEILENZAHL
 
 ! CRTC initialisieren !
 CALL	RESET_DISPLAY
 JP	MAIN
 
 
END INITIAL
$PAGE
IRP31 PROCEDURE
 !*******************************************************
 *
 * Zeilenrefresh im Tristate-Zustand
 * Input:	rr0, aktuelle Tabellen-Adresse
 * Output:	rr2, aktuelle BRAM-ADR
 *
 *******************************************************!
ENTRY
 
 PUSH	RP
 SRP	#%10
 lde	r2,@rr0		! codierte BRAM_Adr.in r2 !
 ld	r3,#%F0		! Adr. decodieren zu BRAM_ADR !
 and	r3,r2
 and	r2,#%0F
 or	r2,#%10		! BRAM_ADR in rr2 !
 inc	r1		! rr0 zeigt auf die naechste ZADR !
 lde	r4,@rr2		! Zeilen-Start-Adr. in DMA-Zaehler laden !
 djnz	r5,UL2
 ld	r5,#ZEILENZAHL
 ld	r1,#LO ZADR_TAB
 
UL2:
 OR	STAT0,#B0	! STAT0_0:=1 !
 LD	P01M,#P01TRI	! Tristate ein !
 LD	IMR,#%20	! Int. T1 freigeben !
 OR	TMR,#%0C	! laden und starten !
 AND	P3,#NB5		! P35 := 0, DMA ein !
 TCM	STAT0,#B1	! MCU-Zugriff aktiv? !
 JR	NZ,UL4
 EI			! T1-Int freigeben !
UL5:
 TCM	STAT0,#B0	! DMA noch aktiv !
 JR	Z,UL5
 
UL4:
 POP	RP
 IRET
 
END IRP31
 
 
 
IRT1 PROCEDURE
 !*******************************************************
 *
 * DMA-Zugriff ausschalten 
 *
 *******************************************************!
ENTRY
 
 OR	P3,#B5		! P35:=1, DMA aus !
 LD	P01M,#P01EXT	! Ports normal !
 AND	STAT0,#NB0	! STAT0_0:=0 !
 AND	IRQ,#NB2	! 2. Int-Req. von P31 loeschen !
 LD	IMR,#%0F	! IRP31, IRP32, IRP33, IRP30 freigeben !
 LD	T1,#ZDMA	! Zeitdauer des DMA !
 AND	P3,#NB6		! P36:=0, VSYN:=0 !
 IRET
 
 
END IRT1
$PAGE
IRP32 PROCEDURE
 !*******************************************************
 *
 * Bildruecklauf einleiten
 * Laenge des VSYN-Signals durch T1 festlegen
 *
 *******************************************************!
ENTRY
 
 OR	P3,#B6		! P36:=1, VSYN:=1 !
 ld	T1,#ZVSY	! Zeitdauer VSYN !
 LD	IMR,#%2A	! IRT1, IRP33, IRP30 freigeben !
 OR	TMR,#%0C	! laden und starten !
 IRET
 
 
END IRP32
 
 
 
IR_REST PROCEDURE
 !*******************************************************
 *
 * Timer 0 Routine (wird nicht benutzt)
 *
 *******************************************************!
ENTRY
 
IRT0:
 IRET
 
 
END IR_REST
 
 
 
IRP33 PROCEDURE
 !*******************************************************
 *
 * Tastatur-Eingabe-Routine (P33-Int.) 
 * IBM (XT) Tastatur
 *	- Tastatur-Flags:
 *	- STAT0_4 = 1 nach Uebernahme des Zeichenbytes
 *	- STAT0_5 = 1 nach Uebernahme des Vorbytes E0
 *
 *******************************************************!
ENTRY
 
	push	rp
	srp	#%20
 
	or	STAT3,#B4	!Eigentest!
 
	tm	STAT0,#B4	!Zeichen verarbeitet ?!
	jp	nz,TINT0	!nein!
 
	ld	r1,P2		!Lesen von Port 2!
 
	rr	r1		!Verschieben D0->D7 ... D7->D6!
 
	cp 	ZBYTE,#%E1	!Vorbyte E1 ?!
	jr	nz,TINT14	!nein!
	or	STAT3,#B7
	jr	TINT0
TINT14:
	cp	ZBYTE,#%E0	!Vorbyte E0 ?!
	jr	nz,TINT1	!nein!
	or	STAT0,#B5
	jr	TINT0
TINT1:
	tm	ZBYTE,#B7	!Scan-/Break-Code ?!
	jr	nz,TINT2	!Break-Code!
 
! Scan-Code !
	cp	ZBYTE,#%2A	!SHIFT-Taste ?!
	jr	nz,TINT4	!nein!
	or	STAT3,#B2	!SHIFT-STATUS ein!
	jr	TINT8
TINT4:
	cp	ZBYTE,#%3A	!CAPS-LOCK-Taste ?!
	jr	nz,TINT5	!nein!
	tm	STAT3,#B0	!war CAPS-LOCK-STATUS ein ?!
	jr	nz,TINT15	!ja!
	or	STAT3,#B0	!CAPS-LOCK-STATUS ein!
	jr	TINT8
TINT15:
	and	STAT3,#NB0	!CAPS-LOCK-STATUS aus!
	jr	TINT8
TINT5:
	cp	ZBYTE,#%38	!CTRL-Taste ?!
	jr	nz,TINT6	!nein!
	and	STAT0,#NB5
	or	STAT3,#B1	!CTRL-STATUS ein!
	jr	TINT8
TINT6:
	tm	STAT3,#B7	!war Vorbyte E1 ?!
	jr	z,TINT7		!nein!
	tm	STAT3,#B6	!1. Zeichen nach E1 ?!
	jr	nz,TINT7
	or	STAT3,#B6
	ld	r0,r1		!1. Zeichen nach E1!
	jr	TINT0
TINT7:
	or	STAT0,#B4
	jr	TINT0
 
! Break-Code !
TINT2:
	and	STAT0,#NB5
	cp	ZBYTE,#%AA	!SHIFT-Taste ?!
	jr	nz,TINT11	!nein!
	and	STAT3,#NB2	!SHIFT-STATUS aus!
	jr	TINT8
TINT11:
	cp	ZBYTE,#%B8	!CTRL-Taste ?!
	jr	nz,TINT12	!nein!
	and	STAT3,#NB1	!CTRL-STATUS aus!
	jr	TINT8
TINT12:
	cp	ZBYTE,#%FC	!Error Tastatur ?!
	jr	nz,TINT13	!nein!
	or	STAT3,#B5	!ERROR-STATUS ein!
	jr	TINT8
TINT13:
	cp	ZBYTE,#%AA	!Okay Tastatur ?!
	jr	nz,TINT8	!nein!
	and	STAT3,#NB5	!ERROR-STATUS aus!
TINT8:
	and	STAT3,#NB6 land NB7
TINT0:
 ld	r4,#TRES
 
TINTW:
 tcm	STAT0,#B0	! DMA aktiv?, wenn ja, warten !
 jr	z,TINTW
 ldc	@rr4,r5		! Schieberegister ruecksetzen !
 pop	rp
 IRET
 
 
END IRP33
$PAGE
IRP30 PROCEDURE
 !*******************************************************
 *
 * Serielle Eingabe-Routine
 * Zeichen werden im SPUFFER abgelegt
 *
 *******************************************************!
ENTRY
 
 TCM	STAT0,#B7	! Fehlerbit gesetzt? !
 JR	Z,SINT1
 CP	SIO,#NUL	! gelesenes Zeichen=00? !
 JR	Z,SINT1		! Zeichen 00 ignorieren !
 cp	SIO,#DEL	! gelesenes Zeichen=7F? !
 jr	z,SINT1		! DEL ignorieren !
 
 LD	@SINPTR,SIO	! SIO lesen !
 and	@SINPTR,#NB7	! bit 7 ausblenden !
 OR	STAT0,#B6	! Zeichenbit setzen !
 INC	SINPTR		! Pointer erhoehen !
 inc	SCOUNT		! Counter erhoehen !
 cp	SCOUNT,#SBUFL-9
 jr	lt,SINT2	! Sprung, wenn r2 kleiner !
 tcm	STAT1,#B2	! CRTL_S bereits gesendet? !
 jr	z,SINT3		! wenn ja, Sprung !
 or	STAT1,#B2	! CTRL_S-Flag setzen !
 push	CHAR
 ld	CHAR,#DC3	! CTRL_S !
 call	SIO_OUT
 pop	CHAR
 
SINT3:
 cp	SCOUNT,#SBUFL	! Puffer Ende erreicht? !
 jr	nz,SINT2
 or	STAT0,#B7	! Fehler-Flag setzen !
 
SINT2:
 CP	SINPTR,#SWERT+SBUFL	! Endwert ueberschritten? !
 JR	NZ,SINT1
 LD	SINPTR,#SWERT	! Inpointer ruecksetzen !
 
SINT1:
 IRET
 
 
END IRP30
 
 
RESET_DISPLAY PROCEDURE
 !*******************************************************
 *
 * Ruecksetzen und Initialisierung des 8275 
 * Ruecksetzen der internen Zaehler des CRTC
 *
 *******************************************************!
ENTRY
 
 LD	R8,#HI CRT_COM
 LD	R9,#LO CRT_COM
 LD	R10,#RES_DISPL
 LDE	@RR8,R10	! Ausgabe Command !
 
 ! Parameter ausgeben !
 DEC	R9		! CRT_PAR ADR einstellen !
 LD	R11,#%4		! Zaehler !
 LD	R12,#HI RES_DISPL_PAR
 LD	R13,#LO RES_DISPL_PAR
 
PARLOOP:
 LDC	R10,@RR12	! Parameter holen !
 LDE	@RR8,R10	! Ausgabe Parameter !
 INCW	RR12
 DJNZ	R11,PARLOOP
 
 ! Zaehler ruecksetzen !
 LD	R10,#PRES_COUNT
 inc	r9		! rr8=Command-Addr !
 LDE	@RR8,R10	! Ausgabe Command !
 lde	@rr8,r10	! Wiederholung !
 
 RET
 
 
END RESET_DISPLAY
 
 
RES_DISPL_PAR ARRAY [4 BYTE] := [RES1 RES2 RES3 RES7]
 
 
END INIT
