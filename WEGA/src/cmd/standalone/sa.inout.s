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
 

sa_inout_s module
 
$nonsegmented


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

end sa_inout_s
