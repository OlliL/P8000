! @[$] setuid.s	2.1  09/12/83 11:43:12 - 87wega3.2 !

setuid module

  external
    cerror	procedure

  global
    _setuid procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#23
	ld	r2,r4
	ret	nc
	jp	cerror
    end _setuid
end setuid
