chown module

  external
    cerror	procedure

  global
    _chown procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	xor	r4,r4
	sc	#16
	ld	r2,r4
	ret	nc
	jp	cerror
    end _chown
end chown
