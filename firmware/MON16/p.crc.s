!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_crc

  Bearbeiter:	J. Zabel
  Datum:	28.10.1987
  Version:	3.0

*******************************************************************************
******************************************************************************!


p_crc module

$SECTION PROM

!******************************************************************************
Speicherzellen fuer die Pruefsummen der 4 EPROM's des U8000-Softwaremonitors
(Modul ist so zu linken, dass er auf Adresse %3FF0 beginnt, d.h.
dass er am Ende des EPROM-Bereiches steht)
******************************************************************************!

  GLOBAL

!Pruefsumme fuer EPROM MON16_1H:
 Anfangsadresse %0000, Laenge %1000, jedes 2. Byte (even)!

MON16_1H	WORD	:=%FFFF		!EPROM-Pruefsumme!
N_MON16_1H	WORD	:=%FFFF		!negierte EPROM-Pruefsumme!

!Pruefsumme fuer EPROM MON16_1L:
 Anfangsadresse %0001, Laenge %1000, jedes 2. Byte (odd)!

MON16_1L	WORD	:=%FFFF		!EPROM-Pruefsumme!
N_MON16_1L	WORD	:=%FFFF		!negierte EPROM-Pruefsumme!

!Pruefsumme fuer EPROM MON16_2H:
 Anfangsadresse %2000, Laenge %0FF8, jedes 2. Byte (even)!
!(in den letzen 8 Byte dieses EPROM's stehen die geradzahligen Werte
 der EPROM-Pruefsummen)!

MON16_2H	WORD	:=%FFFF		!EPROM-Pruefsumme!
N_MON16_2H	WORD	:=%FFFF		!negierte EPROM-Pruefsumme!

!Pruefsumme fuer EPROM MON16_2L:
 Anfangsadresse %2001, Laenge %0FF8, jedes 2. Byte (odd)!
!(in den letzen 8 Byte dieses EPROM's stehen die ungeradzahligen Werte
 der EPROM-Pruefsummen)!

MON16_2L	WORD	:=%FFFF		!EPROM-Pruefsumme!
N_MON16_2L	WORD	:=%FFFF		!negierte EPROM-Pruefsumme!

end p_crc
