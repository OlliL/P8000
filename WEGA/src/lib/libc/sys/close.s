close module

  external
    _cerror

  global
    close procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#6
	ld	r2,r4
	ret	nc
	jp	cerror
    end close
end close
