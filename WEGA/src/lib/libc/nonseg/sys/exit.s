! @[$] exit.s	2.1  09/12/83 11:40:55 - 87wega3.2 !

exxit module

  global
    __exit procedure
      entry
	ld	r0,r7
	sc	#1
	ld	r2,r4
    end __exit
end exxit
