/* The edges of the short form of jpr: jr up to +256 and -254 bytes (from the start of the instruction, with the instruction at its
 * short length). Each distance is tried forward and backward, also with a condition code; the blocks are even, so the edges are hit exactly.
 */
	.code
	jpr	L1
	.blkb	246
L1:
L2:
	.blkb	246
	jpr	L2
	jpr	eq,L3
	.blkb	246
L3:
	jpr	L4
	.blkb	248
L4:
L5:
	.blkb	248
	jpr	L5
	jpr	eq,L6
	.blkb	248
L6:
	jpr	L7
	.blkb	250
L7:
L8:
	.blkb	250
	jpr	L8
	jpr	eq,L9
	.blkb	250
L9:
	jpr	L10
	.blkb	252
L10:
L11:
	.blkb	252
	jpr	L11
	jpr	eq,L12
	.blkb	252
L12:
	jpr	L13
	.blkb	254
L13:
L14:
	.blkb	254
	jpr	L14
	jpr	eq,L15
	.blkb	254
L15:
	jpr	L16
	.blkb	256
L16:
L17:
	.blkb	256
	jpr	L17
	jpr	eq,L18
	.blkb	256
L18:
	jpr	L19
	.blkb	258
L19:
L20:
	.blkb	258
	jpr	L20
	jpr	eq,L21
	.blkb	258
L21:
	jpr	L22
	.blkb	260
L22:
L23:
	.blkb	260
	jpr	L23
	jpr	eq,L24
	.blkb	260
L24:
	jpr	L25
	.blkb	262
L25:
L26:
	.blkb	262
	jpr	L26
	jpr	eq,L27
	.blkb	262
L27:
