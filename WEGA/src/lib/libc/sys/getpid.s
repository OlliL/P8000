! @[$] getpid.s	2.1  09/12/83 11:41:32 - 87wega3.2 !

getpid module

  global
    _getpid procedure
      entry
	sc	#20
	ld	r2,r4
	ret
    end _getpid
end getpid
