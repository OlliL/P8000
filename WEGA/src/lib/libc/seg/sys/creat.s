! @[$] creat.s	2.1  09/12/83 11:40:14 - 87wega3.2 !

creat module

$segmented

  external
    cerror	procedure

  global
    _creat procedure
      entry
	ldl	rr0,rr6
	ld	r2,r5
	sc	#8
	ld	r2,r4
	ret	nc
	jp	cerror
    end _creat
end creat
