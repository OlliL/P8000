.seg
.extern _xf, _xd, _xb, _xc
.psec
.code
_x1::
 calr _xf
 ldl rr2,_xd
 ld r1,_xb
 ldr r2,_xd
 ldl rr4,_xc
 call _xf
 jp _xf
 ret
.psec data
.data
_x1d:: .word _xd
 .long _xd
 .long _xf
 .long _xb
 .word _xc
