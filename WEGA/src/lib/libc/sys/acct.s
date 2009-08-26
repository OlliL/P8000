acct module

  external
    cerror	procedure

  global
    _acct procedure
      entry
	ld	r0,r7
	xor	r4,r4
	sc	#51
	ld	r2,r4
	ret	nc
	jp	cerror
    end _acct
end acct
