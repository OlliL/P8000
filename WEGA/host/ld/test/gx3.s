.seg
.psec
.code
_xf::
 ld r0,#2
 ret
.psec data
.data
_xd::
 .word 1
_abs1::= %12345678
.comm 20, _xb
