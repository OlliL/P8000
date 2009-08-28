! @[$] setret.s	2.1  09/12/83 11:30:27 - 87wega3.2 !

setret module

  global
    _setret procedure
      entry
	ld	r2,r7
	ld	r7,@r15
	ldm	@r2,r7,#9
	clr	r2
	ret
    end _setret

    _saveret procedure
      entry
	ldm	%0002(r7),r8,#6
	ret
    end _saveret

    _longret procedure
      entry
	ld	r3,r7
	ld	r2,r6
	ldm	r7,@r3,#9
	ld	@r15,r7
	ret
    end _longret
end setret
