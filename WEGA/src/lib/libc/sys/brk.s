brk module

  external
    _end	procedure
    cerror	procedure

  global
    nd
	word    := _end
  
    _brk procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#17
	ld	r2,r4
	jp	c,cerror
	ld	nd,r0
	ret
    end _brk

    _sbrk procedure
      entry
	ld	r0,r7
	ld	r2,nd
	add	r0,r2
	ld	r1,r0
	dec	r1,#1
	dec	r2,#1
	cpb	rh1,rh2
	jr	z,t1
	sc	#17
	ld	r2,r4
	jp	c,cerror
t1:
	ld	r2,nd
	ld	nd,r0
	ret
   end _sbrk
end brk
