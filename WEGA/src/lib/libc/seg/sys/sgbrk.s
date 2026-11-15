! @[$] sgbrk.s	2.2  08/17/83 15:23:01 - 87wega3.2 !

sgbrk module

$segmented

  external
    cerror	procedure

  global
    _sgbrk procedure
      entry
	ldl	rr0,rr6
	xor	r4,r4
	sc	#%11
	ldl	rr2,rr4
	jp	c,cerror
	ret
    end _sgbrk
end sgbrk
