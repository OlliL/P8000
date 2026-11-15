! @[$] sprofil.s	1.1  08/17/83 15:23:42 - 87wega3.2 !

sprofil module

$segmented

  external
    _stkseg	procedure
    cerror	procedure

  global
    _sprofil procedure
      entry
	ld	r8,r2
	ld	r9,|_stkseg+4|(r15)
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	ld	r4,r8
	ld	r5,r9
	sc	#%34
	ld	r2,r4
	jp	c,cerror
	ret
    end _sprofil
end sprofil
