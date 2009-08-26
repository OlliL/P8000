lseek module

  external
    cerror	procedure

  global
    _lseek procedure
      entry
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	subl	rr4,rr4
	sc	#19
	ldl	rr2,rr4
	ret	nc
	jp	cerror
    end _lseek
end lseek
