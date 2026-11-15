! @[$] sptrace.s	2.2  09/12/83 11:43:19 - 87wega3.2 !

sptrace module

$segmented

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
	ld	r2,r4
	ld	r4,r3
	ld	r3,r5
	sc	#58
	ld	r2,r4
	ret	nc
	jp	cerror
    end _sptrace
end sptrace
