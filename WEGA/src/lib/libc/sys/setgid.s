setgid module

  external
    cerror	procedure

  global
    _setgid procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#46
	ld	r2,r4
	ret	nc
	jp	cerror
    end _setgid
end setgid
