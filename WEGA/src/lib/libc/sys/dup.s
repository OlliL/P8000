! @[$] dup.s	2.1  09/12/83 11:40:18 - 87wega3.2 !

dup module

  external
    cerror	procedure

  global
    _dup procedure
      entry
	ld	r0,r7
	ld	r1,r6
	sc	#41
	ld	r2,r4
	ret	nc
	jp	cerror
    end _dup
end dup
