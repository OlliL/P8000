/* A backward jpr B (a negative distance) at the edge of the short form. Its span holds a jpr F with a target at the edge, and inside the span of F a
 * jpr G with a target that is too far: G grows at once, F grows because of G, and the distance of B becomes longer by the growth of F. B jumps to the label plus an offset, so that
 * its distance stays at the edge (-6 ... +6 bytes) while G (252 ... 258 bytes of block behind it) is too far for the short form.
 */
	.code
Tn1:
	jpr	Zn2
	jpr	Fn3
	.blkb	252
Zn2:
	jpr	Tn1
Fn3:
Tn4:
	jpr	Zn5
	jpr	Fn6
	.blkb	252
Zn5:
	jpr	Tn4+1
Fn6:
Tn7:
	jpr	Zn8
	jpr	Fn9
	.blkb	252
Zn8:
	jpr	Tn7+2
Fn9:
Tn10:
	jpr	Zn11
	jpr	Fn12
	.blkb	252
Zn11:
	jpr	Tn10+3
Fn12:
Tn13:
	jpr	Zn14
	jpr	Fn15
	.blkb	252
Zn14:
	jpr	Tn13+4
Fn15:
Tn16:
	jpr	Zn17
	jpr	Fn18
	.blkb	252
Zn17:
	jpr	Tn16+5
Fn18:
Tn19:
	jpr	Zn20
	jpr	Fn21
	.blkb	252
Zn20:
	jpr	Tn19+6
Fn21:
Tn22:
	jpr	Zn23
	jpr	Fn24
	.blkb	252
Zn23:
	jpr	Tn22+7
Fn24:
Tn25:
	jpr	Zn26
	jpr	Fn27
	.blkb	252
Zn26:
	jpr	Tn25+8
Fn27:
Tn28:
	jpr	Zn29
	jpr	Fn30
	.blkb	254
Zn29:
	jpr	Tn28
Fn30:
Tn31:
	jpr	Zn32
	jpr	Fn33
	.blkb	254
Zn32:
	jpr	Tn31+1
Fn33:
Tn34:
	jpr	Zn35
	jpr	Fn36
	.blkb	254
Zn35:
	jpr	Tn34+2
Fn36:
Tn37:
	jpr	Zn38
	jpr	Fn39
	.blkb	254
Zn38:
	jpr	Tn37+3
Fn39:
Tn40:
	jpr	Zn41
	jpr	Fn42
	.blkb	254
Zn41:
	jpr	Tn40+4
Fn42:
Tn43:
	jpr	Zn44
	jpr	Fn45
	.blkb	254
Zn44:
	jpr	Tn43+5
Fn45:
Tn46:
	jpr	Zn47
	jpr	Fn48
	.blkb	254
Zn47:
	jpr	Tn46+6
Fn48:
Tn49:
	jpr	Zn50
	jpr	Fn51
	.blkb	254
Zn50:
	jpr	Tn49+7
Fn51:
Tn52:
	jpr	Zn53
	jpr	Fn54
	.blkb	254
Zn53:
	jpr	Tn52+8
Fn54:
Tn55:
	jpr	Zn56
	jpr	Fn57
	.blkb	254
Zn56:
	jpr	Tn55+9
Fn57:
Tn58:
	jpr	Zn59
	jpr	Fn60
	.blkb	254
Zn59:
	jpr	Tn58+10
Fn60:
Tn61:
	jpr	Zn62
	jpr	Fn63
	.blkb	256
Zn62:
	jpr	Tn61
Fn63:
Tn64:
	jpr	Zn65
	jpr	Fn66
	.blkb	256
Zn65:
	jpr	Tn64+1
Fn66:
Tn67:
	jpr	Zn68
	jpr	Fn69
	.blkb	256
Zn68:
	jpr	Tn67+2
Fn69:
Tn70:
	jpr	Zn71
	jpr	Fn72
	.blkb	256
Zn71:
	jpr	Tn70+3
Fn72:
Tn73:
	jpr	Zn74
	jpr	Fn75
	.blkb	256
Zn74:
	jpr	Tn73+4
Fn75:
Tn76:
	jpr	Zn77
	jpr	Fn78
	.blkb	256
Zn77:
	jpr	Tn76+5
Fn78:
Tn79:
	jpr	Zn80
	jpr	Fn81
	.blkb	256
Zn80:
	jpr	Tn79+6
Fn81:
Tn82:
	jpr	Zn83
	jpr	Fn84
	.blkb	256
Zn83:
	jpr	Tn82+7
Fn84:
Tn85:
	jpr	Zn86
	jpr	Fn87
	.blkb	256
Zn86:
	jpr	Tn85+8
Fn87:
Tn88:
	jpr	Zn89
	jpr	Fn90
	.blkb	256
Zn89:
	jpr	Tn88+9
Fn90:
Tn91:
	jpr	Zn92
	jpr	Fn93
	.blkb	256
Zn92:
	jpr	Tn91+10
Fn93:
Tn94:
	jpr	Zn95
	jpr	Fn96
	.blkb	256
Zn95:
	jpr	Tn94+11
Fn96:
Tn97:
	jpr	Zn98
	jpr	Fn99
	.blkb	256
Zn98:
	jpr	Tn97+12
Fn99:
Tn100:
	jpr	Zn101
	jpr	Fn102
	.blkb	258
Zn101:
	jpr	Tn100+2
Fn102:
Tn103:
	jpr	Zn104
	jpr	Fn105
	.blkb	258
Zn104:
	jpr	Tn103+3
Fn105:
Tn106:
	jpr	Zn107
	jpr	Fn108
	.blkb	258
Zn107:
	jpr	Tn106+4
Fn108:
Tn109:
	jpr	Zn110
	jpr	Fn111
	.blkb	258
Zn110:
	jpr	Tn109+5
Fn111:
Tn112:
	jpr	Zn113
	jpr	Fn114
	.blkb	258
Zn113:
	jpr	Tn112+6
Fn114:
Tn115:
	jpr	Zn116
	jpr	Fn117
	.blkb	258
Zn116:
	jpr	Tn115+7
Fn117:
Tn118:
	jpr	Zn119
	jpr	Fn120
	.blkb	258
Zn119:
	jpr	Tn118+8
Fn120:
Tn121:
	jpr	Zn122
	jpr	Fn123
	.blkb	258
Zn122:
	jpr	Tn121+9
Fn123:
Tn124:
	jpr	Zn125
	jpr	Fn126
	.blkb	258
Zn125:
	jpr	Tn124+10
Fn126:
Tn127:
	jpr	Zn128
	jpr	Fn129
	.blkb	258
Zn128:
	jpr	Tn127+11
Fn129:
Tn130:
	jpr	Zn131
	jpr	Fn132
	.blkb	258
Zn131:
	jpr	Tn130+12
Fn132:
Tn133:
	jpr	Zn134
	jpr	Fn135
	.blkb	258
Zn134:
	jpr	Tn133+13
Fn135:
Tn136:
	jpr	Zn137
	jpr	Fn138
	.blkb	258
Zn137:
	jpr	Tn136+14
Fn138:
