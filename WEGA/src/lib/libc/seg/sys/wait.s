! @[$] wait.s	2.1  09/12/83 11:44:17 - 87wega3.2 !

wait module

$segmented

  external
    cerror	procedure

  global
    _wait procedure
      entry
	sc	#7
	ldl	rr2,rr4
	jp	c,cerror
	test	r6
	ret	z
	ld	@rr6,r3
	ret
    end _wait
end wait
