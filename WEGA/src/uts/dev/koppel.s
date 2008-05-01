!******************************************************************************
*******************************************************************************

       W E G A - Quelle      (C) ZFT/KEAW Abt. Basissoftware - 1986

       KERN 3.2  Modul:	koppel.s

       Bearbeiter:	U. Wiesner
       Datum:		02/13/87 10:16:01
       Version:		1.1

*******************************************************************************
******************************************************************************!

koppel module
	
$segmented

!------------------------------------------ Kommentar  -----------------------!

! 
  Dieser Modul enthaelt Routinen zur Bedienung der Koppelschnittstelle
  zum 8-Bit-Mikrorechnerteil
  (Bedienung der 4 Terminals des 8-Bit-Teils;
   Dateitransfer von/zu UDOS;
   Blocktransfer zur Floppy des 8-Bit-Teils)
!

!------------------------------------------ Vereinbarungen  ------------------!


!* Hardwareadressen *!

constant
PIO0	:= %FF91	!Koppel-PIO zum 8-Bit-Teil 
			 Kanal A: Datenausgabe
			 Kanal B: Statusausgabe/Steuerport!
PIO0D_A	:= PIO0 + 0
PIO0D_B	:= PIO0 + 2
PIO0C_A	:= PIO0 + 4
PIO0C_B	:= PIO0 + 6

PIO1	:= %FF99	!Koppel-PIO zum 8-Bit-Teil
                         Kanal A: Dateneingabe
			 Kanal B: Statuseingabe/Steuerport!
PIO1D_A	:= PIO1 + 0
PIO1D_B	:= PIO1 + 2
PIO1C_A	:= PIO1 + 4
PIO1C_B	:= PIO1 + 6

RETI_P	:= %FFE1

!* Moegliche Statusausgaben an 8-Bit-Teil von 16-Bit-Teil 
   fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO0_B)
   (INT8-Bit jeweils = 1) *!

constant

ST_INT		:= 1	!Interrupt INT8 ausloesen!
STAT8_ENDE	:= %00	!Ende eines Read-/Write-Requests (INT8-Bit = 0)!
STAT8_READ	:= %02	!Read-Request an einen Peripheriebaustein 8-Bit-Teil!
STAT8_WRITE	:= %04	!Write-Request an einen Peripheriebaustein 8-Bit-Teil!
STAT8_RETI	:= %1C	!Reti-Request!

STAT8_REMOTE	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers!

!* Moegliche Statuseingaben von 8-Bit-Teil an 16-Bit-Teil 
   fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO1_B)
   (INT16-Bit jeweils = 0) *!

constant
STAT16_ENDE	:= %00	!keine weiteren Zeichen vom SIO!
STAT16_TRANS	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers!

!Status %0x	REC INT (SIO-Empfaengerinterrupt)!
!Status %2x	TRANS INT (SIO-Interrupt Sendepuffer leer)!
!Status %4x	EXT STAT INT (SIO-Externer-Statusinterrupt)!
!Status %6x	SPEC REC INT (SIO-Interrupt Empfangssonderfall)!

!Status %x2	SIO 0 Kanal A!
!Status %x4	SIO 0 Kanal B!
!Status %x6	SIO 0 Kanal A!
!Status %x8	SIO 0 Kanal B!

UD :=  %10
WRITE := 2
ACK := 6

constant
sp := r15

internal

atab array [* long] := [_rint, _xint, _eint, _sint]
kopf array [3 word]	!f. Blocktreiber!


global

!------------------------------------------------------------------------------
	kint ()
VI-Routine bei Interrupt der Koppelschnittstelle
------------------------------------------------------------------------------!

_kint procedure
    entry
	ld	_last_int_serv, #13	!KP-INT!
$1:
	inb	rh2, PIO1D_B	!Statuseingabe!
	andb	rh2, #%7f	!Status ungleich 0 ?!
	jr	z, $1

!Status/Datenbyte von Koppelschnittstelle einlesen!
$2:
	inb	rh2, PIO1D_B	!Statuseingabe, da Einschwingen!
	sub	r6, r6
	inb	rl6, PIO1D_A	!Dateneingabe!

!U880-RETI (ED4D) an PIO ausgeben!
	ld	r1, #%ED4d
	outb	RETI_P, rh1
	outb	RETI_P, rl1

!Test Remote-Interrupt!
	andb	rh2, #%7E		!Statusbits sind Bit 1-6!
	cpb	rh2, #STAT8_REMOTE
	jr	nz, $3
	ld	r7, r6
	cp	_ksflag, #UD
	jpr	z, _udint		!Int. fuer /dev/ud!
	jpr	_fdint			!Int. fuer /dev/fd!

$3:
!Interrupts eines SIO-Kanals (SIO0_A, SIO0_B, SIO1_A, SIO1_B)!
!Bestimmung der dev-Nr.!
	ldk	r7, #0
	ldb	rl7, rh2
	rrb	rl7, #1
	dec	r7, #1
	andb	rl7, #3		!r7 = dev !

!Programmverzweigung entsprechend dem eingelesenen Status!
	
	ldk	r4, #0
	ldb	rl4, rh2
	srlb	rl4, #3
	and	r4, #%0c
	ldl	rr4, atab(r4)
	jp	@rr4
end _kint

!---------------------------------------------------------------------------
	_siooutb ((unsigned)addr, (char)val)
Out-Befehl an SIO (sowohl auf 16-Bit-Teil als auch auf 8-Bit-Teil)
Input:	r7 - Portadresse
	rl6 - auszugebendes Byte
----------------------------------------------------------------------------!
_siooutb procedure
entry
	cp	r7, #%ff00
	jr	c, _koutb
	outb	@r7, rl6
	ret
end _siooutb

!------------------------------------------------------------------------------
	koutb ((char)ioaddr, (char)val)
OUT-Befehl an Peripherieschaltkreis auf 8-Bit-Teil (ueber Koppelschnittstelle)
Input:	rl7 - Portadresse
	rl6 - auszugebendes Datenbyte
------------------------------------------------------------------------------!

_koutb procedure
    entry
	ldctl	r2, fcw
	di	vi,nvi
ko1:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, ko1		!ja!

	ld	r0, #STAT8_WRITE lor ST_INT	!rh0: Ende-Status 0!
	
	outb	PIO0D_A, rl7	!Portadresse an 8-Bit-Teil senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Write Request" senden!
ko2:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, ko2		!ja!

	outb	PIO0D_B, rh0	!Status "Ende des Write-Requests" senden!
	outb	PIO0D_A, rl6	!Datenbyte senden!

	ldctl	fcw, r2
	ret
end _koutb

!---------------------------------------------------------------------------
	_siootirb ((unsigned)addr, (char)val)
Otirb-Befehl an SIO (sowohl auf 16-Bit-Teil als auch auf 8-Bit-Teil)
Input:	rl7 - Portadresse
	rr4 - Speicheranfangsadresse fuer auszugebende Datenbytes
	r3  - Anzahl der auszugebenden Datenbytes
----------------------------------------------------------------------------!
_siootirb procedure
entry
	cp	r7, #%ff00
	jr	c, _kotirb
	ldctl	r2, FCW
	di	vi, nvi
	otirb	@r7, @rr4, r3
	ldctl	FCW, r2
	ret
end _siootirb

!------------------------------------------------------------------------------
	kotirb ((char)ioaddr, (long)memaddr, size)
OTIR-Befehl an Peripherieschaltkreis auf 8-Bit-Teil (ueber Koppelschnittstelle)
Input:	rl7 - Portadresse
	rr4 - Speicheranfangsadresse fuer auszugebende Datenbytes
	r3  - Anzahl der auszugebenden Datenbytes
------------------------------------------------------------------------------!

_kotirb procedure
    entry
	ldctl	r2, fcw		!altes FCW in r2!
	di	vi,nvi
koir1:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, koir1	!ja!

	ld	r0, #STAT8_WRITE lor ST_INT
	outb	PIO0D_A, rl7	!Portadresse an 8-Bit-Teil senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Write Request" senden!

	ld	r7, #PIO0D_A
koirloop:
	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, koirloop	!ja!

	cp	r3, #1
	jr	nz, koir2	!zur noch 1 Datenbyte auszugeben!
	ldb	rl0, rh0	!Status "Ende des Write-Requests"!
	outb	PIO0D_B, rl0	!Status senden!
koir2:
	outib 	@r7, @rr4, r3	!Datenbyte senden!
	jr	nov, koirloop	!naechstes Datenbyte!

	ldctl	fcw, r2
	ret

end _kotirb

!------------------------------------------------------------------------------
int	_sioinb ((unsigned)ioaddr)
IN-Befehl an SIO auf 16-Bit-Teil oder 8-Bit-Teil (ueber Koppelschnittstelle)
Input:	r7 - Portadresse
Output:	rl2 - eingelesener Datenwert
------------------------------------------------------------------------------!
_sioinb procedure
entry
	cp	r7, #%ff00
	jr	c, _kinb
	sub	r2, r2
	inb	rl2, @r7
	ret
end _sioinb

!------------------------------------------------------------------------------
int	kinb ((char)ioaddr)
IN-Befehl an Peripherieschaltkreis auf 8-Bit-Teil (ueber Koppelschnittstelle)
Input:	rl7 - Portadresse
Output:	rl2 - eingelesener Datenwert
------------------------------------------------------------------------------!

_kinb procedure
    entry
	ldctl	r3, fcw
	di	vi,nvi
ki1:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, ki1		!ja!

	ld	r0, #STAT8_READ lor ST_INT
	outb	PIO0D_A, rl7	!Portadresse senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Read Request" senden!
ki2:
	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, ki2		!ja!
	bitb	rh1, #5		!DATEN DA?!
	jr	nz, ki2		!ja!

	inb	rh2, PIO1D_B	!Status 0 ?!
	andb	rh2, #%7e
	jr	nz, ki2
	inb	rl2, PIO1D_A	!eingel. Datenbyte von Peripherieschaltkreis!
	ldctl	fcw, r3
	ret
end _kinb

!------------------------------------------------------------------------------
	koutinb ((char)ioaddr, (char)val)
OUT-Befehl an Peripherieschaltkreis auf 8-Bit-Teil (ueber Koppelschnittstelle)
mit anschliessender Eingabe vom gleichen Port
Input:	rl7 - Portadresse
	rl6 - auszugebendes Datenbyte
Output:	rl2 = eingegebenes Datenbyte
	rh2 = 0, falls kein Fehler
------------------------------------------------------------------------------!
_koutinb procedure
    entry
	ldctl	r3, fcw
	di	vi,nvi
koi1:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, koi1		!ja!

	ld	r0, #STAT8_WRITE lor ST_INT	!rh0: Ende-Status 0!
	outb	PIO0D_A, rl7	!Portadresse an 8-Bit-Teil senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Write Request" senden!
koi2:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, koi2	!ja!

	ldb	rl0, #STAT8_READ
	outb	PIO0D_B, rl0	!Status READ anlegen!
	outb	PIO0D_A, rl6	!Datenbyte ausgeben!

koi3:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, koi3	!ja!

	outb	PIO0D_A, rl7	!Portadresse ausgeben!
	jpr	ki2		!weiter in _kinb!
end _koutinb

!------------------------------------------------------------------------------
	routb ((char)val)
REMOTE: Ausgabe eines Datenbytes an 8-Bit-Teil (ueber Koppelschnittstelle)
Input:	rl7 - auszugebendes Datenbyte
------------------------------------------------------------------------------!

_routb procedure
    entry
	ldctl	r2, fcw
	di	vi,nvi	
ro1:	inb	rh1, PIO0D_B
	bitb	rh1, #6		!ENABLE OUTPUT?!
	jr	nz, ro1		!ja!

	ld	r0, #STAT8_REMOTE lor ST_INT	!rh0: Ende-Status 0!
	outb	PIO0D_A, rl7	!Datenbyte an 8-Bit-Teil senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Write Request" senden!
	ldctl	fcw, r2
	ret
end _routb

!------------------------------------------------------------------------------
	_fd_disk:
Treiber zur Arbeit mit einer blockorientierten Diskette auf 8-Bit-Teil -
Lesen/Schreiben eines Blockes

Input:	r7  - Requestcode (1=Read / 2=Write)
	r6  - Bit 0-3: Drivenummer (0-3)
	      Bit 4-7: Floppytyp (0-F)
	rr4 - Blocknummer (r4=0)

	_fd_addr - Pufferadresse
	_fd_len  - Laenge in Byte

------------------------------------------------------------------------------!

_fd_disk procedure
      entry
	ld	r2, r6
	cpb	rl7, #WRITE	!Startmeldung an 8-Bit-Teil senden!
	lda	rr6, kopf
	ldb	rh2, #'W'
	jr	z, $1
	ldb	rh2, #'R'
$1:
	!Ausgabe Floppytyp/Drive-Nr. an 8-Bit-Teil!
	ld	@rr6, r2
	!Ausgabe Blocknummer an 8-Bit-Teil!
	ld	kopf+2, r5
	!Ausgabe der Datenlaenge in Byte!
	ld	r0, _fd_len
	ld	kopf+4, r0

	ld	r5, #sizeof kopf
	ld	r0, #STAT8_REMOTE lor ST_INT
	ld	r1, #PIO0D_A	!Portadresse!
roir1:	inb	rh3, PIO0D_B
	bitb	rh3, #6		!ENABLE OUTPUT?!
	jr	nz, roir1	!ja!

	outib 	@r1, @rr6, r5	!1. Datenbyte senden!
	outb	PIO0D_B, rh0	!INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rl0	!INT8-Bit und Status "Write Request" senden!
roirloop:
	inb	rh3, PIO0D_B
	bitb	rh3, #6		!ENABLE OUTPUT?!
	jr	nz, roirloop	!ja!

	outib 	@r1, @rr6, r5	!Datenbyte senden!
	jr	nov, roirloop	!naechstes Datenbyte!

!Auswertung des angegebenen Requests!
	cpb	rh2, #'W'
	!Bei Lesen: Warten auf Bereitmeldung von 8-Bit-Teil ueber Interrupt
	 (erfolgt, wenn Block von Diskette gelesen wurde)!
	ret	nz

!Schreiben eines Blockes auf eine blockorientierte Diskette!
	clrb	rh2
	ld	r5, _fd_len
	ldl	rr6, _fd_addr	!Memory Adresse!
	jr	roirloop
end _fd_disk

!---------------------------------------------------------------------------
 Block einlesen!

_fdrint procedure
entry

	ld	r2, _fd_len
	ldl	rr6, _fd_addr	!Memory Adresse!
	ld	r4, #PIO1D_A	!Portadresse Daten!
rloop:
	inb	rh3, PIO0D_B
	bitb	rh3, #5		!Daten Da?!
	jr	nz, rloop
	inib	@rr6, @r4, r2	!Daten lesen!
	jr	nov, rloop
	ldb	rl3, #ACK
r1loop:
	inb	rh3, PIO0D_B
	bitb	rh3, #6		!OUTPUT ENABLE!
	jr	nz, r1loop
	outb	PIO0D_A, rl3	!ACK senden!
	ret
end _fdrint
end koppel
