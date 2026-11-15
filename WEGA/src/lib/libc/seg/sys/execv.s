! @[$] execv.s	2.1  09/12/83 11:40:45 - 87wega3.2 !

execv module

$segmented

  external
    _environ	procedure
    cerror	procedure

  global
    _execv procedure
      entry
	ldl	rr0,rr6
	ldl	rr2,rr4
	ldl	rr4,_environ
	sc	#59
	ld	r2,r4
	jp	cerror
    end _execv
end execv
