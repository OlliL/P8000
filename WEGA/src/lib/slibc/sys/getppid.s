! @[$] getppid.s	1.1  09/12/83 11:41:37 - 87wega3.2 !

getppid module

$segmented

  global
    _getppid procedure
      entry
	sc	#20
	ld	r2,r5
	ret
    end _getppid
end getppid
