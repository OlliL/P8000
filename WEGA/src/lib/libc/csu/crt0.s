! @[$] crt0.s		Rev : 3.2	09/12/83 00:05:03 !

! @[$] crt0.s		Rev : 3.2	09/12/83 00:05:03 !

! @[$] crt0.s		Rev : 3.2	09/12/83 00:05:03 !

! @[$] crt0.s		Rev : 3.2	09/12/83 00:05:03 !

crt0 module

internal
foo		word

external
_environ	word
__callex	procedure
_exit		procedure
_main		procedure

global

start procedure
	entry
		ld	r7,@r15
		lda	r6,%0000+2(r15)
		ld	r2,@r6
		clr	%0000-2(r2)
		ld	r5,r6
		subl	rr0,rr0
		cpir	r0,@r5,r1,eq
		cp	r5,r2
		jr	ult,_foo
		dec	r5,#2
_foo:
		ld	_environ,r5
		call	_main
		ld	r7,r2
		call	__callex
		sc	#1
end start
end crt0
