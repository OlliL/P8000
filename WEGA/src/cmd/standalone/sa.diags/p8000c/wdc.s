!*******************************************************************
********************************************************************

P8000-Firmware                (c) ZFT/KEAW Abt. Basissoftware - 1988
U8000-Stand alone software    Modul:  hd

Bearbeiter:	F.Spielmann
Datum:		1988
Version:	3.0

********************************************************************
*******************************************************************!

hd module

$nonsegmented

constant

!Port-Adressen!
	PIOAD := %FFA1
	PIOBD := %FFA3
	PIOAC := %FFA5
	PIOBC := %FFA7

!PIO-Programmierung!
	OUTPUT  := %0F
	INPUT   := %4F
	BITMODE := %CF
	INTVEC  := %18

! Anschlussbelegung des Controlers am PIO des 16-Bit-Teils
 	Port A   : 8 Bit bidirektionale Daten (D0 bis D7)
 	Port B   : Steuerport
 	  Bit0   : /ST0     (Statusbit 0)
 	  Bit1   : /ST1     (Statusbit 1)
 	  Bit2   : /ST2     (Statusbit 2)
 	  Bit3   : frei                        Ausgang
 	  Bit4   : frei                        Ausgang
 	  Bit5   : RESET    (WDC-Reset)        Ausgang
 	  Bit6   : TE       (Richtung)         Ausgang
 	  Bit7   : TR       (Anforderung)      Eingang
!
! Steuerport WDC !
	RDATA := %4000
	WDATA := %00
 
! Uebergabe der Parameter an den Host-Treiber
	rh7 : Kommandocode
	rl7 : Laufwerksnummer
	r6  : Zylindernummer
	rh5 : Kopfnummer
	rl5 : Sektornummer
	r4  : Datenlaenge
	r3  : Host-Datenadresse
  Uebergabe der Parameter byteweise (9 Byte) an den WDC-Controler:
	-- Kommandocode
	-- Laufwerksnummer
	-- Zyl (low/rl6)
	-- Zyl (high/rh6)
	-- Kopf
	-- Sektor
	-- Datenlaenge (rl4)
	-- Datenlaenge (rh4)
	-- Pruefsumme (wird nicht ausgewertet)
!

	global
wdc procedure
 entry

! Initialisierung des PIO !
! und RESET an den WDC    !
	calr	disk_in

! PIO-Programmierung Host-->WDC !
	ld	r0,#WDATA+OUTPUT
	outb	PIOBD,rh0
	outb	PIOAC,rl0

! Loeschen Fehlerregister !
	ldk	r2,#0
! Warten auf WDC-Bereitschaft !
w_st1:
	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#0
	jp	z,stat7
	cpb	rl0,#%1		!Status 6: PROM-Fehler!
	jp	z,err_20
	cpb	rl0,#6
	jr	nz,w_st1
! Ausgabe der neun Kommandobytes an den WDC !
	outb	PIOAD,rh7	!Kommandocode!
	outb	PIOAD,rl7	!Laufwerk!
	outb	PIOAD,rl6	!Zylinder low!
	outb	PIOAD,rh6	!Zylinder high!
	outb	PIOAD,rh5	!Head!
	outb	PIOAD,rl5	!Sektor-Nr.		!
	outb	PIOAD,rl4	!Sektorlaenge low	!
	outb	PIOAD,rh4	!Sektorlaenge high	!
	outb	PIOAD,rh3
! Warten auf Status=0 !
w_st2:	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#%7
	jr	nz,w_st2
! Warten auf Ergebnis-Status !
w_st3:	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#%7
	jr	z,w_st3
	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#0
	jp	z,stat7
	cpb	rl0,#6
	jp	z,stat1
	cpb	rl0,#5
	jp	z,stat2
	cpb	rl0,#4
	jp	z,stat3
	jp	err_0
!*******************************************!
!       Auswertung Status=1                 !
!*******************************************!
stat1:
	cpb	rh7,#%41
	jp	z,h_ret
	cpb	rh7,#%c2
	jp	z,h_ret
	cpb	rh7,#%48
	jp	z,h_ret
	bitb	rh7,#2
	jp	z,err_1
	jp	h_ret

!*******************************************!
!       Auswertung Status=2                 !
!*******************************************!
stat2:
	cpb	rh7,#%18
	jp	z,wdc_wt
	cpb	rh7,#%68	!Schreiben BTT aus Host!
	jp	z,wdc_wt
	bitb	rh7,#1		!Schreibkommandos!
	jp	z,err_2
wdc_wt:	
	ld	r1,#PIOAD
	otirb	@r1,@r3,r4
! Warten auf Status=0 !
w_st4:
	inb	rl0,PIOBD
	andb	rl0,#7
	cpb	rl0,#7
	jr	nz,w_st4
! Warten auf Ergebnis-Status !
w_st5:
	inb	rl0,PIOBD
	andb	rl0,#7
	cpb	rl0,#7
	jr	z,w_st5
	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#0
	jp	z,stat7
	cpb	rl0,#6
	jp	z,h_ret
	jp	err_5

!*******************************************!
!       Auswertung Status=3                 !
!*******************************************!
stat3:
	cpb	rh7,#%0		!Ready-Test!
	jp	z,wdc_rd
	cpb	rh7,#%08	!Lesen WDC-RAM!
	jp	z,wdc_rd
	cpb	rh7,#%28	!Lesen WDC-Parameterblock!
	jp	z,wdc_rd
	cpb	rh7,#%38	!Lesen WDC-Fehlerstatistik!
	jp	z,wdc_rd
	cpb	rh7,#%58
	jp	z,wdc_rd
	cpb	rh7,#%24	!Formatieren mit Defektspurtabelle!
	jp	z,wdc_rd
	bitb	rh7,#0		!HD-Lesekommandos!
	jp	z,err_3
! PIO-Umprogrammierung !
wdc_rd:
	ld	r0,#RDATA+INPUT
	outb	PIOAC,rl0
	outb	PIOBD,rh0
	inb	rl0,PIOAD	!Scheineingabe!
! Uebernahme der Bytes !
	ld	r1,#PIOAD
	inirb	@r3,@r1,r4
	cpb	rh7,#%24	!Formatieren mit Defektspur?!
	jp	nz,w_stat
	ld	r2,#%21
	jp	w_stat

!*******************************************!
!       Auswertung Status=7                 !
!*******************************************!
stat7:
! PIO-Umprogrammierung !
	ld	r0,#RDATA+INPUT
	outb	PIOAC,rl0
	outb	PIOBD,rh0
	inb	rl0,PIOAD	!Scheineingabe!
! Uebernahme des Fehlerbytes! 
	inb	rl2,PIOAD
 
w_stat:
! Warten auf Ergebnis-Status !
	inb	rl0,PIOBD
	andb	rl0,#7
	cpb	rl0,#7
	jr	nz,w_stat
w_stat1:
	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#%7
	jr	z,w_stat1
	inb	rl0,PIOBD
	andb	rl0,#%7
	cpb	rl0,#6
	jp	nz,err_4
h_ret:
	ret

! Fehlerbehandlung !
err_0:
	ldb	rl2,#%c0
	jr	err_rt
err_1:
	ldb	rl2,#%c1
	jr	err_rt
err_2:
	ldb	rl2,#%c2
	jr	err_rt
err_3:
	ldb	rl2,#%c3
	jr	err_rt
err_4:
	ldb	rl2,#%c4
	jr	err_rt
err_5:
	ldb	rl2,#%c5
	jr	err_rt
err_20:	ldb	rl2,#%20
err_rt:
	jp	h_ret
end wdc

	internal
! Initialisierung des PIO und des WDC !
 init array [* byte] := [
	PIOBC, INTVEC,
	PIOBD, WDATA,
	PIOBC, BITMODE,
	PIOBC, %87,
		]

disk_in procedure
 entry
	ld	r2,#PIOBD
	inb	rl1,@r2
	bitb	rl1,#5		!Reset weggenommen ?!
	ret	z
	lda	r1,init
	ldk	r0,#4		!Anzahl der Ausgaben!
iloop:	ldb	rl2,@r1		!Portadresse!
	inc	r1,#1
	outib	@r2,@r1,r0
	jr	nov,iloop
	ret
end disk_in
end hd

