!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
 
	Standalone	Module ud.s
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:50
	Version:	1.2
 
*******************************************************************************
 
	Device Treiber zum Laden von Wega Procedure Dateien
	von Minidisketten unter P8000 UDOS
 
	Der Modul liegt leider nur als PLZ/ASM Quelle vor.
	Bei Aenderungen der Struktur iob in saio.h und
	der inode in inode.h mussen die Referenzen zu dieser
	Struktur per Hand geaendert werden.
 
*******************************************************************************
******************************************************************************!
 

 
ud_s module

 
  constant
 
	UDIN := %20
	UDWR := %22

 
	ESC := %1B
	ETX := %03
	ACK := %06
	CAN := %18
	STX := %02
	EOT := %04
	NAK := %15
 
	typ := %01	!binary file!
 
!Referenzen auf iob Struktur!
 
  constant
 
	iob_i_ino_i_number := 6	!in ud Pointer auf Filename!
	iob_i_unit	:= 80
	iob_i_ma	:= 98
	iob_i_cc	:= 100
 
 
  internal
 
	udflag1 word
	udflag2 word
 
	MSG_OERR array [* byte] := 'ud: cannot open file%l%00'
	MSG_RERR array [* byte] := 'ud: read error%l%00'
	MSG_WERR array [* byte] := 'ud: cannot write%l%00'
 
 
 
!*------------------------------------------ Proceduren ---------------------*!
 
 
  global
 
 
!*----------------------------------------------------------------------------*
 *
 *udopen
 * Open Request
 *
 * Nur Open fuer Read moeglich
 * udopen sendet an das UDOS System den Filenamen, das Local
 * System beginnt mit dem Senden des 1. Datenblocks, falls die
 * Datei vorhanden ist.
 * input r7: Pointer auf Struktur iob
 *
 *---------------------------------------------------------------------------*!
 
    _udopen procedure
      entry
	pushl @r15, rr8
 
	ldk r8, #0
	ld udflag1, r8	!init udflag!
	ld udflag2, r8
 
	ld iob_i_unit(r7), #'r'!udos open drive nr setzen!
 
	bitb @r7, #1	!bit 1 = 1 = open for write!
	jp nz, udwerror	!write nicht moeglich!
 
	ldb rl0, #'S'	!Kommando Send file an udos!
	sc #UDWR
	sc #UDIN	!acknowledge von udos!
 
	ldb rl0, #typ	!file typ an udos!
	sc #UDWR
	sc #UDIN	!acknowledge von udos!
 
			!Ausgabe filename an udos!
	ld r2, iob_i_ino_i_number(r7)	!file name Pointer!
	ldb rh7, #10	!retries!
	ld r5, r2
udo0:	ldb rl7, #0	!Checksumme init!
	ld r2, r5
	ldb rl0, #STX	!Start of Text!
	sc #UDWR
 
udo1:	ldb rl0 ,@r2	!Zeichen von filename!
	inc r2, #1
	cpb rl0, #0	!ende filename ?!
	jr z, udo2
	xorb rl7, rl0	!Checksumme berechnen!
	sc #UDWR
	jr udo1
 
udo2:	ldb rl0, #ETX	!end of text!
	sc #UDWR
	exb rl0, rl7
	sc #UDWR	!Checksumme ausgeben!
	sc #UDIN	!Warten auf Quittung!
	resb rl3, #7
	cpb rl3, #ACK
	ldk r2, #0	!kein Fehler!
	jp z, udclose4
 
	cpb rl3, #CAN	!Abbrechen ?!
	jr z,udoerr
	dbjnz rh7, udo0	!naechster Versuch!
 
udoerr:
	lda r7, MSG_OERR
	jr uderror
    end _udopen
 
!*----------------------------------------------------------------------------*
 *udclose
 * Close Request
 * 
 * ud beendet die Fileuebertragung mit dem UDOS System
 *
 *---------------------------------------------------------------------------*!
 
    _udclose procedure
	entry
	pushl @r15, rr8
	test udflag2
	jr nz, udclose3	!file war schon zu Ende!
	ldb rl0, #CAN	!file transfer abbrechen!
	sc #UDWR
	jr udclose1
    end _udclose
 
!*----------------------------------------------------------------------------*
 *
 *udstrategy
 * Block Read Request (Write nicht moeglich)
 *
 * Einlesen eines Blocks der eroeffneten Datei vom UDOS System
 * input r7: Pointer auf iob Struktur
 *
 *---------------------------------------------------------------------------*!
 
    _udstrat procedure
      entry
	pushl @r15, rr8
	cp r6, #1	!Read!
	jr nz,udwerror	!write nicht moeglich!
 
udst1:	test udflag2
	jr z,udst2
 
	ldk r2, #0		!file schon close!
	jr udclose3
 
udst2:	ld r8, iob_i_ma(r7)	!Lade Adr!
	ld r9, iob_i_cc(r7)	!Laenge!
	test r9
	jr z, udst3	!0 nicht zulaessig!
 
udstloop:
	push @r15, r7
	calr udread	!64 byte von udos!
	pop r7, @r15
	test r2
	jr z,udst4
 
udst3:	lda r7, MSG_RERR
	jr uderror	!Read Error!
 
udst4:	ld r2, iob_i_cc(r7)	!Laenge!
	cpb rl3, #EOT
	jr nz,udst5
 
	sub r9, r6	!file uebertragen!
	jr c, udclose1
	sub r2, r9	!uebertragene Anzahl Byte!
	jr udclose1
 
udst5:	sub r9, r6
	jr z, udclose3	!1 Block fertig uebertragen!
	jr c, udclose3
	jr udstloop
    end _udstrat
 
!*----------------------------------------------------------------------------*
 * Unterprogramme
 *---------------------------------------------------------------------------*!
 
  internal
 
    udread procedure	!Lesen 1 Block von udos System (64 Byte)!
      entry
	ldb rh7, #10	!retries!
	ldk r2, #0	!kein Fehler!
	ldk r6, #0	!init lenght!
	test udflag1
	jr z, udr0	!Sprung 1.Aufruf!
	ldb rl0, #ACK	!Ack fuer letzen Aufruf!
	sc #UDWR
 
udr0:	ldb rl7, #0	!Checksumme init!
	ld udflag1, #-1
	ld r5, r8	!Buffer Pointer retten!
 
	sc #UDIN
	resb rl3, #7
	cpb rl3, #EOT
	ret z		!file fertig uebertragen!
 
	cpb rl3, #CAN
	jr z, udrerr	!udos gibt Abbruch!
	cpb rl3, #ESC
	jr z, udrerr	!udos gibt Abbruch!
 
	cpb rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr nz, udr2	!warten auf ETX und Uebertragung wiederholen!
 
	sc #UDIN
	ldb rh4, rl3	!high count!
	sc #UDIN
	ldb rl4, rl3	!low count!
	ld r6, r4
udr1:	sc #UDIN	!Daten lesen!
	ldb @r8, rl3	!in Buffer!
	inc r8, #1
	xorb rl7, rl3	!Checksumme berechnen!
	djnz r4, udr1
 
udr2:	sc #UDIN	!warten auf ETX!
	resb rl3, #7
	cpb rl3, #ESC
	jr z, udrerr
	cpb rl3, #CAN
	jr z, udrerr
	cpb rl3, #ETX
	jr nz, udr2
 
	sc #UDIN	!Checksumme!
	cpb rl3, rl7	!uebertragene = berechnete ?!
	ldb rl3, #0	!falls zufaellig EOT!
	ret z		!ja!
 
	decb rl7, #1
	jr z, udrerr	!Abbruch!
 
	ldb rl0, NAK	!n mal wiederholen!
	ld r8, r5	!Buffer Pointer auf Anfang zurueck!
	sc #UDWR
	jr udr0
 
udrerr:	ld r2, #-1
	ret
    end udread
 
 
    uderror procedure
      entry
	ldb rl0, #CAN	!udos abbrechen!
	sc #UDWR
	sc #UDIN
	jr uderror1
    end uderror
 
    udwerror procedure
      entry
	lda r7, MSG_WERR
    end udwerror
 
    uderror1 procedure
      entry
	call _printf
	ld r2, #-1
	jr udclose2
    end uderror1
 
    udclose1 procedure
      entry
	sc #UDIN
    end udclose1
 
    udclose2 procedure
      entry
	ld udflag2, #-1	!udos close!
    end udclose2
 
    udclose3 procedure
      entry
    end udclose3
 
    udclose4 procedure
      entry
	popl rr8, @r15
	ret
    end udclose4
 
end ud_s
