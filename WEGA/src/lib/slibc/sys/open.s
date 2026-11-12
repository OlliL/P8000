! @[$] open.s	3.1  09/12/83 11:42:38 - 87wega3.2 !

open module

$segmented

  external
    cerror	procedure

  global
    _open procedure
      entry
	ldl	rr0,rr6
	ld	r2,r5
	ld	r3,r4
	sc	#5
	ld	r2,r4
	ret	nc
	jp	cerror
    end _open
end open
