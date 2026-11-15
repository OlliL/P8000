! @[$] execv.s	2.1  09/12/83 11:40:45 - 87wega3.2 !

execv module

  external
    _environ	procedure
    cerror	procedure

  global
    _execv procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,_environ
	sc	#59
	ld	r2,r4
	jp	cerror
    end _execv
end execv
