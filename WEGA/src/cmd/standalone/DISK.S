!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
 
	PROM	Modul:	DISK.S fuer P8000-WDC
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
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

!WDC-Kommandos!

	COMMAND	:= %20		!Read/Write ab Blocknummer!
	READ	:= %21		!Read ab Blocknummer!
	WRITE	:= %22		!Write ab Blocknummer!
 
 
  global
 
!------------------------------------------------------------------!
! Eingabeparameter:   r7      .... Request-Code (1=Read, 2=Write)  !
!                     r6      .... Drive-Nr.                       !
!                     rr4     .... Blocknummer                     !
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

wready:	inb	rl0, @r5
	bitb	rl0, #0
	jr	nz, wready	!Warten bis WDC bereit!

!WDC-Kommando ausgeben!

	orb	rl7, #COMMAND
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

wwait0:	inb	rl0, @r5	!Warten bis WDC Status 0!
	bitb	rl0, #0
	jr	z,wwait0

	ld	r6, #%C000	!FCW!
	cpb	rl7, #WRITE
	jr	nz, rwwait

wwait1:	inb	rl0, @r5	!Write!
	bitb	rl0, #1		!Warten, bis WDC bereit fuer Daten!
	jr	nz, wwait1
	ldctl	r1, FCW
	ldctl	FCW, r6
!$segmented!
	otirb	@r4, @r8, r3	!otirb @r4, @rr8, r3!
	ldctl	FCW, r1
!nonsegmented!
 
wwait2:	inb	rl0, @r5	!Warten bis WDC Status 0!
	bitb	rl0, #1
	jr	z,wwait2

rwwait:	inb	rl1, @r5	!Status einlesen!
	bitb	rl1, #0
	jr	nz, rwwait	!auf Beendigung des Kommandos warten!
	inb	rl1, @r5
	andb	rl1, #%07	!Maske!
	ldk	r2, #0		!Returncode kein Fehler!
	cpb	rl1, #%06	!Status 1 = fertig, kein Fehler!
	ret	z

	ld	r0, #RDATA+INPUT
	outb	PIOAC, rl0	!Daten-PIO auf Eingabe!
	outb	@r5, rh0	!Treiber auf Eingabe!
	inb	rl0, @r4	!Scheineingabe!
	cpb	rl1, #%04	!Status 3 Daten einlesen ?!
	jr	nz, error	!nein, Fehler!

	ldctl	r1, FCW
	ldctl	FCW, r6
!$segmented!
	inirb	@r8, @r4, r3	!inirb rr8, @r4, r3!
	ldctl	FCW, r1
!$nonsegmented!
	ret

error:	inb	rl2, @r4	!Fehlercode einlesen!
	ret
    end DISK
 
 
!Initialisierung der Ports und des Controlers!
 
  internal
 
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
 
end disk
