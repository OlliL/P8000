getuid module

  global
    _getuid procedure
      entry
	sc	#24
	ld	r2,r4
	ret
    end _getuid
end getuid
