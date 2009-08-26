! @[$] write.s	2.1  09/12/83 11:44:24 - 87wega3.2 !

write module

  external
    cerror	procedure

  global
    _write procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#4
	ld	r2,r4
	ret	nc
	jp	cerror
    end _write
end write
