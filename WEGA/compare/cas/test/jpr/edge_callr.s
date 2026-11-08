/* The edges of the short form of callr: calr up to +4098 and -4092 bytes (from the start of the instruction, with the instruction at its
 * short length). Each distance is tried forward and backward; the blocks are even, so the edges are hit exactly.
 */
	.code
	callr	Lf1
	.blkb	4090
Lf1:
Lb2:
	.blkb	4090
	callr	Lb2
	callr	Lf3
	.blkb	4096
Lf3:
Lb4:
	.blkb	4096
	callr	Lb4
	callr	Lf5
	.blkb	4098
Lf5:
Lb6:
	.blkb	4098
	callr	Lb6
	callr	Lf7
	.blkb	4100
Lf7:
Lb8:
	.blkb	4100
	callr	Lb8
	callr	Lf9
	.blkb	4102
Lf9:
Lb10:
	.blkb	4102
	callr	Lb10
	callr	Lf11
	.blkb	4106
Lf11:
Lb12:
	.blkb	4106
	callr	Lb12
