*H P8000 U880-FIRMWARE  3.0
*P 84
;******************************************************************************
;*                                                                            *
;*                                                                            *
;*                  	P8000  -  SOFTWARE                                    *
;*                                                                            *
;*                     Entwicklerdokumentation                                *
;*                                                                            *
;*                      Betriebssystem  UDOS                                  *
;*                                                                            *
;*	Programm:      U880EIG                                                *
;*                                                                            *
;*      Version:       3.1                                                    *
;*                                                                            *
;*	Bearbeiter:    M. Weber                                               *
;*                                                                            *
;*	Datum:         18.01.1989                                             *
;*                                                                            *
;*                                                                            *
;******************************************************************************
;
;P8000 U880-Eigentest
;
;
 
	JR	START
	DEFM 	'(C)KEAW_01/89'
;
; Es wird die Funktionsfaehigkeit von PIO2, Kanal A vorausgesetzt.
; Es gilt:  Bit 7 = 1 ---> Spannungs-Reset
;           Bit 7 = 0 ---> Bediener-Reset, es wird kein Eigentest
;       		   durchgefuehrt
;
START
 
	LD	B,0
DECB1	LD	C,0
DECB2	DEC	C
	JR	NZ,DECB2
	DEC	B
	JR	NZ,DECB1
 
 
	LD	A,0CFH	   	;Bit-Mode PIO2, Kanal A
	OUT  	(P2AC),A
	LD     	A,80H	     	;nur Bit 7 ist Eingabebit
	OUT	(P2AC),A
	LD	A,3
	OUT	(P2AC),A
	IN	A,(P2AD)
	BIT	7,A
	JP	Z,INSM1		;kein Spannungs-Reset
 
; Initialisierung der Speicherbanksteuerung
 
ISPBST
	XOR	A
	LD	I,A
	LD	C,WEADP
	LD	DE,10H
	XOR	A		;Anfangsadresse
AUSA    LD      B,A             ;Adressbildung
	OUT     (C),D
	ADD	A,10H		;naechste Adresse wird berechnet 
	DEC	E
	JR	NZ,AUSA
	LD	D,1		;PROM_SEL aktiv
	LD	B,0		;Adresse gleich 0
	OUT	(C),D
	IN	A,(RFF)		;Aktivierung des Adressports
	LD	D,1
	LD	B,10H
	OUT	(C),D
	JR	START1
 
ENSMON
	LD	H,0
	LD	C,0
	EXX
	LD	A,0FH
	OUT	(P1AC),A
	LD	A,3
	OUT	(P1AC),A
 
	LD	C,S0BD
	LD	B,43
	LD	HL,AHWT1
	JP	ATEST
 
 
; Beginn des Eigentest des P8000 8-Bit-Teils
  

; Test des PIO1, Kanal A durch Ausgabe und Ruecklesen.
; Bei Fehler werden Fehlerkode 10H, Adresse 18H, ausgegebener und
; rueckgelesener Wert gemerkt.
; Benutzte Register: A, B, C, H, L
;
START1	JR	WENMI 
 
; Hier Einsprung fuer NMI fuer Hardwaretestung !
 
	JP	4000H
WENMI    
	LD	A,0FH
	OUT	(P1AC),A  	;Ausgabemode PIO1, Kanal A 	
	LD	A,3
	OUT	(P1AC),A
	LD	A,0AAH
	OUT	(P1AD),A
	LD	C,0		;Initialisierung fuer Register C
	IN	A,(P2AD)	;Veraenderung auf dem Datenbus
	IN	A,(P1AD)
	CP	0AAH		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	Z,WTP1A		;Ja, kein Fehler.
;
	LD	H,A		;rueckgelesenen Wert merken
	LD	C,1		;Fehler beim ersten Test
WTP1A
	LD	A,55H
	OUT	(P1AD),A
	IN	A,(P2AD)	;Veraenderung auf dem Datenbus
	IN	A,(P1AD)
	CP	055H		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	Z,WTC11		;Ja, kein Fehler.
;
	LD	L,A		;rueckgelesenen Wert merken
	SET	1,C		;Fehler beim zweiten Test
;
; Es wird der CTC1, Kanal 1 getestet.
; Bei Fehler werden Fehlerkode 15H oder 16H, Portadresse 2DH sowie
; ausgegebener und rueckgelesener Wert gemerkt.
; Benutzte Register: A, C, D, E
;
WTC11
	LD	A,0FFH
	OUT	(P1AD),A	;Start: Ausgabe von 0FFH
;
	LD	A,7
	OUT	(CTC1+1),A
	LD	A,0AAH		;Zeitkonstante ist 0AAH
	OUT	(CTC1+1),A
	LD	D,5
ZC1	DEC	D
	JR	NZ,ZC1
	IN	A,(CTC1+1)
	CP	0AAH		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	C,NTC11		;Nein, kleiner.
;
	LD	D,A		;rueckgelesenen Wert merken
	SET	2,C		;Fehler beim dritten Test
;
NTC11
	LD	A,3
	OUT	(CTC1+1),A	;CTC anhalten
	IN	A,(CTC1+1)
	CP	0AAH		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	Z,WTS0B		;Ja, kein Fehler.
;
	LD	E,A		;rueckgelesenen Wert merken
	SET	3,C		;Fehler beim vierten Test
;
; Test des SIO0 durch Programmierung und Ruecklesen des Interruptvektors (nur
; in Kanal B moeglich).
; Bei Fehler werden Fehlerkode 20H, Adresse 26H, ausgegebener und rueckgelese-
; ner Wert gemerkt.
; Benutzte Register: AF', BC', DE', HL'
;
WTS0B
	LD	A,38H
	OUT	(S0BC),A
	LD	A,10H
	OUT	(S0BC),A
	LD	A,11H
	OUT	(S0BC),A
	XOR	A
	OUT	(S0BC),A
 
 
	LD	A,12H		;Schreibregister 2 ansprechen
	OUT	(S0BC),A
	LD	A,50H		;Wert 50H eintragen
	OUT	(S0BC),A
Daten
				;auf dem Datenbus
	LD	A,12H		;Leseregister 2 ansprechen
	OUT	(S0BC),A
	IN	A,(S0BC),A
	CP	50H		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	Z,NTS0B		;Ja, kein Fehler.
;
	LD	I,A
	SET	4,C		;Fehler beim fuenften Test
NTS0B
	LD	A,12H		;Leseregister 2 ansprechen
	OUT	(S0BC),A
	LD	A,20H
	OUT	(S0BC),A
	LD	A,12H
	OUT	(S0BC),A
	IN	A,(S0BC)	;Einlesen des Interruptvektors
	CP	20H		;Ist der Wert gleich dem ausgegebenen Wert?
	JR	Z,SPF		;Ja, kein Fehler, restliche Fehler speichern.
;
	LD	B,A		;rueckgelesenen Wert merken
	SET	5,C		;Fehler beim sechsten Test
SPF
	EXX			;Werte in den Zweitregistersatz
;
; Initialisierung von CTC1 Kanal 1 und SIO0 Kanal B fuer die Ausgabe der
; Eigentestmeldungen.
; PIO1 Kanal A ist bereits initialisiert.
;
 	LD	A,57H		;Zeitgeber-Mode mit externem Takt
	OUT	(CTC1+1),A
	LD	A,2		;Baudrate von 9600 Bd
	OUT	(CTC1+1),A
	LD	HL,INITAB	;Initialisierungstabelle fuer den SIO
	LD	B,8
	LD	C,S0BC
	OTIR			;acht Werte werden ausgegeben
	LD	C,S0BD
	LD	B,ANZHWT
	LD	HL,AUSHWT
ATEST	OUTI			;Ausgabe der Anfangsaussschrift fuer den Hardwareeigentest
	INC	B
P1WAIT	IN	A,(S0BC)
	BIT	2,A		;Ist der SIO fertig?
	JR	Z,P1WAIT	;Nein, warten.
	LD	C,2
ZAUS11	LD	D,0
ZAUS1	DEC	D
 	JR	NZ,ZAUS1	;Realisierung einer Wartezeit
	DEC	C
	JR	NZ,ZAUS11
	LD	C,S0BD
	DJNZ	ATEST		;alle Zeichen, bis Anzahl gleich 0
;
; Ausgabe eventuell beim Test von CTC1 Kanal 1, SIO0 Kanal B und PIO1 Kanal A
; entstandener und gemerkter Fehler.
;
	EXX			;holen der Werte aus dem Zweitregistersatz
	LD	A,C
	AND	A		;Ist ueberhaupt ein Fehler aufgetreten?
	JR	NZ,AFKP		;Ja, Ausgabe der Fehler.
;
; Ausgabe der Eigentestnummern der durchgefuehrten Tests
;
	LD	HL,TENRAN
	LD	B,ANZANF
NZANF	LD	A,(HL)
	OUT	(S0BD),A
ANFWRS	IN	A,(S0BC)
	BIT	2,A
	JR	Z,ANFWRS
	INC	HL
	LD	D,0
TD1	DEC	D
	JR	NZ,TD1
 	DJNZ	NZANF
	EXX
	LD	H,0		;Register H mit 0 laden, da kein Fehler
	EXX
	LD	A,10H		;Fehlerkode PIO Test
	OUT	(P1AD),A
	LD	A,15H		;Fehlerkode CTC Test
	OUT	(P1AD),A
	LD	A,16H		;zweiter Fehlerkode CTC-Test
	OUT	(P1AD),A
	LD	A,20H		;Fehlerkode SIO Test
	OUT	(P1AD),A
;
	JP	W1EIG		;weiter im Eigentest
;
AFKP	EXX
	LD	E,10H		;Fehlerkode laden
	LD	IX,$+7
	JP	AUSTNR
	EXX
	BIT	0,C		;Fehler beim Test PIO1 Kanal A?
 	JR	Z,NEAUS
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS1	;Anzahl erster Fehler
	LD	HL,AUSF1	;Ausschrift erster Fehler
A1TE	OUTI
	INC	B
P2W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P2W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS2	DEC	D
	JR	NZ,ZAUS2
	DJNZ	A1TE
	EXX			;gespeicherte Werte holen
	LD	A,H
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD2
	ADD	A,7
ADD2	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P3W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P3W
	LD	A,H		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD3
	ADD	A,7
ADD3	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P4W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P4W
 	LD	H,0FFH		;in <H> Fehler merken
	EXX
;
NEAUS	BIT	1,C		;Fehler beim zweiten Test PIO1 Kanal A?
	JR	Z,NEAUS2
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS2	;Anzahl zweiter Fehler
	LD	HL,AUSF2	;Ausschrift zweiter Fehler
A2TE	OUTI
	INC	B
P5W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P5W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS3	DEC	D
	JR	NZ,ZAUS3
	DJNZ	A2TE
	EXX			;gespeicherte Werte holen
	LD	A,L
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD4
	ADD	A,7
ADD4	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P6W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P6W
	LD	A,L		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD5
	ADD	A,7
ADD5	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P7W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P7W
	LD	H,0FFH		;in <H> Fehler merken
	EXX

NEAUS2	EXX
	LD	E,15H		Nein, Fehlerkode CTC Test ausgeben.
	LD	IX,$+7
	JP	AUSTNR
	EXX
	BIT 	2,C		;Fehler beim Test CTC1 Kanal 1?
        JR	Z,NEAUS3
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS3	;Anzahl dritter Fehler
	LD	HL,AUSF3	;Ausschrift dritter Fehler
A3TE	OUTI
	INC	B
P8W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P8W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS4	DEC	D
	JR	NZ,ZAUS4
	DJNZ	A3TE
	EXX			;gespeicherte Werte holen
	LD	A,D
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD6
	ADD	A,7
ADD6	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P9W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P9W
	LD	A,D		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD7
	ADD	A,7
ADD7	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P10W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P10W
	LD	H,0FFH		;in <H> Fehler merken
	EXX
;
NEAUS3	EXX
	LD 	E,16H
	LD	IX,$+7
	JP	AUSTNR
	EXX
	BIT	3,C		;Fehler beim Test CTC1 Kanal 1?
	JR	Z,NEAUS4
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS4	;Anzahl vierter Fehler
	LD	HL,AUSF4	;Ausschrift vierter Fehler
A4TE	OUTI
	INC	B
P11W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P11W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS5	DEC	D
	JR	NZ,ZAUS5
	DJNZ	A4TE
	EXX			;gespeicherte Werte holen
	LD	A,E
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD8
	ADD	A,7
ADD8	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P12W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P12W
	LD	A,E		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD9
	ADD	A,7
ADD9	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P13W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P13W
	LD	H,0FFH		;in <H> Fehler merken
	EXX

NEAUS4	EXX
	LD	E,20H		;Fehlerkode fuer SIO-Test ausgeben.
	LD	IX,$+7
	JP	AUSTNR
	EXX
NEA4	BIT	4,C		;Fehler beim Test SIO0 Kanal A?
	JR	Z,NEAUS5
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS5	;Anzahl fuenfter Fehler
	LD	HL,AUSF5	;Ausschrift fuenfter Fehler
A5TE	OUTI
	INC	B
P14W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P14W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS6	DEC	D
	JR	NZ,ZAUS6
	DJNZ	A5TE
	EXX			;gespeicherte Werte holen
	LD	A,I
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD10
	ADD	A,7
ADD10	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P15W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P15W
	LD	A,I		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD11
	ADD	A,7
ADD11	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P16W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P16W
	LD	H,0FFH		;in <H> Fehler merken
	EXX
;
NEAUS5	BIT	5,C		;Fehler beim Test SIO0 Kanal A?
	JR	Z,W1EIG
;
	EXX			;Ja! Speichern der Werte im Zweitregistersatz
	LD	C,S0BD		;Daten SIO0 Kanal B
	LD	B,ANAUS6	;Anzahl sechster Fehler
	LD	HL,AUSF6	;Ausschrift sechster Fehler
A6TE	OUTI
	INC	B
P17W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P17W
	LD	D,0FFH		;Ja, Zeitschleife
ZAUS7	DEC	D
	JR	NZ,ZAUS7
	DJNZ	A6TE
	EXX			;gespeicherte Werte holen
	LD	A,B
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,ADD12
	ADD	A,7
ADD12	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P18W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P18W
	LD	A,B		;Ja, naechstes Halbbyte
	AND	0FH		;naechstes Halbbyte
	CP	0AH
	JR	C,ADD13
	ADD	A,7
ADD13	ADD	A,30H		;ASCII-Zeichen fertig
	OUT	(S0BD),A
P19W	IN	A,(S0BC)
	BIT	2,A		;Sendepuffer leer?
	JR	Z,P19W
	LD	H,0FFH		;in <H> Fehler merken
	EXX
;
; Nach den Ausgaben evtl. aufgetretener Fehler wird der Eigentest fortgesetzt.
; Das Register H wird in den Zweitregistersatz gerettet.
;
W1EIG	EXX
	LD	E,0
	LD	IX,TEPROM
	JP	AUSTNR
TEPROM	EXX
 
 
; Programm Test EPROM
; Fuer den EPROM wird ausser den letzten zwei Bytes die Pruefsumme gebildet und
; mit dem in den letzten zwei Bytes stehenden Wert verglichen.
; Bei Fehler wird der Fehlerkode 00 an PIO und Terminal ausgegeben.
; Benutzte Register:
;
 
; BERECHNUNG DES CRC VOM PROM NACH DEM SDLC POLYNOM
 
 
; Berechnung des CRC fuer EPROM0
 
CRCANF
	LD	IX,TENEP
	LD	HL,0
	LD	BC,1000H
	ld de,-1
crc1:	ld a,(hl)
	xor d
	ld d,a
	rrca
	rrca
	rrca
	rrca
	and 0fh
	xor d
	ld d,a
	rrca
	rrca
	rrca
	LD	I,A
	and 1fh
	xor e
	ld e,a
	LD	A,I
	LD	I,A
	rrca
	and 0f0h
	xor e
	ld e,a
	LD	A,I
	and 0e0h
	xor d
	ld d,e
	ld e,a
	inc hl
	dec bc
	ld a,b
	or c
	jr nz,crc1
	LD	HL,(CRCEP0)
	LD	A,D
	CP	H
	JP	NZ,FAUSA3
	LD	A,E
	CP	L
	JP	NZ,FAUSA3
	LD	HL,(NCEP0)
	ADD	HL,DE
	LD	A,0FFH
	CP	H
	JP	NZ,FAUSA3
	CP	L
	JP	NZ,FAUSA3
 
; Berechnung und Test des CRC fuer EPROM1
 
TENEP	LD	IX,TSRAM
	LD	HL,1000H
	LD	BC,0FF8H
	ld de,-1
crc10:	ld a,(hl)
	xor d
	ld d,a
	rrca
	rrca
	rrca
	rrca
	and 0fh
	xor d
	ld d,a
	rrca
	rrca
	rrca
	LD	I,A
	and 1fh
	xor e
	ld e,a
	LD	A,I
	LD	I,A
	rrca
	and 0f0h
	xor e
	ld e,a
	LD	A,I
	and 0e0h
	xor d
	ld d,e
	ld e,a
	inc hl
	dec bc
	ld a,b
	or c
	jr nz,crc10
	LD	HL,(CRCEP1)
	LD	A,D
	CP	H
	JP	NZ,FAUSA3
	LD	A,E
	CP	L
	JP	NZ,FAUSA3
	LD	HL,(NCEP1)
	ADD	HL,DE
	LD	A,0FFH
	CP	H
	JP	NZ,FAUSA3
	CP	L
	JP	NZ,FAUSA3
 
	
 
;
; Programm Test SRAM
; Es werden ein Test der Adressleitungen, der Datenleitungen, der  Datenhaltigkeit
; des Speichers durch das Durchschieben von 0 oder 1 und ein Programmtest des Speichers durchgefuehrt.
; Bei auftretenden Fehlern wird der entsprechende Fehlerkode an das Terminal und, wenn vorher kein
; Fehler aufgetreten war, an den PIO Port ausgegeben. Am Terminal werden ausserdem die Speicheradresse,
; der ausgegebene und der rueckgelesene Wert angezeigt.
; Benutzte Register:
;
TSRAM	EXX			;holen Zweitregistersatz
	LD	E,5
	LD	IX,WTSR1
	JP	AUSTNR
WTSR1	EXX
 
; Initialisierung des SRAM auf die Adresse 2000H
;
	LD	D,2		;SRAM_SEL aktiv
	LD	B,20H		;Adresse ist 2000H
	LD	C,WEADP
	OUT	(C),D		;Einschalten des SRAM auf die Adresse 7000H
;
; Test der Datenleitungen des SRAM
;
	XOR	A		;alle Bits auf 0 setzen
TAUFD	LD	HL,2000H	;Anfangsadresse ist 2000H
	LD	DE,2001H
	LD	BC,7FFH		;Datenlaenge ist 2k-1
	LD	(HL),A		;0 oder 0FFH auf den ersten Speicherplatz
	LDIR			;Wert wird in alle Speicherzellen des SRAM eingetragen
	CP	(HL)		;Letztes Byte gleich dem Akkumulator?
	JP	Z,DTSR1
	LD	B,A		;Testwert eintragen
	LD	A,(HL)
	LD	IX,WT2SR
	JR	FAUSA3

; Ausgabe der Testschrittnummer an den SIO0, Kanal B (Terminal)
; An den PIO erfolgt die Ausgabe nur, wenn vorher kein Fehler auftrat.
; Programm arbeitet mit dem Zweitregistersatz, im Register E steht die
; auszugebende Testschrittnummer.

AUSTNR	LD	B,2
BB0	LD	A,8		;Ausgabe von 2 Backspace
	OUT	(S0BD),A
WSBSR	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSBSR
	LD	D,0
Z1AUS	DEC	D
	JR	NZ,Z1AUS
	DJNZ	BB0
	LD	A,E
	LD	IY,$+7
	JP	HCAHL
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	D,0
Z2AUS	DEC	D
	JR	NZ,Z2AUS
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,E
	BIT	0,H
	JR	NZ,VORFE
	OUT	(P1AD),A
VORFE	LD	BC,0
ZAUSN2	DEC	C
	JR	NZ,ZAUSN2
	DEC	B
	JR	NZ,ZAUSN2
	JP	(IX)
 

; Ausgabe der Werte, wenn ein Fehler im SRAM-Test auftritt
; Speicheradresse in HL, Testwert in B, rueckgelesener Wert in D
; in IX ist die Rueckkehradresse eingetragen

FAUSA3	EXX
	LD	HL,ADRERR
	LD	B,ANAUSE
	LD	C,S0BD
NZAERR	OUTI
	INC	B
WSREA	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSREA
	LD	D,0
ZSAE	DEC	D
	JR	NZ,ZSAE
	DJNZ	NZAERR
	LD	A,E
	LD	IY,$+7
	JP	HCAHL
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	H,0FFH
	LD	A,E
	LD	I,A
	EXX	
	LD	D,A
	LD	A,I	
	AND	A
	JP	Z,AENDZ
	LD	A,' '
	OUT	(S0BD),A
WSREA1	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSREA1
	LD	C,B
	LD	B,2
NASP	LD	A,20H
	OUT	(S0BD),A
WSREA2	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSREA2
	LD	E,0
ZSAE1	DEC	E
	JR	NZ,ZSAE1
	DJNZ	NASP
	LD	B,C
	LD	A,H		;Speicheradresse ist erste Ausgabe
	LD	IY,$+7
	JP	HCAHL		;Umwandlung in Hexadezimalzahl
	OUT	(S0BD),A	;Ausgabe an das Terminal
	LD	IY,$+7
	JP	SRAHL		;Ist SIO fertig?
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,L
	LD	IY,$+7
	JP	HCAHL
	OUT	(S0BD),A	;Ausgabe zu lesender Wert
	LD	IY,$+7
	JP	SRAHL
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,20H
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,B		;Ausgabe zu lesender Wert
	LD	IY,$+7
	JP	HCAHL
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,20H
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,D		;Ruecklesen gelesener Wert
	LD	IY,$+7
	JP	HCAHL
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
	LD	A,C
	OUT	(S0BD),A
	LD	IY,$+7
	JP	SRAHL
AENDZ	LD	HL,ENDZEI
	LD	B,ANZEZ
NZENDZ	LD	A,(HL)
	OUT	(S0BD),A
WSIOFR	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSIOFR
	INC	HL
	LD	D,0
ZSAEND	DEC	D
	JR	NZ,ZSAEND
	DJNZ	NZENDZ
	JP	(IX)

SRAHL	IN	A,(S0BC)
	BIT	2,A
	JR	Z,SRAHL
	JP	(IY)

HCAHL	LD	C,A
	AND	0F0H
	RRCA
	RRCA
	RRCA
	RRCA
	CP	0AH
	JR	C,OR0
	ADD	A,7
OR0	ADD	A,30H
	LD	I,A		;in I wird der Wert zwischengespeichert
	LD	A,C
	AND	0FH
	CP	0AH
 	JR	C,OR1
	ADD	A,7
OR1	ADD	A,30H
	LD	C,A
	LD	A,I
	JP	(IY)

ADRM1T	NOP
	ADC	A,B
	LD	BC,07FH
	ADC	A,B
	NOP	
	INC	B
	ADC	A,B
	NOP	
	EX	AF,AF'
	NOP
	EX	AF,AF'
	NOP
	DJNZ	TST_P1
	LD	C,8
TST_P1	INC	B
	ADC	A,B
	DEC	L
	NOP
 	ADC	A,B
	NOP
	EX	AF,AF'
	ADD	A,B
	NOP
	NOP
	JP	(IX)

DTSR1	XOR	0FFH		;Ja.
	JP	NZ,TAUFD	;Test mit 0FFH
;
WT2SR 	EXX
	LD	E,6
	LD	IX,T2SR
	JP	AUSTNR
T2SR	EXX
;
; Test der Adressleitungen des statischen RAM

; Test der oberen Adressleitungen
; Der Speicher wird in Bloecke zu je 256 Byte aufgeteilt. Die Bloecke werden nacheinander
; mit jeweils einem Wert gefuellt (0-7). Anschliessend werden die Bloecke nacheinander
; getestet. War der Test der Datenleitungen fehlerfrei und stimmen die Werte eines
; Blockes mit dem eingegebenen Wert nicht ueberein, dann ist die Ansteuerung fuer
; das Adressbit dieses Blockes fehlerhaft.


; Test der unteren Adressleitungen

TUALSR	LD	IX,NT2SR	;Rueckkehradresse bei Fehler eintragen
 	LD	HL,2000H
	LD	DE,2000H
	LD	C,8
SRT3	XOR	A
SRT4	LD	(DE),A
	INC	DE
	INC	A
	JR	NZ,SRT4
	LD	B,A
SRT5	LD	A,(HL)
	CP	B
	JP	NZ,FAUSA3
	INC	HL
	INC	B
	JR	NZ,SRT5
	DEC	C
	JR	NZ,SRT3

NT2SR	EXX
	LD	E,7
	LD	IX,T01SR
	JP	AUSTNR
T01SR	EXX
; Durchschieben von 0 und 1


; Durchschieben einer Null

TE0SR	
	LD	HL,2000H
	LD	(HL),0FFH
	LD	DE,7FFH
	LD	B,E
	LD	A,(424)		;nur wegen Aenderung auf dem Bus
	LD	A,(HL)
	CP	B		;Testen des ersten Wertes
	JP	NZ,FAUSA3
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
	JP	NZ,FAUSA3
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	Z,KAT3SR
	EX	AF,AF'
	LD	A,B
	RLA
	LD	B,A
	EX	AF,AF'
	JR	TNZSR2

KAT3SR	
 	EXX
	LD	E,8
	LD	IX,T3SR
	JP	AUSTNR
T3SR	EXX

; M1-Test des statischen RAM

	LD	DE,2000H
NT3SR	LD	HL,ADRM1T
 	LD	BC,30
	LDIR
	AND	A
	EX	DE,HL
	LD	DE,30
	SBC	HL,DE
 	LD	IX,WTM1SR
	JP	(HL)
WTM1SR 	LD	DE,100H
	INC	L
	ADD	HL,DE
	EX	DE,HL
	LD	A,D
	CP	28H
	JR	NZ,NT3SR

TALLP	LD	SP,2800H	;Stackpointer mit bestimmten Wert laden
	LD	A,0FFH
	LD	(MAWE),A	;Initialisierung fuer den Merker
	EXX
	LD	E,10H
	CALL	ANRRET		;Ausgabe der Fehlernummer an PIO und Terminal
	EXX

; Es werden alle PIO's im Ausgabemode getestet (ohne Interrupt).
; Im Fehlerfall werden Port-Adresse, eingetragener und rueckgelesener
; Wert an das Terminal ausgegeben.

; Test PIO0, Kanal A im Ausgabemode (ohne Interrupt)

	LD	A,3
	LD	C,P0AC
	OUT	(C),A
	LD	A,55H
	CALL	PTAUS
	CALL	NZ,FA3RET
;

; Test PIO0, Kanal B im Ausgabemode


;LD	C,P0BC
;LD	A,0D5H
;CALL	PTAUS
;CALL	NZ,FA3RET

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


	EI
	IM	2
 
	LD	A,23H
	LD	I,A		;Interruptvektorregister mit 73H laden
	LD	HL,TABISR	;Tabelle fuer alle Interrupts
	LD	DE,ADISR	;Adresse fuer ISR-Adressen
	LD	BC,42H		;Anzahl der Werte
	LDIR
	XOR	A
	LD	HL,M1CISR	;Merker fuer ISR initialisieren
	LD	B,20		;Anzahl der Merkzellen
ALLMZ	LD	(HL),A
	INC	HL
	DJNZ	ALLMZ


	EXX
	LD	A,0FFH
	LD	(MAWE),A	;vorher war Test mit Interrupt
	LD	E,15H
	CALL	ANRRET
	EXX

; Test der CTC-Kanaele als Zaehler (ohne Interrupt)
; Test von CTC0, Kanal 0-3 und CTC1, Kanal 0,2,3

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
	LD	B,90H
	CALL	CZPR
	CALL	NC,FA3RET

	LD	C,C1K3
	LD	B,0A0H
	CALL	CZPR
	CALL	NC,FA3RET

	EXX
	LD	E,16H
	CALL	ANRRET
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
	CALL	ANRRET
	EXX

; Test der CTC-Kanaele mit Interrupt

; CTC0, Kanal0 auf Interrupt programmieren

	XOR	A
	LD	C,C0K0
	OUT	(C0K0),A	;Interruptvektor wird nur einmal programmiert
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

	LD	A,8
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

	EXX
	LD	E,20H
	LD	A,0FFH
	LD	(MAWE),A
	CALL	ANRRET
TSLES	EXX

; Test des SIO durch Lesen des Interruptvektors (ohne Interrupt)
; Eingabe des Interruptvektors und anschliessendes Ruecklesen
; nur Test des SIO1, da SIO0 bereits getestet
; Test ist auch nur im Kanal B moeglich

 
	LD	A,11H
	OUT	(S1BC),A
	XOR	A
	OUT	(S1BC),A
	LD	A,12H		;Schreibregister 2 ansprechen
	OUT	(S1BC),A
	LD	A,50H		;Wert 50H eintragen
	OUT	(S1BC),A
	IN	A,(S1BC)	;Lesen des Leseregisters 0, damit aendern der Daten
				;auf dem Datenbus
	LD	A,12H		;Leseregister 2 ansprechen
	OUT	(S1BC),A
	LD	C,S1AD		;erste SIO-Adresse fuer evt. Fehlerausgabe eintragen
	LD	B,50H
	IN	A,(S1BC)
	CP	B		;Ist der Wert gleich dem ausgegebenen Wert?
	CALL	NZ,FA3RET	;Nein, Fehler bei Aufruf

; Wiederholung gleicher Test, nur mit anderem Wert

	LD	A,12H		;Leseregister 2 ansprechen
	OUT	(S1BC),A
	LD	A,30H
	OUT	(S1BC),A
	IN	A,(S1BC)	;Veraenderung auf dem Datenbus
	LD	A,12H
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
	CALL	ANRRET
TSINT	EXX

; Programmierung SIO0 und SIO1 als Sender (mit Interrupt)
; Aussenden von 0
; Interruptvektoren sind bereits programmiert

	LD	B,CTC1
	LD	E,S0AC
	CALL	SINIT
 
	LD	A,11H
	OUT	(S0AC),A
	OUT	(S0BC),A
	LD	A,6		;Sendeinterrupt einschalten, Status wird nicht modifiziert
	OUT	(S0AC),A
	OUT	(S0BC),A

; Senden von SIO0, Kanal A

	LD	C,S0AD
	CALL	SI0APR		;Interruptprogrammierung SIO aufrufen
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

 ; Senden von SIO0, Kanal B

	LD	C,S0BD
	CALL	SI0BPR
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

	LD	B,CTC1+2
	LD	E,S1AC
	CALL	SINIT
 
	LD	B,CTC0
	LD	E,S1BC
	CALL	SINIT
 
	LD	A,11H
	OUT	(S1AC),A
	OUT	(S1BC),A
	LD	A,6		;Sendeinterrupt einschalten, Status wird nicht modifiziert
	OUT	(S1AC),A
	OUT	(S1BC),A

; Senden von SIO1, Kanal A

	LD	C,S1AD
	CALL	SI1APR
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

; Senden von SIO1, Kanal B

	LD	C,S1BD
	CALL	SI1BPR
	CALL	Z,FA3RET	;kein Interrupt in ausreichender Zeit

TEFDC1
 
	EXX
	LD	A,0FFH
	LD	(MAWE),A
	LD	E,25H
	CALL	ANRRET
	EXX

; Test des FDC
; Programmierung SPECIFY-Kommando und Ruecklesen

	DI
  
	CALL	FDCREA
	JR	NZ,VTDPA	;FDC meldet keine Freigabe, damit keine weitere
				;Initialisierung moeglich
	LD	A,3
	OUT	(DFDC),A
	CALL	FDCREA
	JR	NZ,VTDPA


	LD	A,1FH
	OUT	(DFDC),A
	CALL	FDCREA
	JR	NZ,VTDPA

	LD	A,0FFH
	OUT	(DFDC),A
	CALL	FDCREA
	JR	NZ,VTDPA

	LD	B,0FFH
	LD	C,DFDC
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	LD	A,0CFH		;Bitmode programmieren
	OUT	(P2BC),A
	LD	A,10H
	OUT	(P2BC),A
	LD	A,3


	OUT	(P2BC),A
	LD	A,8FH		;Wert fuer RESET ermitteln
	OUT	(P2BD),A	;RESET an den FDC

	EI
 
VTDPA	EXX
	LD	E,26H
	CALL	ANRRET
TEDMA	EXX

 
; Test des DMA durch Programmieren und Ruecklesen Statusbyte und
; Blocklaenge (ohne Interrupt)

	LD	HL,DMATA
	LD	B,DMATE-DMATA
	LD	C,DMAADR
	OTIR
	LD	B,3BH		;Maske fuer ersten Wert aus Leseregister
	IN	A,(C)
	AND	B
	CALL	Z,FA3RET	;Ergebnis Maske und eingelesener Wert nicht 0
	LD	C,DMAADR
	LD	B,0		;zweiter Testwert wird eingetragen
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET
	LD	C,DMAADR
	LD	B,1		;dritter Testwert wird eingetragen
	IN	A,(C)
	CP	B
	CALL	NZ,FA3RET

	EXX
	LD	A,1
	LD	(MAWE),A
	LD	E,27H
	CALL	ANRRET
DMAIT	EXX

; Test des DMA mit Interrupt
; Programmieren eines Speichertransfers und warten auf den Interrupt

	LD	HL,DMATA
	LD	B,DMATE2-DMATA
	LD	C,DMAADR
	OTIR
	LD	HL,DMATA1
	LD	B,DMATE1-DMATA1
	LD	C,DMAADR
	OTIR			;Programmierung des DMA fuer Interrupt
	CALL	ZEIT
	LD	A,(MIDMA)
	AND	A
	CALL	Z,FA3RET
DIEND	LD	A,0C3H
	OUT	(DMAADR),A	;partielles RESET an den DMA

 
	EXX
	LD	A,0FFH
	LD	(MAWE),A
	LD	E,30H
	CALL	ANRRET
TDRAM	EXX

; Programm Test DRAM
; Es werden ein Test der Adressleitungen, der Datenleitungen, der  Datenhaltigkeit
; des Speichers durch das Durchschieben von 0 oder 1 und ein Programmtest des Speichers durchgefuehrt.
; Bei auftretenden Fehlern wird der entsprechende Fehlerkode an das Terminal und, wenn vorher kein
; Fehler aufgetreten war, an den PIO Port ausgegeben. Am Terminal werden ausserdem die Speicheradresse,
; der ausgegebene und der rueckgelesene Wert angezeigt.
; Benutzte Register:

; Der Test erfolgt in zwei Teilen. Erster Teil: Einschalten des DRAM auf den
; Adressen 3000H-FFFFH und Test dieses Bereichs.

; Initialisierung des DRAM auf den Adressen 3000H-0FFFFH

	LD	DE,040DH	;DRAM_SEL aktiv
	LD	B,30H		;Adresse ist 3000H
	LD	C,WEADP
W1EDR	LD	A,10H
	OUT	(C),D		;Einschalten des DRAM bis E=0
	ADD	A,B
	LD	B,A
	DEC	E
	JR	NZ,W1EDR

; Test der Datenleitungen des DRAM

	XOR	A		;alle Bits auf 0 setzen
	LD	HL,3000H	;Anfangsadresse ist 3000H
	LD	DE,3001H
	LD	BC,0CFFFH	;Datenlaenge ist 32k-1
	LD	(HL),A		;0 oder 0FFH auf den ersten Speicherplatz
	LDIR			;Wert wird in die Speicherzellen des DRAM eingetragen
	LD	A,(HL)
	CP	B		;Testwert ist 0
	CALL	NZ,FAHLR
	LD	A,0FFH		;Test mit 0FFH
	LD	HL,3000H
	LD	DE,3001H
	LD	BC,0CFFFH
	LD	(HL),A
	LDIR
	LD	B,A		;Testwert eintragen
	LD	A,(HL)
	CP	B
	CALL	NZ,FAHLR

 
 
	EXX
	LD	E,31H
	CALL	ANRRET
T2DR	EXX

; Test der Adressleitungen des dynamischen RAM

; Test der oberen Adressleitungen
; Der Speicher wird in Bloecke zu je 256 Byte aufgeteilt. Die Bloecke werden nacheinander
; mit jeweils einem Wert gefuellt (0-7). Anschliessend werden die Bloecke nacheinander
; getestet. War der Test der Datenleitungen fehlerfrei und stimmen die Werte eines
; Blockes mit dem eingegebenen Wert nicht ueberein, dann ist die Ansteuerung fuer
; das Adressbit dieses Blockes fehlerhaft.

	LD	HL,3000H
	LD	DE,3001H
	XOR	A
SLDRT	LD	BC,0FFH
	LD	(HL),A
	LDIR
	INC	HL
	INC	DE
	INC	A
	CP	0D0H
	JR	NZ,SLDRT
	LD	HL,3000H
DRT1	LD	E,0
DRT2	LD	A,(HL)
	CP	B
	CALL	NZ,FAHLR	;Fehler? dann Aufruf
	INC	HL
	DEC	E
	JR	NZ,DRT2
	INC	B		;Erhoehen von B fuer die naechsten 256 Byte
	LD	A,B
	CP	0D0H
	JR	NZ,DRT1


	EXX
	LD	E,32H
	CALL	ANRRET
T01DR	EXX
; Durchschieben von 0 und 1


; Durchschieben einer Null
TE0DR	LD	HL,3000H
	LD	B,0FFH
	LD	(HL),0FFH
	LD	DE,0CFFEH
	LD	A,(HL)
	CP	B
	CALL	NZ,FAHLR
	INC	HL
NZNLDR	RLA
	LD	(HL),A
	INC	HL
	DEC	DE
	PUSH	AF
	LD	A,D
	OR	E
	JR	Z,T2ZDR
	POP	AF
	JR	NZNLDR
T2ZDR	LD	HL,3001H
	LD	DE,0CFFEH
	LD	A,E
	SCF
TNZDR2	PUSH	AF
	LD	B,A
	LD	A,(HL)
	CP	B
	CALL	NZ,FAHLR
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	Z,KAT3DR
	POP	AF
	RLA
	JR	TNZDR2

KAT3DR	POP	AF
	EXX 
	LD	E,33H
	CALL	ANRRET
T3DR	EXX

 
; M1-Test des dynamischen RAM

	LD	DE,3000H
NT3DR	LD	HL,ADRM1T
 	LD	BC,30
	LDIR
	AND	A
	EX	DE,HL
	LD	DE,30
	SBC	HL,DE
 	LD	IX,WTM1DR
	JP	(HL)
WTM1DR 	LD	DE,100H
	INC	HL
	ADD	HL,DE
	EX	DE,HL
	LD	A,D
	AND	A
	JR	NZ,NT3DR

; Der zweite Teil des DRAM-Test wird nach der Initialisierung und nach dem Umladen
; des EPROM's in den dynamischen RAM ausgefuehrt.

	LD	A,0DH
	CALL	AUSSP1
	CALL	AUSSP
	CALL	AUSSP
	CALL	AUSSP

 	JP	INITSM		;Sprung zum Initialisierungsteil

; Unterprogramme fuer die Eigentestroutinen

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
	IN	A,(P1AD)	;Eingabe fuer Aenderung auf dem Bus
	IN	A,(C)
	CP	B		;Vergleich mit dem Testwert
	RET	

; Programmierung CTC als Zaehler (ohne Interrupt)
; Zeitkonstante in B, rueckgelesener Wert in A, Portadresse in C

CZPR	LD	A,7
	OUT	(C),A
	LD	A,B
	OUT	(C),A
	CALL	ZEIT2
	IN	A,(C)
	CP	B
	PUSH	AF
	LD	A,3
	OUT	(C),A		;CTC ausschalten
	POP	AF
	RET


; Programmierung CTC-Kanal als Zaehler (mit Interrupt).
; Portadresse steht in C, Warten auf den Interrupt, nach bestimmter
; Zeit wird abgebrochen.
; In der ISR wird HL entsprechend des ausgewaehlten Kanals veraendert.

CIPR	LD	A,87H
	OUT	(C),A
	LD	B,70H
	LD	HL,ADRM1T	;Inhalt von HL ist 0
	LD	A,0FFH		;fuer ISR mit Wert ungleich 0 laden
	OUT	(C),B
	CALL	ZEIT1
	CALL	ZEIT1
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
	OUT	(C),A
	CALL	ZEIT
	CALL	ZEIT
	INC	C
	LD	A,11H
	OUT	(C),A
	XOR	A
	OUT	(C),A
	LD	A,38H
	OUT	(S0AC),A
	DEC	C
	LD	A,(MIS0KB)
	AND	A
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
 

; Interruptserviceroutinen der CTC-Kanaele

ISC0K0	LD	HL,MIC0K0
ICRET	LD	(HL),A
	LD	A,3
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

ISDMA	LD	A,0A3H
	OUT	(DMAADR),A
	LD	(MIDMA),A
	EI
	RETI

; Zeitschleifen

ZEIT	LD	HL,0FFFFH
DECZL	DEC	L
	JR	NZ,DECZL
	DEC	H
	JR	NZ,DECZL
	RET

ZEIT1	LD	D,0FFH
DECDL	DEC	D
	JR	NZ,DECDL
	RET

ZEIT2	LD	D,6
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
 
; Unterprogramm fuer die Ausgabe der Testschrittnummer an den PIO1, Kanal A
; fuer die Ziffernanzeige und Ausgabe an das Terminal ueber den SIO0, Kanal B
; Die Testschrittnummer wird im Zweitregistersatz im Register E uebergeben.

ANRRET	LD	B,2		;Anzahl von Backspace
NBSP	LD	A,8		;Backspace laden
	OUT	(S0BD),A
WSBSP	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSBSP
	CALL	ZEIT1
	DJNZ	NBSP
	LD	A,E
	BIT	0,H		;Kennzeichnung fuer bereits aufgetretenen Fehler
	JR	NZ,OHNPA
	OUT	(P1AD),A	;bei Fehler keine weiteren Ausgaben an den PIO
OHNPA	CALL	HEXCON
	OUT	(S0BD),A
	CALL	SIOREA
	CALL	ZEIT1
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	LD	C,0
ZAUSNR	CALL	ZEIT1
	DEC	C
	JR	NZ,ZAUSNR
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
	LD	D,A		;rueckgelesenen Wert in <D> speichern
	CALL	AUSERR		;Ausschrift Error, Nummer entsprechend E
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
 	LD	D,A		;rueckgelesenen Wert retten
	CALL	AUSERR
	LD	A,H
	CALL	HEXCON
	OUT	(S0BD),A
	CALL	SIOREA
	LD	A,C
	OUT	(S0BD),A
	CALL	SIOREA
	LD	A,L
	JP	AUS2WE

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

FDCREA	LD	C,MSRFDC
	LD	B,80H
	PUSH	HL
	POP	HL
	PUSH	HL
	POP	HL		;Initialisierung einer Zeit
	IN	A,(C)		;Einlesen Hauptstatusregister
	AND	0C0H
	CP	B
	JR	Z,TW1FDC
	CALL	ZEIT
	CALL	ZEIT
	CALL	ZEIT
	IN	A,(C)
	CP	B
	PUSH	AF
	CALL	NZ,FA3RET
	POP	AF
TW1FDC	RET

; Vereinbarungen fuer den Eigentest

; Portadressen

CTC0	EQU	8
C0K0	EQU	CTC0
C0K1	EQU	CTC0+1
C0K2	EQU	CTC0+2
C0K3	EQU	CTC0+3

CTC1	EQU	02CH
C1K0	EQU	CTC1
C1K1	EQU	CTC1+1
C1K2	EQU	CTC1+2
C1K3	EQU	CTC1+3

SIO0	EQU	024H
S0AD	EQU	SIO0
S0AC	EQU	SIO0+1
S0BD	EQU	SIO0+2
S0BC	EQU	SIO0+3

SIO1	EQU	028H
S1AD	EQU	SIO1
S1AC	EQU	SIO1+1
S1BD	EQU	SIO1+2
S1BC	EQU	SIO1+3

PIO0	EQU	0CH
P0AD	EQU	PIO0
P0AC	EQU	PIO0+1
P0BD	EQU	PIO0+2
P0BC	EQU	PIO0+3

PIO1	EQU	018H
P1AD	EQU	PIO1
P1AC	EQU	PIO1+1
P1BD	EQU	PIO1+2
P1BC	EQU	PIO1+3

PIO2	EQU	1CH
P2AD	EQU	PIO2
P2AC	EQU	PIO2+1
P2BD	EQU	PIO2+2
P2BC	EQU	PIO2+3

DMAADR	EQU	3CH

FDC	EQU	20H
MSRFDC	EQU	FDC
DFDC	EQU	FDC+1

; Vereinbarungen fuer die SIO-Programmierung

R0	EQU	0
R1	EQU	1
R2	EQU	2
R3	EQU	3
R4	EQU	4
R5	EQU	5
R6	EQU	6
R7	EQU	7
;
COMM6	EQU	30H		; FEHLER RUECKSETZEN
COMM3	EQU	18H		; KANAL RUECKSETZEN
COMM2	EQU	10H		; RUECKSETZEN EXT./STAT. INTERRUPT
NOPRTY	EQU	0		; PARITAETSPRUEFUNG AUSSCHALTEN
S2	EQU	0CH		; 2 STOP/BITS JE ZEICHEN
C16	EQU	40H		; *16
RENABLE	EQU	1
B8	EQU	0C0H
XENABLE	EQU	8
T8	EQU	60H
PDAVCT	EQU	18H
SAVECT	EQU	04H
XMTRIE	EQU	02H
C32	EQU	80H

; Initialisierungstabelle fuer den SIO

INITAB
	DEFB	COMM6
	DEFB	COMM3
	DEFB	R4+COMM2
	DEFB	NOPRTY+S2+C32
	DEFB	R3+COMM2
	DEFB	RENABLE+B8
	DEFB	R5+COMM2
	DEFB	XENABLE+T8

; Initialisierungstabelle fuer den DMA

DMATA	DEFB	0C3H
	DEFB	7DH
	DEFB	0
	DEFB	21H
	DEFB	0
	DEFB	1
	DEFB	0CFH
	DEFB	14H
	DEFB	10H
	DEFB	80H
	DEFB	0BDH
	DEFB	0
	DEFB	20H
	DEFB	12H
	DEFB	40H
	DEFB	1
	DEFB	0CFH
	DEFB	5
	DEFB	82H
DMATE2	DEFB	87H
	DEFB	0BBH
	DEFB	7
DMATE
DMATA1	DEFB	0ABH
	DEFB	87H
	DEFB	0BFH
DMATE1

; Initialisierungswerte fuer die ISR-Tabelle

TABISR	DEFW	ISC0K0
	DEFW	ISC0K1
	DEFW	ISC0K2
	DEFW	ISC0K3
	DEFW	ISC1K0
	DEFW	0
	DEFW	ISC1K2
	DEFW	ISC1K3
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	ISS0KB
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	ISS0KA
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	ISS1KB
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	ISS1KA
	DEFW	0
	DEFW	0
	DEFW	0
	DEFW	ISDMA

; Vereinbarungen fuer die Merkzellen

ADISR	EQU	2300H
M1CISR	EQU	234FH
MIDMA	EQU	234FH
MIC0K0	EQU	2350H
MIC0K1	EQU	2351H
MIC0K2	EQU	2352H
MIC0K3	EQU	2353H
MIC1K0	EQU	2354H
MIC1K2	EQU	2355H
MIC1K3	EQU	2356H
MIP0KA	EQU	2357H
MIP0KB	EQU	2358H
MIP1KB	EQU	2359H
MIP2KA	EQU	235AH
MIP2KB	EQU	235BH
MIS0KA	EQU	235CH
MIS0KB	EQU	235DH
MIS1KA	EQU	235EH
MIS1KB	EQU	235FH
MAWE	EQU	2360H


; Ausschrift fuer die Meldung des Eigentest

AUSHWT
	DEFW	0D0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFW	0A0AH
	DEFB	07H
AHWT1	DEFB	0DH
	DEFB	0AH
	DEFM	'P8000 Hardwaretest U880 - Version 3.1'
	DEFB	0AH
	DEFB	0DH
	DEFB	20H
	DEFB	20H		;2* Space wegen Testschrittnummer
ANZHWT	EQU	68

; Ausschrift bei Fehler

ADRERR	DEFW	0808H		;2* Backspace wegen Loeschen der Testschrittnummer
	DEFM	'*** ERROR '
ANAUSE	EQU	12

; Ausgaben am Ende der Zeile

ENDZEI	DEFB	0AH
	DEFB	0DH
	DEFB	20H
	DEFB	20H
ANZEZ	EQU	4

; Ausschrift bei Fehler PIO1, Kanal A, Testwert AA (Test ohne verfuegbaren RAM)

AUSF1	DEFW	0808H
	DEFM	'*** ERROR 10   18 AA '
	DEFW	0D0DH
ANAUS1	EQU	25

; Ausschrift bei Fehler PIO1, Kanal A, Testwert 55 (Test ohne verfuegbaren RAM)

AUSF2	DEFW	0808H
	DEFM	'*** ERROR 10   18 55 '
	DEFW	0D0DH
ANAUS2	EQU	25

; Ausschrift bei Fehler CTC1, Kanal 1 (Test ohne verfuegbaren RAM)

AUSF3	DEFW	0808H
	DEFM	'*** ERROR 15   2D 80 '
	DEFW	0D0DH
ANAUS3	EQU	25

; Ausschrift bei Fehler CTC1, Kanal1 (Test ohne verfuegbaren RAM)

AUSF4	DEFW	0808H
	DEFM	'*** ERROR 16   2D 80 '
	DEFW	0D0DH
ANAUS4	EQU	25

; Ausschrift bei Fehler SIO0, Kanal B (Test ohne verfuegbaren RAM)

AUSF5	DEFW	0808H
	DEFM	'*** ERROR 20   26 A0 '
	DEFW	0D0DH
ANAUS5	EQU	25

; Ausschrift bei Fehler SIO0, Kanal B (Test ohne verfuegbaren RAM)

AUSF6	DEFW	0808H
	DEFM	'*** ERROR 21   26 50 '
	DEFW	0D0DH
ANAUS6	EQU 25

; Ausgabe der Testnummern zu Beginn des Eigentest, wenn kein Fehler
; auftrat (bei Test ohne verfuegbaren RAM)

TENRAN	DEFW	0808H
	DEFM	'10'
	DEFW	0808H
	DEFM	'15'
	DEFW	0808H
	DEFM	'16'
	DEFW	0808H
	DEFM	'20'
ANZANF	EQU	16
 
WEADP	EQU	0
RFF	EQU	4

INSM1
 
MEMINI	LD	C,WEADP
	LD	DE,10H
	XOR	A
AUSWE	LD	B,A
	OUT	(C),D
	ADD	A,10H
	DEC	E
	JR	NZ,AUSWE
 
	LD	D,1
	LD	B,0
	OUT	(C),D
	IN	A,(RFF)
	LD	B,10H
	OUT	(C),D
 
INITSM
 
	LD	A,57H
	OUT	(CTC1+1),A
	LD	A,2
	OUT	(CTC1+1),A
 
	LD	HL,INITAB
	LD	C,S0BC
	LD	B,8
	OTIR
 
	LD	A,12H
	OUT	(S0BC),A
	OUT	(S1BC),A
	LD	A,20H
	OUT	(S0BC),A
	LD	A,30H
	OUT	(S1BC),A
 
 	LD	C,WEADP
	LD	D,4
	LD	E,0EH
	LD	A,20H
AUSDR	LD	B,A
	OUT	(C),D
	ADD	A,10H
	DEC	E
	JR	NZ,AUSDR
	LD	BC,2000H
	LD	DE,0C000H
	LD	HL,0
	LDIR
	JP	RAMADR+0C000H
RAMADR	LD	C,WEADP 
	LD	D,4
	LD	B,0
EINDR	OUT	(C),D
	LD	B,10H
	OUT	(C),D
	LD	BC,0C00H
	LD	DE,0
	LD	HL,0CF00H
	LDIR
	LD	HL,ANFSM+0C000H
	LD	B,ANZASM
NZSM	LD	A,(HL)
	OUT	(S0BD),A
WSFER	IN	A,(S0BC)
	BIT	2,A
	JR	Z,WSFER
	INC	HL
	LD	D,0
SLDEE	DEC	D
	JR	NZ,SLDEE
	DJNZ	NZSM
 
; Initialisierung des PIO2 auf der U880-Karte 
; 	
	LD	B,11
	LD	HL,IOWERT+0C000H
ILOOP	LD	C,(HL)		;Adresse
	INC	HL
	OUTI			;Ausgabe
	JR	NZ,ILOOP
 
; Initialisierung DMA
	LD	HL,DMAINI+0C000H
	LD	BC,0B00H+DMAADR
	OTIR
;
 
	LD A,0FFH
	OUT (P0BD),A	;RESET-U8000 HIGH
 
	LD A,4FH	;PROGRAMMIERUNG PIO0 (KOPPELPIO)
	OUT (P0AC),A	;PORT A: INPUT
	IN A,(P0AD)	;SCHEINEINGABE
 
	LD	BC,0C00H
	LD	HL,0CF00H
	LD	DE,0
NVERGL	LD	A,(DE)
	CP	(HL)
	JR	NZ,ERR35
	INC	DE
	INC	HL 
	DEC	BC
	LD	A,B
	OR	C
	JR	NZ,NVERGL
 
	LD	HL,0D00H
NWZUDR	LD	A,(HL)
	LD	D,A
	XOR	A
	LD	(HL),A
	LD	A,(0C000H)
	LD	A,(HL)
	AND	A
	JR	NZ,ERR34
	LD	A,D
	LD	(HL),A
	LD	A,(0C000H)
	LD	A,(HL)
	CP	D
	JR	NZ,ERR34
	INC	HL
	LD	A,H
	CP	30H
	JR	NZ,NWZUDR
 
	LD	HL,0D00H
NWZDR1	LD	A,(HL)
	LD	D,A
	LD	A,0FFH
	LD	(HL),A
	LD	A,(0C000H)
	LD	A,(HL)
	CP	0FFH
	JR	NZ,ERR34
	LD	A,D
	LD	(HL),A
	LD	A,(0C000H)
	LD	A,(HL)
	CP	D
	JR	NZ,ERR34
	INC	HL
	LD	A,H
	CP	30H
	JR	NZ,NWZDR1

 
 
MZU0	LD	C,S0BC
	LD	A,11H
	OUT	(C),A
	LD	A,1CH
	OUT	(C),A
  
	XOR	A
	OUT	(CTC0),A
 
	XOR	A
	LD	(0E88H),A	;Floppy-Neuinitialisierung fuer Start aus SMON
 
	DI
 
	JP	0
 
ERR34
	LD	HL,AUSE34+0C000H
A34	LD	B,ANZ34
	LD	C,S0BD
SLE34	OUTI
	INC	B
ABFE34
	IN	A,(S0BC)
	BIT	2,A
	JR	Z,ABFE34
	LD	D,0
ZSLA34	DEC	D
	JR	NZ,ZSLA34
	DJNZ	SLE34
	JP	MZU0+0C000H
 
ERR35	LD	HL,AUSE35+0C000H
	JR	A34
;;;;;;;;;;;;;;;;;;;;;;;;;
 
 
 
;
DMAINI	DEFB	0C3H		;DMA Reset
	DEFB	5		;Speicher ist Quelle, FDC ist Ziel, keine Adressen, Datentransfer
	DEFB	54H		;Time Byte folgt, Port A-MREQ, Adresse inkrementiert
	DEFB	0FCH		;Time-Byte (4 Takte)
	DEFB	78H		;Time Byte folgt, Port B-IORQ, Adresse fest
	DEFB	0FCH		;Time Byte (4 Takte)
	DEFB	95H		;Byte-Mode, IC-Byte folgt, Adresse Port B folgt
	DEFB	21H		;Adresse FDC
	DEFB	12H		;Int.-Vektor folgt, Interrupt bei Blockende
	DEFB	INTABD.AND.0FFH	;Interruptvektor
	DEFB	8AH		;Ready ist "high activ"
;
;
; Initialisierungswerte fuer PIO2
IOWERT
	DEFB	PIO2AC
	DEFB	0CFH		;Modeauswahl --> Bit-Mode
	DEFB	PIO2AC
	DEFB	80H
	DEFB	PIO2AC
	DEFB	3
	DEFB	PIO2AD
	DEFB	5FH		;Motoren aus, TC ein, Praekompensation aus
				;5,25"-Laufwerke
	DEFB	PIO2AD
	DEFB	1FH		;TC aus
	DEFB	PIO2BC
	DEFB	INTABP.AND.0FFH	; LOW-TEIL DES INT.VEKT.
	DEFB	PIO2BC
	DEFB	0CFH		;Modeauswahl --> Bitmode
	DEFB	PIO2BC
	DEFB	010H		;Festlegung E/A-Bits fuer Bit-Mode
	DEFB	PIO2BC
	DEFB	0B7H		;Interruptsteuerung Port B
				;EI/Maske folgt/jedes auf HIGH gehende
				;Bit loest Interrupt aus
	DEFB	PIO2BC
	DEFB	0EFH		;nur Bit 4 kann Interrupts ausloesen (FDC)
	DEFB	PIO2BD
	DEFB	0FH		;Index erlaubt, kein Laufwerk selektiert
;
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
 
ANFSM	DEFB	0AH
	DEFB	0DH
	DEFM	'U880-Softwaremonitor Version 3.1 - Press RETURN'
	DEFB	0AH
	DEFB	0DH
ANZASM	EQU	51
 
AUSE34	DEFM	'ERROR 34'
	DEFB	0AH
	DEFB	0DH
ANZ34	EQU	10
AUSE35	DEFM	'ERROR 35'
	DEFB	0AH
	DEFB	0DH
 
 
	ORG	0EF0H
 
	DI
	LD	D,1
	LD	B,0
	LD	C,0
	OUT	(C),D
	LD	B,10H
	OUT	(C),D
	JP	ENSMON
 
  
CRCEP0	EQU	1FF8H
NCEP0	EQU	1FFAH
CRCEP1	EQU	1FFCH
NCEP1	EQU	1FFEH
 
