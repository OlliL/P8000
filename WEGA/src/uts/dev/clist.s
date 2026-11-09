//*************************************************************************
//*************************************************************************
// 
//	W E G A - Quelle
//	KERN 3.2	Modul : clist.s
// 
//	Bearbeiter	: O. Lehmann
//	Datum		: 02.05.08
//	Version		: 1.0
//
//*************************************************************************
//*************************************************************************

// The character lists of the tty driver: assembler versions of the functions of System III clist.c.
// Every function runs with interrupts off and restores the status it found.
// Arguments: first in rr6, second in rr4 (r7 for a char); result in r2 or rr2.

.seg

.extern _cfreelist

// struct clist
C_CC		:= 0;	// int: number of characters
C_CF		:= 2;	// struct cblock *: first block
C_CL		:= 6;	// struct cblock *: last block

// struct cblock (c_next is at offset 0)
C_FIRST		:= 4;	// char: index of the first character
C_LAST		:= 5;	// char: index behind the last character
C_DATA		:= 6;	// char[]: the characters

// struct chead cfreelist (c_next is at offset 0)
C_SIZE		:= 4;	// int: characters in a block

.data

// cas takes %00 in a string as the three characters, not as a NUL: they are in the object
.byte	"@[$]clist.s	Rev : 4.1	09/16/83 03:43:09%00"

.code

// putcf(cp): put block cp on the free list
_putcf::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr4,_cfreelist
	ldl	@rr6,rr4	//cp->c_next = cfreelist.c_next
	ldl	_cfreelist,rr6	//cfreelist.c_next = cp
	ldctl	fcw,r1		//restore processor status
	ret
	}	/* _putcf */

// getcf(): take a block from the free list, empty and with c_last = c_size; 0 if there is none
_getcf::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,_cfreelist	//rr2 = cfreelist.c_next
	testl	rr2
	jr	z,~end		//if its 0, then just return.

	ldl	rr4,@rr2
	ldl	_cfreelist,rr4	//cfreelist.c_next = cfreelist.c_next->c_next

	subl	rr6,rr6
	ldl	@rr2,rr6	//clear c_next in cblock
	ld	r0,_cfreelist+C_SIZE
	ld	rr2(#C_FIRST),r0	//c_first = 0, c_last = size (one word)
~end:
	ldctl	fcw,r1		//restore processor status
	ret
	}	/* _getcf */

// getcb(p): remove the first block of list p and return it; 0 if the list is empty
_getcb::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,rr6(#C_CF)	//rr2 = p->c_cf
	testl	rr2
	jr	z,~end		//if its 0, then just return.

	ld	r0,rr2(#C_FIRST)
	subb	rl0,rh0		//rl0 = c_last - c_first
	subb	rh0,rh0
	ld	r4,@rr6
	sub	r4,r0
	ld	@rr6,r4		//p->c_cc -= characters of the block
	ldl	rr4,@rr2
	ldl	rr6(#C_CF),rr4	//p->c_cf = cp->c_next
	testl	rr4
	jpr	ne,~end
	subl	rr4,rr4
	ldl	rr6(#C_CL),rr4	//list is empty now: p->c_cl = 0
~end:
	ldctl	fcw,r1		//restore processor status
	ret
	}	/* _getcb */

// putcb(cp, p): append block cp to list p
_putcb::
{
	ldctl	r1,fcw		//save processor status
	push	@r14,r1
	di	vi,nvi		//dvi
	ldl	rr2,rr4(#C_CL)	//rr2 = p->c_cl
	testl	rr2
	jpr	ne,~append
	ldl	rr4(#C_CF),rr6	//empty list: cp is the first block
	ldl	rr4(#C_CL),rr6	//and the last
	jpr	~count
~append:
	ldl	rr4(#C_CL),rr6	//p->c_cl = cp
	ldl	@rr2,rr6	//old last block->c_next = cp
~count:
	subl	rr0,rr0
	ldl	@rr6,rr0	//cp->c_next = 0
	ld	r1,rr6(#C_FIRST)
	subb	rl1,rh1		//rl1 = c_last - c_first
	subb	rh1,rh1
	ld	r0,@rr4
	add	r0,r1
	ld	@rr4,r0		//p->c_cc += characters of the block
	pop	r1,@rr14
	ldctl	fcw,r1		//restore processor status
	ret
	}	/* _putcb */

// getc(p): next character of list p, -1 if it is empty. A block that gets empty goes to the free list
_getc::
{
	ldctl	r1,fcw		//save processor status
	push	@r14,r1
	di	vi,nvi		//dvi
	dec	@rr6,#1		//p->c_cc--
	jpr	lt,~empty
	ldl	rr2,rr6(#C_CF)	//rr2 = first block
	lda	rr4,rr2(#C_FIRST)
	ldb	rl1,@rr4	//rl1 = index of the character
	incb	@rr4,#1		//c_first++
	subb	rh1,rh1
	ldb	rl0,rr2(#C_LAST)
	cpb	rl0,@rr4
	jr	eq,~lastchar	//it was the last character of the block
	inc	r5,#2		//rr4 = c_data
	add	r5,r1
	ldb	rl2,@rr4	//the character
	subb	rh2,rh2
~end:
	pop	r1,@rr14
	ldctl	fcw,r1		//restore processor status
	ret
~empty:
	inc	@rr6,#1		//undo p->c_cc--
	ld	r2,#-1
	jpr	~end
~lastchar:
	inc	r5,#2		//rr4 = c_data
	add	r5,r1
	ldb	rl0,@rr4	//the character
	ldl	rr4,@rr2
	ldl	rr6(#C_CF),rr4	//p->c_cf = cp->c_next
	testl	rr4
	jpr	ne,~free
	ldl	rr6(#C_CL),rr4	//list is empty now: p->c_cl = 0
~free:
	ldl	rr4,_cfreelist	//get link to first free cblock
	ldl	@rr2,rr4	//put it into link of new free cblock
	ldl	_cfreelist,rr2	//put address of new free cblock into cfreelist
	ldb	rl2,rl0
	subb	rh2,rh2
	jpr	~end
	}	/* _getc */

// putc(c, p): append character c to list p; 0, or -1 if no block is free
_putc::
{
	ldctl	r6,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,rr4(#C_CL)	//rr2 = last block
	testl	rr2
	jpr	eq,~nolist
~store:
	ldb	rl0,rr2(#C_LAST)
	subb	rh0,rh0
	cp	r0,_cfreelist+C_SIZE
	jpr	ge,~full
	inc	@rr4,#1		//p->c_cc++
	lda	rr4,rr2(#C_DATA)
	add	r5,r0
	ldb	@rr4,rl7	//store the character
	incb	rl0,#1
	ldb	rr2(#C_LAST),rl0	//c_last++
	clr	r2
~end:
	ldctl	fcw,r6		//restore processor status
	ret
~nolist:
	ldl	rr2,_cfreelist	//rr2 = cfreelist.c_next
	testl	rr2
	jpr	eq,~nofree	//if(cfreelist empty) goto ~nofree
	ldl	rr4(#C_CF),rr2	//p->c_cf = rr2
~newblock:
	ldl	rr0,@rr2
	ldl	_cfreelist,rr0	//cfreelist.c_next = cfreelist.c_next->c_next
	clr	@rr2		//c_next, c_first, c_last = 0: this word, then 4 bytes from offset 2
	subl	rr0,rr0
	ldl	rr2(#2),rr0
	ldl	rr4(#C_CL),rr2	//p->c_cl = cp
	jpr	~store
~nofree:
	ld	r2,#-1
	jpr	~end
~full:
	ldl	rr0,_cfreelist	//last block is full: take a new one
	testl	rr0
	jpr	eq,~nofree
	ldl	@rr2,rr0	//last block->c_next = new block
	ldl	rr2,rr0
	jpr	~newblock
	}	/* _putc */
