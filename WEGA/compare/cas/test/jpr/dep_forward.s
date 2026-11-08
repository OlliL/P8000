/* Forward dependencies. The jpr A at the top jumps over K jprs (K = 1 ... 5) with a target that is surely too far (they grow by 2
 * bytes, 4 in the segmented mode); the block behind them is sized so that A is at the edge of the short form (-4 ... +4 bytes
 * from the limit). Whether A stays short depends on the growth of the entries in between.
 */
	.code
	jpr	L50
	jpr	L51
	.blkb	248
L50:
	.blkb	300
L51:
	jpr	L52
	jpr	L53
	.blkb	250
L52:
	.blkb	300
L53:
	jpr	L54
	jpr	L55
	.blkb	252
L54:
	.blkb	300
L55:
	jpr	L56
	jpr	L57
	.blkb	254
L56:
	.blkb	300
L57:
	jpr	L58
	jpr	L59
	.blkb	256
L58:
	.blkb	300
L59:
	jpr	L60
	jpr	L61
	jpr	L61
	.blkb	246
L60:
	.blkb	300
L61:
	jpr	L62
	jpr	L63
	jpr	L63
	.blkb	248
L62:
	.blkb	300
L63:
	jpr	L64
	jpr	L65
	jpr	L65
	.blkb	250
L64:
	.blkb	300
L65:
	jpr	L66
	jpr	L67
	jpr	L67
	.blkb	252
L66:
	.blkb	300
L67:
	jpr	L68
	jpr	L69
	jpr	L69
	.blkb	254
L68:
	.blkb	300
L69:
	jpr	L70
	jpr	L71
	jpr	L71
	jpr	L71
	.blkb	244
L70:
	.blkb	300
L71:
	jpr	L72
	jpr	L73
	jpr	L73
	jpr	L73
	.blkb	246
L72:
	.blkb	300
L73:
	jpr	L74
	jpr	L75
	jpr	L75
	jpr	L75
	.blkb	248
L74:
	.blkb	300
L75:
	jpr	L76
	jpr	L77
	jpr	L77
	jpr	L77
	.blkb	250
L76:
	.blkb	300
L77:
	jpr	L78
	jpr	L79
	jpr	L79
	jpr	L79
	.blkb	252
L78:
	.blkb	300
L79:
	jpr	L80
	jpr	L81
	jpr	L81
	jpr	L81
	jpr	L81
	.blkb	242
L80:
	.blkb	300
L81:
	jpr	L82
	jpr	L83
	jpr	L83
	jpr	L83
	jpr	L83
	.blkb	244
L82:
	.blkb	300
L83:
	jpr	L84
	jpr	L85
	jpr	L85
	jpr	L85
	jpr	L85
	.blkb	246
L84:
	.blkb	300
L85:
	jpr	L86
	jpr	L87
	jpr	L87
	jpr	L87
	jpr	L87
	.blkb	248
L86:
	.blkb	300
L87:
	jpr	L88
	jpr	L89
	jpr	L89
	jpr	L89
	jpr	L89
	.blkb	250
L88:
	.blkb	300
L89:
	jpr	L90
	jpr	L91
	jpr	L91
	jpr	L91
	jpr	L91
	jpr	L91
	.blkb	240
L90:
	.blkb	300
L91:
	jpr	L92
	jpr	L93
	jpr	L93
	jpr	L93
	jpr	L93
	jpr	L93
	.blkb	242
L92:
	.blkb	300
L93:
	jpr	L94
	jpr	L95
	jpr	L95
	jpr	L95
	jpr	L95
	jpr	L95
	.blkb	244
L94:
	.blkb	300
L95:
	jpr	L96
	jpr	L97
	jpr	L97
	jpr	L97
	jpr	L97
	jpr	L97
	.blkb	246
L96:
	.blkb	300
L97:
	jpr	L98
	jpr	L99
	jpr	L99
	jpr	L99
	jpr	L99
	jpr	L99
	.blkb	248
L98:
	.blkb	300
L99:
