!******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Module boot0.md.s (primary bootstrapper)
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:20:26
	Version:	1.1
 
*******************************************************************************
 
	boot0 disk fuer einen Systemstart von der Harddisk
	(primary bootstrapper)
 
	bei autonatischem boot bzw Eingabe von CR bei manuellem
	boot nach der Meldung mit > wird das Programm boot
	(secundary bootstrapper) von der Harddisk
	(vom ersten Filesystem)
	in den Speicher des U8000 Systems
	(Segment 0, ab Adresse %8000) geladen.
	Bei manuellem boot kann nach der Meldung mit > auch
	ein anderer filename (pathname) eingegeben werden.
	Es wird das Filesystem md(0,0) - nicht das root File-
	system benutzt.
	boot0 benutzt den U8000 Monitor.
 
*******************************************************************************
******************************************************************************!
 

 
boot0_md_s module

 
$SECTION boot0
 
 
  constant
 
	DISK_BUFFER := %FA00	!array [%200 byte]!
	BLOCK_POINTER := %FC00	!array [%228 byte]!
	PATH_NAME := %FE28	!n * 14 byte fuer pathname!
 
	MOVE_ADR := %F800
	LOAD_ADR := %8000
 
	TTIN := %04
	TTWR := %06
	DISK := %1E
 
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
	ld	r14, #(DISK_BUFFER - MOVE_ADR)
	ldirb	@r2, @r1, r14
	jp	@r15
    end START
 
!Dieses Programm ist voll relativ!
!Es wird auf MOVE_ADR gemoved!
 
 
!*---------------------------------------------------------------------------*!
!*
 * Main Procedure
 * Eingabe filename (pathname) bei manuellem boot
 * bzw Standardname fuer secundary boot bei automatischem boot
 * Suchen des files in der directory (bzw den derectories)
 * Laden ab 8000 hex, Segment 0
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
	calr	PRINT_MSG	!Meldung zur Eingabeanforderung!
	ld	r10, #PATH_NAME
	ld	r11, r10	!r11 Anf adr Puffer!
MAIN3:
	ld	r12, r11	!r12 aktueller Puffer Pointer!
MAIN4:
	calr	GETA
	cpb	rl0, #%0A	!lf = Ende der Eingabe!
	jr	z, MAIN6
	cpb	rl0, #'/'	!Name fuer directory file!
	jr	z, MAIN5
 
	ldb	@r12, rl0	!Zeichen in Puffer!
	inc	r12, #%01	!Pointer inc!
	jr	MAIN4		!naechstes Zeichen!
 
MAIN5:
	cp	r12, r11	!1. eingegebenes Zeichen ?!
	jr	z, MAIN4	!name eingeben!
	calr	CLEAR		!Namensfeld bis Ende mit 00 beschreiben!
	jr	MAIN3		!naechsten Namen eingeben!
 
MAIN6:				!Ende der Eingabe!
	calr	CLEAR		!Rest vom Namensfeld mit 00 fuellen!
	clrb	@r11		!erstes Zeichen vom naechsten Namensfeld = 0!
 
 
!directories nach file durchsuchen!
 
	ldk	r11, #%02	!inumber der root directory!
MAIN7:
	calr	READ_INODE	!inode lesen!
	jr	nz, MAIN16	!Disk Error!
	testb	@r10		!<> 0 directory einlesen!
				!0 = gewuenschten file einlesen!
	jr	z, MAIN11	!Sprung Procedure file einlesen!
MAIN8:
				!directory lesen!
	test	r13		!directory ende ?!
	jr	z, MAIN14	!Ja, file nicht gefunden!
	calr	READ_BLOCK	!naechsten Block der directory einlesen!
	jr	nz, MAIN16	!Disk Error!
MAIN9:
	pop	r11, @r9	!inumber in directory!
	test	r11		!0=keine Datei auf diesem Platz eingetragen!
	jr	z, MAIN10	!naechster Platz in directory!
				!Abstand 10 Hex Bytes 2 byte inumber 14 byte Name!
	ldk	r0, #14		!Laenge filename!
	ld	r1, r9
	ld	r2, r10		!filename in directory Block suchen!
	cpsirb	@r10, @r1, r0, nz
	jr	nz, MAIN7	!Name gefunden!
				!in r11 ist inumber fuer gesuchte file!
 
	ld	r10, r2
MAIN10:
	inc	r9, #14
	cp	r9, #DISK_BUFFER+%200 !Ende Disk Puffer!
	jr	ge, MAIN8	!Name nicht gefunden, naechsten Block lesen!
	jr	MAIN9		!nicht gefunden, weiter im gleichen Block suchen!
 
!Procedure file einlesen!
 
MAIN11:
	test	r13		!file leer ?!
	jr	z, MAIN14
	calr	READ_BLOCK	!1. Block der file lesen!
	jr	nz, MAIN16	!Disk Error!
	cp	@r9, #%E707	!Magic Nr!
				!=nonsegmented, executable!
	jr	nz, MAIN14	!Fehler falsche Magic!
	ld	r11, %10(r9)	!Entry Point des Programms!
	ld	r0, #%18	!size header!
	add	r0, %0A(r9)	!size segment table!
	add	r9, r0		!Anfangsadr Programmcode!
	ld	r10, #LOAD_ADR	!Ladeadr im Segment 0!
	neg	r0
	add	r0, #%200	!%200-header-segtable!
MAIN12:
	ldirb	@r10, @r9, r0	!Move Puffer auf echte Adr!
	test	r13		!noch Datenbloecke einzulesen ?!
	jr	z, MAIN13	!file eingelesen!
	calr	READ_BLOCK	!alle weiteren Bloecke lesen!
	jr	nz, MAIN16	!Disk Error!
	ld	r0, #%0200	!RL!
	jr	MAIN12
 
!Programm starten!
 
MAIN13:
	ld	r15, #MOVE_ADR - 8	!Stackpointer fuer Parameter neu setzen!
	popl	rr4, @r15	!Rueckladen der Monitor Parameter!
	popl	rr6, @r15
	jp	@r11		!Sprung zum geladenen Programm!
 
 
 
!Fehlerbehandlung!
 
MAIN14:
	ldar	r8, MSG_ERROR
MAIN15:
	calr	PRINT_MSG
	ldk	r14, #0		!manuel boot setzen!
	jr	MAIN_LOOP
 
MAIN16:	ldar	r8, MSG_DERROR
	jr	MAIN15
    end MAIN

  internal
 
!*---------------------------------------------------------------------------*!
!*
 * Inode Block fuer vorgegebene inumber lesen
 * richtige inode ermitteln
 *!
!*---------------------------------------------------------------------------*!
 
    READ_INODE procedure
      entry
	inc	r11, #%0F	!aus inumber inode Block berechnen!
	ld	r5, r11
	srl	r5, #%03	!/8 ;8 inodes je Block!
	xor	r4, r4
	calr	READ_DISK	!inode Block in DISK_BUFFER lesen!
	ret	nz		!Disk Error!
	and	r11, #%07
	sll	r11, #%06
	add	r11, #DISK_BUFFER+8
	popl	rr12, @r11	!Laenge file aus inode!
	addl	rr12, #%01FF
	srll	rr12, #9	!in r13 nun Anzahl Bloecke der file!
	ld	r12, #BLOCK_POINTER
	ld	r9, r12		!array der Blockpointer!
	ldk	r1, #10		!10 direkte Pointer!
READ_INODE1:
	clrb	@r9		!direkte Pointer einlesen!
	inc	r9, #%01
	ldk	r0, #%03
	ldirb	@r9, @r11, r0	!move in Pointer array!
	dec	r1, #%01
	jr	nz, READ_INODE1
 
	cp	r13, #%0A	!file < 10 Bloecke ?!
	jr	le, READ_INODE2	!ja=fertig!
 
	xorb	rh4, rh4	!Pointer zu einfach indirekt laden!
	ldb	rl4, @r11
	inc	r11, #%01
	ldb	rh5, @r11
	inc	r11, #%01
	ldb	rl5, @r11
	calr	READ_DISK	!einfach indirekt Pointer Block lesen!
	ret	nz		!Disk Error!
	ld	r1, #DISK_BUFFER
	ld	r0, #%0200	!RL!
	ldirb	@r9, @r1, r0	!in Pointerarray an die 10 direkten Pointer moven!
READ_INODE2:
	setflg	z
	ret	
    end READ_INODE

!*---------------------------------------------------------------------------*!
!*
 * 1 Block von Disk einlesen
 *!
!*---------------------------------------------------------------------------*!
 
    READ_BLOCK procedure
      entry
	popl	rr4, @r12	!naechste Blocknummer!
	calr	READ_DISK
	ret	nz		!Disk Error!
	ld	r9, #DISK_BUFFER
	dec	r13, #%01	!Anzahl Bloecke dec!
	setflg	z
	ret	
    end READ_BLOCK

!*---------------------------------------------------------------------------*!
!*
 * Programme zur Ein- Ausgabe an Console und zur
 * Aufbereitung der Eingaben
 *!
!*---------------------------------------------------------------------------*!
 
    CLEAR procedure
      entry
	inc	r11, #%0E	!Pointer auf nachstes Namensfeld im PATH_NAME Puffer!
CLEAR1:
	cp	r12, r11
	ret	ge
	clrb	@r12	!Rest vom alten Namensfeld mit 00 beschreiben!
	inc	r12, #%01
	jr	CLEAR1
    end CLEAR

    PRINT_MSG procedure
      entry
	ldb	rl0, @r8
	testb	rl0		!Ausgabe bis 00!
	ret	z
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
	sc	#TTIN
	ld	r0, r3
GETA2:
	resb	rl0, #%07	!reset bit 7!
	cpb	rl0, #%0D
	jr	nz, PRINT_CHAR	!Zeichen ausgeben!
	ldk	r0, #%0A	!cr in lf!
    end GETA			!laueft in PRINT_CHAR weiter!

    PRINT_CHAR procedure
      entry
	cpb	rl0, #%0A	!Zeichen = LF ?!
	jr	nz, PUTA	!nein normal Ausgeben!
	cp	r12, r10	!r12=r10=kein Zeichen eingegeben!
	jr	z, SET_BOOT	!Boot Kommando laden!
	ldk	r14, #%00	!Kommando ueber CON eingeben setzen!
	ldk	r0, #%0D	!cr lf ausgeben!
	calr	PUTA
	ldk	r0, #%0A
    end PRINT_CHAR

    PUTA procedure
      entry
	sc	#TTWR
	ret	
    end PUTA

!*---------------------------------------------------------------------------*!
!*
 * Lesen 1 Block von Disk in Puffer
 * Es wird der Disktreiber im Monitor benutzt
 *!
!*---------------------------------------------------------------------------*!
 
    READ_DISK procedure
      entry
	ldk	r8, #10		!retries!
LOOP:
	dec	r15, #6
	ldm	@r15, r8, #3
 
	ldk	r7, #1		!read!
	ldk	r6, #0		!drive 0!
				!in rr4 Blocknummer!
	ldk	r8, #0		!obere 16 bit der pysischen RAM Adr!
	lda	r9, DISK_BUFFER	!Ladeadr untere 16 bit!
	ld	r10, #%200	!Laenge!
	sc	#DISK
	ldm	r8, @r15, #3
	inc	r15, #6
	test	r2		!<> 0 ist Fehler!
	ret	z
 
	djnz	r8, LOOP	!10 Versuche!
	test	r2		!z flag!
	ret	
    end READ_DISK
 
 
 
 
  internal
 
	MSG_PROMPT array [* byte] := '> %00'
	MSG_BOOT array [* byte] := 'boot%l'
	MSG_ERROR array [* byte] := '?%l%00'
	MSG_DERROR array [* byte] := 'DISK ERROR%l%00'
 
end boot0_md_s
