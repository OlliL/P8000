ulimit module

  external
    cerror	procedure

  global
    _ulimit procedure
      entry
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	ldl	rr4,#%00000000
	sc	#54
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end _ulimit
end ulimit
