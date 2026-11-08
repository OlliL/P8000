/* callr at the edge of calr (+4098) with two entries in its span: a jpr with a target that is too far (it grows) and a jpr with a target right
 * behind it (it stays short). Together they would push the callr over the edge, the growth of one alone does not (-6 ... +6 bytes).
 */
	.code
	callr	Zh23
	jpr	Fh24
	jpr	Hh25
Hh25:
	.blkb	4084
Zh23:
	.blkb	300
Fh24:
	callr	Zh26
	jpr	Fh27
	jpr	Hh28
Hh28:
	.blkb	4086
Zh26:
	.blkb	300
Fh27:
	callr	Zh29
	jpr	Fh30
	jpr	Hh31
Hh31:
	.blkb	4088
Zh29:
	.blkb	300
Fh30:
	callr	Zh32
	jpr	Fh33
	jpr	Hh34
Hh34:
	.blkb	4090
Zh32:
	.blkb	300
Fh33:
	callr	Zh35
	jpr	Fh36
	jpr	Hh37
Hh37:
	.blkb	4092
Zh35:
	.blkb	300
Fh36:
	callr	Zh38
	jpr	Fh39
	jpr	Hh40
Hh40:
	.blkb	4094
Zh38:
	.blkb	300
Fh39:
	callr	Zh41
	jpr	Fh42
	jpr	Hh43
Hh43:
	.blkb	4096
Zh41:
	.blkb	300
Fh42:
