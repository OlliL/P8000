!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev6.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev6 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev6 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev6.o udev6.d.s
	ar r LIB2 udev6.o
	rm udev6.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev6.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev6_s module

$segmented

  global
 
    _usr6int procedure
      entry
	jp	_unexint
    end _usr6int

    _usr6ope procedure
      entry
    end _usr6ope

    _usr6clo procedure
      entry
    end _usr6clo

    _usr6str procedure
      entry
    end _usr6str

    _u6open procedure
      entry
    end _u6open

    _u6close procedure
      entry
    end _u6close

    _u6read procedure
      entry
    end _u6read

    _u6write procedure
      entry
    end _u6write

    _u6ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u6ioctl

end udev6_s
