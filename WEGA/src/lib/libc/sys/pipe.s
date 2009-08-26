pipe module

  external
    cerror	procedure

  global
    _pipe procedure
      entry
	sc	#42
	ld	r2,r4
	jp	c,cerror
	ldl	@r7,rr4
	xor	r2,r2
	ret
    end _pipe
end pipe
