/* jpr and callr in one chain: a jpr over K entries that are alternately callr and jpr with a target that is too far. callr grows by the same
 * amount as jpr; the block is at the edge of the short form of the first jpr.
 */
	.code
	jpr	Lm99
	callr	Lm100
	.blkb	250
Lm99:
	.blkb	4200
Lm100:
	jpr	Lm101
	callr	Lm102
	.blkb	252
Lm101:
	.blkb	4200
Lm102:
	jpr	Lm103
	callr	Lm104
	.blkb	254
Lm103:
	.blkb	4200
Lm104:
	jpr	Lm105
	callr	Lm106
	jpr	Lm106
	.blkb	248
Lm105:
	.blkb	4200
Lm106:
	jpr	Lm107
	callr	Lm108
	jpr	Lm108
	.blkb	250
Lm107:
	.blkb	4200
Lm108:
	jpr	Lm109
	callr	Lm110
	jpr	Lm110
	.blkb	252
Lm109:
	.blkb	4200
Lm110:
	jpr	Lm111
	callr	Lm112
	jpr	Lm112
	callr	Lm112
	.blkb	246
Lm111:
	.blkb	4200
Lm112:
	jpr	Lm113
	callr	Lm114
	jpr	Lm114
	callr	Lm114
	.blkb	248
Lm113:
	.blkb	4200
Lm114:
	jpr	Lm115
	callr	Lm116
	jpr	Lm116
	callr	Lm116
	.blkb	250
Lm115:
	.blkb	4200
Lm116:
	jpr	Lm117
	callr	Lm118
	jpr	Lm118
	callr	Lm118
	jpr	Lm118
	.blkb	244
Lm117:
	.blkb	4200
Lm118:
	jpr	Lm119
	callr	Lm120
	jpr	Lm120
	callr	Lm120
	jpr	Lm120
	.blkb	246
Lm119:
	.blkb	4200
Lm120:
	jpr	Lm121
	callr	Lm122
	jpr	Lm122
	callr	Lm122
	jpr	Lm122
	.blkb	248
Lm121:
	.blkb	4200
Lm122:
