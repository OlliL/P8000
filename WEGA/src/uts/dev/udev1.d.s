!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev1.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev1 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev1 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev1.o udev1.d.s
	ar r LIB2 udev1.o
	rm udev1.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev1.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev1_s module

$segmented

  global
 
    _usr1int procedure
      entry
	jp	_unexint
    end _usr1int

    _usr1ope procedure
      entry
    end _usr1ope

    _usr1clo procedure
      entry
    end _usr1clo

    _usr1str procedure
      entry
    end _usr1str

    _u1open procedure
      entry
    end _u1open

    _u1close procedure
      entry
    end _u1close

    _u1read procedure
      entry
    end _u1read

    _u1write procedure
      entry
    end _u1write

    _u1ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u1ioctl

end udev1_s
