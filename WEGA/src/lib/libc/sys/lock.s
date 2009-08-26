! @[$] lock.s	2.1  09/12/83 11:41:58 - 87wega3.2 !

lock module

  external
    cerror	procedure

  global
    _lock procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#53
	ld	r2,r4
	ret	nc
	jp	cerror
    end _lock
end lock
