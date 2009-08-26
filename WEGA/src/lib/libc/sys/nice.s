nice module

  external
    _cerror

  global
    nice procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#34
	ld	r2,r4
	ret	nc
	jp	cerror
    end nice
end nice
