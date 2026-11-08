/* Error case: the negated label in jpr: "Invalid subtraction expression". One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
	jpr	-La
