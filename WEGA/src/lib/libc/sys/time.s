time module

  global
    time procedure
      entry
	sc	#13
	ldl	rr2,rr4
	test	r7
	ret	z
	ldl	@r7,rr2
	ret
    end time
end time
