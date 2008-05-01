!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev3.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev3 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev3 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev3.o udev3.d.s
	ar r LIB2 udev3.o
	rm udev3.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev3.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev3_s module

$segmented

  global
 
    _usr3int procedure
      entry
	jp	_unexint
    end _usr3int

    _usr3ope procedure
      entry
    end _usr3ope

    _usr3clo procedure
      entry
    end _usr3clo

    _usr3str procedure
      entry
    end _usr3str

    _u3open procedure
      entry
    end _u3open

    _u3close procedure
      entry
    end _u3close

    _u3read procedure
      entry
    end _u3read

    _u3write procedure
      entry
    end _u3write

    _u3ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u3ioctl

end udev3_s
