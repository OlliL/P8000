;**************************************************************************
;**************************************************************************
;
;	WDOS-BIOS - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
;	Programm: disk.asm
;
;	Bearbeiter	: P. Hoge
;	Datum		: 24.9.89
;	Version		: 1.1
;
;**************************************************************************
;
;	Hard- u. Floppy Disk-Treiber
;
;**************************************************************************
;**************************************************************************


.PUBLIC DISK
.PUBLIC DISK_SETUP
.PUBLIC DISKINT
.PUBLIC SEEK

.EXTERNAL DDS ;NEAR
.EXTERNAL LONG HF_TBL_VEC
.EXTERNAL LONG HF1_TBL_VEC
.EXTERNAL LONG DISK_POINTER
.external byte DISK_BASE
.EXTERNAL BYTE FD_TBL
.EXTERNAL BYTE FD1_TBL
.EXTERNAL BYTE FD2_TBL
.EXTERNAL BYTE DISKETTE_STATUS
.EXTERNAL BYTE SEEK_STATUS
.external byte intin
.external byte intout
.external byte diskin
.external byte diskout
.external byte hdtyp
.external byte diskrq


;-----INT 13 ---------------------------------------------------
;
;	THIS INTERFARCE PROVIDES ACCESS DISKS
;
;---------------------------------------------------------------
;
;INPUT	   (AH = HEX VALUE)
;
;	(AH)=00 RESET DISK - Nullop
;	(AH)=01 READ THE STATUS OF THE LAST DISK OPERATION INTO (AL)
;		NOTE: DL<80H - DISKETTE
;		      DL>80H - DISK
;	(AH)=02 READ THE DESIRED SECTORS INTO MEMORY
;	(AH)=03 WRITE THE DESIRED SECTORS FROM MEMORY
;	(AH)=04 VERIFY THE DESIRED SECTORS - Nullop
;	(AH)=05 FORMAT THE DESIRED TRACK - Nullop
;	(AH)=06 UNUSED
;	(AH)=07 UNUSED
;	(AH)=08 RETURN THE CURRENT DRIVE PARAMETERS
;	(AH)=09 INITIALIZE DRIVE PAIR CHARACTERISTICS (HD) - Nullop
;		INTERRUPT 41 POINTS TO DATA BLOCK FOR DRIVE 0
;		INTERRUPT 46 POINTS TO DATA BLOCK FOR DRIVE 1
;	(AH)=0A READ LONG (HD) - nicht moeglich
;	(AH)=0B WRITE LONG (HD) - nicht moeglich
;	(AH)=0C SEEK (HD) - Nullop
;	(AH)=0D ALTERNATE DISK RESET (HD) - Nullop
;	(AH)=0E UNUSED
;	(AH)=0F UNUSED
;	(AH)=10 TEST DRIVE READY (HD) - nicht moeglich
;	(AH)=11 RECALIBRATE (HD) - Nullop
;	(AH)=12 UNUSED
;	(AH)=13 UNUSED
;	(AH)=14 CONTROLLER INTERNAL DIAGNOSTIC (HD) -Nullop
;	(AH)=15 READ DASD TYPE
;	(AH)=16 READ DISK CHANGE (FD)
;	(AH)=17 SET DISK TYPE (FD) - Nullop
;
;		REGISTERS USED FOR DISK OPERATIONS
;
;		(DL) - DRIVE NUMBER
;		(DH) - HEAD NUMBER
;		(CH) - CYLINDER NUMBER (Track)
;		(CL) - SECTOR NUMBER
;		(AL) - NUMBER OF SECTORS (MAXIMUN POSSIBLE RANGE 1 - 80H,
;		(ES:BX) - ADDRESS OF BUFFER FOR READS AND WRITES,
;OUTPUT
;	AH = STATUS OF CURRENT OPERATION
;	     STATUS BITS ARE DEFINED IN THE EQUATES BELOW
;	CY = 0 SUCCESSFUL OPERATION (AH=0 ON RETURN)
;	CY = 1 FAILED OPERATION (AH HAS ERROR REASON)
;
;	IF DRIVE PARAMETERS WERE RAQUESTED
;
;	DL = NUMBER OF CONSECUTIVE ACKNOWLEDGING DRIVES ATTACHED (0-2)
;	DH = MAXIMUM USEABLE VALUE FOR HEAD NUMBER
;	CH = MAXIMUM USEABLE VALUE FOR CYLINDER NUMBER
;	CL = MAXIMUM USEABLE VALUE FOR SECTOR NUMBER
;	     AND CYLINDER NUMBER HIGH BITS
;
;	IF READ DASD TYP WAS REQUESTED,
;
;	AH =0 - NOT PRESENT
;	    1 - DISKETTE - NO CHANGE LINE AVAILABLE (360 kByte Drive)
;	    2 - DISKETTE - CHANGE LINE AVAILABLE (720 KByte / 1.2 MByte Drive)
;	    3 - FIXED DISK
;	CX,DX = NUMBER OF 512 BYTE BLOCKS WHEN AH = 3
;
;	REGISTERS WILL BE PRSERVED EXEPT WHEN THEY ARE USED TO RETURN
;	INFORMATION.
;---------------------------------------------------------------------------

BAD_CMD 	EQU	01H
INIT_FAIL	EQU	07H
TIME_OUT	EQU	80H
DISK_CHANGE	EQU	06H

LASTD:	defb	0	;letztes angesprochenes Drive
NSECT:	defb	0	;Sektoren/Spur LW 0
	defb	0	;LW 1
	defb	0	;LW 2
	defb	0	;LW 3
	defb	0	;LW 4
	defb	0	;LW 5

;-----------------------------------------------------
;DISK_SETUP
;	THIS ROUTINE DOES A PRELIMINARY CHECK TO SEE
;	WHAT TYPE OF DISKETTE DRIVES ARE ATTACH TO THE
;	SYSTEM.
;-----------------------------------------------------
DISK_SETUP ;NEAR
	push	ds
	push	bx
	sub	ax,ax
	mov	ds,ax
	mov	cs:LASTD,al
	mov	cs:NSECT,ax
	mov	cs:NSECT+2,ax
	mov	cs:NSECT+4,ax
	mov	al,cs:hdtyp+0		;Type HD 0
	mov	bx,offset FD2_TBL	;20 MByte HD
	cmp	al,2	
	jz	DS1
	mov	bx,offset FD1_TBL	;10 MByte HD
	cmp	al,1
	jz	DS1
	mov	bx,offset FD_TBL	;5 MByte HD
DS1:	mov	HF_TBL_VEC,bx

	mov	al,cs:hdtyp+1		;Type HD 1
	mov	bx,offset FD2_TBL	;20 MByte HD
	cmp	al,2	
	jz	DS2
	mov	bx,offset FD1_TBL	;10 MByte HD
	cmp	al,1
	jz	DS2
	mov	bx,offset FD_TBL	;5 MByte HD
DS2:	mov	HF1_TBL_VEC,bx		;Drive 0

	CALL	DDS
	MOV	SEEK_STATUS,0
	pop	bx
	POP	DS
	RET

;---------------------------------------
;	DISK BIOS ENTRY POINT
;---------------------------------------
DISK ;PROC FAR
	STI			;ENABLE INTERRUPTS
	CMP	AH,08H		;GET PARAMETERS IS A SPECIAL CASE
	JNZ	DISK1
	JMP	GET_PARM

DISK1:	CMP	AH,15H		;READ DASD TYPE IS ALSO
	JNZ	DISK2
	JMP	DISK_TYPE

DISK2:	PUSH	BX		;SAVE REGISTERS DURING OPERATION
	PUSH	CX
	PUSH	DX
	PUSH	DS
	PUSH	SI
	CALL	DISK_CONT	;PERFORM THE OPERATON
	MOV	AH,DISKETTE_STATUS	;GET STATUS FROM OPERATION
	CMP	AH,1		;SET THE CARRY FLAG TO INDICATE
	CMC			;SUCCESS OR FAILURE
	POP	SI
	POP	DS
	POP	DX
	POP	CX
	POP	BX
	RET FAR 2		;THPROW AWAY SAVED FLAGS

M1:				;FUNCTION TRANSFER TABLE
	DW	RETURN_OK	;00H DISK RESET
	DW	READ_STATUS	;01H
	DW	DISK_READ	;02H
	DW	DISK_WRITE	;03H
	DW	RETURN_OK	;04H DISK VERIFY
	DW	RETURN_OK 	;05H FORMAT TRACK
	DW	BAD_COMD	;06H FORMAT BAD SECTORS
	DW	BAD_COMD	;07H FORMAT DRIVE
	DW	BAD_COMD	;08H RETURN PARMS
	DW	RETURN_OK	;09H INIT DRIVE
	DW	BAD_COMD 	;0AH READ LONG
	DW	BAD_COMD	;0BH WRITE LONG
	DW	RETURN_OK	;0CH SEEK
	DW	RETURN_OK	;0DH DISK RESET
	DW	BAD_COMD	;0EH READ BUFFER
	DW	BAD_COMD	;0FH WRITE BUFFER
	DW	RETURN_OK 	;10H TEST READY
	DW	RETURN_OK	;11H RECALIBRATE
	DW	BAD_COMD	;12H RAM DIAGNOSTIC
	DW	BAD_COMD	;13H DRIVE DIAGNOSTIC
	DW	RETURN_OK	;14H CONTROLLER DIAGNISTIC
	DW	BAD_COMD	;15H GET DISK TYPE
	DW	RDISK_CHANGE	;16H READ DISK CHANGE
	DW	RETURN_OK	;17H SET DISK TYPE
M1L:	EQU	$-M1

DISK_CONT ;PROC NEAR
	CALL	DDS
	CMP	AH,1
	JZ	READ_STATUS
	MOV	DISKETTE_STATUS,0
	PUSH	AX
	MOV	AL,AH			;GET INTO LOW BYTE
	XOR	AH,AH			;ZERO HIGH BYTE
	SAL	AX,1			;*2 FOR TABLE LOOKUP
	MOV	SI,AX			;PUT INTO SI FOR BRANCH
	CMP	AX,M1L			;TEST WITHIN RANGE
	JNB	BAD_COMD_P
	POP	AX
	JMP	CS:[SI]+M1

BAD_COMD_P:
	POP	AX
BAD_COMD:
	MOV	AL,0
	MOV	DISKETTE_STATUS,BAD_CMD
RETURN_OK:
	RET

;-------------------------------------------------
;READ STATUS (AH = 02H)
;-------------------------------------------------
READ_STATUS ;PROC NEAR
	MOV	AL,DISKETTE_STATUS	;OBTAIN REVIOUS STATUS
	RET

;-------------------------------------------------
;DISK READ ROUTINE (AH = 02H)
;-------------------------------------------------
DISK_READ ;PROC NEAR
	mov	ah,1
	jmp	short DRW0

;------------------------------------------------
;DISK WRITE ROUTINE (AH = 03H)
;------------------------------------------------
DISK_WRITE ;PROC NEAR
	mov	ah,2

DRW0:	push	cx
	push	dx
	push	ax
	push	bx
	mov	cs:LASTD,dl
	cmp	dl,80h			;HD ?
	jnc	DRW6
	push	ax
	sub	ax,ax
	mov	ds,ax
	pop	ax
	lds	si,DISK_POINTER
	cmp	si,522h			;Standard Parametervektor
	jnz	DRW1
	sub	bx,bx
	mov	bl,dl
	mov	bl,cs:NSECT[bx]		;Sektoren/Spur aus internem Merker
	or	bl,bl			;0= kein Eintrag
	jnz	DRW2

DRW1:	mov	bl,[si]+4		;Sektoren/Spur aus Parametervector
DRW2:	cmp	bl,0ffh			;cpmform und msform setzen diesen
	jnz	DRW4			;Wert auf ff, die Anzahl ergibt sich:
	sub	bx,bx
	mov	bl,dl
	mov	bl,cs:NSECT[bx]		;Sektoren/Spur aus internem Merker
	or	bl,bl			;0= kein Eintrag
	jnz	DRW4
	mov	bl,al			;Wert aus al

DRW4:	cmp	bl,8			;8*512 in 9*512 umwandeln
	jnz	DRW5
	inc	bl

DRW5:	mov	bh,[si]+3		;Sektorlaenge
	mov	al,2			;Anzahl der Koepfe stets 2
	jmp	short DRW7

DRW6:	call	GET_VEC
	mov	si,bx
	mov	bh,2			;Sektorlaenge 512 Byte
	mov	bl,[si]+14		;Anzahl der Sektoren
	mov	al,[si]+2		;Anzahl der Koepfe

DRW7:	cmp	dl,80H
	jc	DRW8
	sub	dl,7eh			;Drive 0,1 FD, 2,3 HD, 4-7 FD
DRW8:	mov	cs:diskrq+1,dl		;Drive

;Berechnung der Blocknummer:
;(((Cylinder * Anzahl Koepfe) + Kopfnummer) * Anzahl Sektoren) + Sektornummer-1
	push	dx
	mov	dh,cl
	shr	dh,1			;obere 2 Bit der Cylinder Nr.
	shr	dh,1
	shr	dh,1
	shr	dh,1
	shr	dh,1
	shr	dh,1
	mov	dl,ch
	sub	ah,ah
	mul	dx			;Cyl * Koepfe
	pop	dx
	mov	dl,dh			;Head
	sub	dh,dh
	add	ax,dx
	mov	dl,bl			;Anzahl der Sektoren
	sub	dh,dh
	mul	dx			;* Anzahl der Sektoren
	sub	ch,ch
	and	cl,3FH			;Sektor
	dec	cl
	add	ax,cx			;Ergebnis: Blocknummer
	mov	dx,ax

;Berechnung der Sektorlaenge
	mov	cx,80h
	or	bh,bh			;Sektorlaenge
	jz	DRW10			;128 Byte
DRW9:	add	cx,cx
	dec	bh
	jnz	DRW9
DRW10:	mov	ax,cx

;Berechnung des Byte-Offsets
;Offset = Blocknummer * Sektorlaenge
	mul	dx
	mov	cs:diskrq+2,dh
	mov	cs:diskrq+3,dl
	mov	cs:diskrq+4,ah
	mov	cs:diskrq+5,al

;Berechnung der Byteantahl
	pop	bx
	pop	ax
	mov	cs:diskrq+0,ah		;Request-Code
	sub	ah,ah			;al = Anzahl der Sektoren
	mul	cx			;* Sektorlaenge
	mov	cs:diskrq+10,ah
	mov	cs:diskrq+11,al

;Berechnung der physischen Adresse
	mov	ax,es
	mov	cl,12
	shr	ax,cl
	mov	dx,ax
	mov	ax,es
	mov	cl,4
	shl	ax,cl
	add	ax,bx
	jnc	DRW11
	inc	dx			;ES:BX changet to 000X:ax
DRW11:	mov	cs:diskrq+6,dh
	mov	cs:diskrq+7,dl
	mov	cs:diskrq+8,ah
	mov	cs:diskrq+9,al

	mov	word ptr cs:diskout+2,0c00h	;Anzahl der Bytes

DRW12:	sti
	nop
	cli
	mov	ah,cs:intout		;kann Int gegeben werden ?
	or	ah,ah
	jnz	DRW12
	mov	cs:intout,3		;Disk-Int
	out	0,al			;Int ausloesen
	sti
	call	DDS

;	MOV	AX,09001H	;LOAD WAIT CODE AND TYPE
;	INT	15H		;PERFORM OTHER FUNCTION
WT1:	TEST	SEEK_STATUS,80H	;TEST FOR INTERRUPT
	JZ	WT1
	MOV	SEEK_STATUS,0	;TURN OFF INTERRUPT FLAG

;internen Wert Sektoren/Spur aus Sektor 0/ Spur 0 ermitteln
	pop	dx
	pop	cx
	cmp	dl,6		;Floppy ?
	jnc	WT4
	or	ch,dh		;Spur 0 / Head 0 ?
	jnz	WT4
	cmp	cl,1		;Sektor 1
	jnz	WT4
	sub	dh,dh
	mov	cl,es:[bx]+21	;Format-ID Byte
	cmp	cl,0f9h
	jc	WT2		;keine DOS-Diskette
	mov	cl,es:[bx]+24	;Anzahl Sektoren/Track
	cmp	cl,8
	jz	WT3
	cmp	cl,9
	jz	WT3
	cmp	cl,15
	jz	WT3
WT2:	sub	cl,cl		;spezielles Format
WT3:	mov	bx,dx
	mov	cs:NSECT[bx],cl
WT4:	ret

;-------------------------------------------------------------
;DISK_INT
;	THIS ROUTINE HANDLES THE DISKETTE INTERRUPT
;INPUT
;	NONE
;OUTPUT
;	THE INTERRUPT FLAG IS SET IS SEEK_STATUS
;-------------------------------------------------------------
DISKINT ;PROC FAR		;>>> ENTRY POINT FOR ORG 0EF57H
	PUSH	DS		;SAVE REGISTERS
	PUSH	AX
	CALL	DDS		;SETUP DATA ADDRESSING
	mov	cs:intin,0	;weitere Ints erlauben
	mov	al,cs:diskin+2
	mov	DISKETTE_STATUS,al
	mov	cs:diskin+1,0	;weiteren Disk-Int erlauben
	sti
	MOV	SEEK_STATUS,0FFH ;TURN ON INTERRUPT OCCURRED
;	MOV	AX,09101H	;INTERRUPT OTHER TASK
;	INT	15H		;GO PERFORM OTHER TASK
	POP	AX		;RECOVER REG
	POP	DS
	IRET

;---------------------------------------------
;	GET PARAMETERS	     (AH = 8)
;---------------------------------------------
GET_PARM ;NEAR			;GET DRIVE PARAMETERS
	PUSH	DS
	PUSH	BX
	CALL	DDS
	MOV	DISKETTE_STATUS,0
	cmp	dl,80h
	jc	G3

	AND	DL,7FH		;Hard-Disk
	CMP	DL,2		;TEST WITHIN RANGE
	JAE	G4

	call	GET_VEC
	MOV	AX,[BX]		;MAX NUMBER OF CYLINDERS
	dec	ax
	MOV	CH,AL
	AND	AX,0300H	;HIGH TWO BITS OF CYL
	SHR	AX,1
	SHR	AX,1
	OR	AL,[BX]+14	;SECTORS
	MOV	CL,AL
	MOV	DH,[BX]+2	;HEADS
	DEC	DH
	MOV	DL,2		;DRIVE COUNT

G1:	SUB	AX,AX
G2:	POP	BX		;RESTORE REGISTERS
	POP	DS
	RET FAR 2

G3:	cmp	dl,2
	jnc	G4
	mov	cl,cs:DISK_BASE+4	;letzte Sektor-Nr.
	mov	ch,79		;letzte Track-Nr.
	mov	dh,1		;letzte Kopfnummer
	mov	dl,2		;Anzahl der Laufwerke
	jmp	short G1

G4:	sub	ax,ax
	mov	cx,ax
	mov	dx,ax
	mov	ah,INIT_FAIL
	mov	DISKETTE_STATUS,ah
	stc			;Set Error Flag
	jmp	short G2

;------------------------------------------------
;READ DASD TYPE		(AH = 15H)
;------------------------------------------------
DISK_TYPE ;PROC NEAR
	push	ds
	call	DDS
	mov	DISKETTE_STATUS,0
	cmp	dl,80h
	jnc	DT2
	cmp	dl,2
	jnc	DT3
	mov	ah,1			;CHANGE LINE not available
DT1:	sub	al,al
	pop	ds
	ret far 2

DT2:	push	bx
	and	dl,7fh
	cmp	dl,2			;Test drive number
	jnc	DT3
	CALL	GET_VEC 		;GET DISK PARM ADDRESS
	MOV	AL,[BX]+2		;HEADS
	MOV	CL,[BX]+14
	IMUL	CL			;*NUMBER OF SECTORS
	MOV	CX,[BX]			;MAX NUMBER OF CYLINDERS
	IMUL	CX			;NUMBER OF SECTORS
	MOV	CX,DX			;HIGH ORDER HALF
	MOV	DX,AX			;LOW ORDER HALF
	mov	ah,3			;Hard Disk
	pop	bx
	jmp	short DT1

DT3:	sub	ax,ax
	mov	cx,ax
	mov	dx,ax
	jmp	short DT1


;---------------------------------------
;SET UP DS:BX -> DISK PARMS
;---------------------------------------
GET_VEC ;PROC	NEAR
	SUB	AX,AX
	MOV	DS,AX
	LDS	BX,HF_TBL_VEC	;DS:BX -> DRIVE PARAMETERS
	TEST	DL,1
	JZ	GV
	mov	ds,ax
	LDS	BX,HF1_TBL_VEC	;DS:BX -> DRIVE PARAMETERS
GV:	RET

;------------------------------------------------
;READ DISK CHANGE (AH = 16H)
;------------------------------------------------
RDISK_CHANGE ;PROC NEAR
	cmp	dl,cs:LASTD
	jz	RC1
	mov	DISKETTE_STATUS,DISK_CHANGE
	mov	cs:LASTD,dl
RC1:	ret

;-----------------------
;SEEK
;-----------------------
SEEK ;PROC NEAR
	clc			;Clear CY
	ret			;RETURN TO CALLER

	END
