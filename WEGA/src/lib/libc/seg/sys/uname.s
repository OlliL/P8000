! @[$] uname.s	1.1  09/12/83 11:43:55 - 87wega3.2 !

uname module

$segmented

  external
    cerror	procedure

  global
    _uname procedure
      entry
	ld	r2,#%0016
	jp	z,cerror
	ldl	rr0,rr6
	ld	r3,#%0000
	sc	#57
	ld	r2,r4
	ret	nc
	jp	cerror
    end _uname
end uname
