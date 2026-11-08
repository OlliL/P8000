/* Several sections. The code section is left and entered again between a jpr and its label, the data and bss sections get
 * labels, and a jpr in the code section jumps to labels of the data section (always long) while the entries of the code section are
 * optimized together at the end.
 */
	.code
	jpr	Ls29
	.blkb	240
Ls29:
	.data
Ls30:
	.word	1
	.code
	jpr	Ls30
	callr	Ls30
	.bss
	.blkw	4
	.code
	.data
	.word	0
	.code
	jpr	Ls31
	.blkb	246
Ls31:
	jpr	Ls32
	.blkb	244
Ls32:
	.data
Ls33:
	.word	1
	.code
	jpr	Ls33
	callr	Ls33
	.bss
	.blkw	4
	.code
	.data
	.word	0
	.code
	jpr	Ls34
	.blkb	248
Ls34:
	jpr	Ls35
	.blkb	248
Ls35:
	.data
Ls36:
	.word	1
	.code
	jpr	Ls36
	callr	Ls36
	.bss
	.blkw	4
	.code
	.data
	.word	0
	.code
	jpr	Ls37
	.blkb	250
Ls37:
