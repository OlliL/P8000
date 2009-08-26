! @[$] getpgrp.s	1.1  09/12/83 11:41:27 - 87wega3.2 !

getpgrp module

  external
    cerror	procedure

  global
    _getpgrp procedure
      entry
	xor	r0,r0
	xor	r4,r4
	sc	#39
	ld	r2,r4
	ret	nc
	jp	cerror
    end _getpgrp
end getpgrp
