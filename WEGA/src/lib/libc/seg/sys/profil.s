! @[$] profil.s	2.1  09/12/83 11:42:49 - 87wega3.2 !

profil module

$segmented

  global
    _profil procedure
      entry
	ldl	rr0,rr6
	ld	r2,r5
	ld	r7,r3
	ld	r3,r4
	ld	r4,r7
	sc	#44
	xor	r2,r2
	ret
    end _profil
end profil
