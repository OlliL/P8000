!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
 
	KERN 3.2	Modul:	rd.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		29.7.88
	Version:	1.1
 
*******************************************************************************
 
	dummy RAM-Disk-Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein rd.c vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o rd.o rd.d.s
	ar r LIB2 rd.o
	rm rd.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
*******************************************************************************
******************************************************************************!
 

 
rd_s module

$segmented

  global
 
    _rdint procedure
      entry
	jp	_unexint
    end _rdint

    _rdopen procedure
      entry
    end _rdopen

    _rdclose procedure
      entry
    end _rdclose

    _rdstrat procedure
      entry
    end _rdstrat

    _rdread procedure
      entry
    end _rdread

    _rdwrite procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _rdwrite

end rd_s
