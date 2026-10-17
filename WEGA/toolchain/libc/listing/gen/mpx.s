
fp		:= r15;
sp		:= r15;
.even
_vec:
.=.+10
/*~~mpx:*/

.code

_mpx::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,r7
	testb	@r2
	jpr	eq,L21
	ld	_vec,r7
	ld	_vec+4,r6
	ldk	r7,#5
	ld	r6,#_vec
	jpr	L20000
L21:
	ldk	r7,#6
	sub	r6,r6
L20000:
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _mpx */

.data

/*~~chan:*/

.code

_chan::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	_vec+6,r7
	ldk	r7,#1
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _chan */

.data

/*~~join:*/

.code

_join::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	_vec+4,r7
	ld	_vec+6,r6
	ldk	r7,#2
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _join */

.data

/*~~connect:*/

.code

_connect::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	_vec+4,r7
	ld	_vec+6,r6
	ld	_vec+8,r5
	ldk	r7,#7
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _connect */

.data

/*~~attach:*/

.code

_attach::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	_vec+4,r7
	ld	_vec+6,r6
	ldk	r7,#4
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _attach */

.data

/*~~detach:*/

.code

_detach::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	_vec+4,r7
	ld	_vec+6,r6
	ldk	r7,#8
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _detach */

.data

/*~~extract:*/

.code

_extract::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	_vec+4,r7
	ld	_vec+6,r6
	ld	_vec+8,r5
	ldk	r7,#3
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _extract */

.data

/*~~debug:*/

.code

_debug::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	_vec+4,r7
	ld	_vec+6,r6
	ldk	r7,#10
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _debug */

.data

/*~~npgrp:*/

.code

_npgrp::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	_vec+4,r7
	ld	_vec+6,r6
	ld	_vec+8,r5
	ldk	r7,#11
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _npgrp */

.data

/*~~ckill:*/

.code

_ckill::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	_vec+4,r7
	ld	_vec+6,r6
	ld	_vec+8,r5
	ldk	r7,#12
	ld	r6,#_vec
	callr	_mpxcall
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ckill */

.data

