
!******************************************************************************
*******************************************************************************

  P8000/compact-Firmware	       (C) ZFT/KEAW Abt. Basissoftware - 1989
  Hardware-Test fuer
  WDOS-Erweiterungs-Modul

  Modul:	twem
  Bearbeiter:	M. Klatte, V. Gustavs
  Datum:	1/89, 5/89, 6/89
  Version:	1.1

*******************************************************************************
******************************************************************************!


twem	module

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Externe Prozeduren und Datenfelder
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	EXTERNAL
WR_2BS		procedure
WR_2SP		procedure
WR_TESTNR	procedure
MSG_ERROR	procedure


	EXTERNAL
ERRPAR_ID	word
ERR_CNT		word



!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Konstantendeklaration
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

! System Calls !

	CONSTANT
SC_SEGV		:= 1
SC_NSEGV	:= 2

S_BNK		:= %FFC1	! Speicherbankumschaltung !

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Startadressen fuer U80601-Code im Erweiterungs-RAM
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

RAMZIEL3	:= %FF00FFF0	! Startadresse fuer U80601-Code Test3 !
RAMZIEL4	:= %FF00FFD0	! Startadresse fuer U80601-Code Test4 !
RAMZIEL5	:= %FF00FFAC	! Startadresse fuer U80601-Code Test5 !
RAMZIEL6	:= %FF00FFA8	! Startadresse fuer U80601-Code Test6 !
RETCODE		:= %FF00FFFC	! U80601 returncode-Bereich !
INTTAB		:= %F0000000	! U80601-INT-Tabelle im U8000-SEG-Nr.70 !
INT1		:= %FF01	! INT-Vektor 1 !

! U80601-Port Definitionen !

CPU2PORT	:= %FF1F	! U80601 Portadresse !
RESET		:= %FEFF	! U80601 RESET !
RUN		:= %FFFF	! U80601 Starten !

! Konstanten fuer Testmuster !

MUSTERLEN	:= 17
MUPROSEG	:= %FFFF / MUSTERLEN

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Teststatuscodes und Fehlercodes
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

TESTNR1		:= %A0		! EXRAM-Test durch U8000 !
TESTNR2		:= %B0		! Paritaetstest durch U8000 !
TESTNR3		:= %C0		! U80601 Anlauftest !
TESTNR4		:= %D0		! INT-Test U80601 <-> U8000 !
TESTNR5		:= %E0		! EXRAM-Test vom U80601 !
TESTNR6		:= %F0		! Test des DPRC 8207 !

errx1		:= 1		! Datenuebertragungsfehler beim U80601-Programm!
errx2		:= 2		! U80601 laeuft nicht an !
errx3		:= 3		! Falsche SEG-Nr. beim Speichertest !
errx4		:= 4		! Falsches Datenbyte beim Speichertest !

err11		:= TESTNR1+errx1	! nicht benutzt !
err12		:= TESTNR1+errx2	! nicht benutzt !
err13		:= TESTNR1+errx3	! Falsche SEG-Nr. beim Speichertest !
err14		:= TESTNR1+errx4	! Falsches Datenbyte beim Speichertest !

err21		:= TESTNR2+errx1	! Uebertragungsfehler in DRAM !
err22		:= TESTNR2+errx2	! nicht benutzt !
err23		:= TESTNR2+errx3	! nicht benutzt !
err24		:= TESTNR2+errx4	! Falsches Datenbyte beim Speichertest !

err31		:= TESTNR3+errx1	! moveerror im Test 3 !
err32		:= TESTNR3+errx2	! U80601 laeuft nicht an !

err41		:= TESTNR4+errx1	! moveerror im Test 4 !
err42		:= TESTNR4+errx2	! U80601 laeuft nicht an !
err43		:= TESTNR4+errx3	! nicht benutzt !
err44		:= TESTNR4+errx4	! nicht benutzt !
err45		:= TESTNR4 + 5		! U80601 springt falsche ISR an !
err46		:= TESTNR4 + 6		! U80601 springt ISR nicht an !
err47		:= TESTNR4 + 7		! U8000 hat keinen NVI empfangen !

err51		:= TESTNR5+errx1	! moveerror im Test 5 !
err52		:= TESTNR5+errx2	! U80601 laeuft nicht an !
err53		:= TESTNR5+errx3	! Falsche SEG-Nr. beim Speichertest !
err54		:= TESTNR5+errx4	! Falsches Datenbyte beim Speichertest !

err61		:= TESTNR6+errx1	! moveerror im Test 6 !
err62		:= TESTNR6+errx2	! U80601 laeuft nicht an !
err63		:= TESTNR6+errx3	! Falsche SEG-Nr. beim Speichertest !
err64		:= TESTNR6+errx4	! Falsches Datenbyte beim Speichertest !

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Pointer fuer U80601-ISR-Tabelle
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

INTERNAL

ISRP6	byte := %E4		! Startadresse ISR PC-Lowteil !
	byte := %FF		!      "        "  PC-Highteil!
	byte := %00		! Code-Segment Lowteil !
	byte := %F0		!   "      "   Highteil !

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Pointer fuer U80601-INT-SERVICE-ERROR-ROUTINE
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

ISRERRP	byte := %F2		! Startadresse ISRERR PC-Lowteil !
	byte := %FF		!      "          "   PC-Highteil!
	byte := %00		! Code-Segment Lowteil !
	byte := %F0		!   "      "   Highteil !


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Testmuster zum Testen des RAM-Bereiches
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
INTERNAL

MUSTER  ARRAY	[MUSTERLEN BYTE] := [%1 %2 %3 %4 %5 %6 %7 %8 %9
				%10 %11 %12 %13 %14 %15 %16 %17]


REM_PARNMI	word	! Merker: Paritaets-NMI !
REM_NVI		word	! Merker: NVI vom U80601 !




!*******************************************************************************
Prozeduren
*******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test600
Diese Prozedur fuehrt fuenf Tests durch:
1. RAM-Test vom U8000 aus
2. Paritaetstest
3. Anlauftest des U80601
4. Test der Interrupts zwischen U8000 und U80601
5. RAM-Test vom U80601 aus
6. Test des Dualport-RAM-controllers
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
_WEM_	PROCEDURE
	entry

	sc	#SC_NSEGV		! Nichtsegmentierte Vers. einstellen !

	dec	r15,#14			! Retten der Saferegister r8-r14 !
	ldm	@r15,r8,#7
	xorb	rl0, rl0
	outb	S_BNK, rl0		! MONITOR PROM & RAM einschalten !
	call	WR_2SP
	ld	ERR_CNT, #0

	call	test1			! U80601-RAM-Test vom U8000	!
	jr	nz, error		! wenn Fehler, jp		!

	call	test2			! Paritaetstest 		!
	jr	nz, error		! wenn Fehler, jp		!

	call	test3			! U80601 Anlauf-Test		!
	jr	nz, error		! wenn Fehler, jp		!

	call	test4			! INT-TEST U8000 <-> U80601	!
	jr	nz, error		! wenn Fehler, jp		!

	call	test5			! RAM-Zugriff vom U80601	!
	jr	nz, error		! wenn Fehler, jp		!

	call	test6			! Test des DPRC 8207		!
	jr	nz, error		! wenn Fehler, jp		!

	jr	teste

error:	call	MSG_ERROR		! Felerausgabe !

teste:
	call	WR_2BS
	call	WR_2SP
	call	WR_2BS

	ldm	r8,@r15,#7		! Saveregister r8-r14 regenerieren !
	inc	r15,#14
	ret

end _WEM_


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test1
Schreiben und Lesen des U80601-RAM-Bereiches, Vergleich mit vorgegebenem
Datenmuster in Feld MUSTER;
Ausgabe der Adresse sowie Soll- und Istdaten des fehlerhaften RAM-bytes

Benutzte Register: r4 ... r13
r4	Fehler-Istwert
r5	Sollwert bei Fehler
rr6	Zieladresse
r8	Testnummer
r9	Anzahl der Segmente
rr10	Quelladresse
r12	Blocklaenge
r13	Anzahl der Bloecke pro Segment
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test1	procedure
	entry

	call	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR1		! Test-Nr. laden !
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !

!Paritaetskontrolle ausschalten!

	inb	rl5, S_BNK
	resb	rl5, #3
	outb	S_BNK, rl5

!U80601-RAM mit Muster fuellen!

	ld	r10, #%8000		!Quellsegment des Datenmusters!
	ld	r6, #%F000		!Zielsegment= %70!
	ld	r9, #16			!16 Segmente a 64K = 1MByte !

test11:	ld	r13, #MUPROSEG		!Anzahl der zu schreibenden Bloecke!
	ld	r7, #0			! Zieloffset !
	sc	#SC_SEGV

test12:	ld	r11, #MUSTER		!Offset Quelle!
	ld	r12, #MUSTERLEN		!Blocklaenge!
	ldirb	@r6, @r10, r12		!Speicher schreiben!
	djnz	r13, test12		!Wiederholung!

	ldb	@r6, rh6		!Segmentnummer eintragen!
	sc	#SC_NSEGV
	incb	rh6			!naechstes Segment!
	djnz	r9, test11

!Speicher testen!

	ld	r10,#%8000		!SEG des Soll-Musters!
	ld	r6, #%F000		!Segment 70 testen!
	ld	r9, #16			!16 Segmente testen!
	calr	sp_test			!Daten im U80601-RAM mit Sollwert vergl.!

	! Setzen des Z-Flag in Abhaenigkeit des Testergebnisses !

	ld	r3, r8
	and	r3, #%F			!wenn Fehler, dann ZF=0!
	ret

end	test1

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test2
Test 2 : Paritaetstest
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test2	PROCEDURE
	entry

	call	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR2		!Testnr. laden!
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !


!Speicher mit Testdatenmuster fuellen!

	ld	r2, #16			!Anzahl der Segmente!
	ldl	rr10, #%F0000000	!Zieladresse Segment %70! 
test21:	ld	r3, #8			! 8 * %2000 = 64k = 1 SEG!
test22:	ld	r4, #%2000		!Datenlaenge!
	ldl	rr6, #%80000000		!Quelladresse  (Monitorprogramm)!
	calr	MOVE6
	bit	r8,#0			! move error ? !
	jr	nz,test2end		! wenn Fehler, JP !
	djnz	r3, test22

	add	r10, #%100		!naechstes Segment!
	djnz	r2, test21

	inb	rl5, S_BNK
	setb	rl5, #3			!Paritaetskontrolle einschalten!
	outb	S_BNK, rl5

	ldl	rr10, #%F0000000	!erste zu testende RAM-Adresse Seg 70!

test24:	ld	REM_PARNMI, #0		!Merkzelle Paritaets-NMI loeschen!
	sc	#SC_SEGV
	ld	r4, @r10		!Wort ruecklesen!
	sc	#SC_NSEGV
	test	REM_PARNMI		!Paritaetsfehler?!
	jr	z, test25		!nein!
	ld	ERRPAR_ID, #%EA		! r6=SEG, r7=off, r5=Soll, r4=Ist !
	add	r8, #errx4		! Fehlercode fuer falsche Daten !
	ldl	rr6,rr10		! Fehler-Adresse umladen !
	and	r11, #%1FFF		! Sollwert-Adresse bilden !
	ld	r5, @r11		! Sollwert aus SEG 0 laden !
	jr	test2end

test25:	inc	r11, #2			!naechstes Wort!
	test	r11			!ganzes Segment geprueft?!
	jr	nz, test24		!nein!

	add	r10, #%100		!naechstes Segment!
	jr	nz, test24

	inb	rl5, S_BNK
	resb	rl5, #3			!Paritaetskontrolle ausschalten!
	outb	S_BNK, rl5

	!Setzen des Z-Flags in Abhaengigkeit des Testergebnisses!

test2end:
	ld	r3, r8
	and	r3, #%F			!wenn Fehler, dann ZF=0!
	ret

end	test2

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test3
Test 3 : Anlauf U80601
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test3	PROCEDURE
	entry

	call	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR3		! Testnr. laden !
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !


	ld	r6, #%8000		! SEG-Nr. Quelle !
	ld	r7, #t3c86		! OFFSET Quelle laden !
	ld	r4, lt3c86		! Laenge in Byte !
	ldl	rr10, #RAMZIEL3
	calr	MOVE6			! Testprogramm f. U80601 in letztes SEG laden !
	bit	r8,#0			! move error ? !
	jr	nz,test3end		! wenn Fehler, JP !

	call	run80601		! Start des Testprogramms !

	call	time			! 1ms warten !

	call	getrc			! r6 := U80601 returncode !
	cp	r6, r8			!Codewort testen (returncode=Test-Nr.?)!
	jr	z,test3end		! wenn ja, Codewort o.k., JP !

	ld	r7, r6			! Istwert holen !
	ld	r6, r8			! Sollwert laden !
	ld	r8, #err32		! Fehlercode laden !
	ld	ERRPAR_ID, #%A0		! Fehlerparameter: r6=Soll, r7=Ist !
	
test3end:
	call	reset80601

	! Setzen des Z-Flag in Abhaenigkeit des Testergebnisses !

	ld	r3, r8
	and	r3, #%F			! wenn Fehler, dann ZF=0 !
	ret

end	test3


!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test4
Test 4 : INT-Verkehr U8000 <-> U80601
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test4	PROCEDURE
	entry

	push	@r15, r0
	push	@r15, r1

	call	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR4		! Testnr. laden !
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !


! Systeme fuer INT-Pruefung U80601 initialisieren !

	! INT-Vector-Tabelle fuer U80601 erzeugen !

	ld	r6, #%8000		! SEG-NR. des Quellbereichs !
	ld	r7, #ISRERRP		! OFFSET ISRERR-Pointer !
	ld	r4, #4			! Laenge in Byte !
	ldl	rr10, #INTTAB		! Zieladr. der INT-Tabelle !
	calr	MOVE6			! move des ersten INT-Pointers !
	bit	r8, #0			! Fehler beim copieren ?   !
	jr	nz, test4end		! Wenn ja, JP !

	! Vervielfachung dieses Pointers auf alle 256 INT-Adressen !

	ld	r9, #4			! Startwert fuer Byte-Zaehler !
	ld	r2, #8			! Startwert Wiederholungszaehler !

test41:	ldl	rr6, #INTTAB		! Quelladresse laden !
					! Zieladr. steht noch in rr10 !
	ld	r4,r9			! Aktuelle Byteanzahl laden !
	calr	MOVE6			! Copieren der ersten Pointer !
	bit	r8, #0			! Fehler beim copieren ?   !
	jr	nz, test4end		! Wenn ja, JP !

	sll	r9, #1			! Neuer Startwert = 2 * Alt  !
	djnz	r2, test41		! Wiederholen bis alle Pointer geladen !

	! Code fuer U80601 laden !

	ld	r6, #%8000		! SEG-Nr. Quelle !
	ld	r7, #t4c86		! OFFSET Quelle !
	ld	r4, lt4c86		! Laenge in Byte !
	ldl	rr10, #RAMZIEL4		! Zieladresse laden !
	calr	MOVE6			! U80601-Code in Zielbereich bringen !
	bit	r8, #0
	jr	nz,test4end		! wenn Fehler, JP !

	! U8000 fuer NVI Vorbereiten (Programm-Status fuer NVI laden) !

	ldctl	r9, PSAPOFF		! PSAP-OFFSET holen !
	ld	%32(r9), #%4000		! FCW laden !
	ld	%34(r9), #%8000		! PC-SEG laden !
	ld	%36(r9), #NVISR6	! PC-OFFSET laden !

	! U80601 fuer INT initialisieren und ueberpruefen !

	calr	run80601		! U80601 fuer INT initialisieren !
	calr	time
	calr	getrc			! r6 := U80601 returncode !
	cp	r6,r8
	jr	z,test42		! wenn o.k., JP !

	ld	r8, #err42		! U80601 laeuft nicht an !
	ld	ERRPAR_ID, #%80		! als Fehlerparameter nur r6 !
	jr	test4end

! 256 INT-Vektoren an den U80601 geben und die Ausfuehrung kontr. !

test42: ld	ERRPAR_ID, #%A0		! Parameter : r6 r7 !
	ld	r0, r8			! r0 := Test-Nr. !
	ld	r1, #INT1		! erster INT-Vektor !
	ld	r2, #256		! INT-Zaehler laden !
	ldl	rr12, #INTTAB+4		! Zeiger auf INT-Pointer 1 !

test43:	
	! ISR-Pointer an aktuelle Position in die INT-Tabelle eintragen !

	ld	r6, #%8000		! SEG-NR. des Quellbereichs !
	ld	r7, #ISRP6		! OFFSET ISR-Pointer !
	ld	r4, #4			! Laenge in Byte !
	ldl	rr10, rr12		! Zieladr. in der INT-Tabelle !
	calr	MOVE6
	bit	r8, #0			! Fehler beim copieren ?   !
	jr	nz, test4end		! Wenn ja, JP !

	clr	REM_NVI			! Merker fuer NVI loeschen !
	ei	nvi			! NVI erlauben  !
	out	CPU2PORT, r1		! VINT an U80601 !
	calr	time			! auf NVI vom U80601 warten !

	! Ueberpruefung ob U80601 seine ISR angesprungen hat !

	calr	getrc			! r6 := Returncode (rh6=INTV / rl6=Test-Nr.) !
	ldb	rh0, rl1		! rh0 := Soll-INT-Vektor !
	ld	ERRPAR_ID, #0		! keine Parameterausgabe !
	cpb	rl6, #err43		!falsche ISR angesprungen?!
	jr	eq, test4end		!ja!
	cp	r6, r0			! Istwert = Sollwert ? !
	jr	eq, test44		! wenn o.k., jp !
	ld	ERRPAR_ID, #%A0		! r6=Soll, r7=Ist !
	ld	r8, #err44		! Fehlercode laden !
	ld	r7,r6			! Istwert laden !
	ld	r6,r0			! Sollwert laden !
	jr	test4end

	! Test ob U8000 NVI registriert hat !

test44:	test	REM_NVI			! NVI gespeichert ? !
	jr	nz, test45		! wenn ja, jp !
	ld	r8, #err45		! Fehlercode laden !
	jr	test4end

test45:
	! INT-Pointer in INT-Tabelle auf ISRERR zurueckstellen !

	ld	r6, #%8000		! SEG-NR. des Quellbereichs !
	ld	r7, #ISRERRP		! OFFSET ISRERR-Pointer !
	ld	r4, #4			! Laenge in Byte !
	ldl	rr10, rr12		! aktuelle Zieladr. in der INT-Tabelle !
	calr	MOVE6
	bit	r8, #0			! Fehler beim copieren ?   !
	jr	nz, test4end		! Wenn ja, JP !

	inc	r13, #4			! OFFSET des aktuellen ISR-Pointers erhoehen !
	and	r13, #%3FF		! auf MODULO %400 bringen !
	incb	rl1			! naechster INT-Vektor !

	djnz	r2, test43

test4end:
	di	nvi			! U8000 NVI sperren !
	calr	reset80601

	! Setzen des Z-Flag in Abhaenigkeit des Testergebnisses !

	ld	r3, r8
	and	r3, #%F			! wenn Fehler, dann ZF=0 !

	pop	r1, @r15
	pop	r0, @r15
	ret

end	test4

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test5
Test 5 : U80601 traegt Muster in RAM, U8000 testet
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test5	PROCEDURE
	entry

	call	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR5		! Testnr. laden !
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !

	
	!Speicherbereich fuer U80601 loeschen!

	calr	clrmem

	! Code fuer U80601 laden !

	ld	r6, #%8000		! SEG-Nr. Quelle !
	ld	r7, #t5c86		! OFFSET Quelle !
	ld	r4, lt5c86		! Laenge in Byte !
	ldl	rr10, #RAMZIEL5		! Zieladresse laden !
	calr	MOVE6			! U80601-Code in Zielbereich bringen !
	bit	r8, #0
	jr	nz, test5end		! wenn Fehler, JP !

	!Datenfeld fuer U80601 laden!

	ld	r6, #%8000		!Seg-Nr. Quelle!
	ld	r7, #MUSTER		!Offset Quelle!
	ld	r4, #MUSTERLEN		!Blocklaenge!
	ldl	rr10, #%FF000000	!Zieladresse Seg. 7F!
	calr	MOVE6			!Datenfeld in Zielbereich bringen!
	bit	r8, #0			!MOVE6-ERROR ?!
	jr	nz, test5end		!ja!

	calr	run80601		!U80601 starten!

	ld	r12, #%B00
test51:	call	time			!1s warten!
	djnz	r12, test51

	calr	getrc			! r6 := U80601 returncode !
	cp	r8,r6			! Soll=Ist ?  !
	jr	eq,test52		! wenn o.k., JP !

	ld	r8, #err52		! U80601 laeuft nicht an !
	ld	ERRPAR_ID, #%80		! als Fehlerparameter nur r6 !
	jr	test5end

test52:	calr	reset80601

!Speicher testen!

	ld	r10,#%8000		!Segmentnummer Quelle (Sollwerte)!
	ld	r6, #%F000		!Segmentnummer Ziel (Istwerte)!
	ld	r9, #15			!Anzahl der zu testenden Segmente!
					!Segment 7F wird nicht getestet!
	calr	sp_test

test5end:

	! Setzen des Z-Flag in Abhaenigkeit des Testergebnisses !

	ld	r3, r8
	and	r3, #%F			! wenn Fehler, dann ZF=0 !
	ret

end	test5

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE test6
Test 6 : Test DPRC 8207 (gemeinsamer Speicherzugriff)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
test6	PROCEDURE
	entry

	calr	reset80601		! RESET an U80601 !
	ld	r8, #TESTNR6		! Testnr. laden !
	xorb	rh1, rh1		! Flagbyte rh1 loeschen !
	call	WR_TESTNR		! Ausgabe der Testschrittnummer !

	calr	clrmem			!Speicherbereich fuer U80601 loeschen!

	! Code fuer U80601 laden !

	ld	r6, #%8000		! SEG-Nr. Quelle !
	ld	r7, #t6c86		! OFFSET Quelle !
	ld	r4, lt6c86		! Laenge in Byte !
	ldl	rr10, #RAMZIEL6		! Zieladresse laden !
	calr	MOVE6			! U80601-Code in Zielbereich bringen !
	bit	r8, #0
	jr	nz, test6end		! wenn Fehler, JP !

	!Daten fuer U80601 laden!

	ld	r6, #%8000		!Seg-Nr. Quelle!
	ld	r7, #MUSTER		!Offset Quelle!
	ld	r4, #MUSTERLEN		!Blocklaenge!
	ldl	rr10, #%FF000000	!Zieladresse Seg. 7F!
	calr	MOVE6			!Datenfeld in Zielbereich bringen!
	bit	r8, #0
	jr	nz, test6end

	calr	run80601		!U80601 starten!
	
	!  Speicher schreiben 
beginnend mit SEG %70 OFFSET 0000 und endend mit SEG %7E werden 17 Bytes mit
Muster gefuellt und die naechsten 17 Bytes werden frei gelassen (werden vom
U80601 gefuellt) !

	ld	r6, #%8000		!Segmentnummer Quelle!
	ld	r10, #%F000		!Segmentnummer Ziel!
	ld	r9, #15			!Anzahl der zu testenden Segmente!
	sc	#SC_SEGV

test61: ld	r13, #MUPROSEG / 2	!Anzahl der zu schreibenden Bloecke pro
								halb-SEG !
	ld	r11, #%0000		!Offset Ziel!

test62: ld	r7, #MUSTER		!Offset Quelle!
	ld	r12, #MUSTERLEN		!Blocklaenge!
	ldirb	@r10, @r6, r12		!Speicher schreiben!
	add	r11, #MUSTERLEN		!17 Byte freilassen fuer U80601!
	djnz	r13, test62		!Wiederholung!

	add	r10, #%100		!naechstes Segment!
	djnz	r9, test61
	
	sc	#SC_NSEGV
	calr	reset80601

!Speicher testen!

	ld	r10,#%8000		!Segmentnummer Quelle!
	ld	r6, #%F000		!Ziel-SEG = %70!
	ld	r9, #15			!Anzahl der zu testenden Segmente!
					!Segment %7F wird nicht getestet!
	calr	sp_test

test6end:

	! Setzen des Z-Flag in Abhaenigkeit des Testergebnisses !

	ld	r3, r8
	and	r3, #%F			! wenn Fehler, dann ZF=0 !
	ret

end	test6

!*******************************************************************************

	H i l f s p r o g r a m m e

*******************************************************************************!

	
!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MOVE6
transportiert Daten von Quelladresse zur Zieladresse
inputs:		rr6 = Quelladresse
		rr10 = Zieladresse
		 r4 = Datenlaenge in Bytes

outputs:	r4 = Datenwort EXRAM (Istwert)
		r5 = Datenwort SEG 0 (Sollwert)
	       rr6 = Adresse Istwert
	        r8 = Testnummer / Fehlernummer
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
MOVE6	PROCEDURE
	
	entry

	sc	#SC_SEGV		! segmentierte Version !
	and	r8, #%FFF0		!Fehlerbit ruecksetzten!

	dec	r15, #16
	ldm	@r14, r4, #8		! Parameter retten !

	ldirb	@r10, @r6, r4		! copy !

	ldm	r4, @r14, #8
	inc	r15, #16		!Parameter rueckladen!

	cpsirb	@r10, @r6, r4, ne	! Vergleich bis Ungleichheit !
	jr	nz, MOVE6_END		! JP wenn kein Fehler !

	dec	r11			! Adressen zurueckstellen !
	dec	r7
	ldb	rl5, @r6		! Sollwert laden !
	ldb	rl4, @r10		! Fehlerbyte laden !
	sc	#SC_NSEGV
	add	r8, #errx1		! Transferfehler setzen !
	ld	ERRPAR_ID, #%E5		! r6=SEG, r7=off, r5=Soll, r4=Ist !
	ldl	rr6, rr10		! Fehleradresse umladen !

MOVE6_END:
	sc	#SC_NSEGV
	ret

end MOVE6

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE clrmem
	U80601-RAM loeschen ( 16 Segmente)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

GLOBAL
clrmem	PROCEDURE
	entry

	ldl	rr10, #%F0000002	! 1. Zieladresse !
	ld	r5, #16			! Anzahl der SEGMENTE !
	sc	#SC_SEGV
clrmem1:
	ldl	rr6, #%F0000000		!Startadresse Segment 70!
	ld	r11, #0
	ld	@r6, #0			! erstes Wort loeschen !
	ld	r2, #%8000		! Zaehler fuer Worte pro SEG !
	ldir	@r10,@r6,r2		!Speicher-SEG fuellen!
	add	r10, #%0100		!naechstes Segment!
	djnz	r5, clrmem1	

	sc	#SC_NSEGV
	ret

end	clrmem


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE sp_test
WDOS-Erweiterungs-DRAM testen
inputs:	r10 = SEG-Nr. Sollwerte
	r6  = SEG-Nr. Istwerte
	r9  = Anzahl der zu testenden Segmente
	rl8 = Testnummer

output: rll8 = Fehlerstatus (0: kein Fehler)
	rr10 = Adr. Sollbyte
	rr6  = Adr. Fehlerhaftes Byte
	rl5  = Sollwert
	rl4  = Istwert

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
sp_test	PROCEDURE
	entry

	sc	#SC_SEGV
sp_t0:	ld	r13,#MUPROSEG		!Anzahl der zu testenden Bloecke!
	ld	r7, #0000		!Offset Ziel!

sp_t1:	ld	r11, #MUSTER		!Offset Quelle!
	ld	r12, #MUSTERLEN		!Muster-Blocklaenge!
	cpsirb	@r6, @r10, r12, ne	!ein Muster-Block testen!
	jr	z, sp_tf1		!Fehler im Block!
	djnz	r13, sp_t1		!naechster Block!

	cpb	rh6, @r6		! Segmentnummer testen!
	jr	eq, sp_t5		!Test beendet, kein Fehler!
	ldb	rl4, @r6		! fehlerhafte SEG-Nr. laden !
	ldb	rl5, rh6		! Soll-SEG-Nr. laden !
	sc	#SC_NSEGV
	ld	ERRPAR_ID, #%E5		!r6=SEG-Nr., r7=off, rl5=Soll, rl4=Ist!
	or	r8, #errx3		!Fehlercode fuer falsche SEG-Nr. setzen!
	jr	sp_tend

sp_tf1:	dec	r11			! offset adr. Sollbyte !
	dec	r7			! offset Fehleradr. !
	ldb	rl5, @r10		! Sollwert holen !
	ldb	rl4, @r6		! Istwert holen !
	sc	#SC_NSEGV		! Fehlerbehandlung bei Musterfehler !
	ld	ERRPAR_ID, #%E5		!r6=SEG-Nr., r7=off, rl5=Soll, rl4=Ist !
	or	r8, #errx4		!Fehlercode fuer Fehler im Daten-Muster!
	jr	sp_tend	

sp_t5:	incb	rh6			!naechstes Segment!
	djnz	r9, sp_t0
	sc	#SC_NSEGV
sp_tend:
	ret

end	sp_test


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE reset80601
U80601 RESET 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
reset80601	PROCEDURE
	entry

	push	@r15,r3
	ld	r3, #RESET
	out	CPU2PORT,r3
	pop	r3,@r15
	ret

end	reset80601

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE run80601
Start U80601 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
run80601	PROCEDURE
	entry

	push	@r15,r3
	ld	r3, #RUN
	out	CPU2PORT,r3
	pop	r3,@r15
	ret

end	run80601

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE time
Zeitschleife fuer U8000 (1 ms)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
time	procedure
	entry

	push	@r15,r3
	ld	r3, #364	! Zeitkonstante fuer 1ms !
time1l:	djnz	r3,time1l
	pop	r3,@r15
	ret

end time

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE getrc
U80601-returncode abholen und in r6 uebergeben
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
getrc	procedure
	entry

	push	@r15,r10
	push	@r15,r11

	sc	#SC_SEGV
	ldl	rr10, #RETCODE		! Returncode-Adresse laden !
	ld	r6,@r10			! Returncode holen	!
	sc	#SC_NSEGV

	pop	r11,@r15
	pop	r10,@r15
	ret

end getrc

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NVI-Service-Routiene 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL
NVISR6	procedure
	entry

	ld	REM_NVI, #%FF		! NVI-Merker setzen !
	sc	#SC_SEGV		! SEG-Version fuer iret !
	iret

end NVISR6


!*******************************************************************************
U80601-PROZEDUREN
*******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
U80601-Code fuer Test 3 (Anlauftest)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

	GLOBAL

t3c86	array [2 word] := [%8CC8 %8ED8]
	array [4 byte] := [%B8 %00 TESTNR3 %A3]
	array [4 word] := [%FCFF %F400 %0000 %0000]
lt3c86		 word  := $-t3c86

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
U80601-Code fuer Test 4 (INT-Test)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

t4c86	array [* word] := [%8CC8 %8ED8 %8ED0 %BCD0 %FFFB %B800]
	array [* byte] := [TESTNR4 %A3 %FC %FF]
	array [* word] := [%F4E9 %FCFF %BCD0 %FFFE %06FC %FF33 %D2EF %FBF4]
	array [* word] := [%EBDE]
	array [* byte] := [%B0 err43]
	array [* word] := [%A2FD %FFEB %EB00 %0000 %0000 %0000]
lt4c86		 word  := $-t4c86

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
U80601-Code fuer Test 5 (SpeicherTESTNR)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

t5c86	array [*byte] := [%8C %C8 %8E %D0 %8E %D8] 
	array [*byte] := [%BC %00 %FF %B8 %00 %00]

	array [*byte] := [%8E %C0 %B8 %0F %0F %FC %BF %00 %00] 
	array [*byte] := [%BE %00 %00 %B9 %11 %00 %F3 %A4] 

	array [*byte] := [%48 %3D %00 %00 %75 %F2 %8C %C0] 
	array [*byte] := [%B1 %04 %D3 %E8 %05 %00 %F0]

	array [*byte] := [%26 %88 %25 %8C %C0 %05 %00 %10 %8E %C0 %3D] 

	array [*byte] := [%00 %F0 %75 %D3 %B8 %00 TESTNR5 %A3] 
	array [*byte] := [%FC %FF %F4 %EB %FD %EB %BA %00] 

	array [*byte] := [%00 %00 %00 %00 %00 %00 %00 %00] 
	array [*byte] := [%00 %00 %00 %00 %00]
lt5c86		word  := $-t5c86

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
U80601-Code fuer Test 6 (gemeinsamer Speicherzugriff)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

t6c86	array [*byte] := [%8C %C8 %8E %D0 %8E] 
	array [*byte] := [%D8 %BC %00 %FF %B8 %00 %00 %8E]

	array [*byte] := [%C0 %B8 %87 %07 %FC %BF %11 %00] 
	array [*byte] := [%BE %00 %00 %B9 %11 %00 %F3 %A4]

	array [*byte] := [%81 %C7 %11 %00 %48 %3D %00 %00] 
	array [*byte] := [%75 %EE %8C %C0 %B1 %04 %D3 %C8]
	array [*byte] := [%05 %00 %F0 %26 %88 %25 %8C %C0 %05 %00]

	array [*byte] := [%10 %8E %C0 %3D %00 %F0 %75 %CF] 
	array [*byte] := [%B8 %00 TESTNR6 %A3 %FC %FF %F4 %EB]

	array [*byte] := [%FD %EB %B6 %00 %00 %00 %00 %00] 
	array [*byte] := [%00 %00 %00 %00 %00 %00 %00 %00]

	array [*byte] := [%00]
lt6c86		word  := $-t6c86

end twem

