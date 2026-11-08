/* Entries that jump over each other: A jumps forward over a block and over a jpr that jumps back to A; and three jprs in a ring. The
 * sizes are at the edge of the short form (-6 ... +6 bytes), so that the growth of one entry decides the others.
 */
	.code
L150:
	jpr	L151
	.blkb	244
	jpr	L150
L151:
L153:
	jpr	L154
	.blkb	117
L155:
	jpr	L153
	.blkb	117
L154:
	jpr	L155
L156:
	jpr	L157
	.blkb	246
	jpr	L156
L157:
L159:
	jpr	L160
	.blkb	118
L161:
	jpr	L159
	.blkb	118
L160:
	jpr	L161
L162:
	jpr	L163
	.blkb	248
	jpr	L162
L163:
L165:
	jpr	L166
	.blkb	119
L167:
	jpr	L165
	.blkb	119
L166:
	jpr	L167
L168:
	jpr	L169
	.blkb	250
	jpr	L168
L169:
L171:
	jpr	L172
	.blkb	120
L173:
	jpr	L171
	.blkb	120
L172:
	jpr	L173
L174:
	jpr	L175
	.blkb	252
	jpr	L174
L175:
L177:
	jpr	L178
	.blkb	121
L179:
	jpr	L177
	.blkb	121
L178:
	jpr	L179
L180:
	jpr	L181
	.blkb	254
	jpr	L180
L181:
L183:
	jpr	L184
	.blkb	122
L185:
	jpr	L183
	.blkb	122
L184:
	jpr	L185
L186:
	jpr	L187
	.blkb	256
	jpr	L186
L187:
L189:
	jpr	L190
	.blkb	123
L191:
	jpr	L189
	.blkb	123
L190:
	jpr	L191
