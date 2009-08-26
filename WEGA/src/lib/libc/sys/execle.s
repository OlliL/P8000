execle module

  external
    cerror	procedure

  global
    _execle procedure
      entry
	ld	r0,@r15
	ex	r0,r7
	ld	@r15,r2
	push	@r15,r3
	push	@r15,r4
	push	@r15,r5
	push	@r15,r6
	ld	r1,r15
	ld	r2,r1
	ld	r3,#%8000
	xor	r4,r4
	cpir	r4,@r2,r3,z
	ld	r2,@r2
	sc	#59
	ld	r2,r4
	inc	r15,#8
	ld	@r15,r7
	jp	cerror
    end _execle
end execle
