! @[$] sptrace.s	2.2  09/12/83 11:43:19 - 87wega3.2 !

sptrace module

  external
    cerror	procedure
    _errno	word

  global
    _sptrace procedure
      entry
	xor	r0,r0
	ld	_errno,r0
	ld	r0,r7
	ld	r1,r6
	ld	r6,r3
	ldl	rr2,rr4
	ld	r4,r6
	sc	#58
	ld	r2,r4
	ret	nc
	jp	cerror
    end _sptrace
end sptrace
