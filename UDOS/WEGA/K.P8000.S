;******************************************************************************
;******************************************************************************
;
;	U D O S - Quelle	(C) ZFT/KEAW Abt. Baisisoftware - 1987
;
;	burn Modul: K.P8000.S
;
;
;	Bearbeiter:	P. Hoge
;	Change:		R. Kuehle
;
;	Datum:		27.03.1988
;	Version:	1.1
;
;******************************************************************************
;******************************************************************************
 
 
;P8000 EPROM-Programmer (alter und neuer Programmer)
 
  EXTERNAL WAIT		;label
  EXTERNAL LODNET	;procedure
  EXTERNAL SNDNET	;procedure
  EXTERNAL prom		;procedure
  EXTERNAL errflg	;byte
  EXTERNAL UEB16	;procedure
  EXTERNAL Vpp		;byte programmierspannung
  EXTERNAL PM		;byte program mode
  EXTERNAL BUF		;array [%4000 byte]
 
  GLOBAL PROM	;procedure(entry burn - prom programmer wega)
  GLOBAL intppb ;procedure(req byte, type byte)
  GLOBAL rbyte 	;procedure(promadr word)
		;returns(data byte)
  GLOBAL wbyte 	;procedure(promadr word,data byte)
  GLOBAL p2708 	;procedure(nbytes word)
  GLOBAL bcoff 	;procedure
  GLOBAL schar  ;procedure(byte)
  GLOBAL lchar  ;procedure returns(byte)
  GLOBAL ueb16	;procedure
  GLOBAL sdlc	;procedure
  GLOBAL old	;byte prom programmer board


;INTERNAL
 
l2val:	defb	-1		;Steuerbits von Latch 2
l3prog:	defb	-1		;latch 3 beim Programmieren
l3chg:	defb	-1		;latch 3 Umschalten Prog auf Verify
l3ver:	defb	-1		;latch 3 bei Verify
old:	defb	-1
nmax:	defb	1
time:	defb	0
count:	defw	100		;loop counter 2708
 
;CONSTANT

ctc:	equ	08h
pio1ad:	equ	18h
pio1ac:	equ	19h
pio1bd:	equ	1ah
pio1bc:	equ	1bh
 
 
;PROM - entry burn - prom programmer (wega)
 
PROM:	res	3,(hl)
	call	prom
	jp	WAIT


;intppb - Initialiesieren Programmer fuer
;    req=0   Anfangsinitialisierung
;    req=1   read
;    req=2   write
 
ireq	equ 6
ityp	equ 4
 
intppb:	push	ix		;save callers ix
	ld	ix,0
	add	ix,sp		;point to in parameters
	ld	a,(ix+ireq)	;get request type
	and	a
	jr	nz,rinit	;not initial request

	ld	hl,isrctc	;isr adr ctc 0 kanal 3
	ld	(0f04h),hl
	ld	b,inilen
	ld	hl,initab
iloop1:	ld	c,(hl)		;io address
	inc	hl
	outi			;data byte
	jr	nz,iloop1
	in	a,(pio1ad)	;alter Programmer = FF
				;neuer Programmer = 00
	ld	(old),a
	or	a
	ld	b,ninilen	;Initialisierung neuer Programmer
	jr	z,iloop2
	ld	b,oinilen	;Initialisierung alter Programmer
iloop2:	ld	c,(hl)		;io address
	inc	hl
	outi			;data byte
	jr	nz,iloop2
	jp	initret		;return to caller

;Anfangsinitialisierung  Programmer
 
initab:	defb	pio1ac		;port a input
	defb	0cfh
	defb	pio1ac
	defb	0ffh
	defb	pio1bc		;port b
	defb	0
	defb	pio1bd
	defb	0fch		;daten
	defb	pio1bc		;port b bit mode
	defb	0cfh
	defb	pio1bc
	defb	40h		;bit 6 input
inilen:	equ	($-initab)/2

itab1:
	defb	pio1bd
	defb	0ffh
	defb	pio1ac		;port a output
	defb	0fh
ninilen: equ	($-itab1)/2
	defb	pio1bd
	defb	11h
	defb	pio1bd
	defb	9dh		;strobe latch 1; reset kurzschluss
	defb	pio1bd
	defb	11h
oinilen: equ	($-itab1)/2

;Initialiesierung Read/Write fuer alten Programmer

rinit:	cp	1		;initialize for read?
	jr	nz,winit	;no

	ld	e,(ix+ityp)	;prom typ
	call	typ
	jr	nz,initret	;invalid prom typ
	ld	a,(old)
	or	a
	jr	z,ri1		;neue Programmer-Karte
	ld	hl,ritab
	jr	rwinit

ri1:	call	nrinit
	jr	initret

winit:	cp	2		;initialize for write?
	jr	nz,initret	;no just return

	ld	e,(ix+ityp)
	call	typ
	jr	nz,initret	;invalid prom typ
	ld	a,(old)
	or	a
	jr	nz,wi1
	call	nwinit		;neuer Programmer
	jr	initret

wi1:	ld	hl,witab
rwinit:	ld	e,(ix+ityp)
	ld	d,0
	add	hl,de		;entry in table
	add	hl,de
	add	hl,de		;* 3
	ex	de,hl
	ld	a,(de)		;latch 2
	ld	(l2val),a	;control bits latch 2
	ld	h,a
	call	latch2		;latch 2 init
	inc	de
	ld	a,0ffh
	call	latch3		;latch 3 inaktiv
	ld	a,(de)		;prg inaktiv
	inc	de
	or	81h		;+5V eprom
	out	(pio1bd),a
	call	wait50
	and	7fh
	out	(pio1bd),a	;reset Kurzschluss
	ld	a,(de)		;latch 3 init
	or	10h		;prog spannung aus
	call	latch3
	call	wait1
	ld	a,(de)		;prog spannung 1 ms spaeter anlegen
	call	latch3		;latch 3 init
	call	nz,ks		;Kurzschluss

initret:
	pop	ix		;restore callers ix
	pop	hl		;return address
	pop	de
	pop	de		;deallocate in parameters
	jp	(hl)
 
ritab:	defb	0ch		;2708 (typ 0) latch 2
	defb	00h		;prg bit 5
	defb	57h		;latch 3
	defb	0d8h		;2716 (typ 1)
	defb	00h
	defb	5eh
	defs	5*3		;(typ 2-6)
	defb	0d0h		;2732 (typ 7)
	defb	00h
	defb	5fh
	defb	0d0h		;2732A (typ 8)
	defb	00h
	defb	5fh
	defb	80h		;2764 (typ 9)
	defb	20h
	defb	5fh

witab:	defb	0fch		;2708 latch 2
	defb	00h		;prg bit 5
	defb	07h		;latch 3
	defb	0d8h		;2716
	defb	00h
	defb	8eh
	defs	5*3
	defb	0d0h		;2732
	defb	20h
	defb	8dh
	defb	0d0h		;2732A
	defb	20h
	defb	2dh
	defb	80h		;2764
	defb	20h
	defb	2fh


;Kurzschluss EPROM
 
ks:	ld	a,2
	ld	(errflg),a
 
;Grundzustand alter Programmer
 
bcoff:	ld	a,(old)
	or	a
	jr	z,nbcoff	;neue Programmer-Karte
	ld	a,0ffh		;latch 3 inaktiv
	call	latch3
	call	wait4
	ld	a,11h		;+5V off
	out	(pio1bd),a
	ret

;Grundzustand neuer Programmer

nbcoff:	ld	a,0ffh
	out	(pio1bd),a
	ret

;Initialiesierung Read/Write fuer neuen Programmer

nrinit:	ld	hl,nritab
	ld	a,4
	jr	nrwinit

nwinit:	ld	hl,nwitab
	ld	a,7
nrwinit:
	ld	e,(ix+ityp)
	ld	d,0
nrw1:	add	hl,de		;entry in table
	dec	a
	jr	nz,nrw1
	ex	de,hl
	ld	l,0		;low adressen low
	call	nlch1
	ld	a,(de)		;latch 2
	inc	de
	ld	(l2val),a
	ld	h,a
	call	nlch2
	ld	a,(de)
	inc	de
	ld	(l3prog),a
	set	2,a		;Programierspannung abgeschaltet lassen
	call	nlch3
	ld	a,(de)
	inc	de
	bit	5,a
	jr	z,nrw4		;read bei 5V
	and	11100011B	;Vpp 25V, UB 6V
	ld	c,a
	ld	a,(PM)
	or	a
	jr	z, nrw2		;intelligent Programmieren
	set	7,c
	res	5,c		;5V
nrw2:	ld	a,(Vpp)		;Programmierspannung einstellen
	cp	1
	jr	z,nrw3		;25V
	set	2,c
	cp	2
	jr	z,nrw3		;21V
	res	2,c		;sonst 12,5V
	set	3,c
	set	4,c

nrw3:	ld	a,c
nrw4:	call	nlch4
	ld	a,(de)
	inc	de
	out	(pio1bd),a	;latchs output enable
	ld	a,(de)
	inc	de
	ld	(l3chg),a
	ld	a,(de)
	inc	de
	ld	(l3ver),a
	ld	a,(PM)		;Programmier-Mode 0: intelligent
	or	a		;		  1: normal
	ld	a,1
	jr	nz,nrw5
	ld	a,(de)
nrw5:	ld	(nmax),a
	call	wait200
	ld	a,(l3prog)
	call	nlch3		;Programmierspannung bei Write anlegen
	in	a,(pio1bd)
	bit	6,a
	call	nz,nks		;Kurzschluss
	ret			;ok

nritab:	defs	4		;Typ 0 nicht vorhanden
	defb	11111000B	;2716 (typ 1) latch 2
	defb	00101110B	;latch 3
	defb	10000000B	;latch 4
	defb	11101101B	;pio b
	defs	5*4		;(typ 2-6)
	defb	11110000B	;2732 (typ 7)
	defb	00111110B
	defb	10000000B
	defb	11101101B
	defb	11110000B	;2732A (typ 8)
	defb	00111110B
	defb	10000100B
	defb	11101101B
	defb	11100000B	;2764 (typ 9)
	defb	00011110B
	defb	11000100B
	defb	01101101B
	defb	11100000B	;2764A (typ 10)
	defb	00011110B
	defb	11011000B
	defb	01101101B
	defb	11000000B	;27128 (typ 11)
	defb	00011110B
	defb	11000100B
	defb	01101101B
	defb	11000000B	;27128A (typ 12)
	defb	00011110B
	defb	11011000B
	defb	01101101B
	defb	10000000B	;27256 (typ 13)
	defb	00011110B
	defb	11011000B
	defb	11101101B
	defs	4		;(typ 14)
	defb	00000000B	;27512 (typ 15)
	defb	00111110B
	defb	11011000B
	defb	11101101B
	
nwitab:	defs	7		;Typ 0 nicht vorhanden
	defb	11111000B	;2716, 25V Upp (typ 1) latch 2
	defb	11101011B	;latch 3
	defb	00100000B	;latch 4
	defb	11101111B	;pio
	defb	10101011B	;latch 3 change
	defb	10101011B	;latch 3 verify
	defb	15		;max. Anzahl der 1ms Prog. Impulse, 1 = 50ms
	defs	5*7		;(typ 2-6)
	defb	11110000B	;2732, 25V Upp (typ 7)
	defb	11110011B
	defb	00100000B
	defb	01101111B
	defb	11111011B
	defb	00111010B
	defb	15
	defb	11110000B	;2732A, 21V Upp (typ 8)
	defb	11110011B
	defb	00100100B
	defb	01101111B
	defb	11111011B
	defb	00111010B
	defb	15
	defb	11100000B	;2764, 21V Upp (typ 9)
	defb	01011011B
	defb	01100100B
	defb	01101111B
	defb	00011011B
	defb	00011011B
	defb	15
	defb	11100000B	;2764A, 12,5V Upp (typ 10)
	defb	01011011B
	defb	01111000B
	defb	01101111B
	defb	00011011B
	defb	00011011B
	defb	25
	defb	11000000B	;27128, 21V Upp (typ 11)
	defb	01011011B
	defb	01100100B
	defb	01101111B
	defb	00011010B
	defb	00011010B
	defb	15
	defb	11000000B	;27128A, 12,5V Upp (typ 12)
	defb	01011011B
	defb	01111000B
	defb	01101111B
	defb	00011010B
	defb	00011010B
	defb	25
	defb	10000000B	;27256, 12,5V Upp (typ 13)
	defb	11011010B
	defb	01111000B
	defb	01101111B
	defb	10011010B
	defb	10011010B
	defb	25
	defs	7		;(typ 14)
	defb	00000000B	;27512, 12,5V Upp (typ 15)
	defb	11110010B
	defb	01111000B
	defb	01101111B
	defb	11111010B
	defb	00111110B
	defb	25
	
 
;rbyte procedure - Read 1 Byte (alter Programmer)
 
radr	equ 4
rdata	equ 6
 
rbyte:	push	ix		;save callers ix
	ld	ix,0
	add	ix,sp		;point to in parameters
	ld	l,(ix+radr)	;prom address
	ld	h,(ix+radr+1)
	ld	a,(old)
	or	a
	jr	z,rd2		;neue Programmer-Karte
	call	latch1		;output low address
	call	latch2		;output high address
	jr	z,rd1
	call	ks		;Kurzschluss
	jr	rdret
rd1:	ld	a,0cfh		;port a input
	out	(pio1ac),a
	ld	a,0ffh
	out	(pio1ac),a
	in	a,(pio1bd)
	and	0f0h		;OE aktiv
	out	(pio1bd),a
	ld	c,pio1ad
	in	l,(c)		;read data byte from prom
	ld	(ix+rdata),l	;store in out parameters
	or	01h		;oe inaktiv
	out	(pio1bd),a
	ld	a,0fh		;port a output
	out	(pio1ac),a
	jr	rdret

rd2:	call	nrbyte
rdret:	pop	ix		;restore callers ix
	pop	hl		;return address
	pop	de		;deallocate in parameter
	jp	(hl)
 
 
;wbyte procedure - write 1 Byte (alter Programmer)
 
wadr	equ 6
wdata	equ 4
 
wbyte:	push	ix		;save callers ix
	ld	ix,0
	add	ix,sp		;point to in parameters
	ld	l,(ix+wadr)	;prom address
	ld	h,(ix+wadr+1)
	ld	a,(old)
	or	a
	jr	z,wr2		;neuer Programmer
	call	latch1
	call	latch2
	jr	z,wr1
	call	ks		;Kurzschluss
	jr	wrret
 
wr1:	ld	a,(ix+wdata)	;get data byte
	cp	0ffh		;is data = FF ?
	jr	z,wrret
 
	out	(pio1ad),a
	in	a,(pio1bd)
	and	0f0h
	or	02h		;OE aktiv, PRG inaktiv
	out	(pio1bd),a
	ld	b,50		;50 ms
	call	prog
	or	03h		;OE inaktiv
	out	(pio1bd),a
	jr	wrret

wr2:	call	nwbyte
wrret:	pop	ix		;restore callers ix
	pop	hl		;return address
	pop	de		;deallocate in parameters
	pop	de
	jp	(hl)
	
 
;Programmierung 2708 (nur alter Programmer)
 
nbyt	equ 4
 
p2708:	call	wait50
	push	ix		;save callers ix
	ld	ix,0
	add	ix,sp		;point to in parameters
	ld	iy,count
	ld	(iy),100	;loop thru prom 100 times
 
p2708a:	ld	c,(ix+nbyt)
	ld	b,(ix+nbyt+1)
	ld	hl,0000h	;prom address
	ld	de,BUF		;buffer - external
 
p2708b:	push	bc
	call	latch1		;output low address
	call	latch2		;output high address
	pop	bc
	jr	nz,pks
 
	ld	a,(de)		;data byte
	out	(pio1ad),a
	ld	a,02h		;OE aktiv, PRG inaktiv
	out	(pio1bd),a
	push	hl
	push	bc
	ld	b,1		;1ms
	call	prog
	or	03h		;OE inaktiv
	out	(pio1bd),a
	pop	bc
	pop	hl
 
	inc	hl		;inc prom address
	inc	de		;inc buffer address
	dec	bc		;dec byte count
	ld	a,b
	or	c
	jr	nz,p2708b	;next byte
	dec	(iy)
	jr	nz,p2708a	;next loop
 
pret	pop	ix		;restore callers ix
	pop	hl		;return address
	pop	de		;deallocate parameter
	jp	(hl)		;return
	
pks:	call	ks	;Kurzschluss
	jr	pret

;Read 1 Byte (neuer Programmer)

nrbyte:	call	nlch1		;Anlegen der Adresse
	call	nlch2
	ld	a,0cfh		;port a input
	out	(pio1ac),a
	ld	a,0ffh
	out	(pio1ac),a
	call	nz,nks		;Kurzschluss
	in	a,(pio1bd)
	and	0feh		;OE bidirekt. Treiber aktiv
	out	(pio1bd),a
	ld	c,pio1ad
	in	l,(c)		;read data byte from prom
	ld	(ix+rdata),l	;store in out parameters
	or	01h		;OE inaktiv
	out	(pio1bd),a
	ld	a,0fh		;port a output
	out	(pio1ac),a
	ret

;Write 1 Byte (neuer Programmer)

nwbyte:	call	nlch1
	call	nlch2
	call	nz,nks		;Kurzschluss
	ld	a,(ix+wdata)	;get data byte
	cp	0ffh		;is data = FF ?
	ret	z
 
	out	(pio1ad),a
	ld	d,a
	ld	a,(nmax)
	cp	1
	jr	nz,nwr1		;intelligente Programmierung

;unintelligente Programmierung

	in	a,(pio1bd)		
	and	0feh		;OE bidirekt. Treiber aktiv, PRG inaktiv
	out	(pio1bd),a
	ld	b,50		;50ms
	call	nprog
	or	01h		;OE inaktiv
	out	(pio1bd),a
	ret

;intelligente Programmierung

nwr1:	ld	e,0
nwr2:	in	a,(pio1bd)		
	and	0feh		;OE bidirekt. Treiber aktiv, PRG inaktiv
	out	(pio1bd),a
	ld	b,1		;1ms
	call	nprog
	or	01h		;OE inaktiv
	out	(pio1bd),a

	ld	a,(l3chg)
	ld	hl,l3ver
	cp	(hl)
	jr	z,t1
	call	nlch3
t1:	ld	a,(l3ver)	;Byte verify
	call	nlch3		;verify einstellen
	call	nz,nks		;Kurzschluss
	ld	a,0cfh		;port a input
	out	(pio1ac),a
	ld	a,0ffh
	out	(pio1ac),a
	in	a,(pio1bd)
	and	0fdh		;Richtung lesen, OE bid. Treiber aktiv
	out	(pio1bd),a
	and	0fch
	out	(pio1bd),a
	ld	c,pio1ad
	in	l,(c)		;read data byte from prom
	or	01h		;Richtung schreiben, OE inaktiv
	out	(pio1bd),a
	or	03h
	out	(pio1bd),a
	ld	a,0fh		;port a output
	out	(pio1ac),a
	push	hl
	ld	a,(l3chg)
	ld	hl,l3ver
	cp	(hl)
	jr	z,t2
	call	nlch3
t2:	pop	hl
	ld	a,(l3prog)	;wieder auf Programmieren einstellen
	call	nlch3
	ld	a,d
	out	(pio1ad),a	;Daten wieder anlegen
	call	nz,nks		;Kurzschluss

	inc	e
	ld	a,l		;rueckgelesenes Byte vergleichen
	cp	d
	jr	z,nwr3		;Ueberprogrammieren
	ld	a,(nmax)	;max Anzahl Impulse erreicht ?
	dec	a
	cp	e
	jr	nc,nwr2		;naechster Prog. impuls
	ld	a,3
	ld	(errflg),a
	ret			;Fehler

nwr3:	ld	b,4		;4n Ueberprogrammieren
	ld	a,(nmax)
	cp	16
	jr	c,nwr4
	ld	b,3		;3n Ueberprogrammieren
nwr4:	xor	a
nwr5:	add	a,e
	djnz	nwr5
	ld	b,a
	in	a,(pio1bd)		
	and	0feh		;OE bidirekt. Treiber aktiv, PRG inaktiv
	out	(pio1bd),a
	call	nprog		;in b Anzahl der ms
	or	01h		;OE inaktiv
	out	(pio1bd),a
	ret
 
;Kurzschluss neue Programmer-Karte
 
nks:	call	ks		;Kurzschluss
	pop	hl		;Ausgleich fuer call nks
	ret
 
 
;Ueberpruefung EPROM-Typ alte Programmer-Karte
 
typ:	ld	a,(old)
	or	a
	ld	a,e		;prom typ
	jr	z,ntyp
	cp	0		;2708 ?
	ret	z
	cp	1		;2716 ?
	ret	z
	cp	7		;2732, 32A, 64 ?
	jr	c,typerr
	cp	10
	jr	nc,typerr
	xor	a		;ok
	ret

;Ueberpruefung EPROM-Typ neue Programmer-Karte

ntyp:	cp	1		;2716
	ret	z
	cp	7		;2732, 32A, 64, 64, 128, 128, 256, 512 ?
	jr	c,typerr
	cp	14
	jr	z,typerr
	cp	16
	jr	nc,typerr
	xor	a		;ok
	ret
 
typerr:	xor	a
	inc	a
	ld	(errflg),a
	ret

;Output to latch's (alte Programmer-Karte)
 
latch1:	ld	c,0dh		;l = low address
	ld	a,l
	jr	l0
 
latch2:	ld	a,(l2val)	;control bits PRG
	or	h		;high address
	ld	c,09h
	jr	l0
 
latch3:	ld	c,05h		;l = control bits
l0:	out	(pio1ad),a
	in	a,(pio1bd)
	and	0f0h
	or	c
	out	(pio1bd),a	;strobe
	and	0f0h
	or	01h
	out	(pio1bd),a
	bit	6,a		;Kurzschluss ?
	ret
 
;Output to latch's (neue Programmer-Karte)

L1STROBE:	equ	00h
L2STROBE:	equ	04h
L3STROBE:	equ	08h
L4STROBE:	equ	0ch
LNOSTROBE:	equ	20h
LMASK:		equ	0d3h
 
nlch1:
	ld	a,l		;l = low address
	ld	c,L1STROBE
	jr	nl0
 
nlch2:
	ld	a,(l2val)	;control bits
	or	h		;high address
	ld	c,L2STROBE
	jr	nl0
 
nlch3:
	ld	c,L3STROBE
	jr	nl0

nlch4:
	ld	c,L4STROBE
nl0:	out	(pio1ad),a
	in	a,(pio1bd)
	and	LMASK
	or	c
	out	(pio1bd),a	;strobe
	or	LNOSTROBE
	out	(pio1bd),a
	bit	6,a		;Kurzschluss ?
	ret
 
 
;wait routines 
 
wait200:
	ld	b,200
	jr	wait

wait50:
	ld	b,50
	jr	wait
 
wait4:	ld	b,4
	jr	wait
 
wait1:	ld	b,1
wait:	ld	c,0
	jr	w1

prog:
	ld	c,20h		;prog bit alter Programmer
	jr	w1

nprog:
	ld	c,80h		;prog bit neuer Programmer
w1:	push	af
	ld	a,b
	ld	(time),a
	ld	a,0a5h
	out	(ctc+2),a
	ld	a,15
	out	(ctc+2),a
	in	a,(pio1bd)
	xor	c		;Prog. impuls ein, wenn progbit gesetzt
	out	(pio1bd),a
	xor	c
	push	af
wloop:	call	ueb16
	ld	a,(time)
	or	a
	jr	nz,wloop
	pop	af
	out	(pio1bd),a	;Prog. impuls aus, falls vorher ein
	pop	af
	ret

isrctc:	push	af
	ld	a,(time)
	dec	a
	ld	(time),a
	jr	nz,isrc1
	ld	a,03h		;ctc aus
	out	(ctc+2),a
isrc1:	pop	af
	ei
	reti
 
 
; lchar - procedure
; lesen von "dev/ud" (16-bit/8-bit p8000)
 
zchar	equ 4
 
lchar:	push	ix
	ld	ix,0
	add	ix,sp
 
	push	ix
kzei:
	call	LODNET
	jr	z,kzei
 
	pop	ix
	ld	(ix+zchar),a
	pop	ix
	pop	hl
	jp	(hl)
 
 
; schar - procedure
; schreiben nach "dev/ud" (8-bit/16-bit p8000)
 
schar:	push	ix
	ld	ix,0
	add	ix,sp
	ld	a,(ix+zchar)
 
	call	SNDNET
	pop	ix
	pop	hl
	pop	de
	jp	(hl)
 
ueb16:	push	ix
	call	UEB16
	pop	ix
	ret
 
 
; BERECHNUNG DES CRC VOM PROM NACH DEM SDLC POLYNOM
 
	GLOBAL sdlc
 
sdlc:	push ix		; plz ix retten
	ld ix,0		; parameter uebernahme
	add ix,sp
	ld a,(ix+8)	;char
	ld d,(ix+6)	;crc
	ld e,(ix+4)
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
	push af
	and 1fh
	xor e
	ld e,a
	pop af
	push af
	rrca
	and 0f0h
	xor e
	ld e,a
	pop af
	and 0e0h
	xor d
	ld d,e
	ld e,a
	
	ld (ix+12),e	; out parameter uebergeben
	ld (ix+10),d
	pop ix		; restore plz ix
	pop hl		; ret adr
	pop de		; dealocate inparameter
	pop de
	pop de
	jp (hl)		; ret to plz
