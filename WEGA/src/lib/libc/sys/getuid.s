! @[$] getuid.s	2.1  09/12/83 11:41:41 - 87wega3.2 !

getuid module

  global
    _getuid procedure
      entry
	sc	#24
	ld	r2,r4
	ret
    end _getuid
end getuid
