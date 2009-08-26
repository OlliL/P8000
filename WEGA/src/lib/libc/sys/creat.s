creat module

  external
    _cerror

  global
    creat procedure
      entry
	ld	r0,r7
	ld	r1,r6
	sc	#8
	ld	r2,r4
	ret	nc
	jp	cerror
    end creat
end creat
