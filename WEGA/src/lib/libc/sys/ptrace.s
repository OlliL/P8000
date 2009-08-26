ptrace module

  internal
    _errno
	word    :=2
  
  external
    _cerror

  global
    ptrace procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	ld	r3,r4
	xor	r4,r4
	ld	_errno,r4
	sc	#26
	ld	r2,r4
	ret	nc
	jp	cerror
    end ptrace
end ptrace
