access module

  external
    _cerror

  global
    access procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#33
	ld	r2,r4
	ret	nc
	jp	cerror
    end access
end access
