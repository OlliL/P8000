ulimit module

  external
    _cerror

  global
    ulimit procedure
      entry
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	ldl	rr4,0
	sc	#54
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end ulimit
end ulimit
