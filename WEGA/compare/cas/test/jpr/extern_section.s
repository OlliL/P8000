/* jpr to an external symbol, then another section: what the assembler does with the entry when the section ends.
 */
	.code
	.extern	ext1
	jpr	ext1
	.data
Ldat:
	.word	1
