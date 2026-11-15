! @[$] chmod.s	2.1  09/12/83 11:39:50 - 87wega3.2 !

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
