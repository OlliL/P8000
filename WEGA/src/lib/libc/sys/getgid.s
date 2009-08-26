! @[$] getgid.s	2.1  09/12/83 11:41:23 - 87wega3.2 !

getgid module

  global
    _getgid procedure
      entry
	sc	#47
	ld	r2,r4
	ret
    end _getgid
end getgid
