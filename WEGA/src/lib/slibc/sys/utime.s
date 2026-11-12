! @[$] utime.s	2.1  09/12/83 11:44:13 - 87wega3.2 !

utime module

$segmented

  external
    cerror	procedure

  global
    _utime procedure
      entry
	ldl	rr0,rr6
	ldl	rr2,rr4
	xor	r4,r4
	sc	#30
	ld	r2,r4
	ret	nc
	jp	cerror
    end _utime
end utime
