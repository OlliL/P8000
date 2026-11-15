mcount_s module

  external
    countbase
      WORD

  global
    mcount procedure
      entry
	ld	r1,%0002(r15)
	ld	r0,@r1
	test	r0
	jr	nz,havestruct
	ld	r1,countbase
	test	r1
	ret	z
	inc	countbase,#6
	ld	r0,@r15
	ld	@r1,r0
	lda	r0,%0002(r1)
	ld	r1,%0002(r15)
	ld	@r1,r0
havestruct:
	ld	r1,r0
	inc	%0002(r1),#1
	ret	nz
	inc	@r1,#1
	ret
    end mcount
end mcount_s
