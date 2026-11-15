exit_s module

  external
    __cleanup	procedure

  global
    _exit procedure
      entry
	ld	r8,r7
	call	__cleanup
	ld	r0,r8
	sc	#1
    end _exit
end exit_s
