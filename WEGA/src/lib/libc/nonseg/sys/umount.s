! @[$] umount.s	2.1  09/12/83 11:43:51 - 87wega3.2 !

umount module

  external
    cerror	procedure

  global
    _umount procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#22
	ld	r2,r4
	ret	nc
	jp	cerror
    end _umount
end umount
