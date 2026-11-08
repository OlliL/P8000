/* Error case: a product with a label in jpr: "Both sides of * must be constants". One error in a file, because an error of this kind ends the assembly.
 */
	.code
La:
	jpr	La*2
