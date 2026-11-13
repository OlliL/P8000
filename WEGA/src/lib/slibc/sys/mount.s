! @[$] mount.s	2.1  09/12/83 11:42:30 - 87wega3.2 !

mount module

$segmented

  external
    cerror	procedure

  global
    _mount procedure
      entry
	ldl	rr0,rr6
	ld	r7,r3
	ldl	rr2,rr4
	ld	r4,r7
	sc	#21
	ld	r2,r4
	ret	nc
	jp	cerror
    end _mount
end mount
