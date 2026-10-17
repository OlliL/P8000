
fp		:= r15;
sp		:= r15;
L21:
.byte	%77,%2b,%0
.even
/*~~tmpfile:*/

.code

_tmpfile::
{
	sub	fp,#~L2
	ld	22(fp),r14
	lda	r7,~L1(fp)
	callr	_tmpnam
	lda	r7,~L1(fp)
	ld	r6,#L21
	callr	_fopen
	ld	r14,r2
	lda	r7,~L1(fp)
	callr	_unlink
	ld	r2,r14
	ld	r14,22(fp)
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 24
	}	/* _tmpfile */

.data

