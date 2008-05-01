!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
 
	KERN 3.2	Modul:	disk.s fuer P8000-WDC
 
	Bearbeiter:	P. Hoge
	Datum:		5.4.88
	Version:	1.3
 
*******************************************************************************
******************************************************************************!
 
disk module

$segmented

  constant

!Port Adressen!
 
	PIOAD	:= %FFA1	!Daten lesen / schreiben!
	PIOBD	:= %FFA3	!Steuerport !
	PIOAC	:= %FFA5
	PIOBC	:= %FFA7

	RETI	:= %FFE1

!PIO Programmierung!
 
	OUTPUT	:= %0F
	INPUT	:= %4F
	BITMODE	:= %CF
	INTVEC	:= %18
	PIOEI	:= %97FE
	PIODI	:= %17FF
 
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

	READ	:= %21		!Read ab Blocknummer!
	WRITE	:= %22		!Write ab Blocknummer!
 
 
  global
 
!*
 *mdcmd
 * Kommando an WDC ausgeben
 *
 * r7	Request-Code (1=Read, 2=Write)
 * r6	Drive-Nr.
 * rr4	Blocknummer
 * r3	Laenge
 *!
 
    _mdcmd procedure
      entry
	ei	vi, nvi
	ld	r1, #PIOAD
	ld	r2, #PIOBD
	ld	r0, #WDATA+OUTPUT
	outb	@r2, rh0	!Datentreiber auf Ausgabe!
	outb	PIOAC, rl0	!Datenport auf Ausgaberichtung!

wready:	inb	rl0, @r2
	bitb	rl0, #0
	jr	nz,wready	!Warten bis WDC bereit!

	ld	r0, #PIOEI	!PIO-Interrupt freigeben!
	outb	PIOBC, rh0
	outb	PIOBC, rl0
	ldk	r0, #0		!M1 fuer PIO erzeugen!
	outb	RETI, rl0
 
!WDC-Kommando ausgeben!

	ldb	rh0, #READ
	testb	rl7
	jr	nz, cmd
	ldb	rh0, #WRITE
cmd:	outb	@r1, rh0	!Kommando ausgeben!
	outb	@r1, rl6	!Laufwerknummer!
	outb	@r1, rl5	!Blocknummer low!
	outb	@r1, rh5	!Blocknummer mitte low!
	outb	@r1, rl4	!Blocknummer mitte high!
	outb	@r1, rh4	!Blocknummer high!
	outb	@r1, rl3	!Laenge!
	outb	@r1, rh3
	outb	@r1, rl0	!Pruefsumme, nicht benutzt!
	testb	rl7
	ret	nz		!Read!

wwait:	inb	rl0, @r2	!Write!
	bitb	rl0, #1		!Warten, bis WDC bereit fuer Daten!
	jr	nz, wwait
	ret
    end _mdcmd

!*
 *mdin
 *!
    _mdin procedure
      entry
	ei	vi, nvi
	ld	r4, #PIOAD
	inirb	@rr6, @r4, r5
	di	vi, nvi
	ret
    end _mdin

!*
 *mdout
 *!
    _mdout procedure
      entry
	ld	r4, #PIOAD
	otirb	@r4, @rr6, r5
	ret
    end _mdout
 
!*
 *mdint2
 * Return: r2 Fehlercode
 *!
    _mdint2 procedure
      entry
	ld	r0, #PIODI	!Interrupt PIO sperren!
	outb	PIOBC, rh0
	outb	PIOBC, rl0
	ld	r4, #PIOAD
	ld	r5, #PIOBD
	ldk	r2, #0		!Returncode kein Fehler!
	inb	rl1, @r5	!Status einlesen!
	comb	rl1		!logisch negieren!
	andb	rl1, #%07	!Maske!
	cpb	rl1, #%01	!Status 1 = fertig, kein Fehler!
	ret	z

	ld	r0, #RDATA+INPUT
	outb	PIOAC, rl0	!Daten-PIO auf Eingabe!
	outb	@r5, rh0	!Treiber auf Eingabe!
	inb	rl0, @r4	!Scheineingabe!
	cpb	rl1, #%03	!Fehler ?!
	ret	z		!nein!

error:	inb	rl2, @r4	!Fehlercode einlesen!
	ret
    end _mdint2
 
 
!Initialisierung der Ports und des Controlers!
 
  internal
 
    init array [* byte] := [
	PIOBC, INTVEC		!Int Vektor holt PIO Port aus Reset!
	PIOBD, WDATA		!Reset wegnehmen, Richtung auf Write!
	PIOBC, BITMODE		!Port B auf Bitmode programmieren!
	PIOBC, %87		!bit 7,2,1,0 auf Eingabe!
		]

  global
 
    _mdinit procedure
      entry
	ld	r5, #PIOBD
	lda	rr2, init
	ldk	r0, #(sizeof init)/2	!Anzahl der Ausgaben!
iloop:	ldb	rl5, @rr2		!low Portadresse!
	inc	r3, #1
	outib	@r5, @rr2, r0
	jr	nov, iloop
	ret
    end _mdinit
end disk
