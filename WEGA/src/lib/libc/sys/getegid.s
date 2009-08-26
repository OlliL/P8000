! @[$] getegid.s	2.1  09/12/83 11:41:15 - 87wega3.2 !

getegid module

  global
    _getegid procedure
      entry
	sc	#47
	ld	r2,r5
	ret
    end _getegid
end getegid
