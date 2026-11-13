! @[$] ssgbrk.s	2.1 08/17/83 15:23:22 - 87wega3.2 !

ssgbrk module

$segmented

  external
    cerror	procedure

  global
    _ssgbrk procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ldl	rr4,#%00000000
	sc	#%2D
	ldl	rr2,rr4
	jp	c,cerror
	ret
    end _ssgbrk
end ssgbrk
