; ------------------------------------------
; ! INITIALISIERUNG FDC-CONTROLLER P8000 !
; ------------------------------------------
;
;
U880M	EQU	1		; =0 --> Verwendung im RAM
				; =1 --> Verwendung im U880-Softwaremonitor
;
; Initialisierung des PIO2 und FDC auf der U880-Karte P8000
; Belegung von Port A und B des PIO2:
;	A0 -- Ausgang -- Motor LW 0 an               -- /LCK0
;	A1 -- Ausgang -- Motor LW 1 an               -- /LCK1
;	A2 -- Ausgang -- Motor LW 2 an               -- /LCK2
;	A3 -- Ausgang -- Motor LW 3 an               -- /LCK3
;	A4 -- Ausgang -- Takt                        -- T
;	A5 -- Ausgang -- Head Load                   -- HL
;	A6 -- Ausgang -- Terminal Count              -- TC
;	A7 -- Eingang -- Spann.- oder Bedienerreset  -- RES
;
;       B0 -- Ausgang -- Select LW0    		     -- /DS0
;       B1 -- Ausgang -- Select LW1		     -- /DS1
;       B2 -- Ausgang -- Select LW2 		     -- /DS2
;       B3 -- Ausgang -- Select LW3		     -- /DS3
;       B4 -- Eingang -- Interrupt vom FDC	     -- INT
;       B5 -- Ausgang -- Praecompensation 1	     -- PR1
;       B6 -- Ausgang -- Praecompensation 2	     -- PR2
;	B7 -- Ausgang -- Reset FDC		     -- RFDC
;
;
 COND U880M
; Adressen der Interruptserviceroutinen
	LD	HL,ISRPB	;Adresse PIO2(FDC)-Interruptserviceroutine
	LD	(IP2KB),HL
;
	LD	HL,ISRCT1	;Adresse CTC-Interruptserviceroutine
	LD	(INTAB+2),HL
;
	LD	HL,ISRDMA	;Adresse DMA-Interruptserviceroutine
	LD	(INTABD),HL
 ENDC
;
 COND 1-U880M
; Initialisierung DSEL
	LD	A,80H
	LD	(DSEL),A
 ENDC
	LD	A,5
	LD	(FDCONF),A
; Initialisierung DMA

	LD	BC,10
	LD	HL,DMAT
	LD	DE,DMALSW
	LDIR
 COND 1-U880M
	RET
 ENDC
;
	JR	FL1END
;
;
;DMA-Tabelle fuer die Uebernahme/Uebergabe der Daten vom/zum FDC
DMAT	DEFB	79H		;Datenlaenge und Datenadresse ist einzutragen
	DEFB	0		;Daten-Adresse Low-Teil
	DEFB	0		;Daten-Adresse High-Teil
	DEFB	0		;Datenlaenge Low-Teil
	DEFB	0		;Datenlaenge High-Teil
	DEFB	0CFH		;Laden der Adresse und Laenge
	DEFB	5		;Richtungsumkehr programmieren
	DEFB	0CFH		;Laden Richtungsumkehr
	DEFB	0ABH		;Interruptfreigabe fuer DMA
	DEFB	87H		;DMA aktivieren
;
FL1END
;Adressen der benutzten peripheren Schaltkreise
PIO2AD	EQU	01CH
PIO2AC	EQU	01DH
PIO2BD	EQU	01EH
PIO2BC	EQU	01FH
MSR	EQU	020H
DATREG	EQU	021H
CTC	EQU	08H
;
;Adressen der Interruptserviceroutinen
INTABP	EQU	0F1AH
INTAB	EQU	0F00H
INTABD	EQU	0F1CH
