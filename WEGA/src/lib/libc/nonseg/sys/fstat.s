! @[$] fstat.s	2.1  09/12/83 11:41:08 - 87wega3.2 !

fstat module

  external
    cerror	procedure

  global
    _fstat procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#28
	ld	r2,r4
	ret	nc
	jp	cerror
    end _fstat
end fstat
