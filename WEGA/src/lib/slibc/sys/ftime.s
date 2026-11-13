! @[$] ftime.s	2.1  09/12/83 11:41:12 - 87wega3.2 !

ftime module

$segmented

  external
    cerror	procedure

  global
    _ftime procedure
      entry
	ld	r2,#%0016
	ldl	rr0,rr6
	xor	r4,r4
	sc	#35
	ld	r2,r4
	ret	nc
	jp	cerror
    end _ftime
end ftime
