*P 84
 
;TERMINAL-EMULATOR MODUL FUER DAS REMOTE-PAKET
;REM.CRTEM.S
 
	;EXTERNE GROESSEN AUS DEM GERAETESPEZ. TEIL:
	EXTERNAL SYSTEM ACTCMD DEACT
	EXTERNAL INIPTY RESPTY
	EXTERNAL LODTTY SNDTTY
	EXTERNAL HWSTART
	EXTERNAL HWINIT
 
	;EXTERNE GROESSEN AUS DEM DATEI-AKTIVITAETEN-MODUL:
	EXTERNAL ACTION ACTFLG ACTNOW MORACT
 
	GLOBAL PUTMSG PUTSTR
	GLOBAL LODNET
	GLOBAL NETSND
	GLOBAL ERROR
	GLOBAL LOOP
	GLOBAL SNDNET RETURN
	GLOBAL CHAR1 CHAR2 CHAR3
 
 
ENTRY:		;REMOTE ENTRY
INIT:			;INITIALIZE REMOTE WORKING
	LD (STACK),SP
	CALL INIPTY
	CALL HWINIT
	LD HL,ACTFLG
	LD (HL),0
	LD IY,ACTCMD
	CALL SYSTEM
	BIT 6,(IY+0AH)
	JP NZ,ACTIFL
	LD IY,ASGNVC
	CALL SYSTEM
	BIT 6,(IY+0AH)
	JR Z,NETOK
	LD HL,NODEVM
	CALL PUTMSG	;PUT NODEVMESSAGE OUT AND RETURN TO UDOS
	XOR A
	JP RETURN
 
CAPMSG:
	DEFB CAPLEN	;STRING LENGTH OF MSG
	DEFM 'You are now connected with the WEGA-System !'
	DEFB 0DH
CAPLEN:	EQU $-CAPMSG-1
 
NETOK:
	LD HL,CAPMSG
	CALL PUTMSG
	LD IY,NETSTW
	CALL SYSTEM	;SET THE NET-PARAMETERS
	ld a,0dh
	call SNDNET
 
;++++++++++
;TERMINAL MODE
	;FUER P8000: START U8000 !!!
	CALL HWSTART
 
LOOP:
	LD HL,COUNT
	LD (HL),060H
LOOP1:
	CALL LODNET
	PUSH AF
	CALL LODTTY
LGOON:	PUSH AF
	CALL NZ,SNDNET
	POP AF
	JR Z,LOOP4
	AND 7FH
	CP 13H		;^S
	JR NZ,LOOP4
LWAIT:	CALL LODTTY
	JR Z,LWAIT
	LD HL,COUNT
	INC (HL)
	DEC (HL)
	JR NZ,LGOON	;COUNT DARF BEI FILE UEBERTRAGUNG IM 
			;TERMINALMODE NIE 0 WERDEN, DH ZUR
			;FILE UEBERTRAGUNG MUESSEN BLOCKLAENGEN
			;KLEINER C0H VERWENDET WERDEN, WAS AUCH
			;DER FALL IST (QUITTUNGSBETRIEB)
			;NUR GUELTIG FALLS REMOTE SYSTEM (HOST)
			;UEBER MEHRERE LOCAL SYSTEME BEDIENT WIRD
	LD (HL),1
	JR LGOON
 
LOOP4:	POP AF
	LD HL,COUNT
	JR NZ,LOOP2
 
	LD A,(HL)	;NO CHAR FROM NET
	LD (HL),060H
	OR A
	LD A,11H	;^Q
	CALL Z,SNDNET
	JR LOOP1
 
LOOP2:	PUSH AF
	LD A,(HL)
	OR A
	JR Z,LOOP3
	DEC (HL)
	LD A,13H	;^S
	CALL Z,SNDNET
LOOP3:	POP AF
 
	PUSH AF
	AND 7FH
	LD HL,ACTFLG
	BIT 0,(HL)
	JR NZ,OVLUP1
 
	CP 01H		;CTRL-A STARTS ALL COMMAND SEQUENCES
	JR NZ, NOACTN
	JP ACTION
 
OVLUP1:	BIT 1,(HL)
	JP NZ,ACTNOW
	CP 1BH		;ESCAPE
	JP Z, MORACT
 
	RES 0,(HL)
	LD A,(CHAR1)	;SEND OLD CHAR
	CALL SNDTTY
 
NOACTN:	POP AF
	CALL SNDTTY
	JR LOOP1
 
COUNT:	DEFB 0C0H
CHAR1:	DEFS 1
CHAR2:	DEFS 1
CHAR3:	DEFS 1
 
;++++++++++
;RETURN TO LOCAL
 
RETURN:
	LD SP,(STACK)
	PUSH AF
	LD IY,DEACT
	CALL SYSTEM
	POP AF
 
	LD HL,RMSG1
	CALL NZ,PUTMSG
	LD HL,RMSG2
	CALL PUTMSG
	CALL RESPTY
	RET
 
RMSG1:	DEFT 'Logout and'
RMSG2:	DEFB RLEN2
	DEFM 'Return to Local-System'
	DEFB 0DH
RLEN2:	EQU $-RMSG2-1
 
 
;++++++++++
;LOAD A BYTE FORM NET
 
LODNET:
	LD IY,NETSTR
	CALL SYSTEM	;READ NET STATUS
	LD A,(NETSTS)
	BIT 7,A		;ARE THERE CHAR IN INPUTBUFFER
	RET Z
 
	LD IY,NETVCR	;READ-ABSOLUTE FROM NET
	CALL SYSTEM
	OR 01
	LD A,(CHRBUF)
	RET		;NZ-RETURN IF CHAR IN CHRBUF
 
;++++++++++
;SEND A BYTE TO NET
 
SNDNET:
	LD (CHRBUF),A
	LD IY,NETVCW	;WRITE ABSOLUT TO NET
	JP SYSTEM
 
NETSND:
	LD A,(HL)
	CP 80H
	RET Z
	INC HL
	PUSH HL
	CALL SNDNET
	POP HL
	JR NETSND
 
;++++++++++
;NET DRIVER NOT ACTIVATE
 
ACTIFL:
	LD HL,ACTFAL
	CALL PUTMSG	;ACTIVATE FAILED MESSAGE AND UDOS-RETURN
	XOR A
	JP RETURN
 
ACTFAL:	DEFB 10H
	DEFM 'ACTIVATE FAILED '
 
 
;++++++++++
;ERROR
 
ERROR:
	LD A,(IY+0AH)
	LD HL,ERRMSN
	CALL BTOHEX
	LD HL,ERRMSG
 
PUTMSG:
	LD DE,OUTBUF
	LD A,(HL)
	INC A
	LD C,A
	LD B,0
	PUSH DE
	LDIR
	EX DE,HL
	LD (HL),0DH
	POP HL		;HL = OUTBUF
	LD (HL),A
PUTSTR:
	LD A,(HL)
	INC HL
	LD (STRDTA),HL
	LD (STRLEN),A
	LD IY,STRVEC
	CALL SYSTEM
	CALL INIPTY
	RET
 
BTOHEX:
	PUSH AF
	RRCA
	RRCA
	RRCA
	RRCA
	CALL BYTE1
	POP AF
BYTE1:
	AND 0FH
	CP 0AH
	JR C,HEXDIG
	ADD A,07
HEXDIG:
	ADD A,30H	;ADD ASCII-BIAS
	LD (HL),A
	INC HL
	RET
 
 
;RAM-DEFINITIONEN UND DATA SECTION
 
ERRMSG:	DEFB 0DH	;STRLENGTH OF ERRMSG
	DEFM 'FILE ERROR '
ERRMSN:	DEFW 2020H
NODEVM:	DEFB 1BH	;LENGTHGH OF NODEVM
	DEFM 'INVALID OR INACTIVE DEVICE '
 
 
 
;VECTOREN FUER UDOS-CALLS
 
ASGNVC:	DEFB 19		;NET ASSIGN
	DEFB 2
	DEFW 0
	DEFW 0
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW ASGSPV
ASGSPV:
	DEFB 80H
 
NETSTW:	DEFB 19		;NET STATUS WRITE
	DEFB 42H
	DEFW NETSET
	DEFW 7
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW 0
;
; ACHTUNG!!! STATUS FUER $SI-TREIBER DES P8000 !!!
; (UNTERSCHIEDLICH ZUM $SIBDRIVER UND $ASVDRIVER)
;
NETSET:
	DEFB 0
	DEFB 7CH
	DEFB 0FFH
	DEFB 82H
	DEFB 83H
	DEFB 4		;ZEITKONSTANTE BAUDRATE
			; 4 = 9600 BAUD
	DEFB 00
NETMSK:	DEFB 7FH
 
NETSTR:	DEFB 19		;NET STATUS READ
	DEFB 40H
	DEFW NETSTS
	DEFW 1
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW 0
 
NETSTS:
	DEFS 1
 
NETVCR:	DEFB 19		;NET READ  ABSOLUT
	DEFB 46H
	DEFW CHRBUF
	DEFW 1
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW 0
 
CHRBUF:	DEFS 1
 
NETVCW:	DEFB 19		;NET WRITE ABSOLUTE
	DEFB 48H
	DEFW CHRBUF
	DEFW 1
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW 0
 
 
STRVEC:	DEFB 02		;CON WRITE BIN
	DEFB 0EH
STRDTA:	DEFS 2
STRLEN:	DEFS 2
	DEFW 0
	DEFW 0
	DEFB 0
	DEFW 0
 
STACK:	DEFS 20H
OUTBUF:	DEFS 81
