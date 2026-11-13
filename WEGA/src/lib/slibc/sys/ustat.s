! @[$] ustat.s	1.1  09/12/83 11:44:09 - 87wega3.2 !

ustat module

$segmented

  external
    cerror	procedure

  global
    _ustat procedure
      entry
	ldl	rr0,rr4
	ld	r2,r7
	ld	r3,#%0002
	sc	#57
	ld	r2,r4
	ret	nc
	jp	cerror
    end _ustat
end ustat
