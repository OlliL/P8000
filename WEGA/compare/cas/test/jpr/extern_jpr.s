/* jpr and callr to an external symbol: the target is not known, so the instruction is jp or call at once.
 */
	.code
	.extern	ext1
	jpr	ext1
	callr	ext1
	jpr	eq,ext1
