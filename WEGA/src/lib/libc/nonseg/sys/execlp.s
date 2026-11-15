! @[$] execlp.s	2.2  09/12/83 11:23:49 - 87wega3.2 !

execlp module

  external
    _execvp	procedure

  global
    _execlp procedure
      entry
	ex	r2,@r15
	push	@r15,r3
	push	@r15,r4
	push	@r15,r5
 	push	@r15,r6
	ld	r6,r15
	push	@r15,r2
	call	_execvp
	ld	r1,@r15
	inc	r15,#12
	jp	@r1
    end _execlp
end execlp

