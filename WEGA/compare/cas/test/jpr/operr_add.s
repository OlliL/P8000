/* Error case: the sum of two labels in jpr: "Invalid addition expression". One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
Lb:
	jpr	La+Lb
