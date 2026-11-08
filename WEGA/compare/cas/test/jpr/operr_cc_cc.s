/* Error case: two condition codes in jpr. One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
	jpr	eq,ne,La
