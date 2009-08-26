setuid module

  external
    _cerror

  global
    setuid procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#23
	ld	r2,r4
	ret	nc
	jp	cerror
    end setuid
end setuid
