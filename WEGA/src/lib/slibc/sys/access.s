! @[$] access.s	2.1  09/12/83 11:39:31 - 87wega3.2 !

access module

$segmented

  external
    cerror	procedure

  global
    _access procedure
      entry
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#33
	ld	r2,r4
	ret	nc
	jp	cerror
    end _access
end access
