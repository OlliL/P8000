!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Module rm.s
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:45
	Version:	1.2
 
*******************************************************************************
 
	Device Treiber zum Laden von Wega Procedure Dateien
	von einem Local System wie GDS 6000 unter UDOS ueber
	den Console TTY Kanal.
 
	Der Modul liegt leider nur als PLZ/ASM Quelle vor.
	Bei Aenderungen der Struktur iob in saio.h und
	der inode in inode.h mussen die Referenzen zu dieser
	Struktur per Hand geaendert werden.
 
*******************************************************************************
******************************************************************************!
 

 
rm_s module

 
  constant
 
	TTIN := %04
	TTWR := %06
 
	FLAG0 := %3F0		!relative Adresse zu Beginn Monitor RAM!
 
	SOH := %01
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
 
	iob_i_ino_i_number := 6	!in rm Pointer auf Filename!
	iob_i_unit	:= 80
	iob_i_ma	:= 98
	iob_i_cc	:= 100
 
 
  internal
 
	rmflag1 word
	rmflag2 word
 
	MSG_OERR array [* byte] := 'rm: cannot open file on local system%l%00'
	MSG_RERR array [* byte] := 'rm: read error from local system%l%00'
	MSG_WERR array [* byte] := 'rm: cannot write to local system%l%00'
 
 
 
!*------------------------------------------ Proceduren ---------------------*!
 
 
  global
 
 
!*----------------------------------------------------------------------------*
 *
 *rmopen
 * Open Request
 *
 * Nur Open fuer Read moeglich
 * rmopen sendet an das Local System den Filenamen, das Local
 * System beginnt mit dem Senden des 1. Datenblocks, falls die
 * Datei vorhanden ist.
 * input r7: Pointer auf Struktur iob
 *
 *---------------------------------------------------------------------------*!
 
    _rmopen procedure
      entry
	pushl @r15, rr8
	ldctl r8, PSAP
	sub r8, #(%400-FLAG0)
	setb @r8, #0	!control S und esc abschalten!
 
	ldk r8, #0
	ld rmflag1, r8	!init rmflag!
	ld rmflag2, r8
 
	ld iob_i_unit(r7), #'r'!remote open drive nr setzen!
 
	bitb @r7, #1	!bit 1 = 1 = open for write!
	jp nz, rmwerror	!write nicht moeglich!
 
	ldk r0, #SOH	!Kommando Send file an local!
	sc #TTWR
	ldb rl0, #ESC
	sc #TTWR
	ldb rl0, #'S'
	sc #TTWR
	sc #TTIN	!acknowledge von local!
 
	ldb rl0, #typ	!file typ an local!
	sc #TTWR
	sc #TTIN	!acknowledge von local!
 
			!Ausgabe filename an local!
	ld r2, iob_i_ino_i_number(r7)	!file name Pointer!
	ldb rh7, #10	!retries!
	ld r5, r2
rmo0:	ldb rl7, #0	!Checksumme init!
	ld r2, r5
	ldb rl0, #STX	!Start of Text!
	sc #TTWR
 
rmo1:	ldb rl0 ,@r2	!Zeichen von filename!
	inc r2, #1
	cpb rl0, #0	!ende filename ?!
	jr z, rmo2
	xorb rl7, rl0	!Checksumme berechnen!
	sc #TTWR
	jr rmo1
 
rmo2:	ldb rl0, #ETX	!end of text!
	sc #TTWR
	exb rl0, rl7
	sc #TTWR	!Checksumme ausgeben!
	sc #TTIN	!Warten auf Quittung!
	resb rl3, #7
	cpb rl3, #ACK
	ldk r2, #0	!kein Fehler!
	jp z, rmclose4
 
	cpb rl3, #CAN	!Abbrechen ?!
	jr z,rmoerr
	dbjnz rh7, rmo0	!naechster Versuch!
 
rmoerr:
	lda r7, MSG_OERR
	jr rmerror
    end _rmopen
 
!*----------------------------------------------------------------------------*
 *rmclose
 * Close Request
 * 
 * rm beendet die Fileuebertragung mit dem Local System
 *
 *---------------------------------------------------------------------------*!
 
    _rmclose procedure
	entry
	pushl @r15, rr8
	ldctl r8, PSAP
	sub r8, #(%400-FLAG0)
	setb @r8, #0	!control S und esc abschalten!
 
	test rmflag2
	jr nz, rmclose3	!file war schon zu Ende!
	ldb rl0, #CAN	!file transfer abbrechen!
	sc #TTWR
	jr rmclose1
    end _rmclose
 
!*----------------------------------------------------------------------------*
 *
 *rmstrategy
 * Block Read Request (Write nicht moeglich)
 *
 * Einlesen eines Blocks der eroeffneten Datei vom Local System
 * input r7: Pointer auf iob Struktur
 *
 *---------------------------------------------------------------------------*!
 
    _rmstrat procedure
      entry
	pushl @r15, rr8
	ldctl r8, PSAP
	sub r8, #(%400-FLAG0)
	setb @r8, #0	!control S und esc abschalten!
 
	cp r6, #1	!Read!
	jr nz,rmwerror	!write nicht moeglich!
 
rmst1:	test rmflag2
	jr z,rmst2
 
	ldk r2, #0		!file schon close!
	jr rmclose3
 
rmst2:	ld r8, iob_i_ma(r7)	!Lade Adr!
	ld r9, iob_i_cc(r7)	!Laenge!
	test r9
	jr z, rmst3	!0 nicht zulaessig!
 
rmstloop:
	push @r15, r7
	calr rmread	!64 byte von local!
	pop r7, @r15
	test r2
	jr z,rmst4
 
rmst3:	lda r7, MSG_RERR
	jr rmerror	!Read Error!
 
rmst4:	ld r2, iob_i_cc(r7)	!Laenge!
	cpb rl3, #EOT
	jr nz,rmst5
 
	sub r9, r6	!file uebertragen!
	jr c, rmclose1
	sub r2, r9	!uebertragene Anzahl Byte!
	jr rmclose1
 
rmst5:	sub r9, r6
	jr z, rmclose3	!1 Block fertig uebertragen!
	jr c, rmclose3
	jr rmstloop
    end _rmstrat
 
!*----------------------------------------------------------------------------*
 * Unterprogramme
 *---------------------------------------------------------------------------*!
 
  internal
 
    rmread procedure	!Lesen 1 Block von local System (64 Byte)!
      entry
	ldb rh7, #10	!retries!
	ldk r2, #0	!kein Fehler!
	ldk r6, #0	!init lenght!
	test rmflag1
	jr z, rmr0	!Sprung 1.Aufruf!
	ldb rl0, #ACK	!Ack fuer letzen Aufruf!
	sc #TTWR
 
rmr0:	ldb rl7, #0	!Checksumme init!
	ld rmflag1, #-1
	ld r5, r8	!Buffer Pointer retten!
 
	sc #TTIN
	resb rl3, #7
	cpb rl3, #EOT
	ret z		!file fertig uebertragen!
 
	cpb rl3, #CAN
	jr z, rmrerr	!local gibt Abbruch!
	cpb rl3, #ESC
	jr z, rmrerr	!local gibt Abbruch!
 
	cpb rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr nz, rmr2	!warten auf ETX und Uebertragung wiederholen!
 
	sc #TTIN
	ldb rh4, rl3	!high count!
	sc #TTIN
	ldb rl4, rl3	!low count!
	ld r6, r4
rmr1:	sc #TTIN	!Daten lesen!
	ldb @r8, rl3	!in Buffer!
	inc r8, #1
	xorb rl7, rl3	!Checksumme berechnen!
	djnz r4, rmr1
 
rmr2:	sc #TTIN	!warten auf ETX!
	resb rl3, #7
	cpb rl3, #ESC
	jr z, rmrerr
	cpb rl3, #CAN
	jr z, rmrerr
	cpb rl3, #ETX
	jr nz, rmr2
 
	sc #TTIN	!Checksumme!
	cpb rl3, rl7	!uebertragene = berechnete ?!
	ldb rl3, #0	!falls zufaellig EOT!
	ret z		!ja!
 
	decb rl7, #1
	jr z, rmrerr	!Abbruch!
 
	ldb rl0, NAK	!n mal wiederholen!
	ld r8, r5	!Buffer Pointer auf Anfang zurueck!
	sc #TTWR
	jr rmr0
 
rmrerr:	ld r2, #-1
	ret
    end rmread
 
 
    rmerror procedure
      entry
	ldb rl0, #CAN	!remote abbrechen!
	sc #TTWR
	sc #TTIN
	jr rmerror1
    end rmerror
 
    rmwerror procedure
      entry
	lda r7, MSG_WERR
    end rmwerror
 
    rmerror1 procedure
      entry
	call _printf
	ld r2, #-1
	jr rmclose2
    end rmerror1
 
    rmclose1 procedure
      entry
	sc #TTIN
    end rmclose1
 
    rmclose2 procedure
      entry
	ld rmflag2, #-1	!remote close!
    end rmclose2
 
    rmclose3 procedure
      entry
	ldctl r8, PSAP
	sub r8, #(%400-FLAG0)
	resb @r8, #0	!FLAG0 im Monitor RAM ruecksetzen!
    end rmclose3
 
    rmclose4 procedure
      entry
	popl rr8, @r15
	ret
    end rmclose4
 
end rm_s
