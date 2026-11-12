! @[$] getpgrp.s	1.1  09/12/83 11:41:27 - 87wega3.2 !

getpgrp module

$segmented

  global
    _getpgrp procedure
      entry
	ld	r0,#%0000
	sc	#39
	ld	r2,r4
	ret
    end _getpgrp
end getpgrp
