open module

  external
    _cerror

  global
    open procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#5
	ld	r2,r4
	ret	nc
	jp	cerror
    end open
end open
