getppid module

  global
    getppid procedure
      entry
	xor	r4,r4
	xor	r5,r5
	sc	#20
	ld	r2,r5
	ret
    end getppid
end getppid
