! @[$] stime.s	2.1  09/12/83 11:43:27 - 87wega3.2 !

stime module

$segmented

  external
    cerror	procedure

  global
    _stime procedure
      entry
	ldl	rr0,@rr6
	xor	r4,r4
	sc	#25
	ld	r2,r4
	ret	nc
	jp	cerror
    end _stime
end stime
