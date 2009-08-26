cerror module

  global
    _errno
	WORD
    _deverr
	WORD
    cerror procedure
      entry
	ld	_errno,r2
	ld	_deverr,r5
	ldl	rr2,#%ffffffff
	ret
    end cerror
end cerror
