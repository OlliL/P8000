alarm module

  global
    alarm procedure
      entry
	ld	r0,r7
	sc	#27
	ld	r2,r4
	ret
    end alarm
end alarm
