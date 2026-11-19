! smcrt0.s: the start-up code of a segmented program with profiling (scc -p) !

smcrt0_s module

$segmented

internal
foo		word
msg		array [* byte] := 'No space for monitor buffer%r'

external
_stkseg		procedure
_environ	long
_sbuff		array [60 byte]
_segsizes	array [40 byte]
_sgstat		procedure
_segmon		procedure
__cleanup	procedure
_main		procedure

global

start procedure
	entry
		lda	rr6,_segsizes
		call	_sgstat
		cp	r2,#%FFFF
		jr	eq,_nospace
		lda	rr2,_segsizes
		lda	rr6,_sbuff
_loop:
		ldl	rr4,@rr2
		ldl	@rr6,rr4
		cpb	rh4,#%FF
		jr	eq,_done
		inc	r7,#4
		ld	@rr6,#0
		cpl	rr2,#_segsizes
		jr	ne,_next
		ldl	rr0,#eprol
		ld	@rr6,r1
_next:
		inc	r3,#4
		inc	r7,#2
		jp	_loop
_done:
		lda	rr6,_sbuff
		ld	r5,#300
		call	_segmon
		dec	r15,#4
		ldl	|_stkseg|(r15),rr8
		ld	r7,|_stkseg+4|(r15)
		lda	rr4,|_stkseg+6|(r15)
		ldl	rr2,@rr4
		sub	r3,#2
		clr	@rr2
		add	r3,#2
		ld	r8,r7
		inc	r8,#1
		sla	r8,#2
		ldl	rr2,rr4
		add	r3,r8
		ldl	_environ,rr2
		ldl	rr8,|_stkseg|(r15)
		inc	r15,#4
		call	_main
		ld	r8,r2
		call	_exit
_nospace:
		ld	r0,#2
		lda	rr2,msg
		ld	r1,r2
		ld	r2,r3
		ld	r3,#28
		sc	#4
		ld	r7,#%FFFF
		call	_exit
end start

_exit procedure
	entry
		ld	_sbuff,#%FFFF
		lda	rr6,_sbuff
		ld	r5,#0
		call	_segmon
		call	__cleanup
		ld	r0,r8
		sc	#1
eprol:
end _exit
end smcrt0_s
