/* A jpr that stands directly at its label (the label is the next address): the old cas does not count it in the distance
 * of a forward jump (">" where the shipped cas has ">="), the shipped cas does.
 */
	.code
	.blkb	2
	jpr	M1
M1:
	jpr	M1
	callr	N1
N1:
	.blkb	4
	jpr	M2
M2:
	jpr	M2
	callr	N2
N2:
	.blkb	6
	jpr	M3
M3:
	jpr	M3
	callr	N3
N3:
	.blkb	8
	jpr	M4
M4:
	jpr	M4
	callr	N4
N4:
	.blkb	10
	jpr	M5
M5:
	jpr	M5
	callr	N5
N5:
	.blkb	12
	jpr	M6
M6:
	jpr	M6
	callr	N6
N6:
	jpr	P1
	jpr	P1
P1:
	jpr	P1
