/* Odd and even lengths. A block of D bytes (D = 1 ... 7) in front of or inside the distance, .even and .odd in front of the
 * jpr: the instruction is aligned to a word, and the distance to the label changes with the length of the block.
 */
	.code
	.blkb	1
	jpr	Lp1
	.blkb	251
Lp1:
	.blkb	1
Lp2:
	.blkb	251
	jpr	Lp2
	.byte	1
	.even
	jpr	Lp3
	.blkb	249
Lp3:
	.byte	1
	.odd
	.blkb	2
Lp4:
	.blkb	249
	jpr	Lp4
	.blkb	2
	jpr	Lp5
	.blkb	252
Lp5:
	.blkb	2
Lp6:
	.blkb	252
	jpr	Lp6
	.byte	2
	.even
	jpr	Lp7
	.blkb	250
Lp7:
	.byte	2
	.odd
	.blkb	2
Lp8:
	.blkb	250
	jpr	Lp8
	.blkb	3
	jpr	Lp9
	.blkb	253
Lp9:
	.blkb	3
Lp10:
	.blkb	253
	jpr	Lp10
	.byte	3
	.even
	jpr	Lp11
	.blkb	251
Lp11:
	.byte	3
	.odd
	.blkb	2
Lp12:
	.blkb	251
	jpr	Lp12
	.blkb	4
	jpr	Lp13
	.blkb	254
Lp13:
	.blkb	4
Lp14:
	.blkb	254
	jpr	Lp14
	.byte	4
	.even
	jpr	Lp15
	.blkb	252
Lp15:
	.byte	4
	.odd
	.blkb	2
Lp16:
	.blkb	252
	jpr	Lp16
	.blkb	5
	jpr	Lp17
	.blkb	255
Lp17:
	.blkb	5
Lp18:
	.blkb	255
	jpr	Lp18
	.byte	5
	.even
	jpr	Lp19
	.blkb	253
Lp19:
	.byte	5
	.odd
	.blkb	2
Lp20:
	.blkb	253
	jpr	Lp20
	.blkb	6
	jpr	Lp21
	.blkb	256
Lp21:
	.blkb	6
Lp22:
	.blkb	256
	jpr	Lp22
	.byte	6
	.even
	jpr	Lp23
	.blkb	254
Lp23:
	.byte	6
	.odd
	.blkb	2
Lp24:
	.blkb	254
	jpr	Lp24
	.blkb	7
	jpr	Lp25
	.blkb	257
Lp25:
	.blkb	7
Lp26:
	.blkb	257
	jpr	Lp26
	.byte	7
	.even
	jpr	Lp27
	.blkb	255
Lp27:
	.byte	7
	.odd
	.blkb	2
Lp28:
	.blkb	255
	jpr	Lp28
