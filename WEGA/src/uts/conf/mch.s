!******************************************************************************
*******************************************************************************

       W E G A - Quelle      (C) ZFT/KEAW Abt. Basissoftware - 1986

       KERN 3.2  Modul:	mch.s

       Bearbeiter:	J. Zabel, P. Hoge
       Datum:		10.11.88
       Version:		1.4

*******************************************************************************
******************************************************************************!

mch module
	
$segmented

!
Von mch werden benutzt:

Segment %3B:	Zugriff zum User-Bereich (DATA-/STACK-MMU)
Segment %3C:	physischer Zugriff
Segment %3D:	physischer Zugriff
Segment %3F:	nichtsegmentierte User-Programme
!

  global

_serial		long := %0	!Seriennummer!
_numterm	word := %0	!Anzahl der Terminals!
_udos		word := %0	!00-kein 8-Bit-Teil!
				!01-BFOS verfuegbar!
				!02-UDOS verfuegbar!
_manboot	word := %0	!Merker:manuelles Boot (0-nein)!
_kclicks	word := %0	!Merker: Groesse des Kerns (in Clicks)!
_nofault	long := %0	!Rueckkehradresse!

_seg3b		byte := -1

_mmut		word := CODE_MMU
_mmud		word := DATA_MMU
_mmus		word := STACK_MMU

  internal

MMUINIT		array [4 BYTE] := [%00 %00 %00 %00]
PSTAT		array [4 word] := [%0000 %1800 %3F00 %0000]

!* Hardwareadressen *!

  constant

STACK_MMU	:= %00F6
DATA_MMU	:= %00FA
CODE_MMU	:= %00FC

NBREAK		:= %FFD1	!Normal Break Register!
TRAP_LOW	:= %FFF1
TRAP_PC_LOW	:= %FFF9
LON		:= %FFD9	!LED lbolt on!
LOFF		:= %FFB9	!LED lbolt off!

!* Sonstiges *!

  constant

sp := r15


  global

!------------------------------------------------------------------------------
	start ()
Entry Point des WEGA-Kerns (Init MMU, Stackpointer, PSAP)
Input:	rr6 - Seriennummer
	r5  - Anzahl der Terminals
	r4  - Merker: manuelles Boot (0-nein)
	r3  - Groesse Code-, Data- und BSS-Bereich des Kerns (in Clicks)
	r2  - Merker: Floating Point Prozessor im System (0-nein)
------------------------------------------------------------------------------!
start procedure
    entry
	ldl	_serial, rr6	!Monitorparameter uebernehmen!
	ldb	_numterm+1, rl5
	ldb	_udos+1, rh5
	ld	_manboot, r4
	ldb	rh1, #%3e	!Segmentnummer des Stacks!
	soutb	CODE_MMU+%0100, rh1 !Write SAR mit Segmentnr. %3E!

	add	r3, #%000a	!Groesse der User-Struktur in Clicks (USIZE)
				 addieren!
	ld	_kclicks, r3	!Gesamtgroesse des Kerns in Clicks!
	ld	r0, r3
	sub	r0, #%0100	!Wert fuer Basisadressen-Feld des SDR %3E!
	ldb	rh1, #%f5	!Wert fuer Limit-Feld des SDR %3E
				 (%F5 Bloecke a 256 Byte)!
	ldb	rl1, #%22	!Wert fuer Attribut-Feld des SDR %3E
				 (DIRW - Segment wird in fallender Adress-
				 richtung beschrieben; SYS - Zugriff auf das
				 Segment nur im Systemmode)!
	lda	rr4, MMUINIT	!Adr. des Feldes f. MMU-Initialisierungswerte!
	ldl	@rr4, rr0	!MMU-Initialisierungswerte abspeichern!
	ld	r1, #CODE_MMU+%0B00 !Write SDR!
	ldk	r0, #4		!4 Byte ausgeben!
	sotirb	@r1, @rr4, r0	!Write SDR Segment %3E (CODE_MMU)!

	ldl	rr14, #%3e00fffe	!Stackpointer laden!

	ldb	rl0, #%d3	!Wert fuer Mode-Register!
	soutb	STACK_MMU, rl0	!Write Mode-Register (STACK_MMU)
				 (ID=3; MST; TRNS; MSEN)!

	lda	rr2, _u		!User-Struktur + Stackbereich loeschen
				 	 <%3E> %F600 ... <%3E> %FFFF!
L_0438:
	clr	@rr2
	inc	r3, #2
	jr	nz, L_0438

	lda	rr0, _psa	!PSAP laden (<0> %0000)!
	ldctl	psapseg, r0
	ldctl	psapoff, r1
	call	_main		

	lda	rr2, PSTAT
	ldps	@rr2		!Programm-Status laden!
				!Programmfortsetzung bei angeg. PC-Stand
				 <%3F>%0000 (nichtsegmentiert)!
end start

!------------------------------------------------------------------------------
	loadsd ((char)addr, (char)segnr, (long)pointer)
Ein SDR einer MMU laden
Input:	rl7 - MMU-Adresse
	rl6 - Segment-Nummer
	rr4 - Zeiger auf Datenfeld (4 Byte), das die SDR-Werte fuer das Segment
	      <rl6> enthaelt
------------------------------------------------------------------------------!
_loadsd procedure
    entry
	ldctl	r1, fcw
	ldb	rl2, #-1	!Segment 3b nicht aktuell setzen!
	ldb	_seg3b, rl2
	andb	rl6, #%3f
	ldb	rh7, #%01	!Write SAR!
	lda	rr2, MMUINIT
	di	vi,nvi
	ldb	@rr2, rl6
	soutib	@r7, @rr2, r0	!Write SAR mit Segmentnr. <rl6>!
	ldb	rh7, #%0B	!Write SDR!
	ldk	r0, #4		!4 Byte ausgeben!
	sotirb	@r7, @rr4, r0	!Write SDR des Segments <rl6>!
	ldctl	fcw, r1
	ret	
end _loadsd

!------------------------------------------------------------------------------
	getsd ((char)addr, (char)segnr, (long)pointer)
Ein SDR einer MMU lesen
Input:	rl7 - MMU-Adresse
	rl6 - Segment-Nummer
	rr4 - Zeiger auf ein Feld (4 Byte), in dem die SDR-Werte des Segments
	      <rl6> abgespeichert werden sollen
------------------------------------------------------------------------------!
_getsd procedure
    entry
	ldctl	r1, fcw
	andb	rl6, #%3f
	ldb	rh7, #%01	!Write SAR!
	lda	rr2, MMUINIT
	di	vi,nvi
	ldb	@rr2, rl6
	soutib	@r7, @rr2, r0	!Write SAR mit Segmentnummer <rl6>!
	ldb	rh7, #%0B	!Read SDR!
	ldk	r0, #4		!4 Byte einlesen!
	sinirb	@rr4, @r7, r0	!Read SDR des Segments <rl6>!
	ldctl	fcw, r1
	ret	
end _getsd

!------------------------------------------------------------------------------
	invsdrs ()
Setzen aller CPU-Inhibit-Flags der DATA- und STACK-MMU 
(im Attribut-Feld der SDR's)
------------------------------------------------------------------------------!
_invsdrs procedure
    entry
	ldctl	r1, fcw
	di	vi,nvi
	soutb	DATA_MMU+%1500, rh0
	soutb	STACK_MMU+%1500, rh0
	ldctl	fcw, r1
	ret	
end _invsdrs

!------------------------------------------------------------------------------
addupc 
------------------------------------------------------------------------------!
_addupc procedure
    entry
	pushl	@rr14, rr8
	pushl	@rr14, rr10
	push	@rr14, r12

	ldl	rr8, rr4
	ld	r12, r3
	ld	r0, rr8(#%0006)
	sub	r7, r0
	resflg	c
	rrc	r7
	ld	r1, rr8(#%0008)
	resflg	c
	rrc	r1
	mult	rr0, r7
	sral	rr0, #14
	inc	r1
	res	r1, #0
	ld	r2, rr8(#%0004)
	cp	r1, r2
	jr	nc, L_051e
	ld	r7, rr8(#%0002)
	add	r7, r1
	ld	r6, @rr4
	ldl	rr10, rr6
	ldl	rr4, rr14
	dec	sp, #2
	calr	_fuword
	pop	r5, @rr14
	test	r2
	jr	nz, L_0518

	add	r5, r12
	ldl	rr6, rr10
	calr	_suword
	test	r2
	jr	z, L_051e

L_0518:
	sub	r0, r0
	ld	rr8(#%0008), r0

L_051e:
	pop	r12, @rr14
	popl	rr10, @rr14
	popl	rr8, @rr14
	ret	
end _addupc

!------------------------------------------------------------------------------
int	nsseg (???)
???
Input:	r7 - ???
Output: r2 - %3F00, wenn ???
	     %7F00, wenn ???
------------------------------------------------------------------------------!
_nsseg procedure
    entry
	inb	rl2, NBREAK
	cpb	rh7, rl2
	jr	ugt, L_0534
	ld	r2, #%3f00
	ret	

L_0534:
	ld	r2, #%7f00
	ret	
end _nsseg

!------------------------------------------------------------------------------
	mapustk ()
Vorbereitung des Zugriffs auf den User-Stack (STACK-MMU, Segment %3F) im
Systemmode (ueber CODE-MMU, Segment %3B)
------------------------------------------------------------------------------!
_mapustk procedure
    entry
	ldb	rh6, #%7f
	calr	mmu3b
	ldctl	fcw, r1
	ret	
end _mapustk

!------------------------------------------------------------------------------
long	physaddr ((long)logaddr)
Umwandlung 23-Bit logische Adresse in 24-Bit physische Adresse
Input:	rr6 - logische Adresse
Output:	rr2 - physische Adresse
------------------------------------------------------------------------------!
_physaddr procedure
    entry
	ldctl	r1, fcw
	andb	rh6, #%3f
	sub	r2, r2
	ld	r3, r2
	di	vi,nvi
	soutb	CODE_MMU+%0100, rh6	!Write SAR mit Segmentnummer <rh6>!
	sinb	rl2, CODE_MMU+%0800	!Read Basisadressen-Feld!
	sinb	rh3, CODE_MMU+%0800
	sub	r6, r6
	addl	rr2, rr6		!Offsetadr. zu r3 add. --> phys. Adr.!
	ldctl	fcw, r1
	ret	
end _physaddr

!------------------------------------------------------------------------------
void	resmmu ()
Reset Trap der MMU's (Reset Violation-Type-Register)
------------------------------------------------------------------------------!
_resmmu procedure
    entry
	soutb	CODE_MMU+%1100, rl0
	soutb	DATA_MMU+%1100, rl0
	soutb	STACK_MMU+%1100, rl0
	ret
end _resmmu

!------------------------------------------------------------------------------
long	Cbupc ()
Ermittlung des PC-Standes (Seg/Offset) beim Trap der CODE-MMU
Output:	rr2 - PC-Stand, bei dem Trap der CODE-MMU auftrat
------------------------------------------------------------------------------!
_Cbupc procedure
    entry
	sinb	rh3, CODE_MMU+%0700	!PC-Offset High-Byte!
	inb	rl3, TRAP_PC_LOW	!PC-Offset Low-Byte!
	sinb	rh2, CODE_MMU+%0600	!PC-Seg!
	and	r2, #%3f00		!Segment 0-63!
	ret	
end _Cbupc

!------------------------------------------------------------------------------
long	Cvaddr ()
Ermittlung Violation-Addr (Seg/Offset) beim Trap der CODE-MMU
Output:	rr2 - addr
------------------------------------------------------------------------------!
_Cvaddr procedure
    entry
	sinb	rh3, CODE_MMU+%0400	!Violation-Addr High-Byte!
	inb	rl3, TRAP_LOW		!Violation-Addr Low-Byte!
	sinb	rh2, CODE_MMU+%0300	!Violation-Seg!
	and	r2, #%3f00		!Segment 0-63!
	ret	
end _Cvaddr

!------------------------------------------------------------------------------
long	Dbupc ()
Ermittlung des PC-Standes (Seg/Offset) beim Trap der DATA-MMU
Output:	rr2 - PC-Stand, bei dem Trap der DATA-MMU auftrat
------------------------------------------------------------------------------!
_Dbupc procedure
    entry
	sinb	rh3, DATA_MMU+%0700	!PC-Offset High-Byte!
	inb	rl3, TRAP_PC_LOW	!PC-Offset Low-Byte!
	sinb	rh2, DATA_MMU+%0600	!PC-Seg!
	and	r2, #%3f00		!Segment 0-63!
	ret	
end _Dbupc

!------------------------------------------------------------------------------
long	Dvaddr ()
Ermittlung Violation-Addr (Seg/Offset) beim Trap der DATA-MMU
Output:	rr2 - addr
------------------------------------------------------------------------------!
_Dvaddr procedure
    entry
	sinb	rh3, DATA_MMU+%0400	!Violation-Addr High-Byte!
	inb	rl3, TRAP_LOW		!Violation-Addr Low-Byte!
	sinb	rh2, DATA_MMU+%0300	!Violation-Seg!
	and	r2, #%3f00		!Segment 0-63!
	ret	
end _Dvaddr

!------------------------------------------------------------------------------
long	Sbupc ()
Ermittlung des PC-Standes (Seg/Offset) beim Trap der STACK-MMU
Output:	rr2 - PC-Stand, bei dem Trap der STACK-MMU auftrat
------------------------------------------------------------------------------!
_Sbupc procedure
    entry
	sinb	rh3, STACK_MMU+%0700	!PC-Offset High-Byte!
	inb	rl3, TRAP_PC_LOW	!PC-Offset Low-Byte!
	sinb	rh2, STACK_MMU+%0600	!PC-Seg!
	orb	rh2, #%40
	and	r2, #%7f00		!Segment 64-127!
	ret	
end _Sbupc

!------------------------------------------------------------------------------
long	Svaddr ()
Ermittlung Violation-Addr (Seg/Offset) beim Trap der Stack-MMU
Output:	rr2 - addr
------------------------------------------------------------------------------!
_Svaddr procedure
    entry
	sinb	rh3, STACK_MMU+%0400	!Violation-Addr High-Byte!
	inb	rl3, TRAP_LOW		!Violation-Addr Low-Byte!
	sinb	rh2, STACK_MMU+%0300	!Violation-Seg!
	orb	rh2, #%40
	and	r2, #%7f00		!Segment 64-127!
	ret	
end _Svaddr

!------------------------------------------------------------------------------
int	pwwarn1 ()
Test, ob Stack-Ende-Vorwarnungs-Trap der CODE-MMU vorliegt 
(d.h. es wurde in den letzten 256 Byte Block geschrieben)
Output:	r2 - Kennzeichnung, ob Vorwarnung (0-nein/1-ja)
------------------------------------------------------------------------------!
_pwwarn1 procedure
    entry
	sinb	rl0, CODE_MMU+%0200	!Violation Type Register lesen!
	jr	L_0600
end _pwwarn1

!------------------------------------------------------------------------------
int	pwwarn3 () 
Test, ob Stack-Ende-Vorwarnungs-Trap der STACK-MMU vorliegt 
(d.h. es wurde in den letzten 256 Byte Block geschrieben)
Output:	r2 - Kennzeichnung, ob Vorwarnung (0-nein/1-ja)
------------------------------------------------------------------------------!
_pwwarn3 procedure
    entry
	sinb	rl0, STACK_MMU+%0200	!Violation Type Register lesen!
L_0600:
	ldk	r2, #0		!Kennzeichnung: keine Vorwarnung!
	bitb	rl0, #5		!Bit 5=1?, d.h. Stackende-Vorwarnung!
	ret	z		!nein!
L_0606:
	andb	rl0, #%df
	tcc	z, r2		!Bit0(r2):=1, wenn Bit5(rl0)=1 war!
	ret	
end _pwwarn3

!------------------------------------------------------------------------------
char	getcbcs ()
Lesen Violation- und Bus-Cycle-Status-Register der CODE-MMU
Output: rl2 - Inhalt BCS-Register
	rh2 - Inhalt Violation-Register
------------------------------------------------------------------------------!
_getcbcs procedure
    entry
	sinb	rh2, CODE_MMU+%0200
	sinb	rl2, CODE_MMU+%0500
	ret	
end _getcbcs

!------------------------------------------------------------------------------
char	getdbcs ()
Lesen Violation- und Bus-Cycle-Status-Register der DATA-MMU
Output: rl2 - Inhalt BCS-Register
	rh2 - Inhalt Violation-Register
------------------------------------------------------------------------------!
_getdbcs procedure
    entry
	sinb	rh2, DATA_MMU+%0200
	sinb	rl2, DATA_MMU+%0500
	ret	
end _getdbcs

!------------------------------------------------------------------------------
char	getsbcs ()
Lesen Violation- und Bus-Cycle-Status-Register der STACK-MMU
Output: rl2 - Inhalt BCS-Register
	rh2 - Inhalt Violation-Register
------------------------------------------------------------------------------!
_getsbcs procedure
    entry
	sinb	rh2, STACK_MMU+%0200
	sinb	rl2, STACK_MMU+%0500
	ret	
end _getsbcs

!------------------------------------------------------------------------------
int	fpbyte ((long)physaddr, (long)dstaddr)
Byte lesen von physischer Speicheradresse
Input:	rr6 - physische Speicheradresse (24 Bit)
	rr4 - Speicheradresse, wo gelesenes Byte abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fpbyte procedure
    entry
	ldctl	r1, fcw
	di	vi,nvi
	ldb	rh6, #%3c
	soutb	CODE_MMU+%0100, rh6	!Write SAR mit Segmentnr. %3C!
	soutb	CODE_MMU+%0800, rl6	!Write Basisadressen-Feld!
	subb	rl6, rl6
	soutb	CODE_MMU+%0800, rl6
	jr	FBYTE			!Byte lesen!
end _fpbyte

!------------------------------------------------------------------------------
int	fkbyte ((long)addr, (long)dstaddr)
Read Byte von Kernel-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu lesenden Bytes
	rr4 - Speicheradresse, wo gelesenes Byte abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fkbyte procedure
    entry
	ldctl	r1, fcw
	di	vi,nvi
	jr	FBYTE
end _fkbyte

!------------------------------------------------------------------------------
int	spbyte ((long)physaddr, (char)val)
Byte schreiben auf physische Speicheradresse
Input:	rr6 - physische Speicheradresse (24 Bit)
	rl5 - zu schreibendes Byte
Output:	r2 - 0, wenn kein Fehler
             -1, wenn Fehler
------------------------------------------------------------------------------!
_spbyte procedure
    entry
	ldctl	r1, fcw
	di	vi,nvi
	ldb	rh6, #%3c
	soutb	CODE_MMU+%0100, rh6	!Write SAR mit Segmentnr. %3C!
	soutb	CODE_MMU+%0800, rl6	!Write Basisadressen-Feld!
	subb	rl6, rl6
	soutb	CODE_MMU+%0800, rl6
	jr	SBYTE			!Byte schreiben!
end _spbyte

!------------------------------------------------------------------------------
int	skbyte ((long)addr, (char)val)
Write Byte zu Kernel-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu schreibenden Bytes
	rl5 - zu schreibendes Byte
Output:	r2 - 0, wenn kein Fehler
             -1, wenn Fehler
------------------------------------------------------------------------------!
_skbyte procedure
    entry
	ldctl	r1, fcw
	di	vi,nvi
	jr	SBYTE
end _skbyte

!------------------------------------------------------------------------------
int	fubyte ((long)addr, (long)dstaddr)
Read Byte von User-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu lesenden Bytes
	rr4 - Speicheradresse, wo gelesenes Byte abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fubyte procedure
    entry
	calr	mmu3b
FBYTE:
	lda	rr2, ERROR	!Fehleradresse!
	ldl	_nofault, rr2
	ldib	@rr4, @rr6, r0	!Byte von @rr6 lesen und in @rr4 abspeichern!
	jr	NOERROR1
end _fubyte

!------------------------------------------------------------------------------
int	fuibyte (addr, (long)dstaddr)
Read Byte von User-Instruction-Bereich (nichtsegmentierte User-Programme;
CODE-MMU, Segment %3F)
Input:	r7  - Speicheradresse (Offsetadresse) des zu lesenden Bytes
	rr4 - Speicheradresse, wo gelesenes Byte abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fuibyte procedure
    entry
	ldctl	r1, fcw
	ld	r6, #%3f00	!Segmentnummer!
	di	vi,nvi
	jr	FBYTE		!Byte lesen!
end _fuibyte

!------------------------------------------------------------------------------
int	fuword ((long)addr, (long)dstaddr)
Read Word von User-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu lesenden Words
	rr4 - Speicheradresse, wo gelesenes Word abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fuword procedure
    entry
	calr	mmu3b
FWORD:
	lda	rr2, ERROR	!Fehleradresse!
	ldl	_nofault, rr2
	ldi	@rr4, @rr6, r0	!Word von @rr6 lesen und in @rr4 abspeichern!
NOERROR1:
	subl	rr2, rr2
	ldl	_nofault, rr2
	ldctl	fcw, r1
	ret	
ERROR:
	subl	rr2, rr2
	ldl	_nofault, rr2
	dec	r2		!r2 = -1!
	ldctl	fcw, r1
	ret	
end _fuword

!------------------------------------------------------------------------------
int	fuiword (addr, (long)dstaddr)
Read Word von User-Instruction-Bereich (nichtsegmentierte User-Programme;
CODE-MMU, Segment %3F)
Input:	r7  - Speicheradresse (Offsetadresse) des zu lesenden Words
	rr4 - Speicheradresse, wo gelesenes Byte abgespeichert werden soll
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_fuiword procedure
    entry
	ldctl	r1, fcw
	ld	r6, #%3f00	!Segmentnummer!
	di	vi,nvi
	jr	FWORD		!Word lesen!
end _fuiword

!------------------------------------------------------------------------------
int	subyte ((long)addr, (char)val)
Write Byte zu User-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu schreibenden Bytes
	rl5 - zu schreibendes Byte
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_subyte procedure
    entry
	calr	mmu3b
SBYTE:
	lda	rr2, ERROR	!Fehleradresse!
	ldl	_nofault, rr2
	ldb	@rr6, rl5	!Byte aus rl5 in @rr6 abspeichern!
	jr	NOERROR1
end _subyte

!------------------------------------------------------------------------------
int	suibyte (addr, (char)val)
Write Byte zu User-Instruction-Bereich (nichtsegmentierte User-Programme;
CODE-MMU, Segment %3F)
Input:	r7  - Speicheradresse (Offsetadresse) des zu schreibenden Bytes
	rl6 - zu schreibendes Byte
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_suibyte procedure
    entry
	ldctl	r1, fcw
	ldb	rl5, rl6	!rl5:=zu schreibendes Byte!
	ld	r6, #%3f00	!Segmentnummer!
	di	vi,nvi
	jr	SBYTE		!Byte schreiben!
end _suibyte

!------------------------------------------------------------------------------
int	suword ((long)addr, val)
Write Word zu User-Data/Stack-Bereich
Input:	rr6 - Speicheradresse des zu schreibenden Words
	r5  - zu schreibendes Word
Output: r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_suword procedure
    entry
	calr	mmu3b
SWORD:
	lda	rr2, ERROR	!Fehleradresse!
	ldl	_nofault, rr2
	ld	@rr6, r5	!Word aus r5 in @rr6 abspeichern!
	jr	NOERROR1
end _suword

!------------------------------------------------------------------------------
int	suiword (addr, val)
Write Word zu User-Instruction-Bereich (nichtsegmentierte User-Programme;
CODE-MMU, Segment %3F)
Input:	r7 - Speicheradresse (Offsetadresse) des zu schreibenden Words
	r6 - zu schreibendes Word
Output: r2 - 0, wenn kein Fehler
             -1, wenn Fehler
------------------------------------------------------------------------------!
_suiword procedure
    entry
	ldctl	r1, fcw
	ld	r5, r6		!r5:=zu schreibendes Word!
	ld	r6, #%3f00	!Segmentnummer!
	di	vi,nvi
	jr	SWORD		!Word schreiben!
end _suiword

!------------------------------------------------------------------------------
Folgende Routinen sind aus fpe.s uebernommen und geaendert
------------------------------------------------------------------------------!

gettext procedure
  entry
	ld	r1, rr8(#%0022)	!stafcw!
	bit	r1, #15
	jr	nz, gett1

	ld	r6, #%3f00	!non segmented user!
	ld	r3, @rr6
	subb	rl2, rl2
	ret

gett1:	calr	mmu3b		!segmented user!
	lda	rr2, ERROR
	ldl	_nofault, rr2
	ld	r4, @rr6

	subl	rr2, rr2
	ldl	_nofault, rr2
	ld	r3, r4
	ldctl	fcw, r1
	ret
end gettext

getmem procedure
  entry
	ld	r1, rr8(#%0022)	!stafcw!
	bit	r1, #15
	jpr	nz, getm1	!segmented user!
	inb	rl2, NBREAK
	cpb	rh7, rl2
	ld	r6, #%7f00
	jr	ugt, getm1	!nonsegmented, Stack-MMU!
	ldb	rh6, #%3f	!nonsegmented, Data-MMU!

getm1:	push	@rr14, r3
	calr	mmu3b
	pop	r3, @rr14

	ld	r2, r1
	lda	rr0, ERROR	!Fehleradresse!
	ldl	_nofault, rr0
	ld	r1, r2

	ldir	@rr4, @rr6, r3	!Speicherbereich kopieren (wordweise)!

	subl	rr2, rr2
	ldl	_nofault, rr2
	ldctl	fcw, r1
	ret	
end getmem

putmem procedure
  entry
	ld	r1, rr8(#%0022)	!stafcw!
	bit	r1, #15
	jpr	nz, putm1	!segmented user!
	inb	rl2, NBREAK
	cpb	rh5, rl2
	ld	r4, #%7f00
	jr	ugt, putm1	!nonsegmented, Stack-MMU!
	ldb	rh4, #%3f 	!nonsegmented, Data-MMU!

putm1:	push	@rr14, r3
	ex	r4, r6
	ex	r5, r7
	calr	mmu3b
	pop	r3, @rr14
	ld	r2, r1
	lda	rr0, ERROR	!Fehleradresse!
	ldl	_nofault, rr0
	ld	r1, r2
	ldir	@rr6, @rr4, r3	!Speicherbereich kopieren (wordweise)!
	subl	rr2, rr2
	ldl	_nofault, rr2
	ldctl	fcw, r1
	ret	
end putmem

  internal

mmu3b procedure
    entry
	ldctl	r1, fcw
	andb	rh6, #%7f
	di	vi,nvi
	cpb	rh6, _seg3b
	jr	nz, mmu0
	ld	r6, #%3b00
	ret

mmu0:	ldb	_seg3b, rh6
	push	@rr14, r4
	ld	r4, #DATA_MMU+%0100	!DATA-MMU fuer Segmentnr. 0-63!
	cpb	rh6, #%40
	jr	lt, mmu1
	ld	r4, #STACK_MMU+%0100	!STACK-MMU fuer Segmentnr. 64-127!
	andb	rh6, #%3f

mmu1:	lda	rr2, MMUINIT
	ldb	@rr2, rh6
	soutib	@r4, @rr2, r0	!Write SAR mit Segmentnummer <rh6>!
	ldb	rh4, #%0B	!Read SDR!
	ldk	r0, #4		!4 Byte einlesen!
	sinirb	@rr2, @r4, r0	!Read SDR des Segments!
	dec	r3
	ldb	rl0, @rr2	!rl0:=Attribut-Feld!
	andb	rl0, #%24	!alle Flags (ausser CPUI und DIRW) loeschen!
	ldb	@rr2, rl0	!geaendertes Attribut-Feld zurueckspeichern!
	dec	r3, #3		!rr2 zeigt auf MMUINIT!

	ld	r6, #%3b00
	soutb	CODE_MMU+%0100, rh6	!Write SAR Segnr. %3B!

	ld	r4, #CODE_MMU+%0B00	!Write SDR!
	ldk	r0, #4
	sotirb	@r4, @rr2, r0		!Write SDR des Segments %3B!
	pop	r4, @rr14
	ret
end mmu3b

  global

!------------------------------------------------------------------------------
int	copyin ((long)srcaddr, (long)dstaddr, count)
Speicherbereich kopieren von User-Data/Stack-Bereich nach Kernel-Data/Stack-
Bereich
Input:	rr6 - Quelladresse
    	rr4 - Zieladresse
 	r3  - Byteanzahl
Output:	r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_copyin procedure
    entry
	push	@rr14, r3
	calr	mmu3b
	pop	r3, @rr14
	jr	COPY
end _copyin

!------------------------------------------------------------------------------
int	copyiin (srcaddr, (long)dstaddr, count)
Speicherbereich kopieren von User-Instruction-Bereich (nichtsegmentierte User-
Programme; CODE-MMU, Segment %3F) nach Kernel-Data/Stack-Bereich
Input:	r7 -  Quelladresse (Offsetadresse)
    	rr4 - Zieladresse
 	r3  - Byteanzahl
Output:	r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_copyiin procedure
    entry
	ldctl	r1, fcw
	ld	r6, #%3f00	!Segmentnummer!
	di	vi,nvi
	jr	COPY		!Bytes kopieren!
end _copyiin

!------------------------------------------------------------------------------
int	copyout ((long)srcaddr, (long)dstaddr, count)
Speicherbereich kopieren von Kernel-Data/Stack-Bereich nach User-Data/Stack-
Bereich
Input:	rr6 - Quelladresse
    	rr4 - Zieladresse
 	r3  - Byteanzahl
Output:	r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_copyout procedure
    entry
	push	@rr14, r3
	ex	r4, r6
	ex	r5, r7
	calr	mmu3b
	ex	r4, r6
	ex	r5, r7
	pop	r3, @rr14
	jr	COPY
end _copyout

!------------------------------------------------------------------------------
int	copyiout ((long)srcaddr, dstaddr, count)
Speicherbereich kopieren von Kernel-Data/Stack-Bereich nach User-Instruction-
Bereich (nichtsegmentierte User-Programme; CODE-MMU, Segment %3F)
Input:	rr6 - Quelladresse
    	r5  - Zieladresse (Offsetadresse)
 	r4  - Byteanzahl
Output:	r2  - 0, wenn kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_copyiout procedure
    entry
	ldctl	r1, fcw
	ld	r3, r4		!r3:=Byteanzahl!
	ld	r4, #%3f00	!Segmentnummer!
	di	vi,nvi
COPY:
	ld	r2, r1
	lda	rr0, ERROR	!Fehleradresse!
	ldl	_nofault, rr0
	ld	r1, r2

	cp	r3, #10		!r3:=Byteanzahl!
	jr	c, co2

	ld	r2, r7
	xor	r2, r5
	bit	r2, #0
	jr	nz, co2		!Quelladr. oder Zieladr ungerade!
	bit	r7, #0
	jr	z, co1		!Quelladr. und Zieladr. gerade!

	ldib	@rr4, @rr6, r3	!1 Byte kopieren, damit Quelladr. und
				 Zieladresse gerade wird!
co1:	resflg	c
	rrc	r3		!Wordanzahl:=Byteanzahl/2!
	ldir	@rr4, @rr6, r3	!Speicherbereich kopieren (wordweise)!
	jr	nc, co3
	ldib	@rr4, @rr6, r3	!1 Byte kopieren!
	jr	co3

co2:	ldirb	@rr4, @rr6, r3	!Speicherbereich kopieren (byteweise)!

co3:	subl	rr2, rr2
	ldl	_nofault, rr2
	ldctl	fcw, r1
	ret	
end _copyiout

!------------------------------------------------------------------------------
int	bcopy ((long)srcaddr, (long)dstaddr, count)
Speicherbereich kopieren von Kernel-Data/Stack-Bereich nach Kernel-Data/Stack-
Bereich 
Input:	rr6 - Quelladresse
    	rr4 - Zieladresse
 	r3  - Byteanzahl
Output:	r2 := 0, d.h. kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_bcopy procedure
    entry
	cp	r3, #10		!r3:=Byteanzahl!
	jr	c, bc2

	ld	r2, r7
	xor	r2, r5
	bit	r2, #0
	jr	nz, bc2		!Quelladr. oder Zieladr ungerade!
	bit	r7, #0
	jr	z, bc1		!Quelladr. und Zieladr. gerade!

	ldib	@rr4, @rr6, r3	!1 Byte kopieren, damit Quelladr. und
				 Zieladresse gerade wird!
bc1:	xor	r2, r2
	resflg	c
	rrc	r3		!Wordanzahl:=Byteanzahl/2!
	ldir	@rr4, @rr6, r3	!Speicherbereich kopieren (wordweise)!
	ret	nc
	ldib	@rr4, @rr6, r3	!1 Byte kopieren!
	ret

bc2:	ldirb	@rr4, @rr6, r3	!Speicherbereich kopieren (byteweise)!
	xor	r2, r2
	ret	
end _bcopy

!------------------------------------------------------------------------------
	bzero ((long)addr, count)
Speicherbereich loeschen (Kernel-Data/Stack-Bereich)
Input:	rr6 - Speicheranfangsadresse
	r5  - Byteanzahl
------------------------------------------------------------------------------!
_bzero procedure
    entry
	ldl	rr2, rr6
	bit	r7, #0
	jr	nz, bz1
	bit	r5, #0
	jr	nz, bz1

	clr	@rr6		!Word-weise!
	dec	r5, #2
	ret	z

	inc	r7, #2
	resflg	c
	rrc	r5		!Wordanzahl:=Byteanzahl/2!
	ldir	@rr6, @rr2, r5
	ret	

bz1:	clrb	@rr6		!Byte-weise!
	dec	r5
	ret	z

	ldl	rr2, rr6
	inc	r7
	ldirb	@rr6, @rr2, r5
	ret	
end _bzero

!------------------------------------------------------------------------------
	ramtest (seg)
Speichertest ueber 64KByte im Segment seg
Input:	r7 - Segment
	r6 - flag, 1 = RAM-Test, 0 = nur RAM mit 0 fuellen
Output:	r2 := 0, d.h. kein Fehler
              -1, wenn Fehler
------------------------------------------------------------------------------!
_ramtest procedure
    entry
	ld	r5, r6
	ld	r6, r7
	ldk	r7, #0
	or	r5, r5
	jr	z,rt0
	ld	r4, #%5555
	calr	rt
	jr	nz, rterr
	com	r4
	calr	rt
	jr	nz, rterr
rt0:	ldk	r4, #0
rt1:	ld	r5, #%7fff
	ld	@rr6, r4
	ldl	rr2, rr6
	inc	r7, #2
	ldir	@rr6, @rr2, r5
	ldk	r2, #0
	ret
rterr:	ld	r2, #-1
	ret

rt:	calr	rt1
	ld	r5, #%8000
rtloop:	cp	r4, @rr6
	ret	nz
	inc	r7, #2
	djnz	r5, rtloop
	ret
end _ramtest

!------------------------------------------------------------------------------
	idle ()
HALT, warten auf Interrupt
------------------------------------------------------------------------------!
_idle procedure
    entry
	outb	LOFF, rl0	!LED lbolt aus!
	ldctl	r1, fcw
	ei	vi,nvi
	halt	
end _idle			!Weiterlauf bei _waitloc!

!------------------------------------------------------------------------------
	waitloc ()
Programmfortsetzung nach Interrupt (_idle)
------------------------------------------------------------------------------!
_waitloc procedure
    entry
	outb	LON, rl0	!LED lbolt ein!
	ldctl	fcw, r1
	ret	
end _waitloc

!------------------------------------------------------------------------------
int	save ((long)pregbuf)
Save Prozess
Retten Register r8-r15 eines Prozesses sowie Rueckkehradresse
Input:	rr6 - Anfangsadresse des Speicherbereiches fuer die geretteten
	      Werte (9 Word)
Output:	r2 := 0
------------------------------------------------------------------------------!
_save procedure
    entry
	popl	rr4, @rr14
	ldm	@rr6, r8, #8
	ldl	rr6(#%0010), rr4
	xor	r2, r2
	jp	@rr4
end _save

!------------------------------------------------------------------------------
int	resume (???, (long)pregbuf)
Zurueckspeichern Register r8-r15 eines Prozesses und Sprung zur Rueckkehr-
adresse (siehe save)
Input:	r7  - ???
	rr4 - Anfangsadresse des Speicherbereiches mit den geretteten
	      Werten (9 Word)
Output: r2 := 1
------------------------------------------------------------------------------!
_resume procedure
    entry
	ldctl	r6, fcw
	add	r7, #-%00F6
	di	vi,nvi
	ldb	rh0, #%3e
	soutb	CODE_MMU+%0100, rh0	!Write SAR mit Segment-Nr. %3E!
	soutb	CODE_MMU+%0800, rh7	!rite Basisadressenfeld!
	soutb	CODE_MMU+%0800, rl7
	ldm	r8, @rr4, #8		!zurueckspeichern Register r8-r15!
	ldctl	fcw, r6
	ldl	rr6, rr4(#%0010)	!rr6 := Rueckkehradresse!
	ldk	r2, #1
	jp	@rr6
end _resume

!------------------------------------------------------------------------------
int	evi ()
Interupt zulassen
Output:	r2 - Stand FCW vor (ei vi)
------------------------------------------------------------------------------!
_evi procedure
    entry
	ldctl	r2, fcw
	ei	vi, nvi
	ret	
end _evi

!------------------------------------------------------------------------------
int	dvi ()
Interrupt sperren
Output:	r2 - Stand FCW vor (di vi)
------------------------------------------------------------------------------!
_dvi procedure
    entry
	ldctl	r2, fcw
	di	vi, nvi
	ret	
end _dvi

!------------------------------------------------------------------------------
	rvi (FCW)
FCW laden
Input:	r7 - Wert fuer FCW
------------------------------------------------------------------------------!
_rvi procedure
    entry
	ldctl	fcw, r7
	ret	
end _rvi

!------------------------------------------------------------------------------
	copyseg (srcaddr, dstaddr)
Kopieren von 256 Byte (1 Click) zwischen zwei physischen Adressbereichen
Input:	r7 - Quelladresse
	r6 - Zieladresse
------------------------------------------------------------------------------!
_copyseg procedure
    entry
	ldctl	r0, fcw
	ld	r2, #%3c00
	sub	r3, r3
	ld	r4, #%3d00
	ld	r5, r3
	ld	r1, #%0080
	di	vi,nvi
	soutb	CODE_MMU+%0100, rh2	!Write SAR mit Segmentnr. %3C!
	soutb	CODE_MMU+%0800, rh7	!Write Basisadressen-Feld!
	soutb	CODE_MMU+%0800, rl7
	soutb	CODE_MMU+%0100, rh4	!Write SAR mit Segmentnr. %3D!
	soutb	CODE_MMU+%0800, rh6	!Write Basisadressen-Feld!
	soutb	CODE_MMU+%0800, rl6
	ldir	@rr4, @rr2, r1		!%80 Words (%100 Bytes) kopieren!
	ldctl	fcw, r0
	ret	
end _copyseg

!------------------------------------------------------------------------------
	clearseg (addr)
Loeschen von 256 Byte (1 Click) eines physischen Adressbereiches
Input:	r7 - Anfangsadresse des Speicherbereiches
------------------------------------------------------------------------------!
_clearseg procedure
    entry
	ldctl	r0, fcw
	ld	r2, #%3c00
	sub	r3, r3
	ld	r4, r2
	ldk	r5, #2
	ld	r1, #%007f
	di	vi,nvi
	soutb	CODE_MMU+%0100, rh2	!Write SAR mit Segmentnr. %3C!
	soutb	CODE_MMU+%0800, rh7	!Write Basisadressenfeld!
	soutb	CODE_MMU+%0800, rl7
	clr	@rr2
	ldir	@rr4, @rr2, r1		!%80 Words loeschen!
	ldctl	fcw, r0
	ret	
end _clearseg

!------------------------------------------------------------------------------
	otirb (ioaddr, (long)memaddr, size)
OTIRB-Befehl als Funktion in C
Input:	r7  - Portadresse
	rr4 - Speicheranfangsadresse
	r3  - Anzahl der auszugebenden Bytes
------------------------------------------------------------------------------!
_otirb procedure
   entry
	otirb	@r7, @rr4, r3
	ret	
end _otirb

!------------------------------------------------------------------------------
	outb (ioaddr, (char)val)
OUTB-Befehl als Funktion in C
Input:	r7  - Portadresse
	rl6 - auszugebendes Datenbyte
------------------------------------------------------------------------------!
_outb procedure
    entry
	outb	@r7, rl6
	ret	
end _outb

!------------------------------------------------------------------------------
int	inb (ioaddr)
INB-Befehl als Funktion in C
Input:	r7  - Portadresse
Output:	rl2 - eingelesenes Byte
	rh2 := 0
------------------------------------------------------------------------------!
_inb procedure
    entry
	inb	rl2, @r7
	xorb	rh2, rh2
	ret	
end _inb

!------------------------------------------------------------------------------
	out (ioaddr, (int)val)
OUT-Befehl als Funktion in C
Input:	r7  - Portadresse
	r6 - auszugebendes Datenwort
------------------------------------------------------------------------------!
_out procedure
    entry
	out	@r7, r6
	ret	
end _out

!------------------------------------------------------------------------------
int	in (ioaddr)
IN-Befehl als Funktion in C
Input:	r7 - Portadresse
Output:	r2 - eingelesenes Datenwort
------------------------------------------------------------------------------!
_in procedure
    entry
	in	r2, @r7
	ret	
end _in

!------------------------------------------------------------------------------
	ltol3 ((long)dstaddr, (long)srcaddr, count)
Konvertiert eine Liste von 4-Byte-Zahlen (srcaddr zeigt darauf) 
in eine Liste von 3-Byte-Zahlen (dstaddr zeigt darauf)
Input:	rr6 - Zieladresse
	rr4 - Quelladresse
	r3  - Datenanzahl (Anzahl der 4-Byte-Zahlen)
------------------------------------------------------------------------------!
_ltol3 procedure
     entry
	inc	r5
	ldk	r2, #3
	ldirb	@rr6, @rr4, r2
	djnz	r3, _ltol3
	ret	
end _ltol3

!------------------------------------------------------------------------------
	l3tol ((long)dstaddr, (long)srcaddr, count)
Konvertiert eine Liste von 3-Byte-Zahlen (srcaddr zeigt darauf)
in eine Liste von 4-Byte-Zahlen (dstaddr zeigt darauf)
Input:	rr6 - Zieladresse
	rr4 - Quelladresse
	r3  - Datenanzahl (Anzahl der 3-Byte-Zahlen)
------------------------------------------------------------------------------!
_l3tol procedure
    entry
	clrb	@rr6
	inc	r7
	ldk	r2, #3
	ldirb	@rr6, @rr4, r2
	djnz	r3, _l3tol
	ret	
end _l3tol

!------------------------------------------------------------------------------
	printf (parm1 parm2 ...)
Entry fuer printf (in prf.c)
Parameter in Stack bringen
------------------------------------------------------------------------------!
_printf procedure
    entry
	ex	r2, _stkseg+%2(sp)
	ex	r3, @rr14
	ex	r2, r3
	push	@rr14, r4
	push	@rr14, r5
	ldl	rr4, rr14
	pushl	@rr14, rr2
	call	_printfv
	ldl	rr2, @rr14
	inc	sp, #12		!Korrektur Stackpointer!
	jp	@rr2		!Return!
end _printf

!------------------------------------------------------------------------------
???	prsp (???)
------------------------------------------------------------------------------!
_prsp procedure
    entry
	ldl	rr6, rr14
	ld	r5, #%0010
	call	_printn
	ret	
end _prsp

!------------------------------------------------------------------------------
???	handler (???)
------------------------------------------------------------------------------!
_handler procedure
    entry
	ldl	rr2, rr8
	ret	
end _handler

!------------------------------------------------------------------------------
	sc ()
System-Call "exit"
------------------------------------------------------------------------------!
_sc procedure
    entry
	sc	#1	!exit!
	ret	
end _sc

!------------------------------------------------------------------------------
(keine Operation bei System ohne Floating Point Prozessor)
------------------------------------------------------------------------------!
_fsetregs procedure
    entry
	ret
end _fsetregs
end mch
