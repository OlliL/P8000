utime module

  external
    cerror	procedure

  global
    _utime procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#30
	ld	r2,r4
	ret	nc
	jp	cerror
    end _utime
end utime
