stime module

  external
    _cerror

  global
    stime procedure
      entry
	ldl	rr0,@r7
	xor	r4,r4
	sc	#25
	ld	r2,r4
	ret	nc
	jp	cerror
    end stime
end stime
