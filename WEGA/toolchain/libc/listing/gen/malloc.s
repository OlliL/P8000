
fp		:= r15;
sp		:= r15;
_sccsid:
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%6d
.byte	%61
.byte	%6c
.byte	%6c
.byte	%6f
.byte	%63
.byte	%2e
.byte	%63
.byte	%9
.byte	%33
.byte	%2e
.byte	%33
.byte	%20
.byte	%20
.byte	%30
.byte	%39
.byte	%2f
.byte	%31
.byte	%32
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%31
.byte	%30
.byte	%3a
.byte	%33
.byte	%32
.byte	%3a
.byte	%33
.byte	%30
.byte	%20
.byte	%2d
.byte	%20
.byte	%38
.byte	%37
.byte	%77
.byte	%65
.byte	%67
.byte	%61
.byte	%33
.byte	%2e
.byte	%32
.byte	%0
.even
_maxbytes:
.word	%fff8
.even
_maxblocks:
.word	%40
.even
_allocs:
.=.+4
.even
_allocp:
.word	0
.even
_alloct:
.word	0
.even
_grown:
.word	0
/*~~malloc:*/

.code

_malloc::
{
	sub	fp,#~L2
	ldm	4(fp),r8,#7
	ld	r14,r7
	ldk	r8,#0
	cp	r7,_maxbytes
	jpr	ule,L20
L20004:
	sub	r2,r2
	jpr	L10000
L20:
	test	_allocp
	jpr	eq,L10003
	test	_grown
	jpr	ne,L21
L10003:
	ldk	r8,#1
L21:
	test	_allocp
	jpr	ne,L22
	ld	_allocp,#_allocs
	ld	_alloct,#_allocs+2
	ld	r2,#_allocs+2
	or	r2,#1
	ld	r3,_allocp
	ld	@r3,r2
	ld	r2,#_allocs
	or	r2,#1
	ld	r4,_alloct
	ld	@r4,r2
L22:
	lda	r2,3(r14)
	res	r2,#0
	rr	r2,#1
	ld	r10,r2
	ld	r13,_allocp
	ld	r2,@r13
	and	r2,#-2
	ld	r12,r2
	ld	r11,_alloct
L25:
	bit	@r13,#0
	jpr	eq,L27
	jpr	L26
L20001:
	ld	r2,@r12
	and	r2,#-2
	ld	r12,r2
	ld	@r13,r2
	ld	_allocp,r2
	jpr	L27
L20003:
	cp	r12,_allocp
	jpr	eq,L20001
	ld	r2,@r12
	and	r2,#-2
	ld	r12,r2
	ld	@r13,r2
L27:
	bit	@r12,#0
	jpr	eq,L20003
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r12,r2
	jpr	ult,L31
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r13,r2
	jpr	ule,L32
L31:
	cp	r12,_alloct
	jpr	ne,L26
	ld	r11,r13
L26:
	ld	r13,r12
	ld	r2,@r13
	and	r2,#-2
	ld	r12,r2
	cp	r13,_allocp
	jpr	ne,L25
	ld	r2,_alloct
	sub	r2,r11
	sra	r2,#1
	ld	r6,r10
	sub	r6,r2
	add	r6,r8
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#1
	addl	rr2,#1023
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#1024
	ld	r9,r7
	cp	r7,_maxblocks
	jpr	ugt,L20004
	ld	r2,r7
	sla	r2,#10
	ld	r14,r2
	sub	r7,r7
	callr	_sbrk
	ld	r13,r2
	add	r2,r14
	cp	r13,r2
	jpr	ugt,L20004
	ld	r7,r14
	callr	_sbrk
	ld	r13,r2
	cp	r2,#-1
	jpr	eq,L20004
	test	r8
	jpr	eq,L37
	or	r2,#1
	ld	r3,_alloct
	ld	@r3,r2
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,r13
	dec	r2,#2
	ld	r12,r2
	ld	_grown,#1
	jpr	L38
L37:
	ld	r13,r11
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,_alloct
	dec	r2,#2
	ld	r12,r2
L38:
	ld	r2,#_allocs
	or	r2,#1
	ld	@r12,r2
	ld	r2,@r12
	ld	@r12,r2
	ld	_alloct,r12
L32:
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	ld	_allocp,r2
	cp	r12,_allocp
	jpr	ule,L39
	ld	@r2,r12
L39:
	ld	r2,_allocp
	or	r2,#1
	ld	@r13,r2
	lda	r2,2(r13)
L10000:
	ldm	r8,4(fp),#7
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _malloc */

.data


.code

_findfree:
{
	dec	fp,#~L2
	ld	r1,r7
	lda	r2,3(r1)
	res	r2,#0
	rr	r2,#1
	ld	r3,r2
	ld	r4,_allocp
	ld	r2,@r4
	and	r2,#-2
	ld	r5,r2
L44:
	bit	@r4,#0
	jpr	eq,L46
	jpr	L45
L20006:
	ld	r2,@r5
	and	r2,#-2
	ld	r5,r2
	ld	@r4,r2
	ld	_allocp,r2
	jpr	L46
L20008:
	cp	r5,_allocp
	jpr	eq,L20006
	ld	r2,@r5
	and	r2,#-2
	ld	r5,r2
	ld	@r4,r2
L46:
	bit	@r5,#0
	jpr	eq,L20008
	ld	r2,r3
	add	r2,r3
	add	r2,r4
	cp	r5,r2
	jpr	ult,L45
	ld	r2,r3
	add	r2,r3
	add	r2,r4
	cp	r4,r2
	jpr	ugt,L45
	ld	r2,r4
	jpr	L10004
L45:
	ld	r4,r5
	ld	r2,@r4
	and	r2,#-2
	ld	r5,r2
	cp	r4,_allocp
	jpr	ne,L44
	sub	r2,r2
L10004:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _findfree */

.data

/*~~free:*/

.code

_free::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	@fp,r7
	ld	r14,~L1(fp)
	ld	r6,r14
	dec	r6,#2
	ld	r14,r6
	ld	_allocp,r6
	ld	r6,@r14
	and	r6,#-2
	ld	@r14,r6
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _free */

.data

/*~~realloc:*/

.code

_realloc::
{
	sub	fp,#~L2
	ldm	8(fp),r8,#7
	ld	@fp,r6
	ld	r14,r7
	dec	r7,#2
	ld	r10,r7
	ld	r7,@r10
	and	r7,#-2
	sub	r7,r14
	sra	r7,#1
	ld	r8,r7
	inc	r6,#1
	res	r6,#0
	rr	r6,#1
	ld	r11,r6
	bit	@r10,#0
	jpr	eq,L55
	ld	r7,r14
	callr	_free
L55:
	ld	r7,~L1(fp)
	callr	_findfree
	ld	r9,r2
	ld	r2,r11
	add	r2,r11
	add	r2,r9
	cp	r10,r2
	jpr	uge,L56
	ld	r2,r11
	add	r2,r11
	add	r2,r9
	ld	r3,r8
	add	r3,r8
	add	r3,r10
	cp	r2,r3
	jpr	uge,L56
	ld	r2,@r9
	or	r2,#1
	ld	@r9,r2
	ld	r7,~L1(fp)
	callr	_malloc
	ld	~L1+4(fp),r2
	ld	r2,@r9
	and	r2,#-2
	ld	@r9,r2
	jpr	L57
L56:
	ld	r7,~L1(fp)
	callr	_malloc
	ld	~L1+4(fp),r2
L57:
	test	~L1+4(fp)
	jpr	eq,L20011
	cp	r14,~L1+4(fp)
	jpr	eq,L20011
	ld	r13,r14
	ld	r12,~L1+4(fp)
	cp	r11,r8
	jpr	ule,L10014
	ld	r2,r8
	jpr	L10015
L10014:
	ld	r2,r11
L10015:
	ld	r11,r2
	jpr	L59
L20010:
	ld	r2,@r13
	ld	@r12,r2
	inc	r13,#2
	inc	r12,#2
L59:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jpr	ne,L20010
L20011:
	ld	r2,~L1+4(fp)
	ldm	r8,8(fp),#7
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _realloc */

.data

/*~~calloc:*/

.code

_calloc::
{
	dec	fp,#~L2
	ldm	6(fp),r12,#3
	ldl	@fp,rr6
	ld	r3,~L1+2(fp)
	mult	rr2,~L1(fp)
	ld	~L1+2(fp),r3
	ld	r7,r3
	callr	_malloc
	ld	r12,r2
	test	r2
	jpr	ne,L63
	sub	r2,r2
	jpr	L10016
L63:
	ld	r14,r12
	ld	r2,~L1+2(fp)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r13,r2
	jpr	L64
L20013:
	clr	@r14
	inc	r14,#2
L64:
	dec	r13,#1
	cp	r13,#0
	jpr	ge,L20013
	ld	r2,r12
L10016:
	ldm	r12,6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _calloc */

.data

/*~~cfree:*/

.code

_cfree::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	r4,r7
	callr	_free
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _cfree */

.data

