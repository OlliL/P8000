!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
	KERN 3.2	Modul:	event.s fuer P8000
 
	Bearbeiter:	U. Wiesner P. Hoge
	Datum:		29.7.88
	Version:	1.2
 
*******************************************************************************
*******************************************************************************
 
	Dieser Modul ist von der speziellen Hardware abhaengig
 
	Erstellung der Objektdatei:
	as -u [-l] -o event.o event.s
	cp event.o /usr/sys/conf/event.o
 
*******************************************************************************
******************************************************************************!
 
event_s module

$segmented
 
  constant
	sp := r15

!Port Adressen!

	CTC0 := %FFA9
	CTC1 := %FFB1
	RETI := %FFE1

  internal

    clkctc array [6 byte] := [

			%03	!CTC1 Kanal 3 Reset!
			%c7
			%50	!Zeitkonstante!
			%03	!CTC1 Kanal 2 Reset!
			%47
			%00]

!*---------------------------------------------------------------------------*!
!*
 * Programm Status Arrea
 * (liegt stets im Code Bereich)
 *!
!*---------------------------------------------------------------------------*!

  global
 
$SECTION event_s_p
 
_psa	array [* long] := [
 
	%0000	!reserviert!
	%0000
 
	%c000	!ext instruction trap!
	UNIMINS
 
	%c000	!priveliged instr trap!
	PRIVINS
 
	%c000	!system call trap!
	SYSCALL
 
	%c000	!segment trap!
	SEGVIO
 
	%c000	!NMI interrupt!
	NMINT
 
	%c000	!NVI interrupt!
	NVINT
 
	%c000	!VI interrupt!
	] 

array [* long] := [			!Vektor 00!
	UNEXINT UNEXINT UNEXINT STEPINT
	KINT    MTINT   UNEXINT CLOKINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	MDINT   UNEXINT UNEXINT UNEXINT
	] 
!SIO Kanaele!
array [* long] := [			!Vektor 20!
	SIOXINT SIOEINT SIORINT SIOSINT
	SIOXINT SIOEINT SIORINT SIOSINT
	] 
array [* long] := [			!Vektor 30!
	SIOXINT SIOEINT SIORINT SIOSINT
	SIOXINT SIOEINT SIORINT SIOSINT
	] 
! unerwartete Interrupts !
array [* long] := [			!Vektor 40!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor 50!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor 60!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor 70!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor 80!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor 90!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor A0!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor B0!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor C0!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor D0!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
array [* long] := [			!Vektor E0!
	UNEXINT UNEXINT UNEXINT UNEXINT
	UNEXINT UNEXINT UNEXINT UNEXINT
	] 
! User Devices !
array [* long] := [			!Vektor F0!
	USR1INT USR2INT USR3INT USR4INT
	USR5INT USR6INT UNEXINT
	]
word := 0
long := SPURINT

! fuer 1/2" Magnetband!
word !realisiert die Adresse %0240 fuer mtcom!
!
_mtcom	array [16 byte]
_mtbuf	array [20*%200 byte]
!
 
!*---------------------------------------------------------------------------*!
!*
 * Adressen der Trap/IS Routinen abstacken
 * gemeinsame Int/Trap Eintritt
 * Call zur speziellen Int/Trap Routine
 * gemeinsamer Int/Trap Austritt
 * bei Trace Step programmieren
 *!
!*---------------------------------------------------------------------------*!
 
event procedure
    entry

CLOKINT:
	push	@rr14, #OFFSET clokint
	push	@rr14, #SEG clokint
	jpr	GO_INT
MDINT:
	push	@rr14, #OFFSET _mdint
	push	@rr14, #SEG _mdint
	jpr	GO_INT1
MTINT:
	push	@rr14, #OFFSET _mtint
	push	@rr14, #SEG _mtint
	jpr	GO_INT
KINT:
	push	@rr14, #OFFSET _kint
	push	@rr14, #SEG _kint
	jpr	GO_INT1
NMINT:
	push	@rr14, #OFFSET _nmint
	push	@rr14, #SEG _nmint
	jpr	GO_INT
NVINT:
	push	@rr14, #OFFSET _cpu2int
	push	@rr14, #SEG _cpu2int
	jpr	GO_INT
PRIVINS:
	push	@rr14, #OFFSET _privins
	push	@rr14, #SEG _privins
	jpr	GO_INT
SEGVIO:
	push	@rr14, #OFFSET _segvio
	push	@rr14, #SEG _segvio
	jpr	GO_INT
SIOEINT:
	push	@rr14, #OFFSET _sioeint
	push	@rr14, #SEG _sioeint
	jpr	GO_INT
SIORINT:
	push	@rr14, #OFFSET _siorint
	push	@rr14, #SEG _siorint
	jpr	GO_INT
SIOSINT:
	push	@rr14, #OFFSET _siosint
	push	@rr14, #SEG _siosint
	jpr	GO_INT
SIOXINT:
	push	@rr14, #OFFSET _sioxint
	push	@rr14, #SEG _sioxint
	jpr	GO_INT1
SPURINT:
	push	@rr14, #OFFSET _spurint
	push	@rr14, #SEG _spurint
	jpr	GO_INT
STEPINT:
	push	@rr14, #OFFSET _stepint
	push	@rr14, #SEG _stepint
	jpr	GO_INT
SYSCALL:
	push	@rr14, #OFFSET _syscall
	push	@rr14, #SEG _syscall
	jpr	GO_INT
UNEXINT:
	push	@rr14, #OFFSET _unexint
	push	@rr14, #SEG _unexint
	jpr	GO_INT
UNIMINS:
	push	@rr14, #OFFSET _unimins
	push	@rr14, #SEG _unimins
GO_INT1:
	sub	sp, #%001c
	ldm	@rr14, r0, #14
	ldctl	r8, nspseg
	ex	r8, |_stkseg + %1c|(sp)
	ldctl	r9, nspoff
	ex	r9, |_stkseg + %1e|(sp)
	ldl	rr6, rr14
	call	@rr8
	jpr	RLOAD

USR1INT:
	push	@rr14, #OFFSET _usr1int
	push	@rr14, #SEG _usr1int
	jpr	GO_INT
USR2INT:
	push	@rr14, #OFFSET _usr2int
	push	@rr14, #SEG _usr2int
	jpr	GO_INT
USR3INT:
	push	@rr14, #OFFSET _usr3int
	push	@rr14, #SEG _usr3int
	jpr	GO_INT
USR4INT:
	push	@rr14, #OFFSET _usr4int
	push	@rr14, #SEG _usr4int
	jpr	GO_INT
USR5INT:
	push	@rr14, #OFFSET _usr5int
	push	@rr14, #SEG _usr5int
	jpr	GO_INT
USR6INT:
	push	@rr14, #OFFSET _usr6int
	push	@rr14, #SEG _usr6int

GO_INT:
! r0 ... r13 in state.s_reg retten !
	sub	sp, #%001c
	ldm	@rr14, r0, #14
! nsp Segno  in state.s_reg[14] retten !
	ldctl	r8, nspseg
	ex	r8, |_stkseg + %1c|(sp)
! nsp Offset in state.s_sp retten !
	ldctl	r9, nspoff
	ex	r9, |_stkseg + %1e|(sp)
!M1 an U880-Schaltkreise !
	ldk	r0, #0
	outb	RETI, rl0
! Trap-Vorbereitung !
	ldl	rr6, rr14
	call	_evntent
! Absprung in Serviceroutine !
	ldl	rr6, rr14
	call	@rr8
! Trap-Nachbereitung !
	ldl	rr6, rr14
	call	_evntret
!M1 an U880-Schaltkreise !
	ldk	r0, #0
	outb	RETI, rl0
! Test TRACE-Bit im state.s_ps !
	bit	|_stkseg + %22|(sp), #0
	jpr	z, RLOAD
! TRACE !
	di	vi,nvi
   ! state.s_ps &= FCWBITS !
	ld	r0, |_stkseg + %22|(sp)
	and	r0, #%f8fc
	ld	|_stkseg + %22|(sp), r0
   ! STEP-Int scharfmachen !
	ldb	rl1, #%03
	outb	CTC0+6, rl1
	ldb	rl1, #%00	!Int Vektor!
	outb	CTC0, rl1
	ldb	rl1, #%c7
	outb	CTC0+6, rl1
	ldb	rl1, #%11
	outb	CTC0+6, rl1
RLOAD:
! nsp rueckladen !
	ld	r0, |_stkseg + %1c|(sp)
	ldctl	nspseg, r0
	ld	r0, |_stkseg + %1e|(sp)
	ldctl	nspoff, r0
! r0 ... r13 rueckladen !
	ldm	r0, @rr14, #14
	add	sp, #%0020
	iret	
end event

!*---------------------------------------------------------------------------*!
!*
 *clokint
 * Aufruf von _clokint und _cpu2clk
 *!
!*---------------------------------------------------------------------------*!

    clokint procedure
      entry
	call	_clokint
	jp	_cpu2clk
    end clokint

!*---------------------------------------------------------------------------*!
!*
 *clkreti
 * reti fuer Clock Kanal
 *!
!*---------------------------------------------------------------------------*!
 
    global _reti label
 
    _clkreti procedure
      entry
_reti:
	ld	r7, #RETI
	ld	r6, #%ED4D
	outb	@r7, rh6
	outb	@r7, rl6
	ret
    end _clkreti
 
!*---------------------------------------------------------------------------*!
!*
 *resstep
 * Reset Step Kanal
 *!
!*---------------------------------------------------------------------------*!
 
    _resstep procedure
      entry
	ldb	rl6, #%03
	outb	CTC0+6, rl6
	calr	_reti
	ret
    end _resstep
 
!----------------------------------------------------------------------------*!
!*
 *clkstart
 * Start des Clock Kanals
 *!
!*---------------------------------------------------------------------------*!
 
    _clkstart procedure
      entry
	ld	r7, #CTC1
	ldb	rl6, #%8	!Interrupt Vektor!
	outb	@r7, rl6
	inc	r7, #6		!Kanal 3!
	ldl	rr4, #clkctc
	calr	clk1
	dec	r7, #2		!Kanal 2!
clk1:	ldk	r6, #3
	otirb	@r7, @rr4, r6
	ret
    end _clkstart
end event_s
