! mcrt0.s: the start-up code of a program with profiling (cc -p) !

mcrt0_s module

internal
foo		word
msg		array [* byte] := 'No space for monitor buffer%r'

external
_environ	word
countbase	word
_etext		procedure
_sbrk		procedure
_monitor	procedure
__cleanup	procedure
_main		procedure

global

start procedure
	entry
		lda	r8,_etext
		sub	r8,#eprol
		inc	r8,#7
		srl	r8,#3
		add	r8,#%0387
		ld	r7,r8
		add	r7,r8
		call	_sbrk
		cp	r2,#%FFFF
		jr	eq,_nospace
		lda	r7,eprol
		lda	r6,_etext
		ld	r5,r2
		inc	r2,#6
		ld	countbase,r2
		ld	r4,r8
		ld	r3,#300
		call	_monitor
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
		ld	r8,r2
		call	_exit
_nospace:
		ld	r0,#2
		lda	r1,msg
		ld	r2,#28
		sc	#4
		ld	r8,#%FFFF
		call	_exit
end start

_exit procedure
	entry
		call	__cleanup
		ldk	r7,#0
		call	_monitor
		ld	r0,r8
		sc	#1
eprol:
end _exit
end mcrt0_s
