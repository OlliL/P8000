*P 84
;******************************************************************************
;
;	Floppydisk Treiber fuer P8000
;
;******************************************************************************

DMA	EQU	1
UDOS	EQU	1
U880SM	EQU	1


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ENTRY FLOPPY TREIBER

DISK
FLOPPY	LD	HL,(DSKVSL)	;ist Parametervektor eingetragen ?
	LD	A,H
	OR	L
	JR	NZ,FLOPPY	;Treiber besetzt --> warten !
	LD	(DSKVSL),IY	;Treiber ist frei --> Parametervektor
				;eintragen und damit als "besetzt" setzen
	CALL	FLP		;CALL-Aufruf des Treibers
WAIT	BIT	0,(IY+1)	;zeitgeteilter Verkehr ?
WAIT1	RET	NZ
	BIT	7,(IY+10)	;Operation fertig ?
	JR	WAIT1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;RETURN FUER WARTEN AUF INTERRUPT

SFLPI	POP	HL		;IN HL ADR WO PROGRAMM NACH
	LD	(INTPNT),HL	;INTERRUPT WEITER LAEUFT
	EI			;falls Programm als ISR des PIO2 laeuft
				;kann neuer INT vom PIO2 erst nach RETI
	RET			;erfolgen

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Auswertung UDOS-Requestcode
FLP
 COND 1-U880SM
; Adressen der Interruptserviceroutinen
	LD	HL,ISRPB	; Initialisierung der Interruptserviceroutinen
	LD	(INTABP),HL	; wenn FLOPPY-Teil als externer Treiber arbeitet.
	LD	HL,ISRCT1
	LD	(INTAB+2),HL
	LD	HL,ISRDMA
	LD	(INTABD),HL
 ENDC
;
	LD	A,(IY+1)	;Request Code
	AND	0FEH		;Loeschen der Abarbeitungsart
	CP	0AH		;Read Binary Request ?
	JR	Z,TYPTEST
	CP	0EH		;Write Binary Request ?
	JR	Z,TYPTEST
	CP	50H		;Format
	JR	Z,TYPTEST
	LD	A,0C1H		;'Invalid  Request'
ERRRTN	CALL	OPFIN
	LD	H,(IY+9)
	LD	L,(IY+8)
	LD	A,H
	OR	L
	JR	Z,FEHL1
	JP	(HL)		;Jump to Error Return Address

FEHL1	BIT	0,(IY+1)
	RET	Z		;Ruecksprung, wenn einfacher Request
	LD	H,(IY+7)
	LD	L,(IY+6)
	JP	(HL)		;Jump to Completion Return Address

ERRC2	LD	A,0C2H
	JR	ERRRTN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Floppy-Typ testen und FDC einstellen

TYPTEST	LD	C,(IY+11)	;Drive/Sector
	LD	A,C
	RLA
	JR	C,ERRC2
	LD	HL,FDCONF
	BIT	6,C		;0 = Drive 0,1
	JR	Z,TYP1
	INC	HL
TYP1	LD	A,(HL)
	BIT	5,C		;0 = Drive 0,2
	JR	Z,TYP2
	RRCA			;Verschiebung fuer Drive 1 und 3
	RRCA
	RRCA
	RRCA
TYP2	AND	0FH
	JR	Z,ERRC2		;Ist Laufwerktyp in FDCONF eingetragen?
	CP	8		;Ja, testen.
	JR	NC,ERRC2
	LD	HL,FDTYP	;Nein, weiter testen.
	CP	(HL)		;altes = neues Format ?
	JR	Z,TYPOK

	LD	(HL),A		;neuen Typ einstellen
	LD	HL,TYPTAB	;Treiber auf Typ einstellen
	LD	DE,11
	LD	B,A
	JR	TYP4
TYP3	ADD	HL,DE
TYP4	DJNZ	TYP3		;solange <DE> addieren, bis <B>:=0, dann ist die
				;richtige Tabelle eingestellt
	LD	C,2
	LD	DE,FDCTAB+1
	LDIR
	INC	DE
	INC	DE		;veraenderliche Werte ueberspringen
	LD	C,8
	LDIR

	DI			;besser DI
	IN	A,(PIO2AD)	;Takt einstellen
	SET	4,A		;5" = 4MHz Takt einstellen
	BIT	1,(HL)
	JR	Z,TYP5
	RES	4,A		;8" = 8MHz Takt einstellen
TYP5	OUT	(PIO2AD),A
	LDI			;CMD Byte eintragen
 COND 1-DMA
	LD	HL,FDCTAB+2
	SET	0,(HL)		;NON-DMA setzen
 ENDC
	LD	BC,0303H	;Specify; Werte aus Tabelle eintragen
	CALL	FDCKMD

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Drive, Track und Sektor uebernehmen

TYPOK	LD	HL,0202H	;Setzen der Fehlerzaehler
	LD	(WSCTR),HL	;WSCTR und WTCTR
	LD	A,10
	LD	(CRCCTR),A	;fuer CRC (Data Transfer) Errors (C6)
	XOR	A
	LD	(IY+10),A	;Loeschen Completion Code
	LD	A,(IY+12)	;Ueberpruefen Spurnummer
	LD	(SPURNR),A	;Spurnummer in Speicherzelle eintragen
	LD	HL,TRK
	CP	(HL)
	LD	A,0C5H
	JR	NC,ERR
	LD	A,(IY+11)	;Uebernehmen Sektor/Laufwerk
	LD	C,A
	AND	60H		;Drive Maske
	RLCA
	RLCA
	RLCA
	LD	B,A
	LD	A,C
	LD	C,0		;Head 0
	AND	1FH		;Sektor Maske
	LD	HL,FDCTAB+6	;EOT
	CP	(HL)		;Sektor >= EOT --> Head 1
	JR	C,HEADOK
	SUB	(HL)		;Sektornummer einmal subtrahieren, wegen Doppelkopflaufwerk
	CP	(HL)		;Sektornummer zu gross ?
	JR	NC,HERRC4
	LD	HL,CMD
	BIT	7,(HL)		;Head (Side) 1 zugelassen ?
	JR	NZ,HEAD1
HERRC4	LD	A,0C4H
ERR	JP	ERRRTN

HEAD1	SET	2,B		;Head 1 setzen in Drivenummer
	INC	C
HEADOK	INC	A		;1. PHYSISCHER SEKTOR = 1
	LD	(FDCTAB+4),A	;Sektornummer eintragen
	LD	A,C
	LD	(FDCTAB+3),A	;Head eintragen
	LD	A,B
	LD	(FDCTAB+1),A	;Drive und Head select
	AND	03H		;Laufwerk-Nummer in <A>

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DRIVE SELEKT UND BEREITSCHAFT PRUEFEN

	CALL	READY		;in A Drive
	JP	Z,ERRC2		;Drive not ready"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Laufwerksanwahl ist ok. --> Pruefen der Kopfposition

	LD	HL,CURT0	;Tabelle der Kopfpositionen (nach RESET mit
				;0FFH beschrieben)
				;in DE noch Laufwerk von READY
	ADD	HL,DE		;zeigt auf die Kopfposition des angef. LW
	LD	A,(HL)
	INC	A		;war Kopf schon mal positioniert ?
	LD	(HL),D		;beim 8272 muss sich nicht die Kopfposition
				;gemerkt werden, nur Unterscheidung ob
				;schon mal positioniert
	JR	NZ,HDMOV	;JA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Kopf des angewaehlten LW auf Spur 00 fahren

TR00	LD	BC,0207H	;Recalibrate Command
	CALL	FDCKMD
	CALL	SFLPI

;*************** Warten auf Interrupt *******************

	LD	BC,0207H	;2.Recalibrate falls mehr als 77 Spuren
	CALL	FDCKMD
	CALL	SFLPI

;*************** Warten auf Interrupt *******************

				;auf Fehlerauswertung wird hier verzichtet
				;sie werden spaeter auch noch gemerkt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;In HDMOV wird der Kopf des angewaehlten LW auf die richtige
;Spur gefahren

HDMOV	LD	A,(SPURNR)	;Track Nummer
	LD	HL,CMD
	BIT	0,(HL)		;Doppelstep ?
	JR	Z,HDMOV1
	ADD	A,A		;*2
HDMOV1	LD	(FDCTAB+2),A	;Spur in Kommandotabelle

	LD	HL,(PR1)	;Praekompensation ein/aus
	DI
	LD	C,PIO2BD
	IN	B,(C)
	RES	5,B
	RES	6,B
	CP	L		;1. Praekompensation
	JR	C,CHKP
	SET	5,B
	CP	H		;2. Praekompensation
	JR	C,CHKP
	SET	6,B
CHKP	OUT	(C),B

	LD	BC,030FH	;Kommandotabelle fuer "SEEK"
	CALL	FDCKMD
	CALL	SFLPI

;*************** Warten auf Interrupt *******************

				;auf Fehlerauswertung wird hier verzichtet
				;sie werden spaeter auch noch gemerkt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SEKTOR(EN) LESEN SCHREIBEN

 COND 1-DMA
DTRANS	LD	A,(SPURNR)	;Spur
	LD	(FDCTAB+2),A
	LD	H,(IY+5)
	LD	L,(IY+4)	;Datenlaenge
	LD	A,H
	OR	L
	JP	Z,OPCOMP
	PUSH	DE
	EX	DE,HL
	LD	L,(IY+2)	;Adresse
	LD	H,(IY+3)
	LD	A,3		;Interrupt PIO2 aus, Lesen/Schreiben erfolgt
	OUT	(PIO2BC),A	;ohne Interrupt, Abfrage von INT
	LD	B,9
	LD	A,(CMD)
	AND	0F0H
	OR	5		;Write
	LD	C,A
	BIT	2,(IY+1)	;Lesen oder Schreiben ?
	JR	NZ,WR
	BIT	4,(IY+1)	;Format ?
	JR	NZ,FORMAT

;DATEN EINLESEN

	INC	C		;Read
	CALL	FDCKMD		;Ausfuehrung
LREAD	IN	A,(MSR)
	RLCA
	JR	NC,LREAD	;warten auf Zeichen
	RLCA
	RLCA
	JR	NC,RWEND	;1= NON DMA MODE --> 0= Ende Uebertragung
	INI			;REG C = DATREG in FDCKMD gesetzt
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,LREAD
	JR	RWEND

;DATEN SCHREIBEN

FORMAT	PUSH	HL		;Format
	LD	HL,(FDCTAB+5)
	LD	(FDCTAB+2),HL
	LD	HL,(GPL2)
	LD	H,0E5H
	LD	(FDCTAB+4),HL
	LD	A,H
	LD	(FDTYP),A	;NACH FORMAT IMMER TYP NEU EINSTELLEN
	POP	HL
	LD	B,6
	SET	3,C		;Format
	RES	7,C		;MT nicht bei Format

WR	CALL	FDCKMD
LWRITE	IN	A,(MSR)
	RLCA
	JR	NC,LWRITE
	RLCA
	RLCA
	JR	NC,RWEND	;FDC bereits fertig
	OUTI
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,LWRITE

;ENDE LESEN / SCHREIBEN

RWEND	IN	A,(PIO2AD)
	SET	6,A		;TC auf 1;Abbruch R/W
	OUT	(PIO2AD),A
	RES	6,A		;TC wieder auf 0
	OUT	(PIO2AD),A
	POP	DE
	LD	A,83H		;Interrupt PIO2 erlauben
	OUT	(PIO2BC),A
	CALL	SFLPI
 ENDC

 COND DMA
DTRANS	LD	A,(SPURNR)	;Spur eintragen
	LD	(FDCTAB+2),A
	LD	H,(IY+3)
	LD	L,(IY+2)	;Datenadresse
	LD	(DMALSW+1),HL	;in DMA-Tabelle
	LD	H,(IY+5)
	LD	L,(IY+4)	; Datenlaenge
	LD	A,H
	OR	L
	JP	Z,OPCOMP
	DEC	HL
	LD	(DMALSW+3),HL	;Datenlaenge-1 in DMA-Tabelle
	LD	B,9		;Kommandokode fuer Schreiben
	LD	A,(CMD)
	AND	0F0H
	OR	05H
	LD	C,A
	BIT	2,(IY+1)	;Abfrage Schreiben oder Lesen
	JR	NZ,AUSDMA
	BIT	4,(IY+1)	;FORMAT ?
	JR	Z,DREAD
	LD	HL,(FDCTAB+5)
	LD	(FDCTAB+2),HL
	LD	HL,(GPL2)
	LD	H,0E5H
	LD	(FDCTAB+4),HL
	LD	A,H
	LD	(FDTYP),A	;NACH FORMAT IMMER TYP NEU EINSTELLEN
	LD	B,6
	SET	3,C		;Format
	RES	7,C
	JR	AUSDMA

DREAD	INC	C		;Read
	LD	A,1		;Festlegung der Datenrichtung DMA <--- FDC
	LD	(DMALSW+6),A
	LD	A,7DH
	LD	(DMALSW),A

AUSDMA	PUSH	BC
	LD	HL,DMALSW
	LD	BC,0A00H+DMAADR
	OTIR
	LD	A,5		;Festlegung der Datenrichtung DMA ---> FDC
	LD	(DMALSW+6),A	;Wert fuer Schreiben wieder eintragen
	LD	A,79H
	LD	(DMALSW),A	;Wert fuer Schreiben wieder eintragen

	POP	BC
	CALL	FDCKMD
	CALL	SFLPI

 ENDC

;********** Warten auf Interrupt ************

	CALL	RDRES		;Result einlesen
				;in A (RESUL)
	AND	0C0H		;IC & SE
	JR	Z,OPCOMP	;kein Fehler

				;Fehler bei der Datenuebertragung
	LD	A,(RESUL+1)	;Statusregister 1
	BIT	1,A		;Schreibschutz ?
	JR	NZ,WRPROT
	AND	30H		;CRC-Fehler im ID-Feld oder Overrun ?
	JR	NZ,BADCRC
	LD	A,(RESUL+2)
	BIT	5,A		;CRC Fehler im Datenfeld ?
	JR	NZ,BADCRC
	AND	12H
	JR	NZ,WRNGTR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DISK FEHLER

;Sector Address Errors (C4)

WRNGSEC	LD	HL,(SECERR)
	INC	HL
	LD	(SECERR),HL
	LD	HL,WSCTR
	LD	A,0C4H		;Sektor Address Error
	JR	WRNG1

;CRC-Fehler (C6)

BADCRC	LD	HL,(CRCERR)
	INC	HL
	LD	(CRCERR),HL
	LD	HL,CRCCTR
	LD	A,0C6H		;CRC-Fehler
WRNG1	DEC	(HL)
	JP	NZ,DTRANS	;neuer Versuch
	JR	OPFIN

;Seek-Errors (C5)

WRNGTR	LD	HL,(TRKERR)
	INC	HL
	LD	(TRKERR),HL
	LD	HL,WTCTR
	DEC	(HL)
	JP	NZ,TR00		;neuer Versuch
	LD	A,0C5H		;Seek Error
	JR	OPFIN

;Schreibschutz (C3)

WRPROT	LD	A,0C3H
	JR	OPFIN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Abschluss-Routine

OPCOMP	LD	A,80H		;Completion Code = 80H (Transfer ok.)
OPFIN	LD	(IY+10),A
	AND	40H		;Error wenn bit 6 = 1
	JR	Z,OPFIN1
	XOR	A
	LD	(IY+4),A	;0 Bytes uebertragen
	LD	(IY+5),A
OPFIN1	LD	H,A		;A = 0
	LD	L,A
	LD	(DSKVSL),HL	;Treiber frei
	LD	(DMASK),A	;LW Maske loeschen
	RET			;ab hier beginnt die Zeit fuer Motor aus

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;KOMMANDOAUSFUEHRUNG DES FDC
;Vor dem Einschreiben von Bytes in den FDC muss im Main Status Register
;RQM=1 und DIO=0 sein.
;Vor dem Auslesen von Bytes aus dem FDC muss im Main Status Register
;RQM=1 und DIO=1 sein.
;Programmierung des FDC

FDCKMD	DI
	PUSH	HL
	LD	HL,FDCTAB
	LD	(HL),C		;Kommando eintragen
FDCKM1	LD	C,DATREG
RQMDIO	IN	A,(MSR)		;Lesen des Main Status Register
	AND	0C0H		;nur RQM & DIO
	CP	080H		;RQM=1 & DIO=0 ?
	JR	NZ,RQMDIO	;Warten
	OUTI			;Ausgabe
	PUSH	HL		;mindestens 12 Mikrosek. zwischen Zugriff
	POP	HL		;Daten- und Hauptstatusregister einhalten
	PUSH	HL
	POP	HL
	JR	NZ,FDCKM1
	POP	HL
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;RESULT VOM FDC LESEN

RDRES	LD	HL,RESUL	;Resultatstabelle
	LD	C,DATREG
RDRES1	IN	A,(MSR)		;Lesen Main Status Register
	CPL
	AND	0C0H
	JR	NZ,RDRES1	;warten auf READ-Bereitschaft des FDC
	INI			;FDC --> CPU
	LD	B,6		;Zeit von 12 Mikrosek. einhalten
	DJNZ	$
	IN	A,(MSR)
	BIT	4,A		;FDC noch in einer Kommandodurchfuehrung
	JR	NZ,RDRES1	;ja (noch Resultatsbytes zu lesen)
	LD	A,(RESUL)	;Statusregister 0
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;INTERRUPTSERVICE-ROUTINE FUER PIO2/B-INTERRUPTS
;zeigt den Beginn der "Result Phase" an

ISRPB	PUSH	HL
	PUSH	AF
	PUSH	BC
	PUSH	IY
	LD	IY,(DSKVSL)
	IN	A,(MSR)
	BIT	4,A		;FDC besetzt ?
	JR	NZ,RUECK	;Ja -> Transfer Kommando

				;ermitteln, ob Recalibrate oder Seek Int
	LD	BC,0108H	;FDC-Kommando "Sense Interrupt Status"
	CALL	FDCKMD		;Ausfuehrung des Kommandos
	CALL	RDRES		;Lesen des Resultats
				;in A (RESUL) = Statusregister 0
	AND	0E0H		;IC & SE (Interrupt und Seek End)
	CP	0C0H		;Ready Line Changed ?
	JR	Z,SOFORT	;Ja -> keine Reaktion
RUECK	LD	HL,RTNPTR
	PUSH	HL		;Eintragen der Rueckkehradresse
	LD	HL,(INTPNT)
	JP	(HL)		;weiter bei eingetragener Adresse

RTNPTR	BIT	7,(IY+10)	;Operation fertig
	JR	NZ,CCOK		;ja
SOFORT	POP	IY
	JR	INTEND

CCOK	BIT	6,(IY+10)
	JR	Z,KEINF		;Durchfuehrung fehlerfrei
	LD	H,(IY+9)
	LD	L,(IY+8)	;Error Return Address
	LD	A,H
	OR	L		;=0 ?
	JR	NZ,ERRRET
KEINF	BIT	0,(IY+1)	;zeitgeteilt ?
	JR	Z,SOFORT	;nein
	LD	H,(IY+7)
	LD	L,(IY+6)	;Completion Return Address
ERRRET	POP	IY
	POP	BC
	POP	AF
	EX	(SP),HL
	JR	IRET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ISR CTC fuer Motor aus

ISRCT1	PUSH	HL
	PUSH	AF
	PUSH	BC
	LD	C,01H		;Maske fuer LW 0
	LD	HL,TTAB		;Motorzeitentabelle
CLOOP	LD	A,(DMASK)
	AND	C
	JR	NZ,CLOOPE	;fuer aktuelles LW Motorzeit nicht decrement
	IN	A,(PIO2AD)	;PIO-Port fuer Motorsteuerung
	LD	B,A
	AND	C		;Motor LW n an ?
	JR	NZ,CLOOPE	;nein
	DEC	(HL)		;Zeit herunterzaehlen
	JR	NZ,CLOOPE	;noch nicht Null
	LD	A,B
	OR	C		;MOTORBIT = 1 SETZEN
	LD	B,A
	CPL
	AND	0FH
	JR	NZ,CLOOP1
	LD	A,03H		;kein Motor an, CTC Reset
	OUT	(CTC0+1),A
	RES	5,B		;Kopf ab
CLOOP1	LD	A,B
	OUT	(PIO2AD),A
	IN	A,(PIO2BD)
	OR	C
	OUT	(PIO2BD),A	;Select mit dem Motor abschalten
CLOOPE	INC	HL		;naechste Motorzeit
	SLA	C		;Maske auf naechstes LW
	BIT	4,C		;alle LW abgearbeitet ?
	JR	Z,CLOOP
INTEND	POP	BC
	POP	AF
	POP	HL
 COND 1-DMA
ISRDMA
 ENDC
IRET	EI			;EI fuer ISR PIO2
	RETI

 COND DMA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Interruptserviceroutine fuer den DMA
;enthaelt nur DMA ruecksetzen
;
ISRDMA	PUSH	AF
	LD	A,0A3H		;DMA Interrupt ruecksetzen
	OUT	(DMAADR),A
	POP	AF
	JR	IRET
 ENDC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;READY
;READY prueft die Bereitschaft des angeforderten LW mit Hilfe des
;FDC-Kommandos "Sense Drive Status" und schaltet gegebenenfalls
;den Motor ein.
;A enthaelt Laufwerksnummer

READY	LD	E,A
	LD	D,0
	INC	A		;LW Maske aus LW Nr. bilden
	LD	B,7FH
RDY0	RLC	B
	DEC	A
	JR	NZ,RDY0
	LD	A,B
	CPL
	LD	(DMASK),A	;LW Maske speichen
 COND UDOS
	LD	HL,DSEL
	AND	(HL)
	JR	NZ,RDY1
	BIT	7,(HL)		;INITIALISIERUNG ?
	RES	7,(HL)
	RET	Z		;nein
RDY1	XOR	(HL)		;Drive in DSEL erst einmal not ready setzen
	LD	(HL),A
 ENDC

	PUSH	HL
	LD	HL,TTAB
	ADD	HL,DE
	LD	(HL),100	;2 Sekunden bis Motor aus (ab OPFIN)

	DI			;besser DI
	IN	A,(PIO2BD)
	OR	0FH		;Select altes Laufwerk ruecksetzen
	AND	B
	OUT	(PIO2BD),A	;Ausgabe des "Drive select" an PIO/B
	IN	A,(PIO2AD)
	AND	B		;Motor LW an
	SET	5,A		;Kopf an
	OUT	(PIO2AD),A
	LD	A,0A5H		;CTCINT fuer Motor aus programmieren
	OUT	(CTC0+1),A
	LD	A,255		;16,3ms
	OUT	(CTC0+1),A
	LD	HL,01500H	;Warteschleife
RDY2	LD	BC,0204H	;Kommando "Sense Drive Status"
	CALL	FDCKMD
	PUSH	HL
	CALL	RDRES
	POP	HL
	EI
	BIT	5,A
	JR	NZ,RDY3
	DEC	HL
	LD	A,H
	OR	L
	JR	NZ,RDY2		;not ready
RDY3	POP	HL
 COND UDOS
	RET	Z		;LW nicht bereit
	LD	A,(DMASK)
	OR	(HL)		;bereites LW in DSEL eintragen
	LD	(HL),A
 ENDC
	RET			;mit NZ

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Floppy-Typ und Format Tabellen

 COND U880SM
 
	ORG	0B24H
 
 ENDC
 
TYPTAB
;Standard FM 77 Spuren 26 Sektoren a 128 Byte (Typ 1)

	DEFB	6FH		;10ms, 240ms
	DEFB	28H		;40ms (8MHz)
	DEFB	0
	DEFB	26
	DEFB	07H
	DEFB	128
	DEFB	-1
	DEFB	-1
	DEFB	1BH
	DEFB	77
	DEFB	02H		;MT=0, MFM=0, SK=0, 8"=1, DSTEP=0

;Mini MFM 40 Spuren 16 Sektoren a 256 Byte (Typ 2)

	DEFB	9FH		;14ms, 480ms
	DEFB	14H		;40ms (4MHz)
	DEFB	01H
	DEFB	16
	DEFB	22H
	DEFB	-1
	DEFB	20
	DEFB	29
	DEFB	32H
	DEFB	40
	DEFB	40H		;MT=0, MFM=1, SK=0, 8"=0, DSTEP=0

;Mini MFM 40 Spuren auf 80 Spur Drive 16 Sektoren a 256 Byte (Typ 3)

	DEFB	0BFH		;10ms (bei NEC nur 3ms erforderlich), 480ms
	DEFB	12H		;36ms (fuer NEC Laufwerk)
				;fuer K5600.20 nur 12ms (Kopfberuhigungzeit)
				;erforderlich (DEFB 06H)
	DEFB	01H
	DEFB	16
	DEFB	22H
	DEFB	-1
	DEFB	35
	DEFB	0FFH
	DEFB	32H
	DEFB	40
	DEFB	41H		;MT=0, MFM=1, SK=0, 8"=0, DSTEP=1

;Mini MFM 80 Spuren 16 Sektoren a 256 Byte (Typ 4)

	DEFB	0BFH		;SRT=10ms, HUT=480ms
	DEFB	12H		;HLT=36ms
	DEFB	01H		;N
	DEFB	16		;EOT
	DEFB	22H		;GPL
	DEFB	-1		;DTL
	DEFB	35		;PR 1
	DEFB	0FFH		;PR 2
	DEFB	32H
	DEFB	80		;Anzahl der Spuren
	DEFB	40H		;MT=0, MFM=1, SK=0, 8"=0, DSTEP=0

;Mini MFM Doppelkopf 80 Spuren 2*16 Sektoren a 256 Byte (Typ 5)

	DEFB	0BFH		;10ms, 480ms
	DEFB	12H		;36ms
	DEFB	01H
	DEFB	16
	DEFB	22H
	DEFB	-1
	DEFB	0FFH
	DEFB	0FFH
	DEFB	32H
	DEFB	80
	DEFB	0C0H		;MT=1, MFM=1, SK=0, 8"=0, DSTEP=0

;Mini MFM Doppelkopf 80 Spuren 2*9 Sektoren a 512 Byte (Typ 6)

	DEFB	0EFH		;4ms, 480ms
	DEFB	08H		;16ms
	DEFB	02H
	DEFB	9
	DEFB	1BH
	DEFB	-1
	DEFB	-1
	DEFB	-1
	DEFB	54H
	DEFB	80
	DEFB	0C0H		;MT=1, MFM=1, SK=0, 8"=0, DSTEP=0

;Mini MFM Doppelkopf 80 Spuren 2*5 Sektoren a 1024 bYTE (Typ 7)

	DEFB	0EFH		;4ms, 480ms
	DEFB	08H		;16ms
	DEFB	03H
	DEFB	5
	DEFB	35H
	DEFB	-1
	DEFB	-1
	DEFB	-1
	DEFB	74H
	DEFB	80
	DEFB	0C0H		;MT=1, MFM=1, SK=0, 8"=0, DSTEP=0

;Mini MFM Doppelkopf 80 Spuren 2*15 Sektoren a 512 bYTE (Typ 8)

;DEFB	0DFH		;3ms, 240ms
;DEFB	10H		;16ms (8MHz)
;DEFB	02H
;DEFB	15
;DEFB	1BH
;DEFB	-1
;DEFB	-1
;DEFB	-1
;DEFB	54H
;DEFB	80
;DEFB	0C1H		;MT=1, MFM=1, SK=0, 8"=1, DSTEP=0

;Mini MFM Doppelkopf 80 Spuren 2*8 Sektoren a 1024 Byte (Typ 9)

;DEFB	0DFH		;3ms, 240ms
;DEFB	10H		;16ms (8MHz)
;DEFB	03H
;DEFB	8
;DEFB	35H
;DEFB	-1
;DEFB	-1
;DEFB	-1
;DEFB	74H
;DEFB	80
;DEFB	0C1H		;MT=1, MFM=1, SK=0, 8"=1, DSTEP=0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;VEREINBARUNGEN
;Kommandotabelle fuer den FDC

 COND 1-U880SM

FDCTAB	DEFB	00H		;Command Codes (MT,MFM,SK & Read/Write)
	DEFB	00H		;Bit 2,1,0: HDS,DS1,DS0
	DEFB	0		;Spur
	DEFB	00H		;Kopf
	DEFB	00H		;Record
	DEFB	01H		;256 Byte/Sector
	DEFB	16		;EOT (last sector on track)
	DEFB	0EH		;GPL (Length of gap 3)
	DEFB	0FFH		;DTL (N=0 --> Data length)

PR1	DEFB	35
PR2	DEFB	45
GPL2	DEFB	32H
TRK	DEFB	80
CMD	DEFB	40H

;DMA-Tabelle fuer die Uebernahme/Uebergabe der Daten vom/zum FDC
DMALSW	DEFB	79H		;Datenlaenge und Datenadresse ist einzutragen
	DEFB	0		;Daten-Adresse Low-Teil
	DEFB	0		;Daten-Adresse High-Teil
	DEFB	0		;Datenlaenge Low-Teil
	DEFB	0		;Datenlaenge High-Teil
	DEFB	0CFH		;Laden der Adresse und Laenge
	DEFB	5		;Richtungsumkehr programmieren
	DEFB	0CFH		;Laden Richtungsumkehr
	DEFB	0ABH		;Interruptfreigabe fuer DMA
	DEFB	87H		;DMA aktivieren

;Resultats-Tabelle der FDC-Kommandos

RESUL	DEFS	7

;Motorzeittabelle

TTAB	DEFB	0		;Motor 0 aus
	DEFB	0		;Motor 1 aus
	DEFB	0		;Motor 2 aus
	DEFB	0		;Motor 3 aus

;Fehlerzaehler
WSCTR	DEFB	0		;Fehlerzaehler Sektor
WTCTR	DEFB	0		;Fehlerzaehler Spur
CRCCTR	DEFB	0		;CRC-Fehlerzaehler

DSKVSL	DEFW	0		;Disk Semaphor
INTPNT	DEFW	0		;Adresse fuer Programmfortsetzung
DMASK	DEFB	0		;LW Maske fuer Motor aus und ...
FDTYP	DEFB	0
CURT0	DEFB	-1		;Tabelle der Kopfpositionen der Laufwerke
	DEFB	-1
	DEFB	-1
SPURNR	DEFB	0
	DEFB	-1
FDCONF	DEFW	0
SECERR	DEFW	0
CRCERR	DEFW	0
TRKERR	DEFW	0
PIO2AD	EQU	1CH
PIO2AC	EQU	1DH
PIO2BD	EQU	1EH
PIO2BC	EQU	1FH
MSR	EQU	20H
DATREG	EQU	21H
DMAADR	EQU	3CH
CTC0	EQU	08
 ENDC

