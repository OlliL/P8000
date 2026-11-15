! @[$] times.s	2.2  09/12/83 11:43:39 - 87wega3.2 !

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
