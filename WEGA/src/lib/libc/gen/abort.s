abort module

  global
    abort procedure
      entry
	sc	#20
	ld	r0,r4
	ldk	r1,#6
	sc	#37
	xor	r2,r2
	ret
    end abort
end abort
