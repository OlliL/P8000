! @[$] open.s	3.1  09/12/83 11:42:38 - 87wega3.2 !

open module

  external
    cerror	procedure

  global
    _open procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#5
	ld	r2,r4
	ret	nc
	jp	cerror
    end _open
end open
