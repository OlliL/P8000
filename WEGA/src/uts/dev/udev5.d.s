!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev5.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev5 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev5 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev5.o udev5.d.s
	ar r LIB2 udev5.o
	rm udev5.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev5.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev5_s module

$segmented

  global
 
    _usr5int procedure
      entry
	jp	_unexint
    end _usr5int

    _usr5ope procedure
      entry
    end _usr5ope

    _usr5clo procedure
      entry
    end _usr5clo

    _usr5str procedure
      entry
    end _usr5str

    _u5open procedure
      entry
    end _u5open

    _u5close procedure
      entry
    end _u5close

    _u5read procedure
      entry
    end _u5read

    _u5write procedure
      entry
    end _u5write

    _u5ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u5ioctl

end udev5_s
