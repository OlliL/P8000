!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	udev2.d.s
 
	Bearbeiter:	P. Hoge
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
 
	dummy udev2 - device Treiber
 
	zur Erfuellung der Referenzen im Kern, wenn kein udev2 vorhanden.
	Der dummy Treiber wurde extra in Assembler geschrieben, um
	kuerzesten Code zu sichern.
 
	Einbinden in LIB2:
	as -u -o udev2.o udev2.d.s
	ar r LIB2 udev2.o
	rm udev2.o
	(cp LIB2 /usr/sys/dev/LIB2)
 
	(Bitte nicht udev2.c fuer LIB2 verwenden)
 
*******************************************************************************
******************************************************************************!
 

 
udev2_s module

$segmented

  global
 
    _usr2int procedure
      entry
	jp	_unexint
    end _usr2int

    _usr2ope procedure
      entry
    end _usr2ope

    _usr2clo procedure
      entry
    end _usr2clo

    _usr2str procedure
      entry
    end _usr2str

    _u2open procedure
      entry
    end _u2open

    _u2close procedure
      entry
    end _u2close

    _u2read procedure
      entry
    end _u2read

    _u2write procedure
      entry
    end _u2write

    _u2ioctl procedure
      entry
	ldb	_u+%15, #19	!u.u_error = ENODEV!
	ret	
    end _u2ioctl

end udev2_s
