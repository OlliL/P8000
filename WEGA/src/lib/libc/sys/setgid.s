setgid module

  external
    _cerror

  global
    setgid procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#46
	ld	r2,r4
	ret	nc
	jp	cerror
    end setgid
end setgid
