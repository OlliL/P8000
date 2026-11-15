! @[$] acct.s	2.1  09/12/83 11:39:38 - 87wega3.2 !

acct module

$segmented

  external
    cerror	procedure

  global
    _acct procedure
      entry
	ld	r2,#%0016
	ldl	rr0,rr6
	xor	r4,r4
	sc	#51
	ld	r2,r4
	ret	nc
	jp	cerror
    end _acct
end acct
