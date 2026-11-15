! @[$] unlink.s	2.1  09/12/83 11:43:59 - 87wega3.2 !

unlink module

  external
    cerror	procedure

  global
    _unlink procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#10
	ld	r2,r4
	ret	nc
	jp	cerror
    end _unlink
end unlink
