!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul mch.s
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:11
	Version:	1.1
 
*******************************************************************************
 
	Boot Proceduren die nicht in C geschrieben werden
	koennen.
 
*******************************************************************************
******************************************************************************!
 

 
mch_s module

$nonsegmented

  external
 
	_bsbase long
	_currclk word
	_isseg word
	_manboot word
	_serial long
	_numterm word
	_buffer array [%200 byte]
 
 
  constant
 
	SCR := %FFC1
	SBR := %FFC9
	NBR := %FFD1
	LOFF := %FFB9
	LON :=  %FFD9

  constant
 
	STACK_MMU := %00F6
	DATA_MMU := %00FA
	CODE_MMU := %00FC

  constant
 
	SEGMENT_00 := %0000
	SEGMENT_03 := %0003
	SEGMENT_62 := %003E
	SEGMENT_63 := %003F
 
	BREAK := %00
	STACK := %F800
	MEMBOOT:= %0300		!030000 Beginn boot im RAM!
 
  constant	!System Calls!
 
	TTIN  := %04
	TTWR  := %06
	DISK  := %1E
	FDISK := %24
 
 
 
  global
 
	_codemmu word := CODE_MMU
	_datammu word := DATA_MMU
	_stakmmu word := STACK_MMU
	_myseg word   := SEGMENT_63	!Segm Nr boot Progr!
	_srcseg word  := SEGMENT_62
	_hisseg word  := SEGMENT_00	!Segm Nr fuer nichtseg Progr!
 
	_ups	long := 0	!User Programm Status 4 word!
	_entry	long := 0
 
  internal
 
	SYSADR array [11 word] := [
		_exit _read _write _open _close _lseek
		_loadit _getchar _putchar _getc _getw]
 
 
!*------------------------------------------ Proceduren ---------------------*!
 
 
!Programm Boot wird in Segment 0 geladen und dann nach Segment 63 umgeladen!
!Segment 0 ab Adresse %8000, Monitor liegt ab %0000!
 
  global
 
    start procedure
      entry
	jr	start1
    end start
 
!*---------------------------------------------------------------------------*!
!*
 * System Entry Point
 * Fuer unter boot laufende Anwenderprogramme koennen
 * ueber einen call <SEGMENT_63>%8002 die Funktionen aus dem
 * Modul sys.c benutzt werden.
 * Anwenderprogramme muessen dann aber im nichtsegmentierten
 * Mode laufen
 * Input: in r1 Funktionsnummer, ab r7 abwaerts die Parameter
 *!
!*---------------------------------------------------------------------------*!
 
!$segmented!
 
    sysentry procedure
      entry
	ld	r0,#%5800
	ldctl	FCW, r0
 
!$nonsegmented!
 
	push	@r15, r1
	cpb	rl1, #1		!read ?!
	jr	nz, sys0
	push	@r15, r6
	lda	r6, _buffer
	jr	sys4
 
sys0:	cpb	rl1, #2		!write ?!
	jr	nz, sys1
	ld	r4, r6		!quell adr!
	lda	r6, _buffer
	jr	sys2
 
sys1:	cpb	rl1, #3		!open ?!
	jr	nz, sys3
	ld	r4, r7
	lda	r7, _buffer
 
sys2:	pushl	@r15, rr6
	push	@r15, r5
	push	@r15, r3
	push	@r15, r1
	ld	r7, _myseg	!Zielsegm!
	ld	r5, _hisseg	!Quellsegm!
	lda	r6, _buffer	!Quell offset!
	ld	r3, #%200	!laenge!
	calr	_scopy
	pop	r1, @r15
	pop	r3, @r15
	pop	r5, @r15
	popl	rr6, @r15
 
sys3:	push	@r15, r6
sys4:	addb	rl1, rl1
	ldb	rh1, #0
	ld	r1, SYSADR(r1)
	call	@r1		!call sys programm!
 
	pop	r6, @r15
	pop	r1, @r15
	cpb	rl1, #1		!read ?!
	jr	nz, sys5
 
	ld	r7, _hisseg	!user segment nr!
	ld	r5, _myseg
	lda	r4, _buffer
	ld	r3, #%200	!laenge!
	calr	_scopy		!move ins user segment!
 
sys5:	ld	r0, #%C000
	ldctl	FCW, r0
 
!$segmented!
 
	ret
    end sysentry
 
!*---------------------------------------------------------------------------*!
!*
 * Start des Boot Programms, Initialisierung MMU's und Break Register
 * Boot Programm von Segment 0 nach Segment 63 umladen
 * Aufruf von main im Modul boot, Absprung zu einem geladenen
 * Anwenderprogramm
 *!
!*---------------------------------------------------------------------------*!
 
!$nonsegmented!
 
  internal
 
    start1 procedure
      entry
	ld	r0, #%4000	!Nichtsegm. & System Mode!
	ldctl	FCW, r0		!setzen!
	ld	r15, #STACK	!Stack offset setzen!
 
!init ports und MMU!
!Befehl 15: Set all cpu Inhibit Flags = alle Segmente fuer CPU gesperrt!
!fuer alle 3 MMU's!
	soutb	%1500+CODE_MMU, rl0
	soutb	%1500+DATA_MMU, rl0
	soutb	%1500+STACK_MMU, rl0
 
!Descriptor Counter Register = 0!
!= Zugriff auf hoeherwertiges Byte des Basisadr Feldes!
	ldk	r0, #%0
	soutb	%2000+CODE_MMU, rl0
	soutb	%2000+DATA_MMU, rl0
	soutb	%2000+STACK_MMU, rl0
 
	ldb	rl0, #BREAK
	outb	SBR, rl0
	outb	NBR, rl0
 
	pushl	@r15, rr6	!ZBOOT Parameter retten!
	pushl	@r15, rr4
 
!Aufbau von MMU init Daten im Stack!
	push	@r15, #%FF00	!FF+1 *256 = Laenge des Segmentes (64k)!
				!00 = Attribute (Reset CPU Inhibit)!
	push	@r15, #%0000	!Basisadr = 000000!
	ldk	r6, #SEGMENT_00	!fuer Segment 0!
	ld	r5, r15		!Adr Daten im Stack!
	calr	init_mmus	!Ausgabe an alle 3 MMU'S!
 
	ld	@r15, #MEMBOOT	!Basisadr 030000, Laenge,Attribute wie vor!
				!Beginn der echten MEM Adr fuer boot!
	ld	r6, #SEGMENT_63
	ld	r5, r15
	calr	init_mmus
 
	inc	r15, #%04
 
!Monitor und Boot programm von segment 0 nach 03!
	ld	r7, #SEGMENT_03
	ldk	r6, #%0
	ld	r5, r6
	ld	r4, r6
	lda	r3, _edata	!Beginn bss!
	calr	_scopy		!Transfer Boot!
 
	popl	rr4, @r15	!Rueckladen ZBOOT Parameter!
	popl	rr6, @r15
 
!MMU's scharf machen!
 
	ldb	rl0, #%D1	!Mode Register!
				!Identification fuer Segmentrap AD8+1!
				!untere 64 Segment Nummern!
				!MMU Translate (Adressbildung) ein!
				!MMU aktiv!
				!MMU nur im System Mode (MST=1, NMS=0)!
				!aktiv, N/S pin wird aber fuer Chipselekt!
				!benutzt!
	soutb	CODE_MMU, rl0
	inc	r0, #%01
	soutb	DATA_MMU, rl0	!daten D2!
				!Identifier AD8+2 sonst wie vor!
	inc	r0, #%01
	soutb	STACK_MMU, rl0	!daten D3!
				!Identifier AD8+3 sonst wie vor!
 
	ldb	rl0, #%03
	outb	SCR, rl0
 
!Sprung ins Segment 63, (ehemals Segment 3)!
 
	ld	r2, #SEGMENT_63	!segment nr 63!
	exb	rh2, rl2
	lda	r3, start2
	ld	r0, #%C000
	ldctl	FCW, r0		!segmented & system mode!
!$segmented!
	ld	r14, r2		!system r14 Stackpointer segmentnr auf 63!
	ldctl	PSAPSEG, r2	!segm nr PSA Pointer auf 63!
	jp	@r2		!Sprung zu start2 Segment 63!
!	jp	@rr2		segmentiert!
 
!$nonsegmented!
 

!Programm wird ab hier in Segment 63 abgearbeitet!
 
!in PSA Segmentnummern von 0 auf 63 aendern!
 
start2:
	ld	r0, #%4000
	ldctl	FCW, r0
 
	ldctl	r1, PSAP
	ld	r0, #SEGMENT_63
	exb	rl0, rh0
	ldb	rl2, #6
	inc	r1, #12
start3:
	ld	@r1, r0
	inc	r1, #8
	dbjnz	rl2, start3
 
	ldb	rl2, #128
start4:
	ld	@r1, r0
	inc	r1, #4
	dbjnz	rl2, start4
 
	ld	r0, #%5800
	ldctl	FCW, r0		!Nichtsegm & System mode !
				!interrupts ei!
 
!init bss RAM mit %0!
	lda	r1, _edata
	lda	r2, %02(r1)
	lda	r0, _end
	sub	r0, r1
	resflg	c
	rrc	r0, #%01
	clr	@r1
	dec	r0, #%01
	ldir	@r2, @r1, r0
 
	ldl	_serial, rr6	!Monitor Parameter speichern!
	ld	_numterm, r5
	ld	_manboot, r4	!0=autom. boot!
	ldk	r0, #%0
	ld	r1, #MEMBOOT
	slll	rr0, #%08	!*256 030000 = Beginn boot!
	ldl	_bsbase, rr0
    end start1	!Programm lauft nach __ret weiter!
 
!*---------------------------------------------------------------------------*!
!*
 * Rueckkehr aus User Programm (ueber exit in sys.c)
 *!
!*---------------------------------------------------------------------------*!
 
  global
 
    __ret procedure
      entry
	ld	r15, #STACK	!Systemstack!
	ldk	r0, #0		!Basis Adr fuer MMU's init!
				!ab dieser (echten) Adresse wird ein
				User Programm geladen, currclk wird dann
				um die Laenge des Programms erhoeht!
	ld	_currclk, r0
 
	call	_main		!Kommandoeingabe!
				!+ Programm laden zB Kern!
	xor	r0, r0
	ld	r1, r0
	test	_isseg		!0=nonsegmented 1=segmented!
	tcc	nz, r1
	rr	r1, #%01
	or	r1, #%5800	!fcw =system mode + entsp segm mode!
	ld	r4, #SEGMENT_63	!Segment Nr fuer ldps Befehl!
	exb	rh4, rl4
	lda	r5, _ups
	ldl	@r5, rr0	!fuer user Programm (geladenes Prog)
				fcw setzen in _ups!
 
!fuer Return zu boot den Programmstatus in den Stack!
 
	push	@r15, #__ret	!Return Adr!
	push	@r15, r4	!Return Segment Nr!
	push	@r15, #%5800	!fcw fuer Return!
	push	@r15, #0	!unbenutzt im Prog Status!

	ld	r3, #-1
wait:	djnz	r3, wait	!Warten auf letzten Interrupt!
 
	ldl	rr8, rr4
	ldl	rr6, _serial	!Rueckladen ZBOOT Parameter!
	ld	r5, _numterm
	ld	r4, _manboot
	ld	r3, _currclk	!Laenge des Programms in clicks!
	ld	r2, #0		!kein FPP!
	ld	r0, #%C000
	ldctl	FCW, r0		!segment. & system mode!
!$segmented!
	ldps	@r8
!	ldps	@rr8		segmented!
				!Sprung ins geladene Programm
				durch laden Programm Status Register
				in r8 boot Segm Nr, in r9 Adr
				von User Programm Status array!
    end __ret

!*---------------------------------------------------------------------------*!
!*
 * Ausgabe an alle 3 MMU's
 * (fuer Initialisierung)
 *!
!*---------------------------------------------------------------------------*!
!$nonsegmented!
 
  internal
 
    init_mmus procedure
      entry
	ld	r8, r5
	ldb	rl7, #CODE_MMU
	calr	_loadsd
	ldb	rl7, #DATA_MMU
	ld	r5, r8
	calr	_loadsd
	ldb	rl7, #STACK_MMU
	ld	r5, r8
	calr	_loadsd
	ret	
    end init_mmus

  global
 
!*---------------------------------------------------------------------------*!
!*
 * Programmierung einer MMU
 *!
!*---------------------------------------------------------------------------*!
 
    _loadsd procedure
      entry
	ldb	rh7, #%01	!Ausgabe Segmentadr Register!
	
	dec	r15, #2
	ld	r1, r15
	ldb	@r1, rl6	!Daten in Stack!
	soutib	@r7, @r1, r0
	inc	r15, #2
	ldb	rh7, #%0B	!Ausgabe Segmentdescriptor 4 Byte!
	ldk	r2, #%04	!4 Byte ausgeben!
	sotirb	@r7, @r5, r2
	ret	
    end _loadsd

!*---------------------------------------------------------------------------*!
!*
 * Copy (LDIR) von einem Segment in ein anderes
 *!
!*---------------------------------------------------------------------------*!
 
    _scopy procedure
      entry
	exb	rh7, rl7
	exb	rh5, rl5
	ex	r7, r6
	ex	r5, r4
	resflg	c
	rrc	r3, #%01
	ldctl	r0, FCW		!FCW retten!
	ld	r1, #%C000
	ldctl	FCW, r1		!segment & system mode!
!$segmented!
	ldir	@r6, @r4, r3
!	ldir	@rr6, @rr4, r3	segmentiert!
	ldctl	FCW, r0		!nichtsegm & system mode!
!$nonsegmented!
	ret	
    end _scopy

!*---------------------------------------------------------------------------*!
!*
 * Entry fuer printf (in misc.c)
 * Parameter in Stack bringen
 *!
!*---------------------------------------------------------------------------*!
 
    _printf procedure
      entry
	ex	r2, @r15
	push	@r15, r3
	push	@r15, r4
	push	@r15, r5
	push	@r15, r6
	ld	r6, r15
	push	@r15, r2
	call	_printfv
	ld	r2, @r15
	inc	r15, #%0C	!Korrektur Stack!
	jp	@r2		!return!
    end _printf
 
!*---------------------------------------------------------------------------*!
!*
 * Ausgabe ein Zeichen (in rl7) an Console
 * ueber Systemcall an Monitor- Programm
 *!
!*---------------------------------------------------------------------------*!
 
    _putchar procedure	!Ausgabe rl7 auf Console!
      entry
	push	@r15, r0
	push	@r15, r3
	ld	r2, r7
	cpb	rl7, #%0A
	jr	nz, PUTCHAR1
	ldk	r0, #%0D
	sc	#TTWR		!bei LF CRLF ausgeben!
PUTCHAR1:
	ld	r0, r2
	sc	#TTWR
	pop	r3, @r15
	pop	r0, @r15
	ret	
    end _putchar

!*---------------------------------------------------------------------------*!
!*
 * Eingabe ein Zeichen (nach r2) von Console
 * durch Systemcall an Monitor-Programm
 *!
!*---------------------------------------------------------------------------*!
 
    _getchar procedure
      entry
	push	@r15, r0
	push	@r15, r3
	outb	LOFF, rl0	!lbolt off!
	sc	#TTIN
	outb	LON, rl0	!lbolt on!
	ld	r0, r3
	and	r0, #%7F	!Zeichen eingelesen, reset bit 7!
	cp	r0, #%0D
	jr	nz, GETCHAR1
	ldk	r0, #%0A
GETCHAR1:
	ld	r2, r0
	ld	r7, r2
	calr	_putchar
	pop	r3, @r15
	pop	r0, @r15
	ret	
    end _getchar
 
!*---------------------------------------------------------------------------*!
!*
 * Realisierung von in und out Befehlen fuer C
 *!
!*---------------------------------------------------------------------------*!
 
    _otirb !@r7 @r6 r5! procedure
      entry
	otirb	@r7, @r6, r5
	ret	
    end _otirb

    _outb !@r7 rl6! procedure
      entry
	outb	@r7, rl6
	ret	
    end _outb !@r7 rl6!

    _inb !rl2 @r7! procedure
      entry
	inb	rl2, @r7
	xorb	rh2, rh2
	ret	
    end _inb !rl2 @r7!

!*---------------------------------------------------------------------------*!
!*
 * Aufruf Disk Treiber im Monitor mit System Call
 * vorher Register umladen
 *!
!*---------------------------------------------------------------------------*!
 
    _disk procedure
      entry
	dec	r15, #6
	ldm	@r15, r8, #3
	ld	r2, _myseg	!Ersetzen physische Adresse durch!
	exb	rl2, rh2	!Segmentnummer, da kein DMA!
	ldl	rr8, rr2
	ld	r10, 8(r15)	!Laenge!
	sc	#DISK
	ldm	r8, @r15, #3
	inc	r15, #6
	ret
    end _disk
 
!*---------------------------------------------------------------------------*!
!*
 * Aufruf Floppydisk Treiber im Monitor mit System Call
 * vorher Register umladen
 *!
!*---------------------------------------------------------------------------*!
 
    _fdisk procedure
      entry
	dec	r15, #6
	ldm	@r15, r8, #3
	ld	r2, _myseg	!Ersetzen physische Adresse durch!
	exb	rl2, rh2	!Segmentnummer, da kein DMA!
	ldl	rr8, rr2
	ld	r10, 8(r15)	!Laenge!
	orb	rl6, #%60	!Drivenr. + Typ 6!
	sc	#FDISK
	ldm	r8, @r15, #3
	inc	r15, #6
	ret
    end _fdisk
 
end mch_s
