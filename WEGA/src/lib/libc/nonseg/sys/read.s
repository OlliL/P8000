! @[$] read.s	2.1  09/12/83 11:43:00 - 87wega3.2 !

read module

  external
    cerror	procedure

  global
    _read procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#3
	ld	r2,r4
	ret	nc
	jp	cerror
    end _read
end read
