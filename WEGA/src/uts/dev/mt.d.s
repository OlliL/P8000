!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
 
	KERN 3.2	Modul:	mt.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		29.7.88
	Version:	1.1
 
*******************************************************************************
 
	dummy Magnetband-Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein mt.c vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o mt.o mt.d.s
	ar r LIB2 mt.o
	rm mt.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
*******************************************************************************
******************************************************************************!
 

 
mt_s module

$segmented

  global
 
    _mtint procedure
      entry
	jp	_unexint
    end _mtint

    _mtopen procedure
      entry
    end _mtopen

    _mtclose procedure
      entry
    end _mtclose

    _mtstrat procedure
      entry
    end _mtstrat

    _mtread procedure
      entry
    end _mtread

    _mtwrite procedure
      entry
    end _mtwrite

    _mtioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _mtioctl

end mt_s
