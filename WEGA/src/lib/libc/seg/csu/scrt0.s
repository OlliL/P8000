! @[$] scrt0.s		Rev : 3.2	09/12/83 00:05:11 !

scrt0 module

$segmented

internal
foo		word

external
_stkseg		procedure
_environ	long
__callex	procedure
_exit		procedure
_main		procedure

global

start procedure
	entry
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
		ld	r7,r2
		call	__callex
		sc	#1
end start
end scrt0
