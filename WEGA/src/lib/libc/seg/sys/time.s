! @[$] time.s	2.1  09/12/83 11:43:35 - 87wega3.2 !

time module

$segmented

  global
    _time procedure
      entry
	sc	#13
	ldl	rr2,rr4
	testl	rr6
	ret	z
	ldl	@rr6,rr2
	ret
    end _time
end time
