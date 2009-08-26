ustat module

  external
    cerror	procedure

  global
    _ustat procedure
      entry
	ld	r0,r6
	ld	r1,r7
	ld	r2,#2
	sc	#57
	ld	r2,r4
	ret	nc
	jp	cerror
    end _ustat
end ustat
