.seg
.psec
.code
_xf::
 ld r0,#1
 ret
_xf2::
 calr _xf
 ret
.psec data
.data
_xd::
 .word 7,8,9
.comm 16, _xb
.comm 4, _xc
