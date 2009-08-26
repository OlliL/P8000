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
