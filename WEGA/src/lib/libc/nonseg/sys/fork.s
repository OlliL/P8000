! @[$] fork.s	2.1  09/12/83 11:41:04 - 87wega3.2 !

fork module

  external
    cerror	procedure

  global
    _par_uid
      word

    _fork procedure
      entry
	sc	#2
	jr	uid
	ld	r2,r4
	ret	nc
	jp	cerror
uid:
	ld	_par_uid,r4
	xor	r2,r2
	ret
    end _fork
end fork
