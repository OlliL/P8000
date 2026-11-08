/* callr at the edge of calr (-4092) when an entry in its span grows: a jpr with a target that is too far, the size of the block moves the
 * distance over the edge (-10 ... +10 bytes), backward.
 */
	.code
Tc23:
	.blkb	4076
	jpr	Fc24
	callr	Tc23
Fc24:
Tc25:
	.blkb	4078
	jpr	Fc26
	callr	Tc25
Fc26:
Tc27:
	.blkb	4080
	jpr	Fc28
	callr	Tc27
Fc28:
Tc29:
	.blkb	4082
	jpr	Fc30
	callr	Tc29
Fc30:
Tc31:
	.blkb	4084
	jpr	Fc32
	callr	Tc31
Fc32:
Tc33:
	.blkb	4086
	jpr	Fc34
	callr	Tc33
Fc34:
Tc35:
	.blkb	4088
	jpr	Fc36
	callr	Tc35
Fc36:
Tc37:
	.blkb	4090
	jpr	Fc38
	callr	Tc37
Fc38:
Tc39:
	.blkb	4092
	jpr	Fc40
	callr	Tc39
Fc40:
Tc41:
	.blkb	4094
	jpr	Fc42
	callr	Tc41
Fc42:
Tc43:
	.blkb	4096
	jpr	Fc44
	callr	Tc43
Fc44:
