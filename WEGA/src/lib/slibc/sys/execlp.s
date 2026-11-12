! @[$] execlp.s	2.2  09/12/83 11:23:49 - 87wega3.2 !

execlp module

$segmented

  external
    _execvp	procedure

  global
    _execlp procedure
      entry
	ldl	rr0,@rr14
	ldl	@rr14,rr2
	pushl	@rr14,rr4
	ldl	rr4,rr14
	pushl	@rr14,rr0
	call	_execvp
	popl	rr6,@rr14
	inc	r15,#8
	jp	@rr6
    end _execlp
end execlp

