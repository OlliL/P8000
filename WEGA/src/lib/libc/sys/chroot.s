! @[$] chroot.s	2.1  09/12/83 11:40:07 - 87wega3.2 !

chroot module

  external
    cerror	procedure

  global
    _chroot procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#61
	ld	r2,r4
	ret	nc
	jp	cerror
    end _chroot
end chroot
