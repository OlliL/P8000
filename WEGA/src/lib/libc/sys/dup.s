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
