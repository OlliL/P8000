! @[$] unlk.s	2.1 09/12/83 11:44:04 - 87wega3.2 !

unlk module

  external
    cerror	procedure

  global
    _unlk procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#50
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end _unlk
end unlk
