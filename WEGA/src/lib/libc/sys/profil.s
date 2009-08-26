profil module

  global
    _profil procedure
      entry
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	ld	r3,r4
	sc	#44
	xor	r2,r2
	ret
    end _profil
end profil
