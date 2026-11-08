/* Targets with an offset (label+n, label-n) at the edge of the short form: the offset moves the distance, forward and backward, for jpr
 * (edge 256/-254) and callr (4098/-4092).
 */
	.code
	jpr	Lo78-6
	.blkb	250
Lo78:
Lo79:
	.blkb	250
	jpr	Lo79-6
	callr	Lo80-6
	.blkb	4094
Lo80:
	jpr	Lo81-4
	.blkb	250
Lo81:
Lo82:
	.blkb	250
	jpr	Lo82-4
	callr	Lo83-4
	.blkb	4094
Lo83:
	jpr	Lo84-2
	.blkb	250
Lo84:
Lo85:
	.blkb	250
	jpr	Lo85-2
	callr	Lo86-2
	.blkb	4094
Lo86:
	jpr	Lo87+0
	.blkb	250
Lo87:
Lo88:
	.blkb	250
	jpr	Lo88+0
	callr	Lo89+0
	.blkb	4094
Lo89:
	jpr	Lo90+2
	.blkb	250
Lo90:
Lo91:
	.blkb	250
	jpr	Lo91+2
	callr	Lo92+2
	.blkb	4094
Lo92:
	jpr	Lo93+4
	.blkb	250
Lo93:
Lo94:
	.blkb	250
	jpr	Lo94+4
	callr	Lo95+4
	.blkb	4094
Lo95:
	jpr	Lo96+6
	.blkb	250
Lo96:
Lo97:
	.blkb	250
	jpr	Lo97+6
	callr	Lo98+6
	.blkb	4094
Lo98:
