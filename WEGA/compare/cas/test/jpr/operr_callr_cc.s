/* Error case: callr with a condition code: "Invalid operand combination". One error in a file, because an error of this kind ends the assembly.
 */
	.code
Label:
	callr	eq,Label
