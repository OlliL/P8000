! @[$] execl.s	2.1  09/12/83 11:40:29 - 87wega3.2 !

execl module

$segmented

  external
    _environ	procedure
    cerror	procedure

  global
    _execl procedure
      entry
	ldl	rr0,rr6
	ldl	rr6,@rr14
	ldl	@rr14,rr2
	pushl	@rr14,rr4
	ldl	rr2,rr14
	ldl	rr4,_environ
	sc	#59
	ld	r2,r4
	inc	r15,#8
	pushl	@rr14,rr6
	jp	cerror
    end _execl
end execl

