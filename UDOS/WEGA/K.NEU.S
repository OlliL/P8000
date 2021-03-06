;**************************************************************
;
; P8000 KOPPELSOFTWARE
; GRUNDMODUL
; VERSION: 3.1
;
;**************************************************************
 
 GLOBAL SAVE S0BT
 GLOBAL ERROR UEB16
 GLOBAL	LODNET SNDNET WAIT STATUS WAITACK
 GLOBAL LODDAT SNDDAT SNDACK
 EXTERNAL LOAD SEND PROM
 EXTERNAL ACTFLG
 EXTERNAL BLKINIT
 EXTERNAL BLKRD BLKWT BLKFO
 EXTERNAL RBLOCK WBLOCK
 
	LD B,5
ESTART:
	LD HL,0FFFFH	;ZEITSCHLEIFE FUER FLOPPY
ENTRY:
	DEC HL
	LD A,H
	OR L
	JR NZ,ENTRY
	DJNZ ESTART
 
	DI
	LD HL,ISRPIO
	LD (IP0KB),HL	;ISR-ADR IN TABELLE FUER PIO
 
	LD HL,ITABS0	;RETTEN SIO 0 KANAL B  ISR-ADR.
	LD DE,SAVE
	LD BC,32
	LDIR
 
	LD HL,IP1KA	;RETTEN PIO1 ISR-ADR
	LD DE,SAVEPIO
	LD BC,4
	LDIR
 
	IN A,(P0BD)	;STAND U8000-BIT RETTEN
	PUSH AF
 
	LD HL,BUFFER	;INITIALISIEREN RINGPUFFER
	LD (HL),0
	LD DE,BUFFER+1
	LD BC,BUFEND-BUFFER-1
	LDIR
 
	IN A,(P0AD)	;SCHEINEINGABE DAMIT PORT FREI !
 
	XOR A
	LD (ACTFLG),A
	POP AF
	BIT 7,A
	JR Z,MMT	;U8000 BEREITS AKTIV
 
	XOR A		;RESET-U8000 AUF LOW
	OUT (P0BD),A	;D.H. 16 BIT-TEIL START
	LD B,80H
	DJNZ $
	LD A,80H	;DOPPELRESET FUER
	OUT (P0BD),A	;MP600-ERWEITERUNGSKARTEN
	LD B,80H
	DJNZ $
	XOR A		;NOTWENDIG
	OUT (P0BD),A
 
	IN A,(P0BD)
	BIT 6,A
	JR Z,HWTEST	;PORT NICHT FREI (ALLES I.O.)
			;WENN PORT FREI ,WAS AN DIESER STELLE
			;NICHT SEIN DARF, DANN GEWISSE ZEIT WARTEN
	LD HL,8000H
ZEIT:	DEC HL
	LD A,H
	OR L
	JR NZ,ZEIT

;HARDWARETEST-KOPPELSCHNITTSTELLE

HWTEST:
	LD HL,0FFFFH	;MAX. ZEIT
W8000	IN A,(P0BD)
	BIT 6,A
	JR NZ,TEST
	DEC HL
	LD A,H
	OR L
	JR NZ,W8000
	LD HL,MERR	;U8000 ARBEITET NICHT
	JR TERR1
 
TEST:	LD C,2AH	;STATUS FUER
	LD A,0AAH
	CALL OUTP16	;1.DATENMUSTER
	JR Z,TERR
	LD C,0AH
	CALL WAITACK
	JR NZ,TERR
	IN A,(P0AD)
	CP 0AAH
	JR NZ,TERR
 
	LD A,55H
	LD C,A		;STATUS FUER
	CALL OUTP16	;2.DATENMUSTER
	JR Z,TERR
	LD C,15H
	CALL WAITACK
	JR NZ,TERR
	IN A,(P0AD)
	CP 55H
	JR NZ,TERR
 
	LD HL,KBC0R	;ENDGUELTIGE PROGRAMMIERUNG KOPPELPIO
	LD C,P0BC	;MIT INTERRUPT
	LD B,4
	OTIR
 
	LD A,2		;KENNZEICHEN DAS VON UDOS-PROGRAMM
			;WEGA GESTARTET WURDE !!!
	LD C,0		;STATUS
	CALL OUTP16	;DATENMUSTER
	JR NZ,WREQ	;OK
 
TERR:	LD HL,MTERR	;FEHLER BEI HARDWARETEST
TERR1:	EI
	CALL OUTAS
	JR QUITRST
 
MMT:
	LD A,83H
	OUT (P0BC),A	;PIO-INTERRUPT ERLAUBEN
	EI
	LD HL,MMLDG
	CALL OUTAS
	DI
 
WREQ:	LD HL,SNEUTAB	;SIO ISR-ADR. NEU INITIALISIEREN
	LD DE,ITABS0
	LD BC,32
	LDIR
 
	LD HL,ISP1KA
	LD (IP1KA),HL
	LD HL,ISP1KB	;PIO1-ISR-ADR NEU SETZEN
	LD (IP1KB),HL

	call BLKINIT	;Block-Floppy Init
	EI
 
WAIT:	XOR A
	LD (RSTART),A
	LD A,8FH	;FD-LAUFWERKE READY SETZEN
	LD (DSEL),A
LOOP:	CALL UEB16	;TEST OB INT-WERTE VON SIO'S ANLIEGEN
	LD HL,STATUS
	LD A,(HL)
	OR A
	JR Z,NEXT
	BIT 0,A
	JR NZ,QUIT
	BIT 7,A
	JR NZ,QUITRST
	BIT 1,A
	JP NZ,SEND
	BIT 2,A
	JP NZ,LOAD
	BIT 3,A
	JP NZ,PROM
NEXT:	inc hl
	ld a,(hl)
	or a
	jr z,LOOP
	BIT 0,A
	JP NZ,BLKRD
	BIT 1,A
	JP NZ,BLKWT
	BIT 2,A
	JP NZ,BLKFO
	BIT 3,A
	JP NZ,BLKFO
	BIT 4,A
	JP NZ,BLKFO
	JR LOOP
 
QUITRST:
	LD A,80H	;U8000-RESET
	OUT (P0BD),A
	JR REEX
 
QUIT:
	CALL SNDACK
	LD B,80H	;WARTEZEIT FUER U8000 KINB !!!
	DJNZ $
	LD HL,BASP
	CALL OUTAS
	LD B,10
	DJNZ $
REEX:
	DI
	LD A,11H	;SIO0 KANAL B AUF
	OUT (S0BC),A	;STANDARDINITIALISIERUNG
	LD A,1CH	;DES 8-BIT-PROM
	OUT (S0BC),A
	LD HL,SAVE	;ALTE ISR-WERTE SIO 0 KANAL B
	LD DE,ITABS0	;ZURUECKSPEICHERN
	LD BC,32
	LDIR
 
	LD HL,SAVEPIO
	LD DE,IP1KA	;ALTE WERTE PIO1 ISR-ADR SETZEN
	LD BC,4
	LDIR
 
	LD A,3
	OUT (P0BC),A	;PIO-INT AUS
	EI
	RET		;EXIT ZU UDOS
 
;;;;;;;;;;;;;;;;;;;;;;;;;;
 
UEB16:
	LD IX,RB_S0A
	LD C,83H	;STATUS + STB + INT8000
	CALL UEBSIO
	LD IX,RB_S0B
	LD C,85H
	CALL UEBSIO
	LD IX,RB_S1A
	LD C,87H
	CALL UEBSIO
	LD IX,RB_S1B
	LD C,89H
 
UEBSIO:	DI
	BIT 1,(IX+73H)	;WAR E_INT
	RES 1,(IX+73H)
	JR Z,OUTBUF	;NEIN
	SET 6,C		;E_INT SETZEN
	JR INT1
 
OUTBUF	LD A,(IX+71H)
	CP (IX+72H)	;POINTER GLEICH
	JR Z,S_INT
	CALL INCPOI	;NEIN
	LD (IX+71H),A	;LESEPOINTER+1 UND SPEICHERN
	LD A,(HL)	;ZEICHEN HOLEN
	JR INT2
 
S_INT:
	BIT 2,(IX+73H)	;WAR S_INT
	RES 2,(IX+73H)
	JR Z,T_INT
	LD A,C
	OR 60H		;S_INT SETZEN
	LD C,A
	LD A,(IX+75H)	;RR1 UEBERGEBEN
	JR INT2
 
T_INT:
	BIT 0,(IX+73H)	;WAR T_INT
	RES 0,(IX+73H)
	JR Z,P1KA	;NEIN
	SET 5,C		;T_INT SETZEN
INT1:	LD A,(IX+74H)	;RR0 UEBERGEBEN
INT2:	DI
	LD B,A		;DATEN RETTEN
	OUT (P16A),A	;DATENBYTE
	XOR A
	OUT (P16B),A
	LD A,C
INT3:	OUT (P16B),A
 
WAT16:	EI
	NOP
	DI
	IN A,(P0BD)
	BIT 6,A		;WARTEN BIS DATEN PER INT8000
	JR Z,WAT16	;ABGEHOLT WURDEN
	LD A,(FLAG)	;WURDEN AUSGABEDATEN DURCH
	OR A		;KINB UEBERSCHRIEBEN ?
	JR NZ,NEUAUS	;JA: DATEN NEU ANLEGEN OHNE INT8000
EXIT:	EI
	RET
 
P1KA:	LD HL,STAT2	;WAR PIO1 KANAL A INTERRUPT ?
	BIT 0,(HL)
	JR Z,P1KB	;NEIN
	RES 0,(HL)	;JA
	LD A,0
	LD C,8BH	;STATUS 0AH or 81H
	JR INT2
P1KB:	BIT 1,(HL)	;KANAL B INTERRUPT ?
	JR Z,EXIT	;NEIN
	RES 1,(HL)
	LD A,0
	LD C,0ABH	;STATUS 2AH or 81H
	JR INT2
 
NEUAUS:	XOR A
	LD (FLAG),A	;FLAG RUECKSETZEN
	RES 0,C		;INT8000 AUSSCHALTEN
	LD A,B
	JR INT2		;DATEN UND STATUS NEU ANLEGEN
 
;;;;;;;;;;;;;;;;;;;;;;;;;
 
LODNET:
	CALL UEB16
	DI
	LD IX,RECBUF
	LD A,(IX+71H)
	CP (IX+72H)	;POINTER GLEICH --> KEIN ZEICHEN
	JR Z,ZERE	;ZERO-RETURN
	CALL INCPOI	;LESEPOINTER + 1
	LD (IX+71H),A	;UND SPEICHERN
	INC A		;NZ SETZEN
	LD A,(HL)	;ZEICHEN HOLEN
ZERE	EI
	RET
 
;;;;;;;;;;;;;;;;;;;;;;;;;
SNDNET:	PUSH AF
	CALL UEB16
	POP AF
	LD C,91H	;STATUS + STB + INT8000
	JR INT2
 
;;;;;;;;;;;;;;;;;;;;;;;;;
SNDACK:	LD A,ACK
	LD C,91H
	JR INT2
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SNDDAT:	LD C,90H	;STATUS OHNE INT.
	JR INT2
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LODDAT:
	IN A,(P0BD)
	BIT 5,A		;DATEN DA?
	RET Z		;NEIN ZERO RETURN
	IN A,(P0AD)	;JA --> NZ-RETURN
	RET		;IN REG. A DATENBYTE
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
OUTP16:			;AUSGABE AN 16-BIT PORT
			;ZEICHEN IN A
			;STATUS IN REG. C
			;OHNE INTERRUPT
			
	LD HL,0FFFFH	;MAX. ZEIT
	LD B,A
WTKP	IN A,(P0BD)	;PORT FREI?
	BIT 6,A
	JR Z,DETI	;NEIN
	LD A,B		;JA
	OUT (P16A),A	;DATENBYTE-AUSGABE
	XOR A
	OUT (P16B),A
	LD A,C		;STATUS UND
	OR 80H		;STB-FLANKE OHNE INTERRUPT
	OUT (P16B),A	;BEACHTE IM DI 
	RET		;OKAY --> NZ-RETURN
 
DETI	DEC HL
	LD A,H
	OR L
	JR NZ,WTKP
	RET		;FEHLER --> Z-RETURN
 
;;;;;;;
 
WAITACK:		;WARTEN AUF ACK (FUER TEST - HARDWARE)
			;ACK-CODE IN REG. C
	LD HL,0FFFFH	;MAX. WARTEZEIT
WTL	IN A,(P0BD)
	BIT 5,A
	JR Z,NXTIT	;KEIN ZEICHEN
	AND 1FH		;STATUS FILTERN
	CP C		;ACK VOM U8000
	RET Z		;OKAY --> ZERO-RET
NZRET	OR 0FFH
	RET		;SONST --> NZ-RET
NXTIT	DEC HL
	LD A,H
	OR L
	JR NZ,WTL
	JR NZRET
 
;;;;;;;;;;;;;;;;;;;;;;;;;
 
ISRPIO:	PUSH AF
	PUSH BC
	PUSH DE
	PUSH HL
	PUSH IX
	IN A,(P0BD)	;STATUS
	BIT 5,A		;DATEN DA
	JR Z,ISREND	;NEIN --> FEHLER
 
	AND 16H		;STATUS HERAUSFILTERN
	BIT 4,A
	JR NZ,REQUEST	;REQUESTANFORDERUNG
 
RWQ	LD B,A
	IN A,(P0AD)	;PERIPHERIE-ADRESSE IN
	LD C,A		;REG. C LADEN
	LD A,B
	CP 02H		;READ-STATUS ?
	JR Z,READ	;JA
	CP 04H		;WRITE-STATUS ?
	JR NZ,ISREND	;NEIN --> INVALID STATUS
 
WRITE:	IN A,(P0BD)
	BIT 5,A		;DATEN DA ?
	JR Z,WRITE	;NEIN --> WARTEN
	LD B,A
	IN A,(P0AD)	;DATENBYTE
	OUT (C),A	;AUSGABE AN PERIPHERIE-ADR.
	LD A,B
	AND 16H
	JR Z,ISREND	;ENDESTATUS
	CP 04H		;WRITE-STATUS
	JR Z,WRITE	;NAECHSTE DATENBYTE IST WIEDER FUER WRITE
			;BEI JEDEM ANDEREN STATUS WIRD
WADA:	IN A,(P0BD)	;AUF DATEN GEWARTET
	BIT 5,A
	JR Z,WADA
	AND 16H		;STATUS FILTERN
	JR RWQ		;READ ODER WRITE-REQUEST
 
READ:	IN A,(C)	;EINGABE VON PERIPHERIE-ADR.
	LD B,A
	IN A,(P0BD)	;PORT-AUSGABE FREI ?
	BIT 6,A
	JR NZ,RQEND	;JA KEIN FLAG SETZEN
	LD A,0FFH
	LD (FLAG),A	;FLAG SETZEN, D.H. WERT WURDE UEBERSCHRIEBEN
RQEND:	LD A,B
	OUT (P16A),A	;DATENUEBERGABE
	XOR A
	OUT (P16B),A	;MIT STATUS 0
	OR 80H
	OUT (P16B),A	;STB-FLANKE OHNE INTERRUPT
 
WABH	IN A,(P0BD)
	BIT 6,A		;WARTEN BIS WERT ABGEHOLT WURDE
	JR Z,WABH
 
ISREND:	POP IX
	POP HL
	POP DE
	POP BC
	POP AF
	EI
	RETI
 
;-------
 
REQUEST:
	LD HL,STATUS
	CP 16H		;QUIT-STATUS
	JR Z,RQT
	CP 10H		;REMOTE-STATUS
	JR Z,RRT
	IN A,(P0AD)
	JR ISREND
 
;QUIT-REQUEST
 
RQT:	IN A,(P0AD)
	CP 'R'
	JR NZ,RQT1
	SET 7,(HL)
RQT1:	CP 'Q'
	JR NZ,ISREND
	SET 0,(HL)
	JR ISREND
 
;REMOTE-STATUS
 
RRT:	LD A,(RSTART)
	OR A
	JR NZ,RRTD
	LD A,0FFH
	LD (RSTART),A
	IN A,(P0AD)
	CP 'S'
	JR NZ,LST
	SET 1,(HL)	;SEND
	JR REMST
LST	CP 'L'
	JR NZ,PST
	SET 2,(HL)	;LOAD
	JR REMST
PST	CP 'P'
	JR NZ,BRD
	SET 3,(HL)	;PROM-PROGRAMMER
	JR REMST
BRD	INC HL		;BLOCK-REQUEST
	CP 'R'
	JR NZ,BWT
	SET 0,(HL)	;READ
	CALL RBLOCK
	JR REMST
BWT	CP 'W'
	JR NZ,BFO
	SET 1,(HL)	;WRITE
	CALL WBLOCK
	JR REMST
BFO:	CP 'F'
	JR NZ,BVY
	SET 2,(HL)	;FORMAT
	JR REMST
BVY:	CP 'V'
	JR NZ,BCP
	SET 3,(HL)	;VERIFY
	JR REMST
BCP:	CP 'C'
	JR NZ,NOREMST
	SET 4,(HL)	;COPY DISK
	JR REMST
NOREMST:
	XOR A
	LD (RSTART),A
REMST:	JP ISREND
 
;REMOTE-DATEN
 
RRTD:	LD IX,RECBUF
	LD A,(IX+72H)
	CALL INCPOI	;NEUEN POINTERWERT BERECHNEN
	LD (IX+72H),A	;UND SPEICHERN
	IN A,(P0AD)	
	LD (HL),A	;HL ZEIGT AUF PLATZ ZUM EINSCHREIBEN
	JP ISREND
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
; PIO1 INTERRUPTS (EPROM-PROGR.)
;
ISP1KA:
	PUSH HL		;KANAL A INT
	LD HL,STAT2
	SET 0,(HL)
	POP HL
	EI
	RETI
 
ISP1KB:
	PUSH HL		;KANAL B INT
	LD HL,STAT2
	SET 1,(HL)
	POP HL
	EI
	RETI
 
;;;;;;;;;;;;;;;;;;;;;;;;;
 
; SIO RECEIVE - INTERRUPTS
; ABLEGEN DER WERTE IN ENTSPR. RINGPUFFER
 
 
S0AR:	PUSH BC		;SIO 0 KANAL A RECEIVE-INT.
	PUSH IX
	LD C,S0AC	;DATEN-ADR
	LD IX,RB_S0A	;RINGPUFFER
	JR RINT
 
S0BR:	PUSH BC		;SIO 0 KANAL B RECEIVE-INT.
	PUSH IX
	LD C,S0BC	;DATEN-ADR.
	LD IX,RB_S0B	;RINGPUFFER
	JR RINT
 
S1AR:	PUSH BC		;SIO 1 KANAL A RECEIVE-INT.
	PUSH IX
	LD C,S1AC	;DATEN-ADR.
	LD IX,RB_S1A	;RINGPUFFER
	JR RINT
 
S1BR:	PUSH BC		;SIO 1 KANAL B RECEIVE-INT.
	PUSH IX
	LD C,S1BC	;DATEN-ADR.
	LD IX,RB_S1B	;RINGPUFFER
 
RINT:	PUSH AF		;GEMEINSAMER TEIL FUER ALLE
	PUSH HL		;RECEIVE-INTERRUPTS
	PUSH DE
RINT1:	DEC C
	IN B,(C)	;WERT EINLESEN
	LD A,(IX+72H)	;SCHREIBPOINTER
	CALL INCPOI	;NEUEN WERT BERECHNEN
	LD (IX+72H),A	;UND SPEICHERN
	LD (HL),B	;DATENBYTE IN RINGPUFFER
	INC C		;SIO CONTROL-ADR.
	IN A,(C)	;EINGABE RR0
	BIT 0,A		;NOCH ZEICHEN DA
	JR NZ,RINT1	;JA
	POP DE
	POP HL
	POP AF
	POP IX
	POP BC
	EI
	RETI
 
INCPOI:
	LD D,0
	LD E,A
	PUSH IX 
	POP HL		;HL AUF WERT VON IX
	INC A
	CP 70H
	JR NZ,INCP
	XOR A
INCP	ADD HL,DE
	RET
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
;RESTLICHE SIO-INTERRUPTS: T_INT, E_INT, S_INT FUER ALLE KANAELE:
;
S0BT:	PUSH BC
	PUSH IX
	LD C,S0BC	;SIO CONTROL-ADR.
	LD IX,RB_S0B
	JR TINT
 
S0AT:	PUSH BC
	PUSH IX
	LD C,S0AC
	LD IX,RB_S0A
	JR TINT
 
S1BT:	PUSH BC
	PUSH IX
	LD C,S1BC
	LD IX,RB_S1B
	JR TINT
 
S1AT:	PUSH BC
	PUSH IX
	LD C,S1AC
	LD IX,RB_S1A
TINT:	PUSH AF
	LD A,28H	;RESET T_INT
	OUT (C),A
	IN A,(C)	;EINGABE RR0
	LD (IX+74H),A	;SPEICHERN
	SET 0,(IX+73H)	;SET T_INT BIT
	JR SIOINT
 
S0BE:	PUSH BC
	PUSH IX
	LD C,S0BC
	LD IX,RB_S0B
	JR EINT
 
S0AE:	PUSH BC
	PUSH IX
	LD C,S0AC
	LD IX,RB_S0A
	JR EINT
 
S1BE:	PUSH BC
	PUSH IX
	LD C,S1BC
	LD IX,RB_S1B
	JR EINT
 
S1AE:	PUSH BC
	PUSH IX
	LD C,S1AC
	LD IX,RB_S1A
EINT:	PUSH AF
	IN A,(C)	;EINGABE RR0
	LD (IX+74H),A	;SPEICHERN
	LD A,10H
	OUT (C),A	;RESET E_INT
	SET 1,(IX+73H)	;SET E_INT BIT
	JR SIOINT
 
S0BS:	PUSH BC
	PUSH IX
	LD C,S0BC
	LD IX,RB_S0B
	JR SINT
 
S0AS:	PUSH BC
	PUSH IX
	LD C,S0AC
	LD IX,RB_S0A
	JR SINT
 
S1BS:	PUSH BC
	PUSH IX
	LD C,S1BC
	LD IX,RB_S1B
	JR SINT
 
S1AS:	PUSH BC
	PUSH IX
	LD C,S1AC
	LD IX,RB_S1A
SINT:	PUSH AF
	LD A,1
	OUT (C),A
	IN A,(C)	;EINGABE RR1
	LD (IX+75H),A	;SPEICHERN
	LD A,30H
	OUT (C),A	;RESET S_INT
	SET 2,(IX+73H)	;SET S_INT BIT
 
SIOINT:	POP AF
	POP IX
	POP BC
	EI
	RETI
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;TEXTE:
 
MERR	DEFB 23
	DEFM 'U8000-Unit does not work'
	DEFB 0DH
 
MTERR	DEFB 29		;LAENGE
	DEFM 'Hardware Error in Connection'
	DEFB 0DH
 
BASP	DEFB 1
	DEFB 08H
 
MMLDG	DEFB 22
	DEFM 'U8000-Softwaremonitor'
	DEFB 0DH
 
KBC0R	DEFB 0CFH
	DEFB 7FH
	DEFB 0B7H
	DEFB 0FEH
 
;INT-TABELLEN-EINTRAEGE
 
SNEUTAB DEFW S0BT
	DEFW S0BE
	DEFW S0BR
	DEFW S0BS
 
	DEFW S0AT
	DEFW S0AE
	DEFW S0AR
	DEFW S0AS
 
	DEFW S1BT
	DEFW S1BE
	DEFW S1BR
	DEFW S1BS
 
	DEFW S1AT
	DEFW S1AE
	DEFW S1AR
	DEFW S1AS
 
;BUFFER UND VARIABLEN
 
SAVE	DEFS 32
SAVEPIO DEFS 4
 
;RINGPUFFER FUER ALLE SIO-INTERRUPTS
 
BUFFER:
RB_S0A	DEFS 70H
	DEFS 3		;BUFLEN / BUFPOI / RECPOI
	DEFS 1		;FLAG FUER TES_INT'S
	DEFS 2		;RR0 / RR1 SPEICHER
RB_S0B	DEFS 76H
RB_S1A	DEFS 76H
RB_S1B	DEFS 76H
 
FLAG	DEFS 1		;FUER KINB BEIM UEBERSCHREIBEN
 
;RINGPUFFER FUER UD
 
RECBUF	DEFS 073H
 
STATUS  DEFS 2
STAT2	DEFS 1
ERROR	DEFS 1
RSTART	DEFS 1
BUFEND:	DEFS 0
 
 
;;;;;;;;;;;;;;;;;;;;
;KONSTANTEN:
 
ACK:	EQU 06H
 
P16A	EQU 10H		;AUSGABEPORT 1 (DATEN) AN U8000
P16B	EQU 14H		;AUSGABEPORT 2 (RDY/STATUS/INT8000)
 
PIO0	EQU 0CH
P0AD	EQU PIO0
P0AC	EQU PIO0+1
P0BD 	EQU PIO0+2
P0BC	EQU PIO0+3
 
S0AD	EQU 24H
S0AC	EQU S0AD+1
S0BD	EQU S0AD+2
S0BC	EQU S0AD+3
S1AD	EQU 28H
S1AC	EQU S1AD+1
S1BD	EQU S1AD+2
S1BC	EQU S1AD+3
 
;AUS PROM SATZ:
 
DSEL	EQU 0EDDH
OUTAS	EQU 0BF1H	;AUSGABE ZEICHENKETTE
 
IP0KB:	EQU 0F12H
IP1KA:	EQU 0F14H
IP1KB:	EQU 0F16H
ITABS0	EQU 0F20H
IS1B1	EQU 0F20H
