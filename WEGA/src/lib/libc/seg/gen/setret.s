! @[$] setret.s	2.1  09/12/83 11:30:27 - 87wega3.2 !

setret module

$segmented

  external
    _stkseg	procedure

  global
    _setret procedure
      entry
	ldl	rr2,rr6
	ldl	rr6,|_stkseg|(r15)
	ldm	@rr2,r6,#10
	clr	r2
	ret
    end _setret

    _saveret procedure
      entry
	addl	rr6,#%00000004
	ldm	@rr6,r8,#6
	ret
    end _saveret

    _longret procedure
      entry
	ld	r2,r5
	ldl	rr4,rr6
	ldm	r6,@rr4,#10
	ldl	|_stkseg|(r15),rr6
	ret
    end _longret
end setret
