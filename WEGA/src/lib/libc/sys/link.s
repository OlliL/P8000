link module

  external
    _cerror

  global
    link procedure
      entry
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#9
	ld	r2,r4
	ret	nc
	jp	cerror
    end link
end link
