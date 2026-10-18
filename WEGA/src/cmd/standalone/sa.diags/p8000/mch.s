!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul mch.s
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:11
	Version:	1.2
 
*******************************************************************************
 
	Boot Proceduren die nicht in C geschrieben werden
	koennen.
 
*******************************************************************************
******************************************************************************!
 

 
mch_s module

$nonsegmented

  external
 
	_numterm word

 
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
 
 
 

 
 
!*------------------------------------------ Proceduren ---------------------*!
 
 
!Programm Boot wird in Segment 0 geladen und dann nach Segment 63 umgeladen!
!Segment 0 ab Adresse %8000, Monitor liegt ab %0000!
 
  global

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
	sc	#TTIN
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
	ld	r2, SEGMENT_00	!Ersetzen physische Adresse durch!
	exb	rl2, rh2	!Segmentnummer, da kein DMA!
	ldl	rr8, rr2
	ld	r10, 8(r15)	!Laenge!
	sc	#DISK
	ldm	r8, @r15, #3
	inc	r15, #6
	ret
    end _disk
 
end mch_s
