! @[$] lkdata.s	2.1 09/12/83 11:41:55 - 87wega3.2!

lkdata module

  external
    cerror	procedure

  global
    _lkdata procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#49
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end _lkdata
end lkdata
