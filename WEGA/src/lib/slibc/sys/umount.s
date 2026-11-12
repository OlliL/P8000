! @[$] umount.s	2.1  09/12/83 11:43:51 - 87wega3.2 !

umount module

$segmented

  external
    cerror	procedure

  global
    _umount procedure
      entry
	ld	r2,#%0016
	ldl	rr0,rr6
	xor	r4,r4
	sc	#22
	ld	r2,r4
	ret	nc
	jp	cerror
    end _umount
end umount
