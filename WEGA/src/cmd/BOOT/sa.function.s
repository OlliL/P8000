!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul sa.function.s
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:56
	Version:	1.1
 
*******************************************************************************
 
 Dieser Modul gestattet die Benutzung der Boot Systemfunktionen
 von einem User Programm aus. Dieser Modul wird zusammen mit
 dem Modul misc.c zur libb.a
 
*******************************************************************************
******************************************************************************!
 

sa_function_s module
 
$nonsegmented
 
 
  external
 
	_printfv procedure
 
  global
 
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
	inc	r15, #12
	jp	@r2
    end _printf
 
    _exit procedure
      entry
	ldb	rl1, #0
	jr	system
    end _exit
 
    _read procedure	!read nach Segment 0!
      entry
	ldb	rl1, #1
 	jr	system
    end _read
 
    _write procedure	!von Segment 0!
      entry
	ldb	rl1, #2
	jr	system
    end _write
 
    _open procedure
      entry
	ldb	rl1, #3
	jr	system
    end _open
 
    _close procedure
      entry
	ldb	rl1, #4
	jr	system
    end _close
 
    _lseek procedure
      entry
	ldb	rl1, #5
	jr	system
    end _lseek
 
    _loadit procedure
      entry
	ldb	rl1, #6
	jr	system
    end _loadit
 
    _getchar procedure
      entry
	ldb	rl1, #7
	jr	system
    end _getchar
 
    _putchar procedure
      entry
	ldb	rl1, #8
	jr	system
    end _putchar
 
    _getc procedure
      entry
	ldb	rl1, #9
	jr	system
    end _getc
 
    _getw procedure
      entry
	ldb	rl1, #10
    end _getw
 
  internal
 
    system procedure
      entry
	ld	r0, #%C000
	ldctl	FCW, r0
!$segmented!
	wval	%5F00		!call <3F>%8002!
	wval	%BF00
	wval	%8002
	ld	r0, #%5800
	ldctl	FCW, r0
!nonsegmented!
	ret
    end system


!*--------------------------------------------------------*!
!*
 * Realisierung von in und out Befehlen fuer C
 *!
!*--------------------------------------------------------*!

  global
 
    _otirb !@r7, @r6, r5! procedure
      entry
	otirb	@r7, @r6, r5
	ret	
    end _otirb

    _outb !@r7, rl6! procedure
      entry
	outb	@r7, rl6
	ret	
    end _outb

    _inb !rl2, @r7! procedure
      entry
	inb	rl2, @r7
	xorb	rh2, rh2
	ret	
    end _inb

    _out !@r7, r6! procedure
      entry
	out	@r7, r6
	ret	
    end _out

    _in !r2, @r7! procedure
      entry
	in	r2, @r7
	ret	
    end _in

end sa_function_s
