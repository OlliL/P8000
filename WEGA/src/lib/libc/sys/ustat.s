ustat module

  external
    _cerror

  global
    ustat procedure
      entry
	ld	r0,r7
	ld	r1,r7
	ld	r2,#2
	sc	#57
	ld	r2,r4
	ret	nc
	jp	cerror
    end ustat
end ustat
