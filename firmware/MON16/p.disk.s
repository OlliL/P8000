!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_disk

  Bearbeiter:	P. Hoge / F.Spielmann
  Datum:	30.01.1989
  Version:	3.1

*******************************************************************************
******************************************************************************!
 
disk module


$SECTION PROM
$nonsegmented

  constant

!Port Adressen!
 
	PIOAD	:= %FFA1	!Daten lesen / schreiben!
	PIOBD	:= %FFA3	!Steuerport !
	PIOAC	:= %FFA5
	PIOBC	:= %FFA7

!PIO Programmierung!
 
	OUTPUT	:= %0F
	INPUT	:= %4F
	BITMODE	:= %CF
	INTVEC	:= %18
 
!Anschlussbelegung des Controlers am U880 PIO
 
	PORT A: 8 bit bidirektionale Daten (D0 bis D7)
	PORT B:
	bit 0:	/ST0 (Interrupt Request), Eingang
	bit 1:	/ST1 (Anforderung der Daten), Eingang
	bit 2:	/ST2 (Error), Eingang
	bit 3:	frei, Ausgang
	bit 4:	frei, Ausgang
	bit 5:	RESET (WDC-Reset), Ausgang
	bit 6:	TE und Richtung Datentreiber 0=write 1=read Ausgang
	bit 7:	TR (Transfer Request), Eingang
!
!WDC Steuerport!
 
	RDATA	:= %4000	!Daten lesen!
	WDATA	:= %00		!Daten schreiben!
	RESET	:= %80		!Reset an Controler ausgeben!

 
  global
 
!------------------------------------------------------------------!
! Eingabeparameter:   r7      .... Request-Code                    !
!                                  (1=Read Block, 2=Write Block    !
!                                  3=Read WDC-RAM, 4=Write WDC-RAM !
!                                  5=ungueltiger Kommandokode)     !
!                  bei Requestcode 1,2:                            !
!                     r6      .... Drive-Nr.                       !
!                     rr4     .... Blocknummer                     !
!                  bei Requestcode 3,4:                            !
!                     rl6     .... Low-Teil der WDC-RAM-Adresse    !
!                     rl5     .... High-Teil der WDC-RAM-Adresse   !
!                  bei Requstcode 1,2,3,4:                         !
!                     rr8     .... Pufferadresse                   !
!		      r10     .... Laenge (<=%200)		   !
! Der Return-Code wird in r2 zurueckgegeben			   !
!------------------------------------------------------------------!
 
    DISK procedure
      entry
	callr	_disk_in	!Hardware init!

	ldl	rr2, rr4
	ld	r4, #PIOAD
	ld	r5, #PIOBD
	ld	r0, #WDATA+OUTPUT
	outb	@r5, rh0	!Datentreiber auf Ausgabe!
	outb	PIOAC, rl0	!Daten-PIO auf Ausgabe!

	ldb	rh0, #%10
wrea0:	ld	r1,#%ffff
wready:	inb	rl0, @r5
	andb	rl0, #%7
	cpb	rl0, #%7
	jr	nz,wrea1
	djnz	r1,wready
	dbjnz	rh0, wrea0

errc1:	ld	r2,#%c1
	ret

wrea1:	inb	rl0, @r5
	andb	rl0, #%7
	cpb	rl0, #%6	!Status 1!
	jr	z,wrea2
	cpb	rl0, #%1	!Status 6!
	jr	nz,errc1
	ld	r2, #%20	!Fehler 20!
	ret

!WDC-Kommando ausgeben!
wrea2:	subb	rh7,rh7
	ldb	rl7,kc+0(r7)

	outb	@r4, rl7	!Kommando ausgeben!
	outb	@r4, rl6	!Laufwerknummer!
	outb	@r4, rl3	!Blocknummer low!
	outb	@r4, rh3
	outb	@r4, rl2
	outb	@r4, rh2	!Blocknummer high!
	ld	r3, r10		!Laenge!
	outb	@r4, rl3
	outb	@r4, rh3
	outb	@r4, rh6	!Pruefsumme, nicht benutzt!

	ld	r1,#%ffff
wwait0:	inb	rl0, @r5	!Warten bis WDC Status 0!
	andb	rl0, #%7
	cpb	rl0, #%7
	jr	z,ww01
	djnz	r1,wwait0
	jr	errc1

ww01:	ld	r1, #%ffff	!Warten, bis WDC-Status ungleich 0!
ww02:	inb	rl0, @r5
	andb	rl0, #%7	!nur Statusbits!
	cpb	rl0, #%7	!noch Status=0 ?!
	jr	nz,ww03		!Sprung, wenn Status ungleich 0!
	djnz	r1,ww02
	jr	errc1		!Fehler!

ww03:	inb	rl0, @r5	!Status pruefen!
	andb	rl0, #%7	!nur Statusbits!
	cpb	rl0, #%0	!Fehlerstatus 7 ?!
	jp	z,rww03		!Sprung, wenn Fehlerstatus!

	ld	r6, #%C000	!FCW!
	cpb	rl7,#%21	!Read Block!
	jr	z,rwwait
	cpb	rl7,#%08	!Read WDC-RAM!
	jr	z,rwwait
	cpb	rl7,#%f8	!ungueltiger KC!
	jr	z,rwwait

	ld	r1, #%ffff
wwait1:	inb	rl0, @r5	!Write!
	bitb	rl0, #1		!Warten, bis WDC bereit fuer Daten!
	jr	z, ww11
	djnz	r1, wwait1
	jp	errc1

ww11:	ldctl	r1, FCW
	ldctl	FCW, r6
!$segmented!
	otirb	@r4, @r8, r3	!otirb @r4, @rr8, r3!
	ldctl	FCW, r1
!nonsegmented!
 
	ld	r1, #%ffff
wwait2:	inb	rl0, @r5	!Warten bis WDC Status 0!
	bitb	rl0, #1
	jr	nz,rwwait
	djnz	r1, wwait2
	jp	errc1

rwwait:	ld	r1, #%ffff
rww01:	inb	rl0, @r5	!Status einlesen!
	bitb	rl0, #0
	jr	z, rww02	!auf Beendigung des Kommandos warten!
	djnz	r1, rww01

rww02:	inb	rl0, @r5
	andb	rl0, #%07	!Maske!
	ldk	r2, #0		!Returncode kein Fehler!
	cpb	rl0, #%06	!Status 1 = fertig, kein Fehler!
	ret	z

rww03:	ld	r1, #RDATA+INPUT
	outb	PIOAC, rl1	!Daten-PIO auf Eingabe!
	outb	@r5, rh1	!Treiber auf Eingabe!
	inb	rl1, @r4	!Scheineingabe!
	cpb	rl0, #%04	!Status 3 Daten einlesen ?!
	jr	nz, error	!nein, Fehler!

	ldctl	r1, FCW
	ldctl	FCW, r6
!$segmented!
	inirb	@r8, @r4, r3	!inirb rr8, @r4, r3!
	ldctl	FCW, r1
!$nonsegmented!
	ret

error:	inb	rl2, @r4	!Fehlercode einlesen!
	ldb	rh2, #%0
	ret
    end DISK
 
 
!Initialisierung der Ports und des Controlers!
 
  internal

    kc array [* byte] := [%0,%21,%22,%08,%18,%f8]
 
    init array [* byte] := [
	PIOBC, INTVEC		!Int Vektor holt PIO Port aus Reset!
	PIOBD, WDATA		!Reset wegnehmen, Richtung auf Write!
	PIOBC, BITMODE		!Port B auf Bitmode programmieren!
	PIOBC, %87		!bit 7,2,1,0 auf Eingabe!
		]

    _disk_in procedure
      entry
	ld	r2, #PIOBD
	inb	rl1, @r2	!PIOBD schon initialisiert ?!
	bitb	rl1, #5
	ret	z		!ja!
	lda	r1, init
	ldk	r0, #(sizeof init)/2	!Anzahl der Ausgaben!
iloop:	ldb	rl2, @r1	!low Portadresse!
	inc	r1, #1
	outib	@r2, @r1, r0
	jr	nov, iloop
	ret
    end _disk_in

  internal
	array [%ae byte]	!auffuellung auf %1f0 byte!
 
end disk
