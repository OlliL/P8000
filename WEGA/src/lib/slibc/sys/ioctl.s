! @[$] ioctl.s	2.1  09/12/83 11:41:45 - 87wega3.2 !

ioctl module

$segmented

  external
    cerror	procedure

  global
    _ioctl procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ldl	rr2,rr4
	ldl	rr4,#%00000000
	sc	#54
	ld	r2,r4
	ret	nc
	jp	cerror
    end _ioctl
end ioctl
