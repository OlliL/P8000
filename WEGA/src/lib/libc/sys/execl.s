! @[$] execl.s	2.1  09/12/83 11:40:29 - 87wega3.2 !

execl module

  external
    _environ	procedure
    cerror	procedure

  global
    _execl procedure
      entry
	ld	r0,@r15
	ex	r0,r7
	ld	@r15,r2
	push	@r15,r3
	push	@r15,r4
	push	@r15,r5
 	push	@r15,r6
	ld	r1,r15
	ld	r2,_environ
	sc	#59
	ld	r2,r4
	inc	r15,#8
	ld	@r15,r7
	jp	cerror
    end _execl
end execl

