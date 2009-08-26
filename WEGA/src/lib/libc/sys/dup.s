dup module

  external
    _cerror

  global
    dup procedure
      entry
	ld	r0,r7
	ld	r1,r6
	sc	#41
	ld	r2,r4
	ret	nc
	jp	cerror
    end dup
end dup
