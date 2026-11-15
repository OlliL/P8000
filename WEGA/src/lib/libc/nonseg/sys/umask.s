! @[$] umask.s	2.1  09/12/83 11:43:47 - 87wega3.2 !

umask module

  global
    _umask procedure
      entry
	ld	r0,r7
	sc	#60
	ld	r2,r4
	ret
    end _umask
end umask
