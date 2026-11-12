! @[$] chown.s	2.1  09/12/83 11:39:58 - 87wega3.2 !

chown module

$segmented

  external
    cerror	procedure

  global
    _chown procedure
      entry
	ldl	rr0,rr6
	ld	r2,r5
	ld	r3,r4
	xor	r4,r4
	sc	#16
	ld	r2,r4
	ret	nc
	jp	cerror
    end _chown
end chown
