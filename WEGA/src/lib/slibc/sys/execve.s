! @[$] execve.s	2.1  09/12/83 11:40:51 - 87wega3.2 !

execve module

$segmented

  external
    cerror	procedure

  global
    _execve procedure
      entry
	ldl	rr0,rr6
	ldl	rr6,rr2
	ldl	rr2,rr4
	ldl	rr4,rr6
	sc	#59
	ld	r2,r4
	jp	cerror
    end _execve
end execve
