! @[$] nice.s	2.1  09/12/83 11:42:35 - 87wega3.2 !

nice module

  external
    cerror	procedure

  global
    _nice procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#34
	ld	r2,r4
	ret	nc
	jp	cerror
    end _nice
end nice
