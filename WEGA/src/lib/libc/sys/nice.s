nice module

  external
    cerror	procedure

  global
    _nice procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#34
	ld	r2,r4
	ret	nc
	jp	cerror
    end _nice
end nice
