!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone:	sa.fdisk.s fuer P8000-WDC
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:53
	Version:	1.1
 
*******************************************************************************
******************************************************************************!
 

 
fdisk module

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

	READ	:= %01		!Read mit Sektoradresse!
	FORMAT	:= %04		!Formatieren einer Spur!

  internal

rbuffer array [512 byte]
 
 
!------------------------------------------------------------------!
! Eingabeparameter:   r7      .... Request-Code (1=Read, 3=Format) !
!		      r6      .... Drivenummer			   !
!		      r5      .... Kopf				   !
!		      r4      .... (Anzahl der Sektoren)	   !
!		      r3      .... Sektornummer (fuer Read)	   !
!		      r2      .... Zylindernummer		   !
! Returncode in r2						   !
!------------------------------------------------------------------!
 
  global
 
    _fdisk procedure
      entry
	push	@r15, r5
	push	@r15, r2
	calr	_disk_in	!Hardware initialisieren!
 
	ld	r4, #PIOAD
	ld	r5, #PIOBD
	ld	r0, #WDATA+OUTPUT
	outb	@r5, rh0	!Datentreiber auf Ausgabe!
	outb	PIOAC, rl0	!Daten-PIO auf Ausgabe!

wready:	inb	rl0, @r5
	bitb	rl0, #0
	jr	nz, wready	!Warten bis WDC bereit!

!WDC-Kommando ausgeben!

	cpb	rl7, #3		!Format ?!
	jr	nz, read	!Requestkode ist schon 01!
	ldb	rl7, #FORMAT
read:	outb	@r4, rl7	!Kommando ausgeben!
	outb	@r4, rl6	!Laufwerknummer!
	pop	r1, @r15	!Cylinder!
	outb	@r4, rl1	!Cylinder low!
	outb	@r4, rh1	!Cylinder high!
	pop	r1, @r15	!Kopf!
	outb	@r4, rl1
	outb	@r4, rl3	!Sektor!
	ld	r3, #%200	!Laenge!
	outb	@r4, rl3
	outb	@r4, rh3
	outb	@r4, rh6	!Pruefsumme, nicht benutzt!

wwait0:	inb	rl0, @r5	!Warten bis WDC Status 0!
	bitb	rl0, #0
	jr	z,wwait0

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

	lda	r6, rbuffer
	inirb	@r6, @r4, r3
	ret

error:	inb	rl2, @r4	!Fehlercode einlesen!
	ret
    end _fdisk
 
 
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
end fdisk
