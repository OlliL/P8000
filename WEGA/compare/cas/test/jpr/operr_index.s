/* Error case: an indexed operand: "Invalid operand combination". One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
	jpr	r5(#La)
