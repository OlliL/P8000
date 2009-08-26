setpgrp module

  external
    cerror	procedure

  global
    _setpgrp procedure
      entry
	ld	r0,#%0001
	xor	r4,r4
	sc	#39
	ld	r2,r4
	ret	nc
	jp	cerror
    end _setpgrp
end setpgrp
