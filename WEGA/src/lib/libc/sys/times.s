times module

  external
    cerror	procedure

  global
    _times procedure
      entry
	ld	r0,r7
	subl	rr4,rr4
	sc	#43
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end _times
end times
