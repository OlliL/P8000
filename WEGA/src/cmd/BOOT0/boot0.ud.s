!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Module boot0.ud.s (primary bootstrapper)
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:40
	Version:	1.1
 
*******************************************************************************
 
	boot0 udos fuer einen Systemstart von den P8000-UDOS
	Minidisketten
	(primary bootstrapper)
 
	bei automatischem boot bzw Eingabe von CR bei manuellem
	boot nach der Meldung mit > wird das Programm boot
	(secundary bootstrapper) von der Diskette des UDOS Systems
	uber den Koppel-Kanal in den Speicher des U8000 Systems
	(Segment 0, ab Adresse %8000) geladen.
	Bei manuellem boot kann nach der Meldung mit > auch
	ein anderer filename eingegeben werden.
	boot0 benutzt den U8000 Monitor.
 
*******************************************************************************
******************************************************************************!
 

 
boot0_ud_s module
 
 
$SECTION boot0

 
  constant
 
	MOVE_ADR := %F800
	LOAD_ADR := %8000
 
	TTIN := %04
	TTWR := %06
	UDIN := %20
	UDWR := %22
 
	ESC := %1B
	ETX := %03
	ACK := %06
	CAN := %18
	STX := %02
	EOT := %04
	NAK := %15
 
	typ := %01		!binary file!
 
 
  global
 
!*---------------------------------------------------------------------------*!
!*
 * eigentliches Programm von 8000 auf F800 moven
 *!
!*---------------------------------------------------------------------------*!
 
 
    START procedure
      entry
	ld	r2, #%5800	!nonsegmented Systemmode!
	ldctl	FCW, r2		!fcw setzen!
	ldb	rh2, #(MOVE_ADR shr 8)
	ldar	r1, MAIN	!Quelladr!
	ld	r15, r2
	ld	r14, #%200
	ldirb	@r2, @r1, r14
	jp	@r15
    end START
 
!Dieses Programm ist voll relativ!
!Es wird auf MOVE_ADR gemoved!
 
 
!*---------------------------------------------------------------------------*!
!*
 * Main Procedure
 * Eingabe filename bei manuellem boot
 * (bzw Standardname fuer secundary boot bei automatischem boot)
 * Uebertragung filename an UDOS System, warten auf Bestaetigung
 * dass file vorhanden, Start der blockweisen Fileuebertragung
 * Laden ab Adr 8000 hex, Segment 0
 * Starten des Programms
 *!
!*---------------------------------------------------------------------------*!
 
    MAIN procedure
      entry
	pushl	@r15, rr6	!Monitor Parameter retten!
	pushl	@r15, rr4
	test	r4		!0=automatischer Boot!
	jr	nz, MAIN_LOOP	!manueller Boot r14 ist 0!
 
	ldar	r14, MSG_BOOT	!autom.:boot Kommando laden!
MAIN_LOOP:
	ldar	r8, MSG_PROMPT
	calr	PRINT_MSG
	ldar	r10, CON_BUFFER
	ld	r11, r10	!r11 Anf adr Puffer!
	inc	r11, #%0E	!Ende Puffer!
MAIN3:
	ld	r12, r10	!r12 aktueller Puffer Pointer!
MAIN4:
	calr	GETA
	cpb	rl0, #%0A	!lf = Ende der Eingabe!
	jr	z, MAIN5
	cpb	rl0, #'/'	!Eingabe loeschen!
	jr	z, MAIN3	!Eingabe wiederholen!
 
	ldb	@r12, rl0	!Zeichen in Puffer!
	inc	r12, #%01	!Pointer inc!
	cp	r12, r11
	jr	ge, MAIN3	!Eingabe zu lang neu anfangen!
	jr	MAIN4		!naechstes Zeichen!
 
MAIN5:				!Ende der Eingabe!
	clrb	@r12		!letztes Byte im Puffer = 0!
 
!boot file open!
 
	ld	r2, r10
	calr	udopen
	test	r1
	jr	nz, MAIN9
 
!boot file lesen!
 
	lda	r2, LOAD_ADR
	ld	r9, r2
	calr	udread		!1. Block der file lesen!
	test	r1
	jr	nz, MAIN9	!Fehler!
 
	cp	@r9, #%E707	!Magic Nr!
				!=nonsegmented, executable!
	jr	nz, MAIN7	!Fehler falsche Magic!
	ld	r11, %10(r9)	!Entry Point des Programms!
	ld	r0, #%18	!size header!
	add	r0, %0A(r9)	!size segment table!
	ld	r10, r9		!LOAD_ADR!
	add	r9, r0		!Anfangsadr Programmcode!
	neg	r0
	add	r0, r6		!%200-header-segmtable!
	ldirb	@r10, @r9, r0	!Move Code auf LOAD_ADR!
 
	ld	r2, r10
	calr	udread		!alle weiteren Bloecke lesen!
	test	r1
	jr	nz, MAIN9	!Fehler!
 
				!Ok!
	ld	r15, #MOVE_ADR - 8	!Stackpointer fuer Parameter neu setzen!
	popl	rr4, @r15	!Rueckladen der Monitor Parameter!
	popl	rr6, @r15
	jp	@r11		!Sprung zum geladenen Programm!
 
MAIN7:
	calr	uderror
	ldar	r8, MSG_ERROR	!Fehler, neue Kommandoeingabe!
MAIN8:
	calr	PRINT_MSG
	ldk	r14, #0		!nanuel boot setzen!
	jr	MAIN_LOOP
 
MAIN9:
	ldar	r8, MSG_NO_LOAD
	jr	MAIN8		!Fehler!
    end MAIN

  internal
 
!*---------------------------------------------------------------------------*!
!*
 * Ein- Ausgaben an UDOS System
 *!
!*---------------------------------------------------------------------------*!
 
    PRINT_MSG procedure
      entry
	ldb	rl0, @r8
	testb	rl0
	ret	z		!Ausgabe message bis 00!
	calr	PRINT_CHAR
	inc	r8, #%01
	jr	PRINT_MSG
    end PRINT_MSG

    SET_BOOT procedure
	entry
	ldar	r14, MSG_BOOT
    end SET_BOOT		!laeuft in GETA weiter!

    GETA procedure
      entry
	test	r14
	jr	z, GETA1
	ldb	rl0, @r14	!Zeichen nicht ueber CON!
				!sondern aus Message holen!
	inc	r14, #%01
	jr	GETA2
 
!Zeichen von CON holen!
 
GETA1:
	push	@r15, r3
	sc	#TTIN
	ld	r0, r3
	pop	r3, @r15
GETA2:
	resb	rl0, #%07	!reset bit 7!
	cpb	rl0, #%0D
	jr	nz, PRINT_CHAR	!Zeichen ausgeben!
	ldk	r0, #%0A	!cr in lf!
    end GETA			!laueft in PRINT_CHAR weiter!

    PRINT_CHAR procedure
      entry
	cpb	rl0, #%0A	!Zeichen = LF ?!
	jr	nz, PUTA	!nein noudal Ausgeben!
	cp	r12, r10	!r12=r10=kein Zeichen eingegeben!
	jr	z, SET_BOOT	!Boot Kommando laden!
	ldk	r14, #%00	!Kommando ueber CON eingeben setzen!
	ldk	r0, #%0D	!cr lf ausgeben!
	calr	PUTA
	ldk	r0, #%0A
    end PRINT_CHAR

    PUTA procedure
      entry
	push	@r15, r3
	sc	#TTWR
	pop	r3, @r15
	ret	
    end PUTA
 
!*---------------------------------------------------------------------------*!
!*
 * Fileuebertragung mit UDOS System
 *!
!*---------------------------------------------------------------------------*!
 
!input r2 Pointer zu filename string!
 
    udopen procedure
      entry
	ldk	r5, #0
	ldr	udflag, r5	!init udflag!
	ldb	rl0, #'S'	!Kommando Send!
	sc	#UDWR
	sc	#UDIN		!acknowledge von udos!
 
	ldb	rl0, #typ	!file typ an udos!
	sc	#UDWR
	sc	#UDIN		!acknowledge von udos!
 
				!Ausgabe filename an udos!
	ldb	rh7, #10	!retries!
	ld	r5, r2
udo0:	ldb	rl7, #0		!Checksumme init!
	ld	r2, r5
	ldb	rl0, #STX	!Start of Text!
	sc	#UDWR
 
udo1:	ldb	rl0 ,@r2	!Zeichen von filename!
	inc	r2, #1
	cpb	rl0, #0		!ende filename ?!
	jr	z, udo2
	xorb	rl7, rl0	!Checksumme berechnen!
	sc	#UDWR
	jr	udo1
 
udo2:	ldb	rl0, #ETX	!end of text!
	sc	#UDWR
	exb	rl0, rl7
	sc	#UDWR		!Checksumme ausgeben!
	sc	#UDIN		!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	ldk	r1, #0		!kein Fehler!
	ret	z		!filename richtig uebertragen, file vorhanden!
	cpb	rl3, #CAN	!Abbrechen ?!
	jr	z, uderror
	dbjnz	rh7, udo0	!naechster Versuch!
    end udopen
 
    uderror procedure
      entry
	ldb	rl0, #CAN	!Abbruch zurueck senden!
	sc	#UDWR
	ld	r1, #-1		!Error setzen!
    end uderror
 
    udclose procedure
      entry
	sc	#UDIN
	ret
    end udclose
 
    udread procedure
      entry
	ldb	rh7, #10	!retries!
	ldk	r1, #0		!kein Fehler!
	ldr	r0, udflag
	test	r0
	jr	z, udr0		!Sprung 1.Aufruf!
	ldb	rl0, #ACK	!Ack fuer letzen Aufruf!
	sc	#UDWR
 
udr0:	ldb	rl7, #0		!Checksumme init!
	ld	r5, r2		!Buffer Pointer retten!
	sc	#UDIN
	resb	rl3, #7
	cpb	rl3, #EOT
	jr	z, udclose	!file fertig uebertragen!
 
	cpb	rl3, #CAN
	jr	z, uderror	!udos gibt Abbruch!
	cpb	rl3, #ESC
	jr	z, uderror
 
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, udr2	!warten auf ETX und Uebertragung wiederholen!
 
	sc	#UDIN
	ldb	rh4, rl3	!high count!
	sc	#UDIN
	ldb	rl4, rl3	!low count!
	ld	r6, r4
udr1:	sc	#UDIN		!Daten lesen!
	ldb	@r2, rl3	!in Buffer!
	inc	r2, #1
	xorb	rl7, rl3	!Checksumme berechnen!
	djnz	r4, udr1
 
udr2:	sc	#UDIN		!warten auf ETX!
	resb	rl3, #7
	cpb	rl3, #ESC
	jr	z, uderror
	cpb	rl3, #CAN
	jr	z, uderror
	cpb	rl3, #ETX
	jr	nz, udr2
 
	sc	#UDIN		!Checksumme!
	cpb	rl3, rl7	!uebertragene = berechnete ?!
	jr	nz, udr3
 
	ldr	r0, udflag
	test	r0
	ldr	udflag, r7	!r7 ist ungleich 0!
	jr	nz, udread	!naechsten Block!
	ret			!nur nach 1. Block!
 
udr3:	decb	rh7, #1
	jr	z, uderror	!Abbruch!
 
	ldb	rl0, NAK	!n mal wiederholen!
	ld	r2, r5		!Buffer Pointer auf Anfang zurueck!
	sc	#UDWR
	jr	udr0
    end udread
 
 
 
	MSG_PROMPT array [* byte] := '> %00'
	MSG_BOOT array [* byte] := 'boot%l'
	MSG_ERROR array [* byte] := 'invalid file%l%00'
	MSG_NO_LOAD array [* byte] := 'cannot load boot file%l%00'
 
	udflag word
	CON_BUFFER array [%0F byte]
 
end boot0_ud_s
