! @[$] access.s	2.1  09/12/83 11:39:31 - 87wega3.2 !

access module

  external
    cerror	procedure

  global
    _access procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#33
	ld	r2,r4
	ret	nc
	jp	cerror
    end _access
end access
