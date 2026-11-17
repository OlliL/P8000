! @[$] fputil.s	1.8 01/18/83 13:46:56 - 87wega3.2 !

fputil module

  global
    _frexp procedure
      entry
	fldd	r0,f3
	ld	r4,r0
	sra	r4,#4
	and	r4,#%07FF
	sub	r4,#%03FE
	ld	@r7,r4
	and	r0,#%800F
	or	r0,#%3FE0
	fldd	f0,r0
	ret
    end _frexp

    _ldexp procedure
      entry
	sla	r7,#4
	fldd	r0,f3
	add	r0,r7
	fldd	f0,r0
	ret
    end _ldexp

    _modf procedure
      entry
	fsetmode	rz
	fint	f1,f3
	fsetmode	rn
	fldd	@r7,f1
	fsub	f3,f1
	fabs	f0,f3
	ret
    end _modf

    _initfp procedure
      entry
	subl	rr0,rr0
	fldctl	fflags,rr0
	fldctl	user,rr0
	ret
    end _initfp

    _floor procedure
      entry
	fldctl	rr0,user
	fsetmode	rm
	jr	L1
    end _floor

    _ceil procedure
      entry
	fldctl	rr0,user
	fsetmode	rp
L1:
	fint	f0,f3
	fldctl	user,rr0
	ret
    end _ceil

    _fabs procedure
      entry
	fabs	f0,f3
	ret
    end _fabs

    _sqrt procedure
      entry
	fsqr	f0,f3
	ret
    end _sqrt

end fputil
