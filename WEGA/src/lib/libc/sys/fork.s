fork module

  internal
    _par_uid
      word    :=2

  external
    _cerror

  global
    fork procedure
      entry
	sc	#2
	jr	uid
	ld	r2,r4
	ret	nc
	jp	cerror
uid:
	ld	_par_uid,r4
	xorg	r2,r2
	ret
    end fork
end fork
