! @[$] mdmctl.s	2.2  09/12/83 11:42:19 - 87wega3.2 !

mdmctl module

$segmented

  external
    cerror	procedure

  global
    _mdmctl procedure
      entry
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#62
	ret	nc
	jp	cerror
    end _mdmctl
end mdmctl
