;*************************************************************
;
; P8000 KOPPELSOFTWARE
; MODUL: BLOCK-FLOPPY E/A
; VERSION: 3.0
;
;*************************************************************
 
	GLOBAL BLKRD BLKWT BLKFO
	GLOBAL RBLOCK WBLOCK
	GLOBAL BLKINIT
	EXTERNAL SNDNET SNDACK LODNET
	EXTERNAL SNDDAT LODDAT
	EXTERNAL WAIT UEB16
	EXTERNAL BUF
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Initialisierung der Floppy-Formate 2 und 4

BLKINIT:
	ld	de,TYP2		;Typ 2 wird 48ds16
	ld	hl,TYP5
	ld	bc,11
	ldir
	ld	de,TYP4		;Typ 4 wird 48ds5
	ld	hl,TYP7
	ld	bc,11
	ldir
	ld	l,40		;Anzahl der Spuren
	ld	h,0c1h		;MT=1, MFM=1, SK=0, 8"=0, DSTEP=1
	ld	(TYP2+9),hl
	ld	(TYP4+9),hl
	ld	hl,(TYP5)	;Typ 3 fuer MF 1.6 optimieren
	ld	(TYP3),hl	;LW-Zeiten
	ld	hl,-1		;PR1/PR2
	ld	(TYP3+6),hl
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ZEITGETEILTER FLOPPY ZUGRIFF
 
DISK:	LD	IY,RQVEC
	CALL	FLOPPY
DLOOP:	CALL	UEB16
	LD	A,(IY+10)
	BIT	7,A		;FERTIG ?
	JR	Z,DLOOP
	BIT	6,A
DEND:	RET
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;mehrere Bloecke lesen / schreiben mit Spurwechsel

DISKRQ:	ld	hl,(RESID)
	ld	a,h
	or	l
	ret	z		;Fertig
	call	BCOUNT
	ex	de,hl		;DE Laenge bis Ende der Spur
	ld	hl,(RESID)
	or	a
	sbc	hl,de
	jr	nc,DB1
	ld	hl,(RESID)	;RESID < BCOUNT
	ld	(RQLEN),hl
	ld	hl,0
DB1:	ld	(RESID),hl
	call	DISK
	ret	nz		;Fehler
	ld	hl,RQDA		;naechste Spur
	ld	a,(hl)
	and	0e0h		;Sektor auf 0
	ld	(hl),a
	inc	hl
	inc	(hl)		;Track increment
	ld	hl,(RQADR)
	ld	de,(RQLEN)
	add	hl,de
	ld	(RQADR),hl
	jr	DISKRQ

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;BLOCK LESEN:
 
RBLOCK:	LD	A,0BH		;READ BINARY
	JP	KOPF		;KOPFINFORMATIONEN UEBERNEHMEN
 
BLKRD:	RES	0,(HL)		;RESET READ BIT IM STATUS
	CALL	DISKRQ
	JR	NZ,ERROR
	CALL	SNDACK
	LD	DE,(LEN)	;DATEN UEBERMITTELN
	LD	HL,BUF
BLKRLP:	LD	A,(HL)
	CALL	SNDDAT
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,BLKRLP
WATACK:	CALL	LODDAT
	JR	Z,WATACK	;WARTEN AUF ACK
	JR	BRWEND
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;BLOCK SCHREIBEN:
 
WBLOCK:	LD	A,0FH		;WRITE BINARY
	CALL	KOPF		;KOPFINFORMATIONEN UEBERNEHMEN
	LD	DE,(LEN)	;DATEN EINLESEN
	LD	HL,BUF
BLKWLP:	CALL	LODDAT
	JR	Z,BLKWLP
	LD	(HL),A
	INC	HL
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,BLKWLP
	RET
 
BLKWT:	RES	1,(HL)		;RESET WRITE-BIT IM STATUS
	CALL	DISKRQ
	JR	NZ,ERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BLOCK READ/WRITE Ende

FEND:	LD	A,ACK
ERROR:	CALL	SNDNET
BRWEND	LD	HL,(SAVEFD)	;ALTE WERTE RUECKSCHREIBEN
	LD	(FDCONF),HL
	LD	HL,0C050H	;ALTE WERTE FUER TABELLE NR.6
	LD	(TYP6+9),HL	;WIEDER EINTRAGEN (STANDARD-NR.6)
	JP	WAIT
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;UEBERNAHME KOPFINFORMATIONEN

KOPF:	LD	(RQCOD),A	;REQUEST-CODE
WAFI:	CALL	LODDAT
	JR	Z,WAFI		;1.BYTE ENTHAELT IM OBEREN NIBBLE FLOPPY-TYP
				;UND IM UNTEREN NIBBLE DRIVE-NR.
	ld	hl,(FDCONF)	;Retten FDCONF
	ld	(SAVEFD),hl
	CALL	TYP
WABN1:	CALL	LODDAT
	JR	Z,WABN1
	LD	D,A		;BLOCK-NR. HIGH-BYTE
WABN2:	CALL	LODDAT
	JR	Z,WABN2
	LD	E,A		;BLOCK-NR. LOW-BYTE
	CALL	BLOCK		;KONVERTIERUNG IN SPUR/SEKTOR
WAL1:	CALL	LODDAT
	JR	Z,WAL1
	LD	H,A		;LENGTH HIGH-BYTE
WAL2:	CALL	LODDAT
	JR	Z,WAL2
	LD	L,A		;LENGTH LOW-BYTE
	LD	(LEN),HL
	LD	(RESID),HL
	RET
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TYP:				;Setzen FDCONF und Drive
	ld	hl,BUF
	ld	(RQADR),hl	;Pufferadr setzen
	push	af
	and	0f0h		;Typ
	ld	e,a
	pop	af
	rrca
	rrca
	rrca
	push	af
	and 	0e0h
	ld	(RQDA),a	;DRIVE-NR.
	pop	af
	rrca
	and	0fh		;Typ
	ld	b,a
	or	e
	ld	e,a
	ld	d,a

	ld	hl,OLDTYP	;hat sich Typ geaendert ?
	cp	(hl)
	jr	z,T0		;nein
	ld	(hl),a
	ld	hl,PFDTYP	;Floppy-Treiber muss neuen
	ld	(hl),0		;Typ initialisieren

T0:	cp	88h		;Standardtypen
	jr	c,T1
	ld	de,6666H	;andere Typen auf Tabelle 6 abbilden
	ld	b,6
	jr	z,FD48DS	;Typ 8
	cp	99h
	jr	z,FD48SS	;Typ 9
	cp	0aah
	jr	z,FD96SS	;Typ10
				;alle restlichen werden auf 6 gesetzt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ADRESSE DER TABELLE DER FDTYP-DEFINITIONEN ERMITTELN

T1:	ld	(FDCONF),de
	push	ix
	ld	ix,TYPTAB-11
	ld	de,11		;LAENGE EINES TYPTAB-EINTRAGS
T2:	add	ix,de
	djnz	T2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Diskparameter ermitteln

	ld	a,(ix+3)	;Anzahl Sektoren pro Spur und Seite
	ld	(NSECT1),a
	ld	l,0
	bit	7,(ix+10)	;SS / DS
	jr	z,T3
	add	a,a		;DS
	inc	l
T3:	ld	(NSECT),a	;Anzahl der Sektoren
	ld	a,l
	ld	(DOUBLE),a	;0=SS 1=DS
	ld	a,(ix+2)	;Sektorlaenge
	ld	(SECLEN),a
	ld	a,(ix+9)	;Anzahl der Spuren
	ld	(NTRACK),a
	pop	ix
	ret
 
FD96SS:	LD	HL,4050H	;AUSFUEHREN DER ENTSPR. AENDERUNGEN
	LD	(TYP6+9),HL	;IN DER FLOPPY-TYP TABELLE NR.6
	JR	T1
 
FD48DS:	LD	HL,0C128H
	LD	(TYP6+9),HL
	JR	T1
 
FD48SS:	LD	HL,4128H
	LD	(TYP6+9),HL
	JR	T1
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;UMWANDLUNG BLOCK-NR. IN SPUR- UND SEKTOR-NR. UND SETZEN IM REQUEST-VEKTOR
;DIVIDEND IN AE, DIVISOR IN C

BLOCK:	ex	de,hl		;de Blocknummer
	ld	a,(SECLEN)
	or	a
	jr	z,BLOCK1	;1 Block = 4 Sektoren
	dec	a
	jr	z,BLOCK2	;1 Block = 2 Sektoren
	dec	a
	jr	z,BLOCK3	;1 Block = 1 Sektor
	dec	a
	jr	nz,BLOCK3	;unzulaessige Laenge
	srl	h		;/2 1 Block = 1/2 Sektor
	rr	l
	jr	BLOCK3

BLOCK1:	add	hl,hl
BLOCK2:	add	hl,hl
BLOCK3:	ld	a,(NSECT)	;Anzahl der Sektoren
	ld	c,a
	ld	a,h		;hl (n. Sektor) -> ae
	ld	e,l
	call	DV168		;QUOTIENT IN E (SPUR-NR.)
				;Rest in A = Sektornummer
	ld	hl,RQDA
	or	(hl)		;+ DRIVE
	ld	(hl),a
	inc	hl
	ld	(hl),e		;SPUR-NR.
	ret
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Integer Division
;Divident in AE, Divisor in C, Quotient in E, Rest in A

DV168:	CP	C
	CCF
	RET	C
	LD	B,8
	JR	DV163
DV161:	SUB	C
DV162:	SCF
	DEC	B
	JR	Z,DV165
DV163:	RL	E
	RLA
	JR	C,DV161
	SUB	C
	JR	NC,DV162
	ADD	A,C
	AND	A
	DJNZ	DV163
DV165:	RL	E
	RET
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;BLOCK FORMAT / VERIFY / COPY DISK

BLKFO:	ld	a,(hl)
	ld	(BLKREQ),a
	ld	(hl),0
	ld	hl,(FDCONF)	;Retten FDCONF
	ld	(SAVEFD),hl
	bit	4,a		;COPY DISK ?
	jr	z,BLKF1
BLKF0:	call	LODNET
	jr	z,BLKF0
	ld	(STYP),a	;SOURCE-Typ
BLKF1:	CALL	LODNET
	JR	Z,BLKF1		;STATUS-BYTE (FD-TYP + DRIVE)
	ld	(DTYP),a	;SOURCE-Typ
	CALL	TYP		;SETZEN FDCONF UND DRIVE-NR.
	LD	HL,-1		;POSITIONIERUNG AUF SPUR 0
	LD	(CURT0),HL
	LD	(CURT0+2),HL
	XOR	A
	LD	(RQDA+1),A	;TRACK
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Diskette formatieren
 
FORMAT:	ld	a,(BLKREQ)
	bit	3,a
	jr	nz,VERIFY	;nur Verify
	LD	A,51H		;Requestcode fuer Format
	LD	(RQCOD),A
	LD	E,0		;Seite 1
	CALL	FBUILD
	CALL	DISK		;1 SPUR FORMATIEREN
	JP	NZ,ERROR
	LD	A,(DOUBLE)
	OR	A
	JR	Z,COPYDISK	;SS

	LD	E,1		;Seite 2
	CALL	FBUILD
	LD	HL,RQDA
	LD	A,(HL)
	OR	C		;& SEKTOR NR FUER 2.SEITE
	LD	(HL),A		;AB SEKTOR 0 2. SEITE
	CALL	DISK		;1 SPUR FORMATIEREN
	JP	NZ,ERROR
	LD	HL,RQDA		;Sektor wieder auf 0
	LD	A,(HL)
	AND	0E0H		;Sektor auf 0
	LD	(HL),A
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;COPY DISK

COPYDISK:
	ld	a,(BLKREQ)
	bit	4,a
	jr	z,VERIFY	;kein COPY DISK
	ld	a,(STYP)	;Quelldiskette Lesen
	call	TYP
	ld	a,0bh
	ld	(RQCOD),a	;Requstcode
	call	BCOUNT
	call	DISK
	set	5,a		;in Fehlercode Source markieren
	jp	nz,ERROR
	ld	a,(DTYP)	;Zieldiskette Schreiben
	call	TYP
	ld	a,0fh
	ld	(RQCOD),a	;Requstcode
	call	DISK
	jp	nz,ERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DISKETTE VERIFY
 
VERIFY:	LD	A,0BH
	LD	(RQCOD),A	;Requstcode
	CALL	BCOUNT
	CALL	DISK
	JP	NZ,ERROR
 
	LD	HL,RQDA+1	;TRACK
	INC	(HL)		;NAECHSTE SPUR
	LD	A,(NTRACK)
	CP	(HL)
	JP	NZ,FORMAT
	JP	FEND
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Format Daten aufbauen

FBUILD:	LD	HL,(NSECT1)	;ANZAHL DER SEKTOREN 1 SEITE
	LD	B,L
	LD	C,L
	LD	H,0
	ADD	HL,HL		;*2
	ADD	HL,HL		;*4 (4 BYTE JE SEKTOR AN FDC)
	LD	(RQLEN),HL
	LD	HL,BUF		;FORMAT DATEN AUFBAUEN
	LD	D,1		;1. SEKTOR
FLOOP1:	LD	A,(RQDA+1)	;TRACK
	LD	(HL),A		;SPUR
	INC	HL
	LD	(HL),E		;KOPF
	INC	HL
	LD	(HL),D		;SEKTOR
	INC	D
	INC	HL
	LD	A,(SECLEN)
	LD	(HL),A		;BYTE/SEKTOR
	INC	HL
	DJNZ	FLOOP1
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Berechnung der Bytes von Sektornummer bis Ende der Spur

BCOUNT:	LD	A,(RQDA)
	AND	1FH
	LD	B,A		;START Sektor
	LD	HL,128
	LD	A,(SECLEN)	;SEKTORLAENGE (0 = 128 BYTE)
	OR	A
	JR	Z,BC2
BC1:	ADD	HL,HL		;*2
	DEC	A
	JR	NZ,BC1
BC2:	LD	A,(NSECT)	;ANZAHL DER SEKTOREN
	SUB	B
	LD	B,A
	EX	DE,HL
	LD	HL,0
BC3:	ADD	HL,DE
	DJNZ	BC3
	LD	(RQLEN),HL	;IN HL: LAENGE EINER SPUR
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RQVEC	DEFB	0
RQCOD	DEFS	1
RQADR	DEFW	BUF		;DATEN ADR.
RQLEN	DEFS	2		;LAENGE
	DEFW	DEND
	DEFW	DEND
	DEFS	1
RQDA	DEFS	2		;DISK ADRESSE
 
LEN	DEFS	2
RESID	DEFS	2
SAVEFD	DEFS	2
OLDTYP	defb	0
STYP	defs	1
DTYP	defs	1
NSECT1	defs	1
NSECT	defs	1
NTRACK	defs	1
DOUBLE	defs	1
SECLEN	defs	1
BLKREQ	defs	1
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FLOPPY	EQU	0BFDH
PFDTYP	EQU	0E88H
FDCONF	EQU	0EEBH
TYPTAB	EQU	0B24H
TYP2	EQU	TYPTAB+1*11
TYP3	EQU	TYPTAB+2*11
TYP4	EQU	TYPTAB+3*11
TYP5	EQU	TYPTAB+4*11
TYP6	EQU	TYPTAB+5*11
TYP7	EQU	TYPTAB+6*11
CURT0	EQU	0FCEH
 
POAD	EQU	0CH
POBD	EQU	0EH
P16A	EQU	10H
P16B	EQU	14H
 
ACK	EQU	06H
NAK	EQU	15H
