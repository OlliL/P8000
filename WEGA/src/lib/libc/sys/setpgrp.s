setpgrp module

  external
    _cerror

  global
    setpgrp procedure
      entry
	ld	r0,#%0001
	xor	r4,r4
	sc	#39
	ld	r2,r4
	ret	nc
	jp	cerror
    end setpgrp
end setpgrp
