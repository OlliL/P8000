! @[$] dup2.s	2.1  09/12/83 11:40:25 - 87wega3.2 !

dup2 module

  external
    cerror	procedure

  global
    _dup2 procedure
      entry
	ld	r0,r7
	ld	r1,r6
	set	r0,#6
	sc	#41
	ld	r2,r4
	ret	nc
	jp	cerror
    end _dup2
end dup2
