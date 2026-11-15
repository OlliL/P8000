! @[$] sync.s	2.1  09/12/83 11:43:31 - 87wega3.2 !

sync module

  global
    _sync procedure
      entry
	sc	#36
	ret
    end _sync
end sync
