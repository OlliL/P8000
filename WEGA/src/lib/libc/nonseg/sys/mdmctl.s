! @[$] mdmctl.s	2.2  09/12/83 11:42:19 - 87wega3.2 !

mdmctl module

  external
    cerror	procedure

  global
    _mdmctl procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#62
	jp	c,cerror
	xor	r2,r2
	ret
    end _mdmctl
end mdmctl
