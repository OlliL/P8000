! @[$] ptrace.s	2.1  09/12/83 11:42:55 - 87wega3.2 !

ptrace module

  external
    cerror	procedure
    _errno	word

  global
    _ptrace procedure
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
    end _ptrace
end ptrace
