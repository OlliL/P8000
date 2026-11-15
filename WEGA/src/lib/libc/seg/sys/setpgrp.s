! @[$] setpgrp.s	1.1  09/12/83 11:43:08 - 87wega3.2 !

setpgrp module

$segmented

  global
    _setpgrp procedure
      entry
	ld	r0,#%0000
	sc	#39
	ld	r2,r4
	ret
    end _setpgrp
end setpgrp
