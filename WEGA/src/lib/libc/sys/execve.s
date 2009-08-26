execve module

  external
    cerror	procedure

  global
    _execve procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#59
	ld	r2,r4
	jp	cerror
    end _execve
end execve
