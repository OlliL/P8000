times module

  external
    _cerror

  global
    times procedure
      entry
	ld	r0,r7
	subl	rr4,rr4
	sc	#43
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end times
end times
