! @[$] chkseg.s	2.1 08/17/83 10:24:33 - 87wega3.2 !

chkseg module

$segmented

  global
    chkseg procedure
      entry
	ldb	rl0,rh0
	ldb	rl1,rl0
	and	r0,#%007C
	jr	z,L1
	and	r1,#%003E
	jr	z,L1
	ret
L1:
	ld	r0,#%FFFF
	ret
    end chkseg
end chkseg
