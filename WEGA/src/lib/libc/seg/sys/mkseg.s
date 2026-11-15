! @[$] mkseg.s	2.1 08/17/83 15:18:50 - 87wega3.2 !

mkseg module

$segmented

  external
    cerror	procedure

  global
    _mkseg procedure
      entry
	ld	r0,r7
	ld	r1,r6
	sc	#%37
	ldl	rr2,rr4
	jp	c,cerror
	ret
    end _mkseg
end mkseg
