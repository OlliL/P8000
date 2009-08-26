! @[$] getppid.s	1.1  09/12/83 11:41:37 - 87wega3.2 !

getppid module

  global
    _getppid procedure
      entry
	xor	r4,r4
	xor	r5,r5
	sc	#20
	ld	r2,r5
	ret
    end _getppid
end getppid
