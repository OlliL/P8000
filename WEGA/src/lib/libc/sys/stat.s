! @[$] stat.s	2.1  09/12/83 11:43:23 - 87wega3.2 !

stat module

  external
    cerror	procedure

  global
    _stat procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#18
	ld	r2,r4
	ret	nc
	jp	cerror
    end _stat
end stat
