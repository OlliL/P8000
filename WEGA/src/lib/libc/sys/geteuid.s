geteuid module

  global
    _geteuid procedure
      entry
	sc	#24
	ld	r2,r5
	ret
    end _geteuid
end geteuid
