!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev4.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev4 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev4 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev4.o udev4.d.s
	ar r LIB2 udev4.o
	rm udev4.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev4.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev4_s module

$segmented

  global
 
    _usr4int procedure
      entry
	jp	_unexint
    end _usr4int

    _usr4ope procedure
      entry
    end _usr4ope

    _usr4clo procedure
      entry
    end _usr4clo

    _usr4str procedure
      entry
    end _usr4str

    _u4open procedure
      entry
    end _u4open

    _u4close procedure
      entry
    end _u4close

    _u4read procedure
      entry
    end _u4read

    _u4write procedure
      entry
    end _u4write

    _u4ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u4ioctl

end udev4_s
