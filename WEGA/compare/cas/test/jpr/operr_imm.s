/* Error case: an immediate operand: "Invalid operand combination". One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
	jpr	#La
