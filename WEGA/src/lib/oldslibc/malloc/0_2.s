	.seg
	.extern	_stkseg
	.extern	_ssgbrk
	.psec
	.code

_calloc::
	sub	r15,#%001C
	ldm	_stkseg+18(r15),r9,#5
	ldl	|_stkseg+0|(r15),rr6
	ld	r3,|_stkseg+2|(r15)
	mult	rr2,r6
	ld	|_stkseg+2|(r15),r3
	ld	r7,r3
	call	Laef6
	ldl	rr12,rr2
	testl	rr2
	jr	ne,Laeb6
	subl	rr2,rr2
	jr	Laed4
Laeb6:
	ldl	rr10,rr12
	ld	r2,|_stkseg+2|(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r9,r2
	jr	Laeca
Laec6:
	clr	@rr10
	inc	r11,#2
Laeca:
	dec	r9,#1
	cp	r9,#%0000
	jr	ge,Laec6
	ldl	rr2,rr12
Laed4:
	ldm	r9,_stkseg+18(r15),#5
	add	r15,#%001C
	ret
_cfree::
	dec	r15,#8
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,rr6
	call	Lb168
	inc	r15,#8
	ret
Laef6:
_malloc::
	sub	r15,#%0022
	ldm	_stkseg+22(r15),r8,#6
	ld	|_stkseg+0|(r15),r7
	ldl	rr2,D9136
	testl	rr2
	jr	ne,Laf94
	ldl	rr2,#D913a
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	D9136,rr4
	ldl	rr2,#D9136
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	D913a,rr4
	ldl	rr2,#D913a
	ldl	D9150,rr2
	and	r2,#%7F00
	ldl	D9150,rr2
	ldl	D9142,rr2
	ldl	rr4,#D9136
	ldl	D9150,rr4
	and	r4,#%7F00
	ldl	D9150,rr4
	ldl	D913e,rr4
	ldl	rr0,#D914c
	ldl	D914c,rr0
	ld	r6,D914c
	and	r6,#%7FFF
	ldb	rl6,rh6
	clrb	rh6
	ld	D914c,r6
Laf94:
	ld	r2,|_stkseg+0|(r15)
	inc	r2,#11
	srl	r2,#2
	ld	|_stkseg+14|(r15),r2
	subl	rr8,rr8
	ldl	rr12,D913e
Lafaa:
	clr	D914a
Lafb0:
	ldl	rr2,@rr12
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	ne,Laff6
	ldl	rr8,rr12
	jr	Lafc4
Lafc0:
	ldl	rr2,@rr10
	ldl	@rr12,rr2
Lafc4:
	ldl	rr10,@rr12
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,Lafc0
	ld	r2,|_stkseg+14|(r15)
	sla	r2,#2
	ldl	rr4,rr12
	add	r5,r2
	cpl	rr10,rr4
	jr	c,Laff6
	ld	r2,|_stkseg+14|(r15)
	sla	r2,#2
	ldl	rr4,rr12
	add	r5,r2
	cpl	rr12,rr4
	jp	ule,Lb11a
Laff6:
	ldl	rr10,rr12
	ldl	rr2,@rr12
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr12,rr4
	cpl	rr4,rr10
	jr	ugt,Lafb0
	cpl	rr10,D9142
	jr	ne,Lb01a
	cpl	rr4,D9150
	jr	eq,Lb022
Lb01a:
	subl	rr2,rr2
	jp	Lb15a
Lb022:
	inc	D914a,#1
	cp	D914a,#%0001
	jr	ule,Lafb0
	testl	rr8
	jr	eq,Lb054
	ldl	rr2,@rr8
	cpl	rr2,D9142
	jr	ne,Lb054
	ldl	rr2,D9142
	sub	r3,r9
	sra	r3,#2
	ld	r2,|_stkseg+14|(r15)
	sub	r2,r3
	jr	Lb058
Lb054:
	ld	r2,|_stkseg+14|(r15)
Lb058:
	ld	|_stkseg+16|(r15),r2
	add	r2,#%0100
	ldb	rl2,rh2
	clrb	rh2
	ldb	rh2,rl2
	clrb	rl2
	ld	D914a,r2
	ld	r7,D914c
	sub	r6,r6
	call	_ssgbrk
	and	r2,#%7F00
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr10,rr4
	ld	r2,D914a
	sla	r2,#2
	add	r5,r2
	cpl	rr10,rr4
	jr	ugt,Lb01a
	ld	r6,D914a
	sla	r6,#2
	ld	r7,D914c
	call	_ssgbrk
	and	r2,#%7F00
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr10,rr4
	cpl	rr4,#%FFFFFFFF
	jr	eq,Lb01a
	ldl	rr2,D9142
	ldl	@rr2,rr4
	inc	r3,#4
	cpl	rr10,rr2
	jr	eq,Lb0e4
	ldl	rr2,D9142
	ldl	rr4,@rr2
	or	r5,#%0001
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	@rr2,rr6
Lb0e4:
	ld	r2,D914a
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	dec	r5,#4
	ldl	@rr10,rr4
	ldl	D9142,rr4
	ldl	rr2,#D9136
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr2,D9142
	ldl	@rr2,rr4
	jp	Lafaa
Lb11a:
	ldk	r2,#0
	ld	r3,|_stkseg+14|(r15)
	slal	rr2,#2
	ldl	rr4,rr12
	add	r5,r3
	ldl	D913e,rr4
	cpl	rr10,D913e
	jr	ule,Lb144
	ldl	rr2,rr4
	ldl	rr4,@rr2
	ldl	D9146,rr4
	ldl	rr6,@rr12
	ldl	@rr2,rr6
Lb144:
	ldl	rr2,D913e
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr12,rr4
	ldl	rr2,rr12
	inc	r3,#4
Lb15a:
	ldm	r8,_stkseg+22(r15),#6
	add	r15,#%0022
	ret
Lb168:
_free::
	dec	r15,#16
	ldm	_stkseg+8(r15),r10,#4
	ldl	rr12,rr6
	ldl	rr10,rr6
	dec	r11,#4
	ldl	D913e,rr10
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr10,rr4
	ldm	r10,_stkseg+8(r15),#4
	inc	r15,#16
	ret
_realloc::
	sub	r15,#%0026
	ldm	_stkseg+28(r15),r9,#5
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	ldl	rr2,rr12(#%FFFC)
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,Lb1ba
	calr	Lb168
Lb1ba:
	ldl	rr2,rr12(#%FFFC)
	sub	r3,r13
	sra	r3,#2
	ld	r5,r3
	exts	rr4
	ldl	|_stkseg+20|(r15),rr4
	ld	r7,|_stkseg+0|(r15)
	calr	Laef6
	and	r2,#%7F00
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr10,rr4
	testl	rr4
	jr	eq,Lb266
	cpl	rr4,rr12
	jr	eq,Lb266
	ldl	|_stkseg+10|(r15),rr12
	ldl	|_stkseg+14|(r15),rr4
	ld	r2,|_stkseg+0|(r15)
	inc	r2,#1
	srl	r2,#2
	ld	r9,r2
	ldk	r2,#0
	ld	r3,r9
	cpl	rr2,|_stkseg+20|(r15)
	jr	nc,Lb220
	ld	r3,r9
	ldl	|_stkseg+20|(r15),rr2
	jr	Lb220
Lb20c:
	ldl	rr2,|_stkseg+10|(r15)
	ldl	rr4,|_stkseg+14|(r15)
	ldl	rr6,@rr2
	ldl	@rr4,rr6
	inc	|_stkseg+12|(r15),#4
	inc	|_stkseg+16|(r15),#4
Lb220:
	ldl	rr2,|_stkseg+20|(r15)
	ldl	rr4,rr2
	subl	rr2,#%00000001
	ldl	|_stkseg+20|(r15),rr2
	testl	rr4
	jr	ne,Lb20c
	cpl	rr10,rr12
	jr	nc,Lb266
	ld	r2,r9
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr12,rr4
	jr	ugt,Lb266
	ld	r2,r9
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	sub	r5,r13
	sra	r5,#2
	sla	r5,#2
	ldl	rr2,rr10
	add	r3,r5
	ldl	rr4,D9146
	ldl	@rr2,rr4
Lb266:
	ldl	rr2,rr10
	ldm	r9,_stkseg+28(r15),#5
	add	r15,#%0026
	ret
	.psec data
	.data
D9136:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D913a:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D913e:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D9142:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D9146:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D914a:
	.byte	%00
	.byte	%00
D914c:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D9150:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
