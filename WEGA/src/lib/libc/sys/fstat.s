fstat module

  external
    _cerror

  global
    fstat procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#28
	ld	r2,r4
	ret	nc
	jp	cerror
    end fstat
end fstat
