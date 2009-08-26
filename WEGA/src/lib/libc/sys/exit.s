exit module

  external
    _cerror

  global
    exit procedure
      entry
	ld	r0,r7
	sc	#1
	ld	r2,r4
    end exit
end exit
