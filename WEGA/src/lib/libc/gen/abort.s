! @[$] abort.s	2.1  09/12/83 11:19:15 - 87wega3.2 !

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
