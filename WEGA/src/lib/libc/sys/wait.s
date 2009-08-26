wait module

  external
    cerror	procedure

  global
    _wait procedure
      entry
	sc	#7
	ldl	rr2,rr4
	jp	c,cerror
	test	r7
	ret	z
	ld	@r7,r3
	ret
    end _wait
end wait
