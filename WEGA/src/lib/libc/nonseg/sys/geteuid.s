! @[$] geteuid.s	2.1  09/12/83 11:41:19 - 87wega3.2 !

geteuid module

  global
    _geteuid procedure
      entry
	sc	#24
	ld	r2,r5
	ret
    end _geteuid
end geteuid
