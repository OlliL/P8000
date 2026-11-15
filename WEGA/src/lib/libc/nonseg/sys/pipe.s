! @[$] pipe.s	2.1  09/12/83 11:42:45 - 87wega3.2 !

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
