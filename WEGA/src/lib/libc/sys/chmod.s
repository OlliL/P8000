chmod module

  external
    cerror	procedure

  global
    _chmod procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#15
	ld	r2,r4
	ret	nc
	jp	cerror
    end _chmod
end chmod
