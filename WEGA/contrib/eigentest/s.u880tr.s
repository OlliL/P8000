h65362
s 00002/00001/02109
d D 3.1.1.1 89/11/07 15:18:30 klatte 2 1
c error25
e
s 02110/00000/00000
d D 3.1 89/11/07 14:40:48 klatte 1 0
c date and time created 89/11/07 14:40:48 by klatte
e
u
U
f b 
t
T
I 1
;**************************************************************************
;**************************************************************************

;	UDOS - Quelle          (c) ZFT/KEAW Abt. Basissoftware 1988
;	P8000-UDOS
;	Programm:			EIGENTEST
;	Modul:    			%M%
;	Bearbeiter: 			J.Kubisch, M.Klatte
;	Datum der letzten Release:	%G%
;	UDOS-Release:			%R%.%L%
;	Zweignummer:			%B%.%S%

;**************************************************************************
;*************************************************************************/


; Verbindungen zum C-Modul menu.c:
	
	public 	EPROM_,SRAM_,PIO_,CTC_,SIO_,FDC_,DMA_,DRAM_
	public	errcnt			;Fehlerzaehler (0...32767)
	extrn	ertab			;Fehlerliste 1000 Byte

	begin	0

D 2
	text 	'%W%'
E 2
I 2
	text 	'%W%\'
E 2


; PROM-Test
; Fuer den PROM wird ausser den letzten zwei Bytes die Pruefsumme
; gebildet und mit dem in den letzten zwei Bytes stehenden Wert
; verglichen. Bei Fehler wird der Fehlerkode 00 o. 01 an das Terminal
; ausgegeben.

EPROM_:
	di

	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX
	ld	a,3
	ld	(MAWE),a	;Anzahl auszugebener Werte
	LD	E,0		;TEST-NUMMER SETZEN
	EXX

; PROM select 0000H...1FFFH

	ld	c,WEADP		;adress bank-select
	ld	d,1		;PROM-select
	ld	b,0		;adress:0000
	out	(c),d
	ld	b,10H		;adress:1000H
	out	(c),d

; Berechnung des CRC der PROM's nach dem SDLC Polynom

; Berechnung des CRC fuer EPROM0

	LD	HL,0		;1. Byte-Adr.
	LD	BC,1000H	;Byte-Anzahl
	ld	de,-1
crc1:	ld	a,(hl)
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	rrca
	and	0fh
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	LD	I,A
	and	1fh
	xor	e
	ld	e,a
	LD	A,I
	LD	I,A
	rrca
	and	0f0h
	xor	e
	ld	e,a
	LD	A,I
	and	0e0h
	xor	d
	ld	d,e
	ld	e,a
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,crc1
	LD	HL,(CRCEP0)
	LD	A,D
	CP	H
	JR	NZ,CRCFE0
	LD	A,E
	CP	L
	JR	NZ,CRCFE0
	LD	HL,(NCEP0)
	ADD	HL,DE
	LD	A,0FFH
	CP	H
	JR	NZ,CRCFE0
	CP	L
	JR	Z,CRCE1

CRCFE0	ld	b,d
	ld	a,e		;CRC-Istwert umladen ( ba:=de )
	call	FAHLR		;Fehlerausgabe ( Feh.Nr. CRC-Soll  CRC-Ist )
 
; Berechnung und Test des CRC fuer EPROM1

CRCE1	exx
	ld	e,1		;Fehler-Nr. naechsten Test laden
	exx
 
	LD	HL,1000H	;STARTADRESSE PROM
	LD	BC,0FF8H	;BYTE-ZAHL
	ld	de,-1
crc10:	ld	a,(hl)
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	rrca
	and	0fh
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	LD	I,A
	and	1fh
	xor	e
	ld	e,a
	LD	A,I
	LD	I,A
	rrca
	and	0f0h
	xor	e
	ld	e,a
	LD	A,I
	and	0e0h
	xor	d
	ld	d,e
	ld	e,a
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,crc10
	LD	HL,(CRCEP1)	;CRC-Sollwert holen
	LD	A,D
	CP	H		;Soll-Ist Vergleich High-Teil
	JR	NZ,CRCFE1
	LD	A,E
	CP	L
	JR	NZ,CRCFE1
	LD	HL,(NCEP1)
	ADD	HL,DE
	LD	A,0FFH
	CP	H
	JR	NZ,CRCFE1
	CP	L
	JR	Z,EPROME

CRCFE1	ld	b,d
	ld	a,e		;CRC-Istwert umladen ( ba:=de )
	call	FAHLR		;Fehlerausgabe (Feh.-Nr.  CRC-Soll  CRC-Ist )

 
; DRAM select 0000H...1FFFH

EPROME:	ld	c,WEADP		;adress bank-select
	ld	d,4		;DRAM-select
	ld	b,0		;adress:0000
	out	(c),d
	ld	b,10H		;adress:1000H
	out	(c),d

	ld	a,IVRSYS
	ld	i,a		;I-Reg. auf UDOS-System stellen

	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	ei

	RET			;END OF PROCEDURE

;-----------------------------------------------------------------------

; SRAM-Test
; Es werden ein Test der Adressleitungen,der Datenleitungen,der Daten-
; haltigkeit des Speichers durch das Durchschieben von 0 oder 1 und ein
; Programmtest des Speichers durchgefuehrt. Bei auftretenden Fehlern
; wird der entsprechende Fehlerkode an das Terminal ausgegeben. Am
; Terminal werden ausserdem die Speicheradresse, der ausgegebene und der
; rueckgelesene Wert angezeigt.

SRAM_:
	di
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX			;holen Zweitregistersatz
	LD	E,6		;TEST-NUMMER LADEN
	ld	a,3
	ld	(MAWE),a
	EXX

; Initialisierung Speicherbank, SRAM auf die Adresse 2000H

	LD	D,2		;SRAM_SEL aktiv
	LD	B,20H		;Adresse ist 2000H
	LD	C,WEADP
	OUT	(C),D		;Einschalten des SRAM auf die Adresse 2000H

; Test der Datenleitungen des SRAM

	XOR	A		;alle Bits auf 0 setzen
TAUFD	LD	HL,2000H	;Anfangsadresse ist 2000H
	LD	DE,2001H
	LD	BC,7FFH		;Datenlaenge ist 2k-1
	LD	(HL),A		;0 oder 0FFH auf den ersten Speicherplatz
	LDIR			;Wert wird in alle Speicherzellen des SRAM
				; eingetragen
	CP	(HL)		;Letztes Byte gleich dem Akkumulator?
	JR	Z,DTSR1
	LD	B,A		;Testwert eintragen
	LD	A,(HL)
	call	FAHLR		;Fehlerausgabe
	jr	WT2SR

DTSR1	XOR	0FFH		;Ja.
	JR	NZ,TAUFD	;Test mit 0FFH

WT2SR 	EXX
	LD	E,5
	EXX

; Test der Adressleitungen des statischen  RAM im Bereich 2000...27ffh

; Der Speicher wird in Seiten  zu je 256 Byte aufgeteilt. Die Bytes
; einer Seite  werden fortlaufend nummeriert. Das erste Byte in einer
; Seite ist Gleich dem High-Teil der Speicheradresse dieses Bytes
; (Seiten-Nr.). Anschliessend werden die Seiten  nacheinander getestet.
; War der Test der Datenleitungen fehlerfrei und stimmen die Werte
; einer Seite   mit dem eingegebenen Wert nicht ueberein,dann ist
; die Ansteuerung fuer ein Adressbit dieses Bytes fehlerhaft.

; Laden des Musters

	ld	hl,2000h	;Startadresse SRAM-Teil
	ld	d,8		;Seitenzahl laden (1 Seite = 256 Byte)
sr210	ld	b,0		;b:=256
	ld	a,h		;Seiten-Nr. holen (1.Byte in Seite=Seiten-Nr.)

sr220	ld	(hl),a		;SRAM auf einer Seite (256 Byte) fortlaufend
	inc	a		;nummerieren
	inc	hl
	djnz	sr220

	dec	d		;DEC Seitenzaehler
	jr	nz,sr210	;wenn nicht die letzte, JP

; Test ob Muster richtig geladen wurde

	ld	hl,2000h	;Startadresse SRAM-Teil
	ld	de,0800h	;Seitenzahl*Seitenlaenge laden
sr230	ld	b,h		;Seiten-Nr. holen (Sollwert)

sr240	ld	a,(hl)		;Istwert holen
	cp	b		;Soll=Ist?
	call	nz,FAHLR
	inc	b		;neue Sollwert-Byte-Nr.
	inc	hl		;neue Adresse
	dec	e		;Bytezaehler
	jr	nz,sr240	;wenn Seitenende nicht erreicht, jp

	dec	d		;DEC Seitenzaehler
	jr	nz,sr230	;wenn noch nicht alle Seiten getestet, jp

	EXX
	LD	E,7
	EXX

; Durchschieben von 0 und 1

; Durchschieben einer Null

	LD	HL,2000H
	LD	(HL),0FFH
	LD	DE,7FFH
	LD	B,E
	LD	A,(424)		;nur wegen Aenderung auf dem Bus
	LD	A,(HL)
	CP	B		;Testen des ersten Wertes
	call	nz,FAHLR
	INC	HL
	AND	A
NZNL	RLA
	LD	(HL),A		;Laden des Speichers mit den Werten
	EX	AF,AF'
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR  	Z,T2ZSR
	EX	AF,AF'
	JR	NZNL
T2ZSR	
	LD	HL,2001H
	LD	DE,7FEH
	SCF
	LD	B,E		;Register mit 0FEH laden
	EX	AF,AF'
TNZSR2	
	LD	A,(HL)
	CP	B		;Vergleich der Werte
	call	NZ,FAHLR
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	Z,SRAME
	EX	AF,AF'
	LD	A,B
	RLA
	LD	B,A
	EX	AF,AF'
	JR	TNZSR2

; Initialisierung Speicherbank, DRAM auf die Adresse 2000H zurueckschalten

SRAME:	LD	D,4		;DRAM_SEL aktiv
	LD	B,20H		;Adresse ist 2000H
	LD	C,WEADP
	OUT	(C),D		;Einschalten des DRAM auf die Adresse 2000H

	ld	a,IVRSYS
	ld	i,a		;I-Reg. auf UDOS-System stellen

	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	ei

	RET			;END OF PROCEDURE

;-----------------------------------------------------------------------------

; PIO-Test
; Es werden alle PIO's im Ausgabemode getestet (ohne Interrupt).
; Im Fehlerfall werden Port-Adresse, eingetragener und rueckgelesener
; Wert an das Terminal ausgegeben.

PIO_:
	di

	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX
	LD	E,10H
	LD	A,3
	LD	(MAWE),A	;Initialisierung fuer den Merker
	EXX

; Test PIO0, Kanal A im Ausgabemode (ohne Interrupt)

	LD	A,3
	LD	C,P0AC
	OUT	(C),A
	LD	A,55H
	CALL	PTAUS
	CALL	NZ,FA3RET


; Test PIO0, Kanal B im Ausgabemode

	LD	C,P0BC
	LD	A,0D5H
	CALL	PTAUS
	CALL	NZ,FA3RET

; Test PIO1, Kanal A im Ausgabemode

	ld	a,3
	ld	c,P1AC
	out	(c),a		;DI PIO1A
	ld	a,55h		;Testmuster
	call	PTAUS
	call	nz,FA3RET


; Test PIO1, Kanal B im Ausgabemode

	LD	A,3
	LD	C,P1BC
	OUT	(C),A
	LD	A,0F3H
	CALL	PTAUS
	CALL	NZ,FA3RET

; Test PIO2, Kanal A im Ausgabemode


	LD	A,3
	LD	C,P2AC
	OUT	(C),A
	LD	A,9FH
	CALL	PTAUS
	CALL	NZ,FA3RET

; Test PIO2, Kanal B im Ausgabemode


	LD	A,3
	LD	C,P2BC
	OUT	(C),A
	LD	A,6FH
	CALL	PTAUS
	CALL	NZ,FA3RET

PIOE:
	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	ei

	RET			;END OF PROCEDURE

;----------------------------------------------------------------------------

; CTC-Test

CTC_:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

; Test der CTC-Kanaele als Zaehler (ohne Interrupt)

	EXX
	LD	A,3
	LD	(MAWE),A
	LD	E,15H		;Test-Nr. ausgeben
	EXX

; Zaehltest von CTC0, Kanal 0-3 und CTC1, Kanal 0,2,3

	LD	C,C0K0
	LD	B,40H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C0K1
	LD	B,50H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C0K2
	LD	B,60H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C0K3
	LD	B,70H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C1K0
	LD	B,80H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C1K2
	LD	B,090H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C1K3
	LD	B,0A0H
	CALL	CZPR
	CALL	NC,FA3RET

	EXX
	LD	E,16H
	EXX

; Test des CTC, ob die Zeitkonstanten richtig ruecklesbar sind

	LD	B,40H
	LD	C,C0K0
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	LD	B,50H
	LD	C,C0K1
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	LD	B,60H
	LD	C,C0K2
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

  
	LD	B,70H
	LD	C,C0K3
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET


	LD	B,80H
	LD	C,C1K0
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	ld	c,C1K1		;Kanaladresse
	ld	d,57h		;DI / Zaehler / CLK / TC folgt / Kanal-RESET
	ld	b,2		;Zeitkonstante Sollwert TC=2
	ld	a,53h		;Kanal STOP
	out	(c),a
	in	a,(c)		;Zeitkonstante Istwert holen
	out	(c),d		;Kanalsteuerwort laden
	out	(c),b		;Zeitkonstante laden Kanal START
	cp	b		;Zeitkonstante: Soll=Ist ?
	call	nz,FA3RET	;wenn nicht, call

	LD	B,90H
	LD	C,C1K2
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	LD	B,0A0H
	LD	C,C1K3
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	EXX
	LD	A,1
	LD	(MAWE),A
	LD	E,17H
	EXX

; Test der CTC-Kanaele mit Interrupt

	call	INTINI		; INT-Initialisierung

; CTC0, Kanal0 auf INT programmieren

	XOR	A		;INT.VEC. fuer CTC0 = 00H
	LD	C,C0K0
	OUT	(C),A		;INT-VEKTOR WIRD NUR EINMAL PROGRAMMIERT
	CALL	CIPR		;Interruptprogrammierung CTC	
	CALL	Z,FA3RET	;bei Fehler Ausgabe

	LD	C,C0K1
	CALL	CIPR
	CALL	Z,FA3RET

	LD	C,C0K2
	CALL	CIPR
	CALL	Z,FA3RET

	LD	C,C0K3
	CALL	CIPR
	CALL	Z,FA3RET

	LD	A,8		;INT.VEC. fuer CTC1 = 08H
	LD	C,C1K0
	OUT	(C),A		;Interruptvektor wird einmal ausgegeben
	CALL	CIPR
	CALL	Z,FA3RET

	LD	C,C1K2
	CALL	CIPR
	CALL	Z,FA3RET

	LD	C,C1K3
	CALL	CIPR
	CALL	Z,FA3RET

CTCE	ld	a,IVRSYS
	ld	i,a		;Systemzustand regenerieren

	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	RET			;END OF PROCEDURE

;-----------------------------------------------------------------------------

; SIO-Test
; Test des SIO durch Lesen des Interruptvektors (ohne Interrupt)
; Eingabe des Interruptvektors und anschliessendes Ruecklesen
; Test ist auch nur im Kanal B moeglich
; Ueberpruefen des Sende-INT

SIO_:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX
	LD	E,20H		;TEST-NR. LADEN
	LD	A,3
	LD	(MAWE),A	;3 Werte ausgeben
	EXX

	di			;weil INT.VEC. der SIO's manipuliert werden

; Test SIO 0 Kanal B, durch lesen des INT.VEC (Sollwert IV=20H)

	ld	c,S0BC
	ld	a,12h
	out	(c),a		;RR2 adressieren (INT.VEC.REG.)
	ld	b,20h		;INT.VEC. sollwert laden
	in	a,(c)		;INT.VEC. istwert holen
	and	0f0h		;festen Vektorteil selektieren
	cp	b
	call	nz,FA3RET

; Test SIO 1

	LD	A,11H
	OUT	(S1BC),A
	XOR	A
	OUT	(S1BC),A
	LD	A,12H		;Schreibregister 2 ansprechen
	OUT	(S1BC),A
	LD	A,50H		;Wert 50H eintragen
	OUT	(S1BC),A
	IN	A,(S1BC)	;Lesen des Leseregisters 0, damit aendern der
				; Daten
				;auf dem Datenbus
	LD	A,12H		;Leseregister 2 ansprechen
	OUT	(S1BC),A
	LD	C,S1AD		;erste SIO-Adresse fuer evt. Fehlerausgabe
				; eintragen
	LD	B,50H
	IN	A,(S1BC)
	CP	B		;Ist der Wert gleich dem ausgegebenen Wert?
	CALL	NZ,FA3RET	;Nein, Fehler bei Aufruf

; Wiederholung gleicher Test, nur mit anderem Wert

	LD	A,12H		;Schreibregister 2 adressieren
	OUT	(S1BC),A
	LD	A,30H
	OUT	(S1BC),A	;INT.VEC. SIO1 = 30H laden
	IN	A,(S1BC)	;Veraenderung auf dem Datenbus
	LD	A,12H		;Leseregister 2 adressieren
	OUT	(S1BC),A
	LD	C,S1AD
	LD	B,30H
	IN	A,(S1BC)	;Einlesen des Interruptvektors
	CP	B
	CALL	NZ,FA3RET

	EXX
	LD	A,1
	LD	(MAWE),A
	LD	E,21H
	EXX

; Programmierung SIO0 und SIO1 als Sender (mit Interrupt)

	call	INTINI		; INT-System-Initialisierung

; SIO0 Kanal B und Interruptvektoren sind bereits programmiert

	LD	B,CTC1
	LD	E,S0AC
	CALL	SINIT		;SIO 0  Kanal A programmieren
 
	LD	A,R1+COMM2
	OUT	(S0BC),A
	ld	a,4		;Kanal B-DI, status affects vector
	OUT	(S0BC),A

	LD	A,R1+COMM2
	OUT	(S0AC),A
	LD	A,2		;nur EI fuer Sender Kanal A
	OUT	(S0AC),A

; SIO0 Kanal A sendet 00 und warten auf INT 'Sendepuffer leer'

	LD	C,S0AD
	CALL	SI0APR		;Zeichen senden und auf INT warten
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

; SIO0 Kanal B sendet 00 und warten auf INT 'Sendepuffer leer'

;	LD	C,S0BD
;	CALL	SI0BPR
;	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

; SIO 1 programmieren

	LD	B,C1K2
	LD	E,S1AC
	CALL	SINIT		;SIO 1 Kanal A progr.
 
	LD	B,C0K0
	LD	E,S1BC
	CALL	SINIT		;SIO 1 Kanal B progr.
 
	LD	A,R1+COMM2
	OUT	(S1BC),A
	ld	a,6		;nur EI fuer Sender, status affects vector
	OUT	(S1BC),A

	LD	A,R1+COMM2
	OUT	(S1AC),A
	LD	A,2		;nur EI fuer Sender
	OUT	(S1AC),A

; SIO1 Kanal A sendet 00 und warten auf INT 'Sendepuffer leer'

	LD	C,S1AD
	CALL	SI1APR
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

; SIO1 Kanal B sendet 00 und warten auf INT 'Sendepuffer leer'

	LD	C,S1BD
	CALL	SI1BPR
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

SIOE:	di			;System-status regenerieren
	ld	c,C1K1		;CTC1 Kanal 1 programm.
	ld	a,57h
	out	(c),a
	ld	a,2
	out	(c),a

	ld	hl,INITAB	;Initialis. Tabelle SIO0B
	ld	c,S0BC
	ld	b,12		;anzahl CW
	otir
	
	ld	a,IVRSYS
	ld	i,a		;I-Reg. regenerieren
	
	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	ei

	RET			;END OF PROCEDURE

;-----------------------------------------------------------------------------

; FDC-Test

FDC_:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

 	EXX
	LD	A,3
	LD	(MAWE),A
	LD	E,23H		;Test-Nr. laden
	EXX

; Test des FDC
; Programmierung SPECIFY-Kommando und Ruecklesen

	DI

; FDC-PIO programmieren ( PIO2B )

	LD	A,0CFH		;MODE 3 (Bitmode)
	OUT	(P2BC),A
	LD	A,10H		;Nur Bit4 Eingang
	OUT	(P2BC),A
	LD	A,3		;DI
	OUT	(P2BC),A

	call	FDCRES		; RESET-Impuls an FDC

	CALL	FDCREA		;FDC bereit zur Datenuebernahme ?
	JR	NZ,FDCE		;FDC meldet keine Freigabe, damit keine weitere
				;Initialisierung moeglich

; 1.Test mit FDC-Kommando 'SPECIFY'

	exx
	ld	e,24h
	exx

	LD	A,3		;Kommando-Code 'SPECIFY'
   	OUT	(DFDC),A

	CALL	FDCREA		;FDC bereit zur Befehlsuebernahme?
	JR	NZ,FDCE
	LD	A,1FH		;SRT=1   (Step Rate Time = (16-1)ms = 15ms )
	OUT	(DFDC),A	;HUT=0FH (Head Unload Time = 15*16ms=240ms )
	
	CALL	FDCREA
	JR	NZ,FDCE
	LD	A,55H		;HLT=2AH (Head Load Time = 2AH*2ms =84ms=54H )
				;ND =1   (Non DMA-Mode)
	OUT	(DFDC),A
	
	call	FDCREA		;FDC ready ?
	jr	nz,FDCE

; 2. Test mit FDC-Kommando 'SENSE DRIVE STATUS'

	exx
	ld	e,25h
	exx

	ld	a,4		;Kommando-Code
	out	(DFDC),a	;ins Datenregister des FDC schreiben
	
	call	FDCREA		;FDC bereit zur Befehlsuebernahme?
	jr	nz,FDCE
	ld	a,05		; Select-Byte=05 : HDS=1 , DS1=0 , DS0=1
	out	(DFDC),a
	
	call	FDCREE		;warten auf FDC bereit,lesen moeglich
	jr	nz,FDCE

	; Statusregister ST3 lesen

	ld	b,05		;ST3 Sollwert
	ld	c,DFDC
	in	a,(c)		;ST3 lesen
I 2
	and	7		;andere ausblenden
E 2
	cp	b		;Soll=Ist?
	call	nz,FA3RET	;nein, dann call

FDCE:	call	FDCRES		;FDC-Reset

	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	EI

	RET			;END OF PROCEDURE

;-----------------------------------------------------------------------------

; DMA-Test
; Test des DMA durch Programmieren und Ruecklesen Statusbyte und
; Blocklaenge (ohne Interrupt)
; Test ohne INT:
; Der DMA transferiert 100 Byte von Adresse 0000 zur Adresse 3000h.
; Danach wird das Statusregister und das Zaehlerregister auf Sollwerte
; ueberprueft.
; Test mit INT:
; Wie vorher RAM-MOVE, danach Test ob ISR angesprungen wurde und
; Statusregister den Sollwert besitzt.
; Am Ende ueberprueft die CPU dem RAM-Transfer auf Richtigkeit.

DMA_:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX
	LD	E,26H		;Fehler-Nr. laden
	ld	a,3
	ld	(MAWE),a	;bei Fehler, 3 Werte ausgeben	
	EXX

; Zielpuffer loeschen

	ld	hl,3000h
	ld	de,3001h
	ld	bc,7FFh
	ld	(hl),0
	ldir

; DMA programmieren

	LD	HL,DMAT
Z:	def	DMATE-DMAT
	LD	B,Z
	LD	C,DMAADR
	OTIR

; DMA-START move S=0000 D=3000 L=100

	LD	B,19H		;STATUS-Byte Sollwert
	IN	A,(C)		;Status-Byte lesen
	AND	3BH		;Status-Byte maskieren
	cp	b		;Soll=Ist?
	CALL	NZ,FA3RET	;Wenn Statusbyte fehlerhaft, call

	LD	C,DMAADR
	LD	B,0ffh		;zweiter Testwert wird eingetragen
	IN	A,(C)		;Zaehler low-Teil lesen
	CP	B		; =-1 ?
	CALL	NZ,FA3RET	; Fehler, wenn nicht

	LD	C,DMAADR
	LD	B,0		;dritter Testwert wird eingetragen
	IN	A,(C)		;Zaehler high-Teil lesen
	CP	B
	CALL	NZ,FA3RET

	EXX
	LD	A,1
	LD	(MAWE),A
	LD	E,27H
	EXX

; Test des DMA mit Interrupt
; Programmieren eines Speichertransfers und warten auf den Interrupt

; Zielpuffer loeschen

	ld	hl,3000h
	ld	de,3001h
	ld	bc,7FFh
	ld	(hl),0
	ldir

	call	INTINI		; INT-System-Initialisierung

	LD	HL,DMATI
Z:	def	DMATIE-DMATI
	LD	B,Z
	LD	C,DMAADR
	OTIR

; DMA START : move S=0 D=3000H L=100H

	call	ZEIT1		;auf INT warten
	ld	a,(MISDMA)	;MERKER.INT.SERVICE holen
	and	a		;
	call	z,FA3RET	;if a=0 then call

	ld	a,3
	ld	(MAWE),a

	ld	b,19h		;Statusbyte Sollwert
	IN	A,(C)		;Statusbyte lesen
	AND	3BH		;Maskieren
	cp	b	
	CALL	NZ,FA3RET

	LD	A,0C3H
	OUT	(DMAADR),A	;partielles RESET an den DMA

; Ueberpruefen ob DMA-Transfer o.k.

	EXX
	LD	A,3
	LD	(MAWE),A
	LD	E,28H
	EXX

	ld	hl,0		;Quell-Adr.
	ld	de,3000h	;Vergleichs-Adr.
	ld	bc,100h		;Daten-laenge

dmalop	ld	a,(de)		;istwert holen
	cpi			;vergleichen
	inc	de
	push	bc
	dec	hl		;hl auf aktuelle Solladr. stellen
	ld	b,(hl)		;Sollwert holen
	call	nz,FAHLR	;Transfererror
	inc	hl		;hl auf neue Adr. stellen
	pop	bc
	jp	pe,dmalop	;wenn BC>0 dann P/V-Flag=1 und Sprung


DMAE	ld	a,IVRSYS
	ld	i,a

	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	RET			;END OF PROCEDURE

;----------------------------------------------------------------------------

; DRAM-Test
; Es werden ein Test der Adressleitungen, der Datenleitungen, und der
; Datenhaltigkeit des Speichers durch das Durchschieben von 0 oder 1
; durchgefuehrt. Bei auftretenden Fehlern wird der entsprechende Fehler-
; kode an das Terminal ausgegeben. Am Terminal werden ausserdem die
; Speicheradresse,der geschriebene und der rueckgelesene Wert angezeigt.
; Der Test erfolgt in zwei Teilen. Erster Teil: Einschalten des DRAM auf
; den Adressen 0000H-FFFFH und Test des Bereichs 8000h...ffffh.
; Zweiter Teil: move des bereiches 0000...7fffh nach 'oben' und Test des
; 'unteren' DRAM-Bereiches (0000...7fffh). Danach remove des Programms
; SMON und SA.DIAGS und verlassen des DRAM-Tests.

DRAM_:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

	EXX
	LD	A,3
	LD	(MAWE),A
	LD	E,31H		;TEST-NR. LADEN
	EXX

; Initialisierung des DRAM auf den Adressen 0000H-FFFFH

	LD	D,04H		;DRAM_SEL aktiv
	LD	B,00h		;1. Adresse ist 0000H
	LD	C,WEADP
W1EDR	LD	A,10H
	OUT	(C),D		;Einschalten des DRAM 0000-ffff
	ADD	A,B
	LD	B,A
	JR	NZ,W1EDR

; Test der Datenleitungen des DRAM

	XOR	A		;alle Bits auf 0 setzen
	LD	HL,RAMANF	;Anfangsadresse
	LD	DE,RAMANF+1
	LD	BC,RAMLEN-1
	LD	(HL),A		;0 oder 0FFH auf den ersten Speicherplatz
	LDIR			;Wert wird in die Speicherzellen des DRAM
				; eingetragen
	LD	A,(HL)
	CP	B		;Testwert ist 0
	JP	NZ,DRAMFE	;Fehlerausgabe wenn nicht 00
	LD	A,0FFH		;Test mit 0FFH
	LD	HL,RAMANF
	LD	DE,RAMANF+1
	LD	BC,RAMLEN-1
	LD	(HL),A
	LDIR
	LD	B,A		;Testwert eintragen
	LD	A,(HL)
	CP	B
	JP	NZ,DRAMFE	;Fehlerausgabe wenn nicht FFH

	EXX
	LD	E,30H
	EXX

; Test der Adressleitungen des dynamischen RAM

; Der Speicher wird in Seiten  zu je 256 Byte aufgeteilt. Die Bytes
; einer Seite werden fortlaufend nummeriert. Das erste Byte in
; einer Seite ist Gleich dem High-Teil der Speicheradresse dieses
; Bytes. Anschliessend werden die Seiten  nacheinander getestet.
; War der Test der Datenleitungen fehlerfrei und stimmen die Werte
; einer Seite mit dem eingegebenen Wert nicht ueberein,dann ist
; die Ansteuerung fuer ein Adressbit dieser Seite fehlerhaft.

; Laden des Musters

	ld	hl,RAMANF
	ld	de,RAMLEN
ramta2	ld	b,0		;b:=256
	ld	a,h		;Start-Nr. holen

ramta3	ld	(hl),a
	inc	a
	inc	hl
	djnz	ramta3

	dec	d
	jr	nz,ramta2

; Test ob Muster richtig geladen wurde

	ld	hl,RAMANF
	ld	de,RAMLEN
ramta4	ld	b,h		;Start-Nr. holen (Sollwert)

ramta5	ld	a,(hl)		;Istwert holen
	cp	b		;Soll=Ist?
	jp	nz,DRAMFE	;Fehlerausgabe wenn ungleich
	inc	b		;neue Sollwert-Byte-Nr.
	inc	hl		;neue Adresse
	dec	e
	jr	nz,ramta5	;wenn Blockende nicht erreicht, jp

	dec	d
	jr	nz,ramta4	;wenn noch nicht alle Bloecke getestet, jp

	EXX
	LD	E,32H
	EXX

; Durchschieben von 0 und 1

; Durchschieben einer Null

	LD	HL,RAMANF
	ld	de,RAMLEN
	LD	B,0FEH		;Start-Sollwert laden

NZNLDR	LD	(HL),B
	RLC	B

	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,NZNLDR

; Test ob Muster o.k

	LD	HL,RAMANF
	LD	DE,RAMLEN
	LD	B,0FEH		;Start-Sollwert laden

TNZDR2	LD	A,(HL)		;Istwert holen
	CP	B		;Soll-Istvergleich
	JP	NZ,DRAMFE	;Fehlerausgabe wenn ungleich
	RLC	B

	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,TNZDR2

	di

	ld	hl,0		;Quelladresse
	ld	de,8000h	;Zieladresse
	ld	bc,8000h	;laenge
	ldir			;SMON u. SA.DIAGS move in 8000...ffff

adr:	def	DRAM2+8000h
	jp	adr		;Ansprung der Testroutiene im oberen RAM


; DRAM-Test 2.Teil, Programm laeuft im Bereich ab 8000h
; und testet den Bereich 0000...7fffh. 
;------------------------------------------------------

DRAM2:
	exx
	ld	e,36h		;Fehlernummer laden
	exx

; Test der Datenleitungen des DRAM

	XOR	A		;alle Bits auf 0 setzen
	LD	HL,0		;Anfangsadresse
	LD	DE,1
	LD	BC,7fffh	;laenge
	LD	(HL),A		;0 auf den ersten Speicherplatz
	LDIR			;Wert wird in die Speicherzellen des DRAM
				; eingetragen
	LD	A,(HL)
	CP	B		;Testwert ist 0
	jr	nz,dr2end

	LD	A,0FFH		;Test mit 0FFH
	LD	HL,0		;Startadresse RAM-Test
	LD	DE,1		;Startadresse+1
	LD	BC,7fffh	;laenge
	LD	(HL),A
	LDIR
	LD	B,A		; B:= FFH
	LD	A,(HL)
	CP	B
	jr	nz,dr2end

	EXX
	LD	E,35H
	EXX

; Test der Adressleitungen des dynamischen RAM im Bereich 0000...7fffh

; Der Speicher wird in Seiten  zu je 256 Byte aufgeteilt. Die Bytes
; einer Seite  werden fortlaufend nummeriert. Das erste Byte in
; einer Seite ist Gleich dem High-Teil der Speicheradresse dieses
; Bytes [ (hh00)=hh ]. Anschliessend werden die Seiten  nacheinander
; getestet. War der Test der Datenleitungen fehlerfrei und stimmen die
; Werte einer Seite mit dem eingegebenen Wert nicht ueberein,dann ist
; die Ansteuerung fuer ein Adressbit dieses Bytes fehlerhaft.

; Laden des Musters

	ld	hl,0		;Startadresse DRAM-Teil
	ld	d,80h		;Seitenzahl laden (1 Seite = 256 Byte)
dr210	ld	b,0		;b:=256
	ld	a,h		;Seiten-Nr. holen (1.Byte in Seite=Seiten-Nr.)

dr220	ld	(hl),a		;DRAM auf einer Seite (256 Byte) fortlaufend
	inc	a		;nummerieren
	inc	hl
	djnz	dr220

	dec	d		;neue Seiten-Nr. 
	jr	nz,dr210	;wenn nicht die letzte, JP

; Test ob Muster richtig geladen wurde

	ld	hl,0		;Startadresse DRAM-Teil
	ld	de,8000h	;Seitenzahl*Seitenlaenge laden
dr230	ld	b,h		;Seiten-Nr. holen (Sollwert)

dr240	ld	a,(hl)		;Istwert holen
	cp	b		;Soll=Ist?
	jr	nz,dr2end
	inc	b		;neue Sollwert-Byte-Nr.
	inc	hl		;neue Adresse
	dec	e		;Bytezaehler
	jr	nz,dr240	;wenn Seitenende nicht erreicht, jp

	dec	d		;DEC Seitenzaehler
	jr	nz,dr230	;wenn noch nicht alle Seiten getestet, jp

	EXX
	LD	E,37H
	EXX

; Durchschieben von 0 und 1

; Durchschieben einer Null

	LD	HL,0		;Anfang DRAM-Test
	ld	de,8000h
	LD	B,0FEH		;Start-Sollwert laden

dr250	LD	(HL),B
	RLC	B

	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	nz,dr250

; Test ob Muster o.k

	LD	HL,0
	LD	DE,8000h	;DRAM-laenge
	LD	B,0FEH		;Start-Sollwert laden

dr260	LD	A,(HL)		;Istwert holen
	CP	B		;Soll-Istvergleich
	jr	nz,dr2end
	RLC	B

	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,dr260

	exx
	ld	e,0		;Fehler-Nr.0, d.h. kein Fehler
	exx
	jr	dr2end

;Fehlerbehandlung
	;A = Istwert
	;B = Sollwert
	;E'= Fehlernummer
	;HL= Adresse

dr2end:	ex	af,af'
	exx			;Fehlerstatus in rr' speichern
	ld	a,e		;Fehler-Nr. in a retten
	
	ld	hl,8000h	;Quelladresse
	ld	de,0000		;Zieladresse
	ld	bc,8000h	;laenge
	ldir			;SMON u. SA.DIAGS remove to 0000...7fff

	jp	DRAMW
				;DRAM-Bereich 8000...ffffh
;-----------------------------------------------------------------------------
				;DRAM-Bereich 0000...7fffh	
DRAMW:
	or	a		;wenn Fehler-Nr.=0, dann kein Fehler
	jr	z,DRAME		;wenn kein Fehler, JP

	ld	e,a
	exx			;Fehlerstatus remove
	ex	af,af'
	ei
DRAMFE:	call	FAHLR		;Fehlerausgabe

DRAME:	ei
	pop	iy
	pop	ix
	pop	hl
	pop	de
	pop	bc
	pop	af

	RET			;END OF PROCEDURE

	eject

; Unterprogramme fuer die Testroutinen
;--------------------------------------

; Programmierung PIO im Ausgabemode (ohne Interrupt)
; Testwert in B, rueckgelesener Wert in A, Portadresse in C

PTAUS	LD	B,A		;Testwert in B retten
	LD	A,0FH		;Ausgabemode
	OUT	(C),A
	LD	A,3		;Interrupt sperren
	OUT	(C),A
	DEC	C		;Adresse Datenport einstellen
	LD	A,B		;Testwert fuer die Ausgabe holen
	OUT	(C),A
	IN	A,(S0BD)	;Datenbus veraendern
	IN	A,(C)
	CP	B		;Vergleich mit dem Testwert
	RET	

; Programmierung CTC-Kanal als Zeitgeber ohne Interrupt
; INPUT's:
;	C - Kanaladresse
;	B - Zeitkonstante
; 			Die Zeitkonstante sollte groesser als 7
; 			gewaehlt werden, damit das Zaehlerregister
;			nicht ueberlaeuft.
; OUTPUT's:
;	A - gelesener Wert des Zaehlerregisters
;	CF- CF=1:Kanal zaehlt, Test o.k.
;	    CF=0:Kanal zaehlt nicht, ERROR

CZPR:	LD	A,7		;DI, Timer, 1/16, no CLK, TC folgt, Kanal-Reset
	OUT	(C),A
	LD	A,B		;Zeitkonstante laden
	OUT	(C),A
	CALL	ZEIT2		;0.025 ms warten
	IN	A,(C)		;Zaehlerregister lesen
	CP	B		;wenn Kanal zaehlt, dann CF=1 sonst CF=0
	PUSH	AF		;CF retten
	LD	A,3
	OUT	(C),A		;Kanal stop
	POP	AF
	RET


; Programmierung CTC-Kanal als Zeitgeber mit Interrupt.
; Nach 0.04ms ist INT zu erwarten.
; Maximal wird 1ms auf den Interrupt gewartet.
; In der ISR wird HL entsprechend des ausgewaehlten Kanals veraendert.
; INPUT's:
;	C - Kanaladresse

; OUTPUT's:
;	ZF- ZF=1: ISR wurde nicht angesprungen, ERROR
;	    ZF=0: ISR wurde angesprungen, Test o.k.

CIPR:	LD	A,87H		;EI, Timer, 1/16, no CLK, TC folgt, Kanal-Reset 
	OUT	(C),A
	LD	B,10		;Zeitkonstante TC=10 (ti=10*16*250ns= 40960 ns)
	LD	HL,NULBYT	;Inhalt von HL ist 0
	LD	A,0FFH		;fuer ISR mit Wert ungleich 0 laden
	OUT	(C),B		;TC laden, Kanal START (INT nach ca. 0.04ms)
	CALL	ZEIT1		;ca. 1 ms warten
	LD	A,(HL)		;in der ISR wird HL veraendert
	AND	A
	RET

; Programmierung des SIO fuer Zeichen senden (mit Interrupt).
; Portadresse steht in C, Warten auf den Interrupt, nach bestimmter
; Zeit wird abgebrochen.
; In der ISR wird HL entsprechend ausgewaehltem SIO-Kanal veraendert.

SI0APR
	XOR	A
	OUT	(C),A
	CALL	ZEIT
	CALL	ZEIT
	CALL	SIORES
	LD	A,3
	OUT	(CTC1),A
	LD	A,(MIS0KA)
	AND	A
	RET
 
SI0BPR
	XOR	A
	OUT	(C),A		;Datenbyte 00 senden
	CALL	ZEIT		;auf INT warten
	CALL	ZEIT
	INC	C		;Out-Adr. auf control-port stellen
	LD	A,R1+COMM2	;WR1 adressieren
	OUT	(C),A
	XOR	A		;Kanal DI
	OUT	(C),A
	LD	A,38H		;RETI an SIO 0
	OUT	(S0AC),A
	DEC	C		;Datenadr. einstellen
	LD	A,(MIS0KB)	;Flagbyte fuer INT holen
	AND	A		;if A=0 then ZF=1 else ZF=0
	RET
 
SI1APR
	XOR	A
	OUT	(C),A
	CALL	ZEIT
	CALL	ZEIT
	CALL	SIORES
	LD	A,3
	OUT	(CTC1+2),A
	LD	A,(MIS1KA)
	AND	A
	RET
 
SI1BPR
	XOR	A
	OUT	(C),A
	CALL	ZEIT
	CALL	ZEIT
	CALL	SIORES
	LD	A,3
	OUT	(CTC0),A
	LD	A,(MIS1KA)
	AND	A
	RET
 
	
SIORES
	INC	C
	LD	A,11H
	OUT	(C),A
	XOR	A
	OUT	(C),A
	LD	A,30H
	OUT	(C),A
	LD	A,38H
	OUT	(C),A
	LD	A,18H
	OUT	(C),A
	DEC	C
	RET
 

; INT-System-Initialisierung
;	- I-Register laden
;	- INT-Vektor-Tabelle bereitstellen
;	- INT-Merker initialisieren

INTINI	DI
	IM	2

	LD	HL,IVTAB 	;INT.VEC.TAB. Quell-Adresse
	LD	DE,_end		;HIGH-ADRESS SA.DIAGS laden	
	INC	D		;INT.VEC.TAB. Ziel-Adresse berechnen
	LD	E,0		;modulo 256
	LD	A,D
	LD	I,A		;I-Reg. laden
	LD	BC,42H		;Anzahl der Werte
	LDIR			;INT-Vektor-Tabelle umladen

	XOR	A
	LD	HL,M1CISR	;Merker fuer ISR loeschen
Z:	def	MAWE-M1CISR
	LD	B,Z		;Anzahl der Merkzellen
INTIN1	LD	(HL),A
	INC	HL
	DJNZ	INTIN1

	EI

	RET

; Interruptserviceroutinen der CTC-Kanaele

ISC0K0	LD	HL,MIC0K0
ICRET	LD	(HL),A		;INT-Flag setzen (A>0)
	LD	A,3		;CTC-Kanal DI, STOP
	OUT	(C),A
	EI
	RETI

ISC0K1	LD	HL,MIC0K1
	JR	ICRET

ISC0K2	LD	HL,MIC0K2
	JR	ICRET

ISC0K3	LD	HL,MIC0K3
	JR	ICRET

ISC1K0	LD	HL,MIC1K0
	JR	ICRET

ISC1K2	LD	HL,MIC1K2
	JR	ICRET

ISC1K3	LD	HL,MIC1K3
	JR	ICRET


; Interruptserviceroutinen der SIO-Kanaele

ISS0KA
	PUSH	AF
	LD	A,0FFH
	LD	(MIS0KA),A
ISRET
	INC	C
	LD	A,28H 
	OUT	(C),A
	DEC	C
	POP	AF
	EI
	RETI
 
ISS0KB
	PUSH	AF
	LD	A,0FFH
	LD	(MIS0KB),A
	JR	ISRET
 
ISS1KA
	PUSH	AF
	LD	A,0FFH
	LD	(MIS1KA),A
	JR	ISRET
 
ISS1KB
	PUSH	AF
	LD	A,0FFH
	LD	(MIS1KB),A
	JR	ISRET
 

; Interruptserviceroutine DMA

ISDMA	push	af
	LD	A,0A3H
	OUT	(DMAADR),A	;alle INT reset 
	ld	a,0ffh
	ld	(MISDMA),a	;INT-MERKER setzen
	pop	af
	EI
	RETI

; Zeitschleifen

ZEIT	LD	HL,0FFFFH	;Zeitkonstante fuer ca. 250ms
DECZL	DEC	L
	JR	NZ,DECZL
	DEC	H
	JR	NZ,DECZL
	RET

ZEIT1	LD	D,0FFH		;Zeitkonstante fuer ca. 1ms
DECDL	DEC	D
	JR	NZ,DECDL
	RET

ZEIT2	LD	D,6		;Zeitkonstante fuer ca. 0.025ms
DECD1	DEC	D
	JR	NZ,DECD1
	RET
 
; Unterprogramm SIO-Programmierung
 
SINIT
	LD	C,B
	LD	A,57H
	OUT	(C),A
	LD	A,2
	OUT	(C),A
	LD	HL,INITAB
	LD	B,8
	LD	C,E
	OTIR
	RET
 
; Unterprogramm fuer die Fehlerausgabe an das Terminal fuer drei, zwei
; oder einen Wert entsprechend in MAWE eingetragener Anzahl
; Uebergabewerte sind: Register B enthaelt den Testwert
;                      Register C enthaelt die Portadresse
;                      Akkumulator enthaelt den gelesenen Wert

FA3RET	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL
	call	ETROLL		;errortable roll down
	LD	D,A		;rueckgelesenen Wert in <D> speichern
	CALL	AUSERR		;Ausschrift Error, Nummer entsprechend E
	ld	(iy+1),0	;Eintrag in Fehlertabelle: Adr.-High
	ld	(iy+2),c	;Eintrag in fehlertabelle: Adr.-Low
	LD	A,C		;Portadresse ist erste Ausgabe
AUS2WE	CALL	HEXCON		;Umwandlung in Hexadezimalzahl
	OUT	(S0BD),A	;Ausgabe an das Terminal
	CALL	SIOREA		;Ist SIO fertig?
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	LD	A,(MAWE)	;Anzahl der auszugebenden Werte dekrementieren
	DEC	A
	JR	Z,FEAEND	;bei 0, Sprung an das Ende der Fehlerausgabe
	ld	(iy+3),b	;Eintrag in Fehlertabelle: Sollwert
	LD	C,B		;zu lesenden Wert in <C> speichern
	CALL	AUSSP		;Ausgabe von Space
	LD	A,C
	CALL	HEXCON
	OUT	(S0BD),A	;Ausgabe zu lesender Wert
	CALL	SIOREA
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	CALL	AUSSP
	LD	A,D		;Ruecklesen gelesener Wert
	ld	(iy+4),a	;Eintrag in Fehlertabelle: Istwert
	CALL	HEXCON
	OUT	(S0BD),A	;Ausgabe gelesener Wert
	CALL	SIOREA
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
FEAEND	LD	HL,ENDZEI
	LD	B,ANZEZ
NAEZEI	LD 	A,(HL)
	OUT	(S0BD),A	;Ausgaben am Ende einer Zeile
WEZEI	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WEZEI
	INC	HL
	CALL	ZEIT1
	DJNZ	NAEZEI
	POP	HL
	POP	DE
	POP	BC
	POP	AF
	RET

; Unterprogramm fuer die Ausgabe ERROR und der Fehlernummer

AUSERR	EXX
	LD	HL,ADRERR	
	LD	C,S0BD
	LD	B,ANAUSE	;Anzahl der Werte

WAUSE	LD	A,(HL)
	OUT	(C),A
	CALL	SIOREA
	INC	HL
	DJNZ	WAUSE

	ld	(iy+0),e	;Eintrag in Felertabelle: Fehl.-Nr.
	LD	A,E
	CALL	HEXCON
	OUT	(S0BD),A
	CALL	SIOREA
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	CALL	AUSSP
	CALL	AUSSP
	CALL	AUSSP
	LD	H,0FFH
	EXX
	RET

; Unterprogramm fuer die Ausgabe von HL

FAHLR	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL
	call	ETROLL		;error-code-table roll down
 	LD	D,A		;rueckgelesenen Wert retten
	CALL	AUSERR
	ld	(iy+1),h	;Eintrag in Fehlertabelle: Adr.-High
	LD	A,H
	CALL	HEXCON
	OUT	(S0BD),A
	CALL	SIOREA
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	ld	(iy+2),l	;Eintrag in Fehlertabelle: Adr.-Low
	LD	A,L
	JP	AUS2WE

; UP zur Forbereitung eines Eintrags in die Fehlercodetabelle
; Es koennen von den aufgetretenden Fehlern die letzten 200 Fehler
; gespeichert werden. Ein Fehlereintrag belegt 5 Byte.

ETROLL:	push	bc
	push	de
	push	hl

	ld	hl,ertab+994
	ld	de,ertab+999
	ld	bc,995
	lddr			;Fehlertabelle 5Byte weiter rollen
				;der aelteste Eintrag wird ueberrollt
	ld	hl,ertab
	ld	de,ertab+1
	ld	bc,4
	ld	(hl),0
	ldir			;letzten Eintrag loeschen

	ld	hl,(errcnt)
	inc	hl		;Fehlerzaehler inc
	ld	(errcnt),hl

	ld	iy,ertab

	pop	hl
	pop	de
	pop	bc

	ret

; Unterprogramm fuer die Abfrage der SIO-Fertigmeldung

SIOREA	IN	A,(S0BC)
	BIT	2,A
	JR	Z,SIOREA
	RET

; Unterprogramm fuer die Konvertierung eines Bytes in zwei ASCII-Zeichen

HEXCON	LD	C,A
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ORUP0
	ADD	A,7
ORUP0	ADD	A,30H
	PUSH	AF
	LD	A,C
	AND	0FH
	CP	0AH
 	JR	C,ORUP
	ADD	A,7
ORUP	ADD	A,30H
	LD	C,A
	POP	AF
	RET

; Unterprogramm fuer die Ausgabe von SPACE

AUSSP	LD	A,20H
AUSSP1	OUT	(S0BD),A
	CALL	SIOREA
	RET

; Unterprogramm fuer die Abfrage der Fertigmeldung des FDC

FDCREE:	ld	b,0c0h		;RQM=1 , DIO=1 FDC bereit zum lesezugriff
	jr	FDCRE

FDCREA:	LD	B,80H		;RQM=1 , DIO=0 FDC bereit zum schreibzugr.
FDCRE	ld	c,MSRFDC
	ld	HL,0		;Initialisierung einer Zeit
FDCR1	IN	A,(C)		;Einlesen Hauptstatusregister
	ld	d,a		;Wert in D retten
	AND	0C0H
	CP	B
	JR	Z,TW1FDC	;wenn FDC bereit, JP
	dec	hl
	ld	a,h
	or	l
	jr	nz,FDCR1	;Wenn Zeit noch nicht um, JP	
	ld	a,d		;zuletzt eingelesenen Wert holen (Istwert)
	CALL	FA3RET		;Fehlerausgabe FDC not ready
	inc	l		;ZF:= 0

TW1FDC	RET

; UP: RESET-Impuls an FDC

FDCRES:	LD	A,8FH
	OUT	(P2BD),A	;RESET an den FDC
	call	ZEIT2		;ca. 0.025 ms warten
	ld	a,0fh
	out	(P2BD),a	;RESET inactiv
	call	ZEIT2		;ca. 0.025 ms warten
	ret

	eject

; Vereinbarungen

; Portadressen

CTC0	equ	8
C0K0	equ	CTC0
C0K1	equ	CTC0+1
C0K2	equ	CTC0+2
C0K3	equ	CTC0+3

CTC1	equ	02CH
C1K0	equ	CTC1
C1K1	equ	CTC1+1
C1K2	equ	CTC1+2
C1K3	equ	CTC1+3

SIO0	equ	024H
S0AD	equ	SIO0
S0AC	equ	SIO0+1
S0BD	equ	SIO0+2
S0BC	equ	SIO0+3

SIO1	equ	028H
S1AD	equ	SIO1
S1AC	equ	SIO1+1
S1BD	equ	SIO1+2
S1BC	equ	SIO1+3

PIO0	equ	0CH
P0AD	equ	PIO0
P0AC	equ	PIO0+1
P0BD	equ	PIO0+2
P0BC	equ	PIO0+3

PIO1	equ	018H
P1AD	equ	PIO1
P1AC	equ	PIO1+1
P1BD	equ	PIO1+2
P1BC	equ	PIO1+3

PIO2	equ	1CH
P2AD	equ	PIO2
P2AC	equ	PIO2+1
P2BD	equ	PIO2+2
P2BC	equ	PIO2+3

DMAADR	equ	3CH

FDC	equ	20H
MSRFDC	equ	FDC
DFDC	equ	FDC+1

	begin	1

; Vereinbarungen fuer die SIO-Programmierung

R0	equ	0
R1	equ	1
R2	equ	2
R3	equ	3
R4	equ	4
R5	equ	5
R6	equ	6
R7	equ	7
;
COMM6	equ	30H		; FEHLER RUECKSETZEN
COMM3	equ	18H		; KANAL RUECKSETZEN
COMM2	equ	10H		; RUECKSETZEN EXT./STAT. INTERRUPT
NOPRTY	equ	0		; PARITAETSPRUEFUNG AUSSCHALTEN
S2	equ	0CH		; 2 STOP/BITS JE ZEICHEN
C16	equ	40H		; *16
RENABLE	equ	1
B8	equ	0C0H
XENABLE	equ	8
T8	equ	60H
PDAVCT	equ	18H
SAVECT	equ	04H
XMTRIE	equ	02H
C32	equ	80H

; Initialisierungstabelle fuer den SIO

INITAB	db	COMM6		;Fehler ruecksetzen
	db	COMM3		;Kanal-Reset
	db	R4+COMM2
	db	NOPRTY+S2+C32
	db	R3+COMM2
	db	RENABLE+B8
	db	R5+COMM2
	db	XENABLE+T8

	db	R2+COMM2
	db	20h		;INT.VEC.
	db	R1+COMM2
	db	1CH		;INT.MOD 3 / STATUS AFFECTS VECTOR / TR-DI


; DMA - programmierung

; Definition der Schreibregister WR0...WR6

WR0	equ	0
WR1	equ	4	;DMA-Port A
WR2	equ	0	;DMA-Port B
WR3	equ	80h
WR4	equ	81h
WR5	equ	82h
WR6	equ	83h

; Initialisierungstabelle fuer den DMA ohne INT

DMAT	db	   0C3H		;DMA-reset

	db	WR0+7DH		;Transfer A--->B / A-Adr. u. Laenge folgen
	dw	  0000H		;Anfangsadresse DMA-Port A
	dw	  00FFH		;Datenlaenge-1
	db	   0CFH		;LOAD Adresse A
	db	WR1+10H		;Port A: MEMRQ / INC

	db	WR4+2CH		;Blocktransfer / B-Adr. folgt
	dw	  3000H		;Anfangsadresse DMA-Port B
	db	   0CFH		;LOAD Adresse B
	db	WR2+10H		;Port B: MEMRQ / INC

	db	WR5+00H		;Stop bei Blockende / nur/CE / RDY low activ
	db	   0BBH		;Lesemaske folgt
	db	     7		;Lesemaske fuer STATUSBYTE / Bytezaehler l/h
	db	    87H		;DMA-START
DMATE

; Initialisierungstabelle fuer den DMA mit INT.

DMATI	db	   0C3H		;DMA-reset

	db	WR0+7DH		;Transfer A--->B / A-Adr. u. Laenge folgen
	dw	  0000H		;Anfangsadresse DMA-Port A
	dw	  00FFH		;Datenlaenge-1
	db	   0CFH		;LOAD Adresse A
	db	WR1+10H		;Port A: MEMRQ / INC

	db	WR4+3CH		;Blocktransfer / B-Adr. folgt / INTCW folgt
	dw	  3000H		;Anfangsadresse DMA-Port B
	db	    12H		;INTCW: INT.VEC. folgt / INT nach Blockende
	db	    1CH		;INT.VEC. DMA (NO STATUS AFFECTS VECTOR)
	db	   0CFH		;LOAD Adresse B
	db	WR2+10H		;Port B: MEMRQ / INC

	db	WR5+00H		;Stop bei Blockende / nur/CE / RDY low activ
	db	   0ABH		;DMA-EI
	db	   0BFH		;READ STATUSBYTE
	db	    87H		;DMA-START
DMATIE


; Initialisierungswerte fuer die ISR-Tabelle

IVRSYS	equ	0FH		;I-Register UDOS-SYSTEM

IVTAB	dw	ISC0K0		;IVTAB+00h
	dw	ISC0K1
	dw	ISC0K2
	dw	ISC0K3
	
	dw	ISC1K0		;IVTAB+08h
	dw	0
	dw	ISC1K2
	dw	ISC1K3
	
	dw	0		;IVTAB+10h
	dw	0

	dw	0
	dw	0

	dw	0		;IVTAB+18h
	dw	0

	dw	ISDMA		;IVTAB+1Ch
	dw	0

	dw	ISS0KB		;IVTAB+20h
	dw	0
	dw	0
	dw	0

	dw	ISS0KA		;IVTAB+28h
	dw	0
	dw	0
	dw	0

	dw	ISS1KB		;IVTAB+30h
	dw	0
	dw	0
	dw	0

	dw	ISS1KA		;IVTAB+38h
	dw	0
	dw	0
	dw	0

	dw	ISDMA		;IVTAB+40h

; Vereinbarungen fuer die Merkzellen

NULBYT	db	0
M1CISR:
MIC0K0	db	0
MIC0K1	db	0
MIC0K2	db	0
MIC0K3	db	0
MIC1K0	db	0
MIC1K2	db	0
MIC1K3	db	0
MIP0KA	db	0
MIP0KB	db	0
MIP1KB	db	0
MIP2KA	db	0
MIP2KB	db	0
MIS0KA	db	0
MIS0KB	db	0
MIS1KA	db	0
MIS1KB	db	0
MISDMA	db	0

MAWE	db	0		;Merker fuer Anzahl der Auszugebenen Werte
errcnt	dw	0		;Fehlerzaehler

; Ausschrift bei Fehler

ADRERR	text	'*** ERROR '
ANAUSE	equ	10

; Ausgaben am Ende der Zeile

ENDZEI	db	0AH
	db	0DH
ANZEZ	equ	2

; Adressen fuer MEMORY-BANK-SELECT-LOGIK

WEADP	equ	0		;out-adress
RFF	equ	4		;Speicherbank aktivieren


; PROM-CRC Kontrollspeicheradressen

CRCEP0	equ	1FF8H
NCEP0	equ	1FFAH
CRCEP1	equ	1FFCH
NCEP1	equ	1FFEH

RAMANF	equ	8000h		;ANFANG DRAM-TEST
RAMLEN	equ	8000h		;laenge des zu testenden DRAM-Bereichs

	end
E 1
