/* callr at the edge of calr (+4098) when an entry in its span grows: a jpr with a target that is too far, the size of the block moves the
 * distance over the edge (-10 ... +10 bytes), forward.
 */
	.code
	callr	Zc1
	jpr	Fc2
	.blkb	4082
Zc1:
	.blkb	300
Fc2:
	callr	Zc3
	jpr	Fc4
	.blkb	4084
Zc3:
	.blkb	300
Fc4:
	callr	Zc5
	jpr	Fc6
	.blkb	4086
Zc5:
	.blkb	300
Fc6:
	callr	Zc7
	jpr	Fc8
	.blkb	4088
Zc7:
	.blkb	300
Fc8:
	callr	Zc9
	jpr	Fc10
	.blkb	4090
Zc9:
	.blkb	300
Fc10:
	callr	Zc11
	jpr	Fc12
	.blkb	4092
Zc11:
	.blkb	300
Fc12:
	callr	Zc13
	jpr	Fc14
	.blkb	4094
Zc13:
	.blkb	300
Fc14:
	callr	Zc15
	jpr	Fc16
	.blkb	4096
Zc15:
	.blkb	300
Fc16:
	callr	Zc17
	jpr	Fc18
	.blkb	4098
Zc17:
	.blkb	300
Fc18:
	callr	Zc19
	jpr	Fc20
	.blkb	4100
Zc19:
	.blkb	300
Fc20:
	callr	Zc21
	jpr	Fc22
	.blkb	4102
Zc21:
	.blkb	300
Fc22:
