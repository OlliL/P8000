/* A backward jpr whose label is defined: the distance is known, but K jprs in between (K = 1 ... 4) jump forward to a label that
 * is not defined yet, so they are undecided and may grow. The block is sized so that the backward jpr is at the edge (-4 ... +4
 * bytes). jpr decides at once only when the entries in between cannot change the result.
 */
	.code
Lu38:
	.blkb	244
	jpr	Lu39
	jpr	Lu38
Lu39:
Lu40:
	.blkb	246
	jpr	Lu41
	jpr	Lu40
Lu41:
Lu42:
	.blkb	248
	jpr	Lu43
	jpr	Lu42
Lu43:
Lu44:
	.blkb	250
	jpr	Lu45
	jpr	Lu44
Lu45:
Lu46:
	.blkb	252
	jpr	Lu47
	jpr	Lu46
Lu47:
Lu48:
	.blkb	242
	jpr	Lu49
	jpr	Lu49
	jpr	Lu48
Lu49:
Lu50:
	.blkb	244
	jpr	Lu51
	jpr	Lu51
	jpr	Lu50
Lu51:
Lu52:
	.blkb	246
	jpr	Lu53
	jpr	Lu53
	jpr	Lu52
Lu53:
Lu54:
	.blkb	248
	jpr	Lu55
	jpr	Lu55
	jpr	Lu54
Lu55:
Lu56:
	.blkb	250
	jpr	Lu57
	jpr	Lu57
	jpr	Lu56
Lu57:
Lu58:
	.blkb	240
	jpr	Lu59
	jpr	Lu59
	jpr	Lu59
	jpr	Lu58
Lu59:
Lu60:
	.blkb	242
	jpr	Lu61
	jpr	Lu61
	jpr	Lu61
	jpr	Lu60
Lu61:
Lu62:
	.blkb	244
	jpr	Lu63
	jpr	Lu63
	jpr	Lu63
	jpr	Lu62
Lu63:
Lu64:
	.blkb	246
	jpr	Lu65
	jpr	Lu65
	jpr	Lu65
	jpr	Lu64
Lu65:
Lu66:
	.blkb	248
	jpr	Lu67
	jpr	Lu67
	jpr	Lu67
	jpr	Lu66
Lu67:
Lu68:
	.blkb	238
	jpr	Lu69
	jpr	Lu69
	jpr	Lu69
	jpr	Lu69
	jpr	Lu68
Lu69:
Lu70:
	.blkb	240
	jpr	Lu71
	jpr	Lu71
	jpr	Lu71
	jpr	Lu71
	jpr	Lu70
Lu71:
Lu72:
	.blkb	242
	jpr	Lu73
	jpr	Lu73
	jpr	Lu73
	jpr	Lu73
	jpr	Lu72
Lu73:
Lu74:
	.blkb	244
	jpr	Lu75
	jpr	Lu75
	jpr	Lu75
	jpr	Lu75
	jpr	Lu74
Lu75:
Lu76:
	.blkb	246
	jpr	Lu77
	jpr	Lu77
	jpr	Lu77
	jpr	Lu77
	jpr	Lu76
Lu77:
