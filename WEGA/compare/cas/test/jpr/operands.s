/* Operand forms of jpr and callr that give an object: a label before and after the instruction, with a condition code, with an
 * offset (label+n, label-n), a register in indirect mode, a constant, the location counter, a symbol that is defined later or never
 * (extern), an equate, a global label, a label in the data and the bss section. What jpr turns into jp or jr (or leaves to the
 * second pass) depends on the kind of operand.
 */
	.code
La:
	jpr	La
	jpr	eq,La
	jpr	ne,Lb
	jpr	La+4
	jpr	Lb+6
	jpr	Lb-2
	jpr	@r5
	jpr	eq,@r5
	jpr	100
	jpr	.
	jpr	.+10
	jpr	undef1
	jpr	z,undef2+4
	callr	La
	callr	Lb
	callr	La+2
	callr	undef1
	callr	.
	callr	@r5
Lb:
	jpr	Lb
equ1 := La
equ2 := Lb+4
equ3 := 100
	jpr	equ1
	jpr	equ2
	jpr	equ3
	callr	equ1
	callr	equ2
Lg::
	jpr	Lg
	.data
Ldat:
	.word	1
	.code
	jpr	Ldat
	callr	Ldat
	.bss
Lz:
	.blkw	4
	.code
	jpr	Lz
	callr	Lz
	jpr	La
	callr	Lb
undef1:
undef2:
