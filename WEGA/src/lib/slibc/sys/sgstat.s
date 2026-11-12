! @[$] sgstat.s	2.2  08/17/83 15:23:09 - 87wega3.2 !

sgstat module

$segmented

  external
    cerror	procedure

  global
    _sgstat procedure
      entry
	ld	r0,r6
	ld	r1,r7
	ld	r4,#%0000
	sc	#%38
	ld	r2,r4
	jp	c,cerror
	ret
    end _sgstat
end sgstat
