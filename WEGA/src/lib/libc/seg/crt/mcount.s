mcount_s module

$segmented

  external
    _stkseg	procedure
    _ctbase
      LONG

  global
    _cntno
      WORD

    mcount procedure
      entry
	dec	r15,#8
	ldm	|_stkseg|(r15),r2,#4
	ldl	rr2,|_stkseg+12|(r15)
	ldl	rr0,@rr2
	testl	rr0
	jr	nz,havestruct
	ld	r2,_cntno
	test	r2
	jr	z,done
	dec	_cntno,#1
	ldl	rr2,_ctbase
	inc	_ctbase+2,#8
	ldl	rr4,|_stkseg+8|(r15)
	and	r4,#%7fff
	ldl	@rr2,rr4
	lda	rr0,rr2(#4)
	ldl	rr4,|_stkseg+12|(r15)
	ldl	@rr4,rr0
havestruct:
	ldl	rr2,rr0
	add	r3,#2
	inc	@rr2,#1
	jr	nz,done
	sub	r3,#2
	inc	@rr2,#1
done:
	ldm	r2,|_stkseg|(r15),#4
	inc	r15,#8
	ret
    end mcount
end mcount_s
