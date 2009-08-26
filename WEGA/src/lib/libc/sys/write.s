write module

  external
    _cerror

  global
    write procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#4
	ld	r2,r4
	ret	nc
	jp	cerror
    end write
end write
