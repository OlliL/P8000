close module

  external
    cerror	procedure

  global
    _close procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#6
	ld	r2,r4
	ret	nc
	jp	cerror
    end _close
end close
