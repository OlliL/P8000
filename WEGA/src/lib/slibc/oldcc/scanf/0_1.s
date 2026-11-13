// scanf.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_scanf
	.extern	_stkseg
	.extern	__iob
	.extern	__doscan
	.extern	_fscanf
	.extern	_sscanf
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 238 bytes
	.psec
	.code
_scanf::
	sub	r15,#%0026
	ldm	|_stkseg+4|(r15),r2,#6
	ldl	|_stkseg+16|(r15),rr4
	ldl	|_stkseg+20|(r15),rr2
	ldl	rr0,#_stkseg+42
	add	r1,r15
	ldl	|_stkseg|(r15),rr0
	ldl	rr2,#_stkseg+16
	add	r3,r15
	ldl	rr4,|_stkseg+12|(r15)
	ldl	rr6,#__iob
	call	__doscan
	add	r15,#%0026
	ret
_fscanf::
	sub	r15,#%0026
	ldm	|_stkseg+4|(r15),r2,#6
	ldl	|_stkseg+16|(r15),rr2
	ldl	rr0,_stkseg+42(r15)
	ldl	|_stkseg+20|(r15),rr0
	ldl	rr0,#_stkseg+46
	add	r1,r15
	ldl	|_stkseg|(r15),rr0
	ldl	rr2,#_stkseg+16
	add	r3,r15
	call	__doscan
	add	r15,#%0026
	ret
_sscanf::
	sub	r15,#%0036
	ldm	_stkseg+50(r15),r12,#2
	ldm	|_stkseg+4|(r15),r2,#4
	ldl	rr12,rr6
	ldb	|_stkseg+26|(r15),#%41
	ldl	|_stkseg+22|(r15),rr6
	ldl	|_stkseg+16|(r15),rr6
	clr	|_stkseg+20|(r15)
	jr	L0t_00a0
L0t_009c:
	inc	|_stkseg+20|(r15),#1
L0t_00a0:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L0t_009c
	ldl	rr2,|_stkseg+4|(r15)
	ldl	|_stkseg+28|(r15),rr2
	ldl	rr4,_stkseg+58(r15)
	ldl	|_stkseg+32|(r15),rr4
	ldl	rr6,#_stkseg+62
	add	r7,r15
	ldl	|_stkseg|(r15),rr6
	ldl	rr6,#_stkseg+16
	add	r7,r15
	ldl	rr2,#_stkseg+28
	add	r3,r15
	ldl	rr4,|_stkseg+8|(r15)
	call	__doscan
	ldm	r12,_stkseg+50(r15),#2
	add	r15,#%0036
	ret
