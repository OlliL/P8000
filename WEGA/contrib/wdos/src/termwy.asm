;**************************************************************************
;**************************************************************************
;
;	WDOS-BIOS - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
;	Programm: term.asm
;
;	Bearbeiter	: P. Hoge
;	Datum		: 8.3.90
;	Version		: 1.1
;
;**************************************************************************
;
;	Wyse-Terminal (WY-99GT)
;
;**************************************************************************
;**************************************************************************


.EXTERNAL DDS ;NEAR
.EXTERNAL START ;NEAR
.EXTERNAL BYTE BIOS_BREAK
.EXTERNAL WORD RESET_FLAG
.EXTERNAL BYTE KB_FLAG
.EXTERNAL BYTE KB_FLAG_1
.EXTERNAL BYTE KB_FLAG_2
.EXTERNAL BYTE ALT_INPUT
.EXTERNAL WORD BUFFER_START
.EXTERNAL WORD BUFFER_END
.EXTERNAL WORD BUFFER_HEAD
.EXTERNAL WORD BUFFER_TAIL
.EXTERNAL WORD CURSOR_POSN
.EXTERNAL WORD CURSOR_MODE
.EXTERNAL BYTE CRT_MODE
.EXTERNAL WORD CRT_COLS
.EXTERNAL WORD CRT_LEN
.external word ADDR_6845
.external byte reset
.external byte intin
.external byte intout
.external byte ttyin
.external byte ttyout
.external byte lpout
.external byte ttybuf
.external byte lpbuf

.PUBLIC	PUTA
.PUBLIC	KBOARD
.PUBLIC	KBINT
.PUBLIC	VIDEO
.PUBLIC	PRINTER
.PUBLIC	RS232

;---- INT 16 ------------------------------------------------------------------
;KEYBOARD I/O
;	THESE ROUTINES PROVIDE KEYBOARD	SUPPORT
;INPUT
;	(AH)=0	READ THE NEXT ASCII CHARACTER STRUCK FROM THE KEYBOARD
;		RETURN THE RESULT IN (AL), SCAN	CODE IN	(AH)
;	(AH)=1	SET THE	Z FLAG TO INDICATE IF AN ASCII CHARACTER IS AVAILABLE
;		(ZF)=1 -- NO CODE AVAILABLE
;		(ZF)=0 -- CODE IS AVAILABLE
;		IF ZF =	0, THE NEXT CHARACTER IN THE BUFFER TO BE READ IS
;		IN AX, AND THE ENTRY REMAINS IN	THE BUFFER
;	(AH)=2	RETURN THE CURRENT SHIFT STATUS	IN AL REGISTER
;		THE BIT	SETTINGS FOR THIS CODE ARE INDICATED IN	THE
;		THE EQUATES FOR	KB_FLAG
;	(AH)=5	Extended Write CX to Input-Buffer
;	(AH)=10	Extended ah=0 function
;	(AH)=11 Extended ah=1 function
;	(AH)=12 Extended ah=2 function
;OUTPUT
;	AS NOTED ABOVE,	ONLY AX	AND FLAGS CHANGED
;	ALL REGISTERS RETAINED
;------------------------------------------------------------------------------
KBCNT:	defb	0

KBOARD ;PROC FAR		;>>> ENTRY POINT FOR ORG 0E82EH
	sti			;INTERRUPTS BACK ON
	push	ds		;SAVE CURRENT DS
	push	bx		;SAVE BX TEMPORARILY
	call	DDS		;ESTABLISH POINTER TO DATA REGION
	cmp	ah,5
	jz	K3		;Write to keybord buffer
	cmp	ah,10h
	jz	K0		;Extended keybord read
	cmp	ah,11h
	jz	K1		;Get keystroke status
	cmp	ah,12h
	jz	K2e		;Get extended shift status
	or	ah,ah		;AH=0
	jz	K0		;ASCII_READ
	dec	ah		;AH=1
	jz	K1		;ASCII_STATUS
	dec	ah		;AH=2
	jz	K2		;SHIFT_STATUS
	pop	bx
	pop	ds
	iret			;INVALID COMMAND

;------	SHIFT STATUS

K2e:	mov	ah,KB_FLAG_2	;extended shift status
K2:	mov	al,KB_FLAG	;GET THE SHIFT STATUS FLAGS
	pop	bx
	pop	ds
	iret

;----- write cx to keyboard buffer (extended function)

K3:	push	si
	mov	bx,BUFFER_TAIL	;GET THE END POINTER TO	THE BUFFER
	mov	si,bx		;SAVE THE VALUE
	call	K4		;ADVANCE THE TAIL
	cmp	bx,BUFFER_HEAD	;HAS THE BUFFER	WRAPPED	AROUND
	je	K3a		;Buffer full
	mov	[si],cx		;STORE THE VALUE
	mov	BUFFER_TAIL,bx	;MOVE THE POINTER UP
K3a:	pop	si
	pop	bx
	pop	ds
	iret

;------	READ THE KEY TO	FIGURE OUT WHAT	TO DO

K0:	push	cx
	mov	cx,ax
K0A:				;read loop
	sti
	call	SCREEN_OUT	;"Bildwiederholspeicher" ausgeben

;	MOV	BX,BUFFER_HEAD	;GET POINTER TO	HEAD OF	BUFFER
;	CMP	BX,BUFFER_TAIL	;TEST END OF BUFFER
;	JNE	K0B		;IF ANYTHING IN	BUFFER DONT DO INTERRUPT
;	MOV	AX,09002H	;MOVE IN WAIT CODE & TYPE
;	INT	15H		;PERFORM OTHER FUNCTION

K0B:	CLI			;INTERRUPTS BACK OFF
	MOV	BX,BUFFER_HEAD	;GET POINTER TO	HEAD OF	BUFFER
	CMP	BX,BUFFER_TAIL	;TEST END OF BUFFER
	JZ	K0A		;LOOP UNTIL SOMETHING IN BUFFER

	MOV	AX,[BX]		;GET SCAN CODE AND ASCII CODE
	call	KTRANS
	jz	K0A		;discard Output
	CALL	K4		;MOVE POINTER TO NEXT POSITION
K0C:	MOV	BUFFER_HEAD,BX	;STORE VALUE IN	VARIABLE
	pop	cx
	POP	BX
	POP	DS
	IRET

;------	ASCII STATUS

K1:	push	cx
	mov	cx,ax
	mov	bl,cs:TTYFLG1	;war TTY_WRITE ?
	or	bl,bl
	jnz	K1a
	mov	bl,cs:KBCNT
	inc	cs:KBCNT
	cmp	bl,10
	jnz	K1b
K1a:	call	SCREEN_OUT
	mov	cs:KBCNT,0

K1b:	CLI			;INTERRUPTS OFF
K1c:	MOV	BX,BUFFER_HEAD	;GET HEAD POINTER
	CMP	BX,BUFFER_TAIL	;IF EQUAL (Z=1)	THEN NOTHING THERE
	MOV	AX,[BX]
	jz	K1d
	call	KTRANS
	jnz	K1d
	call	K4		;kein gueltiges Zeichen
	mov	BUFFER_HEAD,bx
	jmp short K1c

K1d:	pop	cx
	POP	BX
	POP	DS
	STI
	RET FAR	2

;Translate Code

KTRANS:	cmp	ch,3		;extended Read ?
	jc	KT1		,no
	cmp	al,0f0h		;xxf0 -> xx00
	jnz	KT0
	xor	al,al
KT0:	cmp	ax,-1
	ret

;Extended Code in Normalcode umwandeln

KT1:	or	ah,ah		;wenn ah = 0, kein translate
	jz	KT0
	cmp	ah,0e0h
	jnz	KT4
	cmp	al,2fh		;wenn e02f -> 352f
	jnz	KT2
	mov	ax,352fh
	jmp short KT0
KT2:	cmp	al,0dh		;wenn e00d -> 1c0d
	jnz	KT3
	mov	ax,1c0dh
	jmp short KT0
KT3:	cmp	al,0ah		;wenn e00a -> 1c0a
	jnz	KT4
	mov	ax,1c0ah
	jmp short KT0
KT4:	cmp	ah,85H		;wenn ah > 84 discard output
	jnc	KT5
	cmp	al,0f0h		;wenn al = f0 discard output
	jz	KT5
	cmp	al,0e0h		;wenn al = e0 -> al = 0
	jnz	KT0
	xor	al,al
	jmp short KT0

KT5:	mov	ax,-1		;discard Output
	jmp short KT0

;------	INCREMENT A BUFFER POINTER

K4 ;PROC NEAR
	INC	BX		;MOVE TO NEXT WORD IN LIST
	INC	BX
	CMP	BX,BUFFER_END	;AT END	OF BUFFER?
	JNE	K4A		;NO, CONTINUE
	MOV	BX,BUFFER_START	;YES, RESET TO BUFFER BEGINNING
K4A:	RET


;---- Tastaturcode
;---- Normal-UK (lower case)

TNGER:	defw	ffffh, 011bh, 0231h, 0332h, 0433h, 0534h, 0635h, 0736h
	defw	0837h, 0938h, 0a39h, 0b30h, 0c2dh, 0d3dh, 0e08h, 0f09h
	defw	1071h, 1177h, 1265h, 1372h, 1474h, 1579h, 1675h, 1769h
	defw	186fh, 1970h, 1a5bh, 1b5dh, 1c0dh, ffffh, 1e61h, 1f73h
	defw	2064h, 2166h, 2267h, 2368h, 246ah, 256bh, 266ch, 273bh
	defw	2827h, 2960h, ffffh, 2b23h, 2c7ah, 2d78h, 2e63h, 2f76h
	defw	3062h, 316eh, 326dh, 332ch, 342eh, 352fh, ffffh, 372ah
	defw	ffffh, 3920h, ffffh, 3b00h, 3c00h, 3d00h, 3e00h, 3f00h
	defw	4000h, 4100h, 4200h, 4300h, 4400h, ffffh, ffffh, 47e0h
	defw	48e0h, 49e0h, 4a2dh, 4be0h, 4cf0h, 4de0h, 4e2bh, 4fe0h
	defw	50e0h, 51e0h, 52e0h, 53e0h, ffffh, ffffh, 565ch, 8500h
	defw	8600h
	defw	1177h, 1372h
	defw	e02fh, e00dh

;---- Shift-UK

TSGER:	defw	ffffh, 011bh, 0221h, 0322h, 049ch, 0524h, 0625h, 075eh
	defw	0826h, 092ah, 0a28h, 0b29h, 0c5fh, 0d2bh, 0e08h, 0f00h
	defw	1051h, 1157h, 1245h, 1352h, 1454h, 1559h, 1655h, 1749h
	defw	184fh, 1950h, 1a7bh, 1b7dh, 1c0dh, ffffh, 1e41h, 1f53h
	defw	2044h, 2146h, 2247h, 2348h, 244ah, 254bh, 264ch, 273ah
	defw	2840h, 297eh, ffffh, 2b7eh, 2c5ah, 2d58h, 2e43h, 2f56h
	defw	3042h, 314eh, 324dh, 333ch, 343eh, 353fh, ffffh, 372ah
	defw	ffffh, 3920h, ffffh, 5400h, 5500h, 5600h, 5700h, 5800h
	defw	5900h, 5a00h, 5b00h, 5c00h, 5d00h, ffffh, ffffh, 4737h
	defw	4838h, 4939h, 4a2dh, 4b34h, 4c35h, 4d36h, 4e2bh, 4f31h
	defw	5032h, 5133h, 5230h, 532eh, ffffh, ffffh, 567ch, 8700h
	defw	8800h
	defw	1157h, 1352h
	defw	e02fh, e00dh

;---- Normal-ASCII (lower case)

TNASC:	defw	ffffh, 011bh, 0231h, 0332h, 0433h, 0534h, 0635h, 0736h
	defw	0837h, 0938h, 0a39h, 0b30h, 0c2dh, 0d3dh, 0e08h, 0f09h
	defw	1071h, 1177h, 1265h, 1372h, 1474h, 1579h, 1675h, 1769h
	defw	186fh, 1970h, 1a5bh, 1b5dh, 1c0dh, ffffh, 1e61h, 1f73h
	defw	2064h, 2166h, 2267h, 2368h, 246ah, 256bh, 266ch, 273bh
	defw	2827h, 2960h, ffffh, 2b5ch, 2c7ah, 2d78h, 2e63h, 2f76h
	defw	3062h, 316eh, 326dh, 332ch, 342eh, 352fh, ffffh, 372ah
	defw	ffffh, 3920h, ffffh, 3b00h, 3c00h, 3d00h, 3e00h, 3f00h
	defw	4000h, 4100h, 4200h, 4300h, 4400h, ffffh, ffffh, 47e0h
	defw	48e0h, 49e0h, 4a2dh, 4be0h, 4cf0h, 4de0h, 4e2bh, 4fe0h
	defw	50e0h, 51e0h, 52e0h, 53e0h, ffffh, ffffh, ffffh, 8500h
	defw	8600h
	defw	1177h, 1372h
	defw	e02fh, e00dh

;---- Shift-ASCII

TSASC:	defw	ffffh, 011bh, 0221h, 0340h, 0423h, 0524h, 0625h, 075eh
	defw	0826h, 092ah, 0a28h, 0b29h, 0c5fh, 0d2bh, 0e08h, 0f00h
	defw	1051h, 1157h, 1245h, 1352h, 1454h, 1559h, 1655h, 1749h
	defw	184fh, 1950h, 1a7bh, 1b7dh, 1c0dh, ffffh, 1e41h, 1f53h
	defw	2044h, 2146h, 2247h, 2348h, 244ah, 254bh, 264ch, 273ah
	defw	2822h, 297eh, ffffh, 2b7ch, 2c5ah, 2d58h, 2e43h, 2f56h
	defw	3042h, 314eh, 324dh, 333ch, 343eh, 353fh, ffffh, 372ah
	defw	ffffh, 3920h, ffffh, 5400h, 5500h, 5600h, 5700h, 5800h
	defw	5900h, 5a00h, 5b00h, 5c00h, 5d00h, ffffh, ffffh, 4737h
	defw	4838h, 4939h, 4a2dh, 4b34h, 4c35h, 4d36h, 4e2bh, 4f31h
	defw	5032h, 5133h, 5230h, 532eh, ffffh, ffffh, ffffh, 8700h
	defw	8800h
	defw	1157h, 1352h
	defw	e02fh, e00dh

;---- Control-ASCII

TCASC:	defw	ffffh, 011bh, ffffh, 0300h, ffffh, ffffh, ffffh, 071eh
	defw	081bh, 091bh, 0a1dh, 0b1dh, 0c1ch, 0d00h, 0e7fh, 94f0h
	defw	1011h, 1117h, 1205h, 1312h, 1414h, 151ah, 1615h, 1709h
	defw	180fh, 1910h, 1a1bh, 1b1dh, 1c0ah, ffffh, 1e01h, 1f13h
	defw	2004h, 2106h, 2207h, 2308h, 240ah, 250bh, 260ch, ffffh
	defw	ffffh, 291eh, ffffh, ffffh, 2c19h, 2d18h, 2e03h, 2f16h
	defw	3002h, 310eh, 320dh, ffffh, ffffh, 351fh, ffffh, 9600h
	defw	ffffh, 3920h, ffffh, 5e00h, 5f00h, 6000h, 6100h, 6200h
	defw	6300h, 6400h, 6500h, 6600h, 6700h, ffffh, ffffh, 77e0h
	defw	8de0h, 84e0h, 8e00h, 73e0h, 8f00h, 74e0h, 9000h, 75e0h
	defw	91e0h, 76e0h, 92e0h, 93e0h, ffffh, ffffh, ffffh, 8900h
	defw	8a00h
	defw	1117h, 1312h
	defw	9500h, e00ah

;---- left Alt-ASCII

TAASC:	defw	4ef0h, 01f0h, 7800h, 7900h, 7a00h, 7b00h, 7c00h, 7d00h
	defw	7e00h, 7f00h, 8000h, 8100h, 0cf0h, 0df0h, 0ef0h, a500h
	defw	1000h, 1100h, 1200h, 1300h, 1400h, 1500h, 1600h, 1700h
	defw	1800h, 1900h, 1af0h, 1bf0h, 1cf0h, ffffh, 1e00h, 1f00h
	defw	2000h, 2100h, 2200h, 2300h, 2400h, 2500h, 2600h, 27f0h
	defw	28f0h, 29f0h, ffffh, 2bf0h, 2c00h, 2d00h, 2e00h, 2f00h
	defw	3000h, 3100h, 3200h, 33f0h, 34f0h, 35f0h, ffffh, 37f0h
	defw	ffffh, 3920h, ffffh, 6800h, 6900h, 6a00h, 6b00h, 6c00h
	defw	6d00h, 6e00h, 6f00h, 7000h, 7100h, ffffh, ffffh, 9700h
	defw	9800h, 9900h, 4af0h, 9b00h, ffffh, 9d00h, 4ef0h, 9f00h
	defw	a000h, a100h, a200h, a300h, ffffh, ffffh, 56f0h, 8b00h
	defw	8c00h
	defw	1100h, 1300h
	defw	a400h, a600h

;---- right Alt-ASCII

TARASC:	defw	4e2bh, 011bh, ffffh, 03fdh, 04fch, ffffh, ffffh, ffffh
	defw	087bh, 095bh, 0a5dh, 0b7dh, 0c5ch, ffffh, ffffh, ffffh
	defw	1040h, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh
	defw	ffffh, ffffh, ffffh, 1b7eh, 1c0dh, ffffh, ffffh, ffffh
	defw	ffffh, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh
	defw	ffffh, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh, ffffh
	defw	ffffh, ffffh, 32e6h, ffffh, ffffh, ffffh, ffffh, 372ah
	defw	ffffh, 3920h, ffffh, 3b00h, 3c00h, 3d00h, 3e00h, 3f00h
	defw	4000h, 4100h, 4200h, 4300h, 4400h, ffffh, ffffh, 4737h
	defw	4838h, 4939h, 4a2dh, 4b34h, 4c35h, 4d36h, 4e3dh, 4f31h
	defw	5032h, 5133h, 5230h, 532eh, ffffh, ffffh, 567ch, 8500h
	defw	8600h
	defw	ffffh, ffffh
	defw	e02fh, e00dh


;------	KEYBOARD INTERRUPT ROUTINE

KBLAST:	defb	0

KBINT ;PROC FAR
	sti
	db	60h		;pusha
	push	ds
	push	es
	push	cs
	pop	es
	cld			;vorwaerts
	call	DDS
	mov	cs:intin,0	;naechsten Int freigeben
	mov	al,cs:ttyin+2
	mov	ah,al
	mov	dl,cs:KBLAST
	and	dl,0feh
	mov	cs:KBLAST,al

;------	TEST FOR OVERRUN SCAN CODE FROM	KEYBOARD

	cmp	al,0ffh		;IS THIS AN OVERRUN CHAR
	jnz	TESTE0
	jmp	BELL		;Bell

;------- Test for e0 1c and e0 35

TESTE0:	and	al,7fh
	cmp	dl,0e0h
	jnz	SYSKEY
	cmp	al,35h
	jnz	TEST1
	mov	al,5bh
	jmp 	BREAK_CODE
TEST1:	cmp	al,1ch
	jnz	SYSKEY
	mov	al,5ch
	jmp	BREAK_CODE


;------- TEST FOR SYSTEM KEY

SYSKEY:	cmp	al,54h		;IS IT THE SYSTEM KEY
	jnz	SHIFTKEY

	mov	cs:ttyin+1,0	;Tastatur Int enable
	test	ah,80h		;CHECK IF THIS A BREAK CODE
	jnz	SYS_BREAK	;DONT TOUCH SYSTEM INDICATOR IF	TRUE

	test	KB_FLAG_1,04h	;SEE IF	IN SYSTEM KEY HELD DOWN
	jnz	KEND0		;IF YES, DONT PROCESS SYSTEM INDICATOR

	or	KB_FLAG_1,04h	;INDICATE SYSTEM KEY DEPRESSED
	or	KB_FLAG_2,80h
	mov	ax,8500h	;FUNCTION VALUE	FOR MAKE OF SYSTEM KEY
	int	15h		;USER INTERRUPT
	jmp short KEND0		;END PROCESSING

SYS_BREAK:
	and	KB_FLAG_1,0fdh	;TURN OFF SHIFT	KEY HELD DOWN
	and	KB_FLAG_2,7fh
	mov	cs:ttyin+1,0	;Tastatur Int enable
	mov	ax,08501H	;FUNCTION VALUE	FOR BREAK OF SYSTEM KEY
	int	15h		;USER INTERRUPT
KEND0:	jmp	KEND1		;IGNORE	SYSTEM KEY

;------	TEST FOR SHIFT KEYS

SHIFTKEY:
	sub	bx,bx
	cmp	al,2ah		;left shift
	jnz	RSHIFT
	mov	bl,02h
	jmp short SET_SHIFT

RSHIFT:	cmp	al,36h		;right shift
	jnz	CSHIFT
	mov	bl,01h
	jmp short SET_SHIFT

CSHIFT:	cmp	al,1dh		;control
	jnz	ASHIFT
	mov	bl,04h
	cmp	dl,0e0h	
	jz	RCSHIFT
	mov	bh,01h		;left control
	jmp short SET_SHIFT

RCSHIFT:
	mov	bh,04h		;right control
	jmp short SET_SHIFT

ASHIFT:	cmp	al,38h		;alt
	jnz	SHIFT_TOGGLE
	mov	bl,08h
	cmp	dl,0e0h
	jz	RASHIFT
	mov	bh,02h		;left alt
	jmp short SET_SHIFT

RASHIFT:
	mov	bh,08h		;right alt

SET_SHIFT:
	test	ah,80h		;break code ?
	jnz	RES_SHIFT

	or	KB_FLAG,bl	;set shift
	or	KB_FLAG_2,bh
KEND3:	jmp	KEND

RES_SHIFT:
	not	bx
	and	KB_FLAG,bl	;res shift
	and	KB_FLAG_2,bh

	cmp	al,38h		;IS THIS ALTERNATE SHIFT RELEASE
	jne	KEND3

;------	ALTERNATE SHIFT	KEY RELEASED, GET THE VALUE INTO BUFFER

	mov	al,ALT_INPUT
	sub	ah,ah		;SCAN CODE OF 0
	mov	ALT_INPUT,ah	;ZERO OUT THE FIELD
	or	al,al		;WAS THE INPUT=0
	jz	KEND3		;INTERRUPT_RETURN
	jmp	BFILL		;IT WASNT, SO PUT IN BUFFER

;------	TOGGLED	SHIFT KEY, TEST	FOR IST	MAKE OR	NOT

SHIFT_TOGGLE:
	test	KB_FLAG,04h	;CHECK CTL SHIFT STATE
	jnz	BREAK_CODE	;JUMP IF CTL STATE

	cmp	al,52h		;CHECK FOR INSERT KEY
	jnz	CAPS_LOCK	;JUMP IF NOT INSERT KEY

	test	KB_FLAG,08h	;CHECK FOR ALTERNATE SHIFT
	jnz	BREAK_CODE	;JUMP IF ALTERNATE SHIFT

	test	KB_FLAG,20h	;CHECK FOR NUM STATE
	jnz	INS1		;JUMP IF NUM LOCK IS ON
	test	KB_FLAG,03h	;LEFT OR RIGHT SHIFT ?
	jnz	BREAK_CODE	;no NUM_LOCK
	jmp short INS2

INS1:				;MIGHT BE NUMERIC
	test	KB_FLAG,03h	;LEFT OR RIGHT SHIFT ?
	jz	BREAK_CODE	;JUMP NUMERIC, NOT INSERT

INS2:				;Insert is tru
	mov	bx,0080h
	jmp short SET_TOGGLE

CAPS_LOCK:
	cmp	al,3ah
	jnz	NUM_LOCK
	mov	bx,4040h
	jmp short SET_TOGGLE

NUM_LOCK:
	cmp	al,45h
	jnz	SCROLL_LOCK
	mov	bx,2020h
	jmp short SET_TOGGLE

SCROLL_LOCK:
	cmp	al,46h
	jnz	BREAK_CODE
	mov	bx,1010h

SET_TOGGLE:
	test	ah,80h
	jnz	RES_TOGGLE

	test	bl,KB_FLAG_1	;IS KEY	ALREADY DEPRESSED
	jnz	KEND		;JUMP IF KEY ALREADY DEPRESSED
	or	KB_FLAG_1,bl	;INDICATE THAT THE KEY IS DEPRESSED
	or	KB_FLAG_2,bh
	xor	KB_FLAG,bl	;TOGGLE	THE SHIFT STATE
	cmp	al,52h		;TEST FOR IST MAKE OF INSERT KEY
	jne	KEND		;JUMP IF NOT INSERT KEY
	jmp short BREAK_CODE

RES_TOGGLE:
	not	bx		;INVERT	MASK
	and	KB_FLAG_1,bl	;INDICATE NO LONGEST DEPRESSED
	and	KB_FLAG_2,bh
	jmp	short KEND


;------ No shift found, test for break code

BREAK_CODE:
	cmp	ah,80h		;TEST FOR BREAK	KEY
	jae	KEND		;NOTHING FOR BREAK CHARS FROM HERE ON

;------	TEST FOR HOLD STATE

	test	KB_FLAG_1,08h	;ARE WE	IN HOLD	STATE
	jz	ALTSHIFT	;BRANCH	AROUND TEST IF NOT
	cmp	al,45h		;NUM_KEY
	je	KEND		;CANT END HOLD ON NUM LOCK
	and	KB_FLAG_1,0f7h ;TURN OFF THE HOLD STATE BIT

KEND:	cli			;INTERRUPT-RETURN
	mov	cs:ttyin+1,0	;Tastatur Int enable
KEND1:
	pop	es		;RESTORE REGISTERS
	pop	ds
	db	61h		;popa
	iret

;------	ALT-INPUT-TABLE

K30:	DB	82,79,80,81,75,76,77
	DB	71,72,73	;10 NUMBERS ON KEYPAD

;------	NOT IN HOLD STATE

ALTSHIFT:
	test	KB_FLAG,08h	;ARE WE	IN ALTERNATE SHIFT
	jz	CTLSHIFT	;JUMP IF NOT ALTERNATE

	test	KB_FLAG_1,01h	;German ?
	jnz	ALTS0		;no
	test	KB_FLAG_2,08h	;right alt ?
	jz	ALTS0
TRANS3:	mov	bx,offset TARASC ;right ALT-Code Table
	jmp	TRANS

;------	TEST FOR RESET KEY SEQUENCE (CTL ALT DEL)

ALTS0:	test	KB_FLAG,04h	;ARE WE	IN CONTROL SHIFT ALSO
	jz	NUM
	cmp	al,53h		;SHIFT STATE IS	THERE, TEST KEY
	jne	NO_RESET

;------	CTL-ALT-DEL HAS	BEEN FOUND, DO I/O CLEANUP

	cli
	mov	cs:ttyin+1,0	;Tastatur Int enable
	mov	RESET_FLAG,1234h ;SET FLAG FOR RESET FUNCTION
	jmp	START		;JUMP TO POWER ON DIAGNOSTICS

;------ CTL-ALT-F1 ASCII

NO_RESET:
	cmp	al,3bh
	jnz	GERMAN
	or	KB_FLAG_1,01h
	jmp	KEND

;------ CTL-ALT-F2 German

GERMAN:	cmp	al,3ch
	jnz	ALT1
	and	KB_FLAG_1,0feh
	jmp	KEND

;------ Rest von CTL-ALT wie ALT1 behandeln

ALT1:	test	KB_FLAG_1,01h	;German ?
	jz	TRANS3		;yes

;------	IN ALTERNATE SHIFT
;------	LOOK FOR KEY PAD ENTRY

NUM:	cmp	dl,0e0h
	jz	ALTS1
	mov	di,offset K30	;ALT-INPUT-TABLE
	mov	cx,10		;LOOK FOR ENTRY	USING KEYPAD
	repne	scasb		;LOOK FOR MATCH
	jne	ALTS1		;NO_ALT-KEYPAD

	sub	di,offset K30+1	;DI NOW	HAS ENTRY VALUE
	mov	al,ALT_INPUT	;GET THE CURRENT BYTE
	mov	ah,10		;MULTIPLY BY 10
	mul	ah
	add	ax,di		;ADD IN	THE LATEST ENTRY
	mov	ALT_INPUT,al	;STORE IT AWAY
	jmp	KEND		;THROW AWAY THAT KEYSTROKE

;------	LOOK FOR ALT CODE

ALTS1:	mov	ALT_INPUT,0	;ZERO ANY PREVIOUS ENTRY INTO INPUT
	mov	bx,offset TAASC	;left ALT-Code Table
ALTS2:	jmp	TRANS

;------	NOT IN ALTERNATE SHIFT

CTLSHIFT:
	test	KB_FLAG,04h	;ARE WE	IN CONTROL SHIFT
	jz	NO_CTL		;NOT-CTL-SHIFT

;------	CONTROL	SHIFT, TEST SPECIAL CHARACTERS
;------	TEST FOR BREAK (CTL SCROLL) AND PAUSE KEYS

	cmp	al,46h		;TEST FOR BREAK
	jne	NO_BREAK
	mov	bx,BUFFER_START	;RESET BUFFER TO EMPTY
	mov	BUFFER_HEAD,bx
	mov	BUFFER_TAIL,bx
	mov	BIOS_BREAK,80h	;TURN ON BIOS_BREAK BIT
	int	1bh		;BREAK INTERRUPT VECTOR
	sub	ax,ax		;PUT OUT DUMMY CHARACTER
	jmp	BFILL		;BUFFER_FILL

NO_BREAK:
	cmp	al,45h		;LOOK FOR PAUSE	KEY
	jne	NO_PAUSE

	or	KB_FLAG_1,08h	;TURN ON THE HOLD FLAG
	mov	cs:ttyin+1,0	;Tastatur Int enable

;------	PAUSE INTERVAL

PAUSE:	test	KB_FLAG_1,08h
	jnz	PAUSE		;LOOP UNTIL FLAG TURNED	OFF
	jmp	KEND1		;INTERRUPT_RETURN_NO_EOI

;------	TEST SPECIAL CASE KEY E0 55

NO_PAUSE:
	cmp	al,37h
	jne	NO_PRINT	;NOT-KEY-55
	cmp	dl,0e0h
	jne	NO_PRINT
	mov	ax,114*256	;START/STOP PRINTING SWITCH
	jmp	BFILL		;BUFFER FILL

;------	SET UP TO TRANSLATE CONTROL SHIFT

NO_PRINT:
	mov	bx,offset TCASC
	jmp	TRANS


;------	NOT IN CONTROL SHIFT

NO_CTL: cmp	dl,0e0h
	jnz	NO_E0

;------	HANDLE SPECIAL CASES

	cmp	al,37h		;PRINT SCREEN KEY
	jne	NO_SHIFT

;------	ISSUE INTERRUPT	TO INDICATE PRINT SCREEN FUNCTION

	mov	cs:ttyin+1,0	;Tastatur Int enable
	int	5h		;ISSUE PRINT SCREEN INTERRUPT
	jmp	KEND1		;GO BACK WITHOUT EOI OCCURRING


NO_E0:	mov	bx,offset TSGER	;German upper case table
	test	KB_FLAG_1,01h
	jz	KPAD
	mov	bx,offset TSASC	;ASCII upper case table
KPAD:	cmp	al,47h		;TEST FOR KEYPAD REGION
	jc	NO_KPAD

;------	KEYPAD KEYS, MUST TEST NUM LOCK	FOR DETERMINATION

	cmp	al,54h
	jnc	NO_KPAD
	test	KB_FLAG,20h	;ARE WE	IN NUM LOCK
	jnz	NUM_STATE	;TEST FOR SURE
NO_KPAD:
	test	KB_FLAG,03h	;LEFT_SHIFT+RIGHT_SHIFT
	jnz	TRANS		;IF SHIFTED, REALLY NUM	STATE
	jmp short NO_SHIFT	;BASE CASE FOR KEYPAD

;------	MIGHT BE NUM LOCK, TEST	SHIFT STATUS

NUM_STATE:			;ALMOST-NUM-STATE
	test	KB_FLAG,03h	;LEFT_SHIFT+RIGHT_SHIFT
	jz	TRANS

;------	LOWER CASE

NO_SHIFT:
	mov	bx,offset TNGER	;German lower case table
	test	KB_FLAG_1,01h
	jz	TRANS
	mov	bx,offset TNASC	;ASCII lower case table

;------	TRANSLATE THE CHARACTER

TRANS:	add	al,al
	sub	ah,ah
	add	ax,bx
	mov	bx,ax
	mov	ax,cs:[bx]
	cmp	ax,-1
	jz	KEND2
	cmp	al,0e0h		;Extended Code ?
	jnz	BFILL
	cmp	dl,0e0h		;last byte e0 ?
	jz	BFILL
	sub	al,al		;no al = 0

;------	PUT CHARACTER INTO BUFFER
;------	HANDLE THE CAPS	LOCK PROBLEM

BFILL:	test	KB_FLAG,40h	;ARE WE	IN CAPS	LOCK STATE ?
	jz	NO_CAPS		;SKIP IF NOT

;------	IN CAPS	LOCK STATE

	test	KB_FLAG,03h	;LEFT_SHIFT+RIGHT_SHIFT
	jz	LOUP		;IF NOT	SHIFT, CONVERT LOWER TO	UPPER

;------	CONVERT	ANY UPPER CASE TO LOWER	CASE

	cmp	al,'A'		;FIND OUT IF ALPHABETIC
	jc	NO_CAPS		;NOT_CAPS_STATE
	cmp	al,'Z'
	ja	UML1
	add	al,20h		;CONVERT TO LOWER CASE
	jmp	short NO_CAPS

;KEND2:	jmp	KEND

;---- Behandlung der Umlaute

UML1:	cmp	al,8eh		;Ae
	jnz	UML2
	mov	al,84h		;ae
	jmp short NO_CAPS

UML2:	cmp	al,99h		;Oe
	jnz	UML3
	mov	al,94h		;oe
	jmp short NO_CAPS

UML3:	cmp	al,9Ah		;Ue
	jnz	NO_CAPS
	mov	al,81h
	jmp short NO_CAPS

;------	CONVERT	ANY LOWER CASE TO UPPER	CASE

LOUP:	cmp	al,'a'		;FIND OUT IF ALPHABETIC
	jc	NO_CAPS
	cmp	al,'z'
	ja	UML4
	sub	al,20h		;CONVERT TO UPPER CASE
	jmp short NO_CAPS

;---- Behandlung der Umlaute

UML4:	cmp	al,84h		;ae
	jnz	UML5
	mov	al,8eh		;Ae
	jmp short NO_CAPS

UML5:	cmp	al,94h		;oe
	jnz	UML6
	mov	al,99h		;Oe
	jmp short NO_CAPS

UML6:	cmp	al,81h		;ue
	jnz	NO_CAPS
	mov	al,9ah		;Ue

NO_CAPS:
	mov	bx,BUFFER_TAIL	;GET THE END POINTER TO	THE BUFFER
	mov	si,bx		;SAVE THE VALUE
	call	K4		;ADVANCE THE TAIL
	cmp	bx,BUFFER_HEAD	;HAS THE BUFFER	WRAPPED	AROUND
	je	BELL		;BUFFER_FULL_BEEP
	mov	[si],ax		;STORE THE VALUE
	mov	BUFFER_TAIL,bx	;MOVE THE POINTER UP
	mov	ax,9102h	;MOVE IN POST CODE & TYPE
	int	15h		;PERFORM OTHER FUNCTION
KEND2:	jmp	KEND		;INTERRUPT_RETURN

;------	BUFFER IS FULL,	SOUND THE BEEPER

BELL:	mov	al,07h		;Bell
	call	PUTA
	jmp	KEND


;--- INT 10 --------------------------------------------------------------
;VIDEO_IO
;	THESE ROUTINES PROVIDE THE CRT INTERFACE
;	THE FOLLOWING FUNCTIONS	ARE PROVIDED:
;	(AH)=0	SET MODE - nur Mode 7 moeglich
;	(AH)=1	SET CURSOR TYPE	- nicht	moeglich
;	(AH)=2	SET CURSOR POSITION
;	(AH)=3	READ CURSOR POSITION
;	(AH)=4	READ LIGHT PEN POSITION	- nicht	moeglich
;	(AH)=5	SELECT ACTIVE DISPLAY PAGE - nur Page 0
;	(AH)=6	SCROLL ACTIVE PAGE UP
;	(AH)=7	SCROLL ACTIVE PAGE DOWN
;CHARAKTER HANDLING ROUTINES
;	(AH)=8	READ ATTRIBUTE/CHARAKTER AT CURRENT CURSOR POSITION
;	(AH)=9	WRITE ATTRIBUTE/CHARAKTER AT CURRENT CURSOR POSITION
;	(AH)=10	WRITE CHARACTER	ONLY AT	CURRENT	CURSOR POSITION
;GRAPHICS INTERFACE
;	(AH)=11	SET COLOR PALETTE - nicht moeglich
;	(AH)=12	WRITE DOT - nicht moeglich
;	(AH)=13	READ DOT - nicht moeglich
;ASCII TELETYPE	ROUTINE	FOR OUTPUT
;	(AH)=14	WRITE TELETYPE TO ACTIVE PAGE
;	(AH)=15	CURRENT	VIDEO STATE
;	(AH)=16	RESERVED
;	(AH)=17	RESERVED
;	(AH)=18	RESERVED
;	(AH)=19	WRITE STRING
;		(AL) = 0
;			BL    -	ATTRIBUTE
;			STRING IS [CHAR,CHAR, ... ,CHAR]
;			CURSOR NOT MOVED
;		(AL) = 1
;			BL    -	ATTRIBUTE
;			STRING IS [CHAR,CHAR, ... ,CHAR]
;			CURSOR IS MOVED
;		(AL) = 2
;			STRING IS [CHAR,ATTR,CHAR,ATTR,	... ,CHAR,ATTR]
;			CURSOR IS NOT MOVED
;		(AL) = 3
;			STRING IS [CHAR,ATTR,CHAR,ATTR,	... ,CHAR,ATTR]
;			CURSOR IS MOVED
;		NOTE:  CARRIAGE	RETURN,	LINE FEED, BACKSPACE, AND BELL ARE
;		       TREATED AS COMMANDS RATHER THEN PRINTABLE CHARACTERS.
;
;	SS,SP,ES,DS,DX,CX,BX,SI,DI,BP PRESERVED	DURING CALL
;	ALL OTHERS DESTROYED.
;-------------------------------------------------------------------------

.PUBLIC	SET_MODE
.PUBLIC	SET_CTYPE
.PUBLIC	SET_CPOS
.PUBLIC	READ_CURSOR
.PUBLIC	READ_LPEN
.PUBLIC	ACT_DISP_PAGE
.PUBLIC	SCROLL_UP
.PUBLIC	SCROLL_DOWN
.PUBLIC	READ_AC_CURRENT
.PUBLIC	WRITE_AC_CURRENT
.PUBLIC	WRITE_C_CURRENT
.PUBLIC	SET_COLOR
.PUBLIC	WRITE_DOT
.PUBLIC	READ_DOT
.PUBLIC	WRITE_TTY
.PUBLIC	VIDEO_STATE

V1	DW	SET_MODE
	DW	SET_CTYPE
	DW	SET_CPOS
	DW	READ_CURSOR
	DW	READ_LPEN
	DW	ACT_DISP_PAGE
	DW	SCROLL_UP
	DW	SCROLL_DOWN
	DW	READ_AC_CURRENT
	DW	WRITE_AC_CURRENT
	DW	WRITE_C_CURRENT
	DW	SET_COLOR
	DW	WRITE_DOT
	DW	READ_DOT
	DW	WRITE_TTY
	DW	VIDEO_STATE
	DW	VIDEO_RETURN	;Reserved
	DW	VIDEO_RETURN	;Reserved
	DW	VIDEO_RETURN	;Reserved
	DW	WRITE_STRING	;CASE 19h, Write string
V1L	EQU	$-V1

VIDEO ;PROC NEAR		;ENTRY POINT FOR ORG 0F065H
	STI			;INTERRUPTS BACK ON
	CLD			;SET DIRECTION FORWARD
	PUSH	ES
	PUSH	DS		;SAVE SEGMENT REGISTERS
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	SI
	PUSH	DI
	PUSH	BP
	PUSH	AX		;SAVE AX VALUE
	cmp	ah,14		;WRITE TTY ?
	jz	V0
	mov	byte ptr cs:TTYFLG,0
V0:	MOV	AL,AH		;GET INTO LOW BYTE
	XOR	AH,AH		;ZERO TO HIGH BYTE
	SAL	AX,1		;*2 FOR	TABLE LOOKUP
	MOV	SI,AX		;PUT INTO SI FOR BRANCH
	CMP	AX,V1L		;TEST FOR WITHIN RANGE
	JB	V2		;BRANCH	AROUND BRANCH
	POP	AX		;THROW AWAY THE	PARAMETER
	JMP	short VIDEO_RETURN

V2:	CALL	DDS
	MOV	AX,0B000H	;SEGMENT FOR BW	CARD
	MOV	ES,AX		;SET UP	TO POINT AT VIDEO RAM AREAS
	POP	AX		;RECOVER VALUE
	CMP	AH,13H		;TEST FOR WRITE	STRING OP
	JNE	V3
	PUSH	BP		;IF IT'S WRITE STRING THEN GET THE
	MOV	BP,SP		;STRINGS SEGMENT,SINCE IT GET CLOBBERED
	MOV	ES,[BP]+10H	;MOV ES,[BP].ES_POS ????
	POP	BP

V3:	JMP	CS:[SI]+V1

;---------------------------------------------------------------
;LIGHT PEN		Implementation nicht moeglich
;ACT_DISP_PAGE
;SET_COLOR
;READ DOT -- WRITE DOT
;---------------------------------------------------------------
READ_LPEN:			;AH = 0	= nicht	verfuegbar
	mov	ah,0		;nicht verfuegbar
	jmp	short VIDEO_RETURN
READ_DOT:			;AL = DOT VALUE
	sub	al,al
ACT_DISP_PAGE:
SET_COLOR:
WRITE_DOT:

;------	NORMAL RETURN FROM ALL VIDEO RETURNS
VIDEO_RETURN:
	POP	BP
	POP	DI
	POP	SI
	POP	BX
	POP	CX
	POP	DX
	POP	DS
	POP	ES
	IRET

;-----------------------------------------------------
;SET_MODE
;Mode setzen und Bildschirm loeschen
;-----------------------------------------------------
CURSOR_TTY:
	defw	-1

SET_MODE ;PROC NEAR
	mov	CRT_MODE,7
	mov	CRT_COLS,80
	mov	CRT_LEN,2*25*80
	mov	ADDR_6845,03b4h	
	mov	bh,07h			;Attribut
	call	CLR_TTY			;Bildschirm loeschen
	mov	CURSOR_POSN,0		;Cursor	Home setzen
	mov	word ptr cs:CURSOR_TTY,-1	;Cursor	synchronisieren
	mov	bx,ax
	call	CURSOR_OUT
	mov	ax,bx
	jmp	short VIDEO_RETURN

CLR_TTY:
;------	ADM 31 Escape-Folge Bildschirm loeschen
	mov	bx,ax
	mov	al,1bh		;Video-Attribute ruecksetzen
	call	PUTA
	mov	al,'G'
	call	PUTA
	mov	al,'0'
	call	PUTA
	mov	al,1bh		;Loeschen
	call	PUTA
	mov	al,';'
	call	PUTA

;------	beide Bildschirmpuffer mit Space fuellen

	xor	di,di			;SET UP	POINTER
	mov	cx,800h+25*80		;NUMBER	OF WORDS
	mov	al,20h			;FILL CHAR
	mov	ah,bh			;Attribut
 mov ah,07h
	rep	stosw			;FILL THE REGEN	BUFFER WITH BLANKS
	mov	ax,bx
	ret

;---------------------------------------------------------
;SET_CTYPE
;---------------------------------------------------------
SET_CTYPE ;PROC	NEAR	;**** nicht moeglich
	mov	CURSOR_MODE,cx
	jmp	short VIDEO_RETURN

;---------------------------------------------------------------
;SET_CPOS
;	THIS ROUTINE SETS THE CURRENT CURSOR POSITION TO THE
;	NEW X-Y	VALUES PASSED
;INPUT
;	DX - ROW, COLUMN OF NEW	CURSOR
;OUTPUT
;	CURSOR IS SET
;---------------------------------------------------------------
SET_CPOS ;PROC	NEAR		;nur 1 Page moeglich
	mov	CURSOR_POSN,dx
	jmp	short VIDEO_RETURN

;---------------------------------------------------------------
;READ_CURSOR
;	THIS ROUTINE READS THE CURRENT CURSOR VALUE
;OUTPUT
;	DX - ROW, COLUMN OF THE	CURRENT	CURSOR POSITION
;	CX - CURRENT CURSOR MODE
;---------------------------------------------------------------
READ_CURSOR ;PROC NEAR		;nur 1 Page
	mov	dx,CURSOR_POSN
	mov	cx,CURSOR_MODE
	POP	BP
	POP	DI
	POP	SI
	POP	BX
	POP	AX		;DISCARD SAVED CX AND DX
	POP	AX
	POP	DS
	POP	ES
	IRET

;---------------------------------------------------------------
;VIDEO_STATE
;	RETURNS	THE CURRENT VIDEO STATE	IN AX
;	AH = NUMBER OF COLUMNS ON THE SCREEN
;	AL = CURRENT VIDEO MODE
;	BH = CURRENT ACTIVE PAGE
;---------------------------------------------------------------
VIDEO_STATE ;PROC NEAR
	mov	ah,80		;GET NUMBER OF COLUMNS
	mov	al,7		;CURRENT MODE
	mov	bh,0		;GET CURRENT ACTIVE PAGE
	POP	BP
	POP	DI
	POP	SI
	POP	CX		;DISCARD SAVED BX
	POP	CX
	POP	DX
	POP	DS
	POP	ES
	IRET

;---------------------------------------------------------------
;SCROLL_UP
;	THIS ROUTINE MOVES A BLOCK OF CHARACTERS UP
;	ON THE SCREEN
;INPUT
;	(AL) = NUMBER OF ROWS TO SCROLL
;	(CX) = ROW/COLUMN OF UPPER LEFT	CORNER
;	(DX) = ROW/COLUMN OF LOWER RIGHT CORNER
;	(BH) = ATTRIBUTE TO BE USED ON BLANKED LINE
;	(DS) = DATA SEGMENT
;	(ES) = REGEN BUFFER SEGMENT
;OUTPUT
;	NONE --	THE REGEN BUFFER IS MODIFIED
;---------------------------------------------------------------
SCROLL_UP ;PROC	NEAR
	call	TEST_LINE_COUNT
	jnz	SUP2			;nicht ganzen BS
	or	bl,bl
	jnz	SUP0
	call	CLR_TTY			;BS loeschen
	jmp	VIDEO_RETURN

SUP0:	push	ax			;Rollen	durch LF realisieren
SUP1:	mov	ax,0e0ah		;Write TTY: LF ausgeben
	int	10h
	dec	bl
	jnz	SUP1
	pop	ax
	jmp	VIDEO_RETURN

SUP2:	PUSH	BX			;SAVE FILL ATTRIBUTE IN	BH
	MOV	AX,CX			;UPPER LEFT POSITION
	CALL	SCROLL_POSITION		;DO SETUP FOR SCROLL
	JZ	SUP6			;BLANK_FIELD
	ADD	SI,AX			;FROM ADDRESS
	MOV	AH,DH			;# ROWS	IN BLOCK
	SUB	AH,BL			;# ROWS	TO BE MOVED
SUP3:					;ROW_LOOP
	CALL	MRAW			;MOVE ONE ROW
	ADD	SI,BP
	ADD	DI,BP			;POINT TO NEXT LINE IN BLOCK
	DEC	AH			;COUNT OF LINES	TO MOVE
	JNZ	SUP3			;ROW_LOOP
SUP4:					;CLEAR_ENTRY
	POP	AX			;RECOVER ATTRIBUTE IN AH
	MOV	AL,' '			;FILL WITH BLANKS
SUP5:					;CLEAR_LOOP
	CALL	CRAW			;CLEAR THE ROW
	ADD	DI,BP			;POINT TO NEXT LINE
	DEC	BL			;COUNTER OF LINES TO SCROLL
	JNZ	SUP5			;CLEAR_LOOP
	jmp	VIDEO_RETURN
SUP6:					;BLANK_FIELD
	MOV	BL,DH			;GET ROW COUNT
	jmp	short SUP4

;---------------------------------------------------------------
;SCROLL_DOWN
;	THIS ROUTINE MOVES THE CHARACTERS WITHIN A DEFINED
;	BLOCK DOWN ON THE SCREEN, FILLING THE TOP LINES
;	WITH A DEFINED CHARACTER
;INPUT
;	(AL) = NUMBER OF LINES TO SCROLL
;	(CX) = UPPER LEFT CORNER OF REGION
;	(DX) = LOWER RIGHT CORNER OF REGION
;	(BH) = FILL CHARACTER
;	(DS) = DATA SEGMENT
;	(ES) = REGEN SEGMENT
;OUTPUT
;	NONE --	SCREEN IS SCROLLED
;----------------------------------------------------------------
SCROLL_DOWN ;PROC NEAR
	call	TEST_LINE_COUNT
	jnz	SD0			;nicht ganzen BS
	or	bl,bl			;BS loeschen ?
	jnz	SD0
	call	CLR_TTY
	jmp	VIDEO_RETURN

SD0:	STD				;DIRECTION FOR SCROLL DOWN
	PUSH	BX			;SAVE ATTRIBUTE	IN BH
	MOV	AX,DX			;LOWEN RIGHT CORNER
	CALL	SCROLL_POSITION		;GET REGEN LOCATION
	JZ	SD4
	SUB	SI,AX			;SI IS FROM ADDRESS
	MOV	AH,DH			;GET TOTAL # HOWS
	SUB	AH,BL			;COUNT TO MOVE IN SCROLL
SD1:
	CALL	MRAW			;MOVE ONE ROW
	SUB	SI,BP
	SUB	DI,BP
	DEC	AH
	JNZ	SD1
SD2:
	POP	AX			;RECOVER ATTRIBUTE IN AH
	MOV	AL,' '
SD3:
	CALL	CRAW			;CLEAR ONE ROW
	SUB	DI,BP			;GO TO NEXT ROW
	DEC	BL
	JNZ	SD3
	JMP	VIDEO_RETURN		;SCROLL_END
SD4:
	MOV	BL,DH
	jmp	short SD2

;------	TEST IF	AMOUNT OF LINES	TO BE SCROLLED = AMOUNT	OF LINES IN WINDOW
;------	IF TRUE	THEN WE	ADJUST AL, IF FALSE WE RETURN...

TEST_LINE_COUNT	;PROC NEAR
	MOV	BL,AL			;SAVE LINE COUNT IN BL
	OR	AL,AL			;TEST IF AL IS ALREADY ZERO
	JZ	BL_SET			;IF IT IS THEN RETURN...
	PUSH	AX			;SAVE AX
	MOV	AL,DH			;SUBTRACT LOWER	ROW FROM UPPER ROW
	SUB	AL,CH
	INC	AL			;ADJUST	DIFERENCE BY 1
	CMP	AL,BL			;TEST IF LCOUNT	= AMOUNT OF ROWS IN WIND
	POP	AX			;RESTORE AX
	JNE	BL_SET			;IF NOT	THEN WE'RE ALL SET
	SUB	BL,BL			;OTHERWISE SET BL TO ZERO
BL_SET:

;Test ob Bildschirmbereich ueberschritten und auf Loeschen ganzer BS

	cmp	dx,184fh
	jc	TL1
	mov	dx,184fh
TL1:	or	cx,cx			;ganzer	Bildschirm ?
	jnz	TL2
	cmp	dx,184fh
TL2:	ret

;------	HANDLE COMMON SCROLL SET UP HERE

SCROLL_POSITION	;PROC NEAR
	CALL	POSITION		;CONVERT TO REGEN POINTER
	MOV	DI,AX			;TO ADDRESS FOR	SCROLL
	MOV	SI,AX			;FROM ADDRESS FOR SCROLL
	SUB	DX,CX			;DX = #ROWS, #COLS IN BLOCK
	INC	DH
	INC	DL			;INCREMENT FOR 0 ORIGIN
	XOR	CH,CH			;SET HIGH BYTE OF COUNT	TO ZERO
	MOV	BP,2*80			;GET NUMBER OF COLUMNS IN DISPLAY
					;TIMES 2 FOR ATTRIBUTE BYTE
	MOV	AL,BL			;GET LINE COUNT
	mov	bh,2*80			;*2 for	Attribute Byte
	MUL	bh			;DETERMINE OFFSET TO FROM ADDRESS
	PUSH	ES			;ESTABLISH ADDRESSING TO REGEN BUFFER
	POP	DS			;FOR BOTH POINTERS
	CMP	BL,0			;0 SCROLL MEANS	BLANK FIELD
	RET				;RETURN	WITH FLAGS SET

;------	MOVE_ROW

MRAW ;PROC NEAR
	MOV	CL,DL			;GET # OF COLS TO MOVE
	PUSH	SI
	PUSH	DI			;SAVE START ADDRESS
	REP	MOVSW			;MOVE THAT LINE	OK SCREEN
	POP	DI
	POP	SI			;RECOVER ADDRESSES
	RET

;------	CLEAR_ROW
CRAW ;PROC NEAR
	MOV	CL,DL			;GET # COLUMNS TO CLEAR
	PUSH	DI
	REP	STOSW			;STORE THE FILL	CHARACTER
	POP	DI
	RET

;---------------------------------------------------------------
;READ_AC_CURRENT
;	THIS ROUTINE READS THE ATTRIBUTE AND CHARACTER AT THE CURRENT
;	CURSOR POSITION	AND RETURNS THEN TO THE	CALLER
;INPUT
;	(DS) = DATA SEGMENT
;	(ES) = REGEN SEGMENT
;OUTPUT
;	(AL) = CHAR READ
;	(AH) = ATTRIBUTE READ
;----------------------------------------------------------------
READ_AC_CURRENT	;PROC NEAR
	MOV	AX,CURSOR_POSN		;GET ROW/COLUMN
	CALL	POSITION		;DETERMINE LOCATION IN REGEN
	MOV	SI,AX			;ESTABLISH ADDRESSING IN SI
	PUSH	ES
	POP	DS			;GET SEGMENT FOR QUICK ACCESS
	LODSW				;GET THE CHAR/ATTR
	JMP	VIDEO_RETURN

;---------------------------------------------------------------
;WRITE_AC_CURRENT
;	THIS ROUTINE WRITES THE	ATTRIBUTE AND CHARAKTER	AT
;	THE CURRENT CURSOR POSITION
;INPUT
;	(CX) = COUNT OF	CHARACTERS TO WRITE
;	(AL) = CHAR TO WRITE
;	(BL) = ATTRIBUTE OF CHAR TO WRITE
;	(DS) = DATA SEGMENT
;	(ES) = REGEN SEGMENT
;OUTPUT
;	NONE
;----------------------------------------------------------------
WRITE_AC_CURRENT ;PROC NEAR
	MOV	AH,BL			;GET ATTRIBUTE TO AH
	PUSH	AX			;SAVE ON STACK
	MOV	AX,CURSOR_POSN		;GET ROW/COLUMN
	CALL	POSITION		;DETERMINE LOCATION IN REGEN
	MOV	DI,AX			;ADDRESS TO DI REGISTER
	POP	AX			;CHARACTER
WAC:					;WRITE_LOOP
	STOSW				;PUT THE CHAR/ATTR
	LOOP	WAC			;AS MANY TIMES AS REQUESTED
	JMP	VIDEO_RETURN

;---------------------------------------------------------------
;WRITE_C_CURRENT
;	THIS ROUTINE WRITES THE	CHARACTER AT
;	THE CURRENT CURSOR POSITION, ATTRIBUTE UNCHANGED
;INPUT
;	(CX) = COUNT OF	CHARACTERS TO WRITE
;	(AL) = CHAR TO WRITE
;	(DS) = DATA SEGMENT
;	(ES) = REGEN SEGMENT
;OUTPUT
;	NONE
;---------------------------------------------------------------
WRITE_C_CURRENT	;PROC NEAR
	PUSH	AX			;SAVE ON STACK
	MOV	AX,CURSOR_POSN		;GET ROW/COLUMN
	CALL	POSITION		;DETERMINE LOCATION IN REGEN
	MOV	DI,AX			;ADDRESS TO DI
	POP	AX			;AL HAS	CHAR TO	WRITE
WCC:					;WRITE_LOOP
	STOSB				;PUT THE CHAR
	INC	DI			;BUMP POINTER PAST ATTRIBUTE
	LOOP	WCC			;AS MANY TIMES AS REQUESTED
	JMP	VIDEO_RETURN
;WRITE_AC_CURRENT

;---------------------------------------------------------------
;WRITE_TTY
;	THIS INTERFACE PROVIDES	A TELETYPE LIKE	INTERFACE.
;	THE INPUT CHARACTER IS WRITTEN TO THE CURRENT
;	CURSOR POSITION, AND THE CURSOR	IS MOVED TO THE	NEXT POSITION.
;	IF THE CURSOR LEAVES THE LAST COLUMN OF	THE FIELD, THE COLUMN
;	IS SET TO ZERO,	AND THE	ROW VALUE IS INCREMENTED. IF THE ROW
;	VALUE LEAVES THE FIELD.	THE CURSOR IS PLACED ON	THE LAST ROW,
;	FIRST COLUMN, AND THE ENTIRE SCREEN IS SCROLLED	UP ONE LINE.
;	WHEN THE SCRENN	IS SCROLLED UP,	THE ATTRIBUTE FOR FILLING THE
;	NEWLY BLANKED LINE IS READ FROM	THE CURSOR POSITION ON THE PREVIOS
;	LINE BEFORE THE	SCROLL.
;ENTRY --
;	(AL) = CHARACTER TO BE WRITTEN
;	       NOTE THAT BACK SPACE, CR, BELL AND LINE FEED ARE	HANDLED
;	       AS COMMANDS RATHER THAN AS DISPLAYABLE GRAPHICS
;EXIT --
;	ALL REGISTERS SAVED
;---------------------------------------------------------------
TTYFLG	defb	0
TTYFLG1	defb	0

WRITE_TTY ;PROC	NEAR
	mov	bx,ax
	call	CURSOR_OUT
	mov	ax,bx
	mov	dl,cs:TTYFLG
	or	dl,dl
	jnz	WT1
	mov	cs:TTYFLG1,1		;BS muss ausgegeben werden
WT1:	mov	cs:TTYFLG,1		;letzter VIDEO-Request:	WRITE_TTY
	mov	dx,CURSOR_POSN		;READ CURSOR POSITION

	call	PUTA
	cmp	al,08h
	jnz	WT2

	mov	al,20h			;Backspace behandeln
	call	PUTA
	mov	al,08h
	call	PUTA
	jmp short U5

;------	DX NOW HAS THE CURRENT CURSOR POSITION

WT2:	CMP	AL,0DH			;IS IT CARRIAGE	RETURN
	JE	U6			;CAR_RET
	CMP	AL,0AH			;IS IT A LINE FEED
	JE	U7			;LINE_FEED
	CMP	AL,07H			;IS IT A BELL
	JE	U4			;Return

;------	WRITE THE CHAR TO THE SCREEN

	mov	ax,dx
	call	POSITION
	mov	di,ax
	mov	al,bl
	mov	ah,07h
	stosb
	mov	es:[di]+0fffh,al	;in 2.Abbild eintragen

;------	POSITION THE CURSOR FOR	NEXT CHAR

	INC	DL
	CMP	DL,80			;TEST FOR COLUMN OVERFLOW
	JNZ	U3			;SET_CURSOR
	MOV	DL,0			;COLUMN	FOR CURSOR
	CMP	DH,24
	JNZ	U2			;SET_CURSOR_INC

;------	SCROLL REQUIRED

U1:	push	ds
	push	es
	pop	ds
	mov	cx,24*80		;Anzahl	der Zeichen
	sub	di,di			;Zieladresse
	mov	si,2*80			;Startadresse
	rep	movsw			;1 Zeile nach oben rollen
	mov	cx,80			;letzte	Zeile loeschen
	mov	ax,0720h
	rep	stosw

	mov	cx,24*80		;2. Abbild rollen
	mov	di,1000h		;Zieladresse
	mov	si,1000h+2*80		;Startadresse
	rep	movsw			;1 Zeile nach oben rollen
	mov	cx,80			;letzte	Zeile loeschen
	mov	ax,0720h
	rep	stosw
	pop	ds
	jmp	short U3

U2:	inc	dh			;SET-CURSOR-INC	NEXT ROW
U3:	mov	CURSOR_POSN,dx		;SET-CURSOR
	mov	cs:CURSOR_TTY,dx

U4:	mov	ax,bx			;RESTORE THE CHARACTER
	jmp	VIDEO_RETURN

;------	BACK SPACE FOUND
U5:	CMP	DL,0			;ALREADY AT END	OF LINE
	JE	U3			;SET_CURSOR
	DEC	DL			;NO -- JUST MOVE IT BACK
	JMP	short U3		;SET_CURSOR

;------	CARRIAGE RETURN	FOUND
U6:	MOV	DL,0			;MOVE TO FIRST COLUMN
	JMP	short U3		;SET_CURSOR

;------	LINE FEED FOUND
U7:	CMP	DH,24			;BOTTOM	OF SCREEN
	JNE	U2			;YES, SCROLL THE SCREEN
	JMP	short U1		;NO, JUST SET THE CURSOR

;---------------------------------------------------------------
;WRITE STRING
;	This routine writes a string of	characters to the crt.
;INPUT
;	(AL) = WRITE STRING COMMAND 0 -	3
;	(CX) = COUNT OF	CHARACTERS TO WRITE. IF	CX == 0	THEN RETURN
;	(DX) = CURSOR POSITION
;	(BL) = ATTRIBUTE OF CHAR TO WRITE IF AL	== 0 | AL == 1
;	(ES) = STRING SEGMENT
;	(BP) = STRING OFFSET
;OUTPUT
;	N/A
;----------------------------------------------------------------
WRITE_STRING ;PROC NEAR
	CMP	AL,04			;TEST FOR INVALID WRITE	STRING OPTION
	JB	W0			;IF OPTION INVALID THEN	RETURN
	JMP	DONE
W0:	OR	CX,CX			;TEST FOR ZERO LENGTH STRING
	JNZ	W1
	JMP	short DONE		;IF ZERO LENGTH	STRING THEN RETURN
W1:	MOV	SI,CURSOR_POSN
	PUSH	SI			;SAVE CURRENT CURSOR POSITION
	mov	CURSOR_POSN,dx		;SET NEW CURSOR	POSITION

WRITE_CHAR:
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	ES
	XCHG	AH,AL			;PUT THE WRITE STRING OPTION INTO AH
	MOV	AL,ES:[BP]		;GET CHARACTER FROM INPUT STRING
	INC	BP			;BUMP POINTER TO CHARACTER

;------	TEST FOR SPECIAL CHARACTER'S

	CMP	AL,8			;IS IT A BACKSPACE
	JE	DO_TTY			;BACK_SPACE
	CMP	AL,0DH			;IS IT CARRIAGE	RETURN
	JE	DO_TTY			;CAR_RET
	CMP	AL,0AH			;IS IT A LINE FEED
	JE	DO_TTY			;LINE_FEED
	CMP	AL,07H			;IS IT A BELL
	JNE	GET_ATTRIBUTE		;IF NOT	THEN DO	WRITE CHARACTER
DO_TTY:
	MOV	AH,14			;WRITE TTY CHARACTER TO	THE CRT
	INT	10H
	MOV	DX,CURSOR_POSN		;GET CURRENT CURSOR POSITION
	POP	ES
	POP	AX			;RESTORE REGISTERS
	POP	BX
	POP	CX
	JMP	short ROWS_SET

GET_ATTRIBUTE:
	MOV	CX,1			;SET CHARACTER WRITE AMOUNT TO ONE
	CMP	AH,2			;IS THE	ATTRIBUTE IN THE STRING
	JB	GOT_IT			;IF NOT	THEN JUMP
	MOV	BL,ES:[BP]		;ELSE GET IT
	INC	BP			;BUMP STRING POINTER

GOT_IT:	MOV	AH,09			;WRITE CHARACTER TO THE	CRT
	INT	10H
	POP	ES
	POP	AX			;RESTORE REGISTERS
	POP	BX
	POP	CX
	INC	DL			;INCREMENT COLUMN COUNTER
	CMP	DL,80			;IF COLS ARE WITHIN RANGE FOR THIS MODE
	JB	COLUMNS_SET		;THEN GOTO COLS	SET
	INC	DH			;BUMP ROW COUNTER BY ONE
	SUB	DL,DL			;SET COLUMN COUNTER TO ZERO
	CMP	DH,25			;IF ROWS ARE < 25 THEN
	JB	ROWS_SET		;GOTO ROWS_SET
					;SAVE WRITE STRING PARAMETER REGS
	PUSH	ES			;SAVE REG'S THAT GET CLOBBERED
	PUSH	AX
	MOV	AX,0E0AH		;DO SCROLL ONE LINE
	INT	10H			;RESET ROW COUNTER TO 24
	DEC	DH
	POP	AX			;RESTORE REG'S
	POP	ES

ROWS_SET:
COLUMNS_SET:
	mov	CURSOR_POSN,dx
	LOOP	WRITE_CHAR		;DO IT ONCE MORE UNTIL CX = ZERO
	POP	DX			;RESTORE OLD CURSOR COORDINATES
	CMP	AL,1			;IF CURSOR WAS TO BE MOVED THEN
	JE	DONE			;WE'RE DONE
	CMP	AL,3
	JE	DONE
	mov	CURSOR_POSN,dx		;ELSE RESTORE OLD CURSOR POSITION
DONE:
	JMP	VIDEO_RETURN		;RETURN	TO CALLER

;---------------------------------------------------------------
;POSITION
;	THIS SERVICE ROUTINE CALCULATES	THE REGEN BUFFER ADDRESS
;	OF A CHARACTER IN THE ALPHA MODE
;INPUT
;	AX = ROW, COLUMN POSITION
;OUTPUT
;	AX = OFFSET OF CHAR POSITION IN	REGEN BUFFER
;---------------------------------------------------------------
POSITION ;PROC NEAR
	PUSH	BX			;SAVE REGISTER
	MOV	BX,AX
	MOV	AL,AH			;ROWS TO AL
	mov	bh,80
	MUL	bh			;DETERMINE BYTES TO ROW
	XOR	BH,BH
	ADD	AX,BX			;ADD IN	COLUMN VALUE
	SAL	AX,1			;*2 FOR	ATTRIBUTE BYTES
	POP	BX
	RET

;---------------------------------------------------------------
;Bildwiederholspeicher vergleichen und Differenzen ausgeben
;---------------------------------------------------------------
CURSON:	defb	1		;sichtbar

CURSOR_OUT:			;CURSOR-Stand vergleichen und Cursor setzen
	mov	dx,CURSOR_POSN
	cmp	dx,1850h
	jnc	COFF
	mov	ax,CURSOR_MODE
	test	ah,20h		;Cursor unterdruecken ?
	jnz	COUT1		;ja

	call	COUT2
	mov	al,cs:CURSON
	or	al,al		;Cursor einschalten
	jnz	COUT0		;Cursor ist eingeschaltet
	mov	cs:CURSON,1
	mov	al,1bh
	call	PUTA
	mov	al,'.'
	call	PUTA
	mov	al,'1'
	jmp	PUTA

COUT:
COUT1:	call	COFF
COUT2:	cmp	dx,cs:CURSOR_TTY
	jz	COUT0		;Cursor	steht auf Bildschirm richtig

COUT3:	mov	cs:CURSOR_TTY,dx
	mov	al,1bh		;ADM 31	ESC-Folge
	call	PUTA
	mov	al,'='
	call	PUTA
	mov	al,dh
	add	al,20h
	call	PUTA
	mov	al,dl
	add	al,20h
	jmp	PUTA
COUT0:	ret

COFF:	mov	al,cs:CURSON	;Cursor ausschalten
	or	al,al
	jz	COUT0		;Cursor ist ausgeschaltet
	mov	cs:CURSON,0
	mov	al,1bh
	call	PUTA
	mov	al,'.'
	call	PUTA
	mov	al,'0'
	jmp	PUTA


SCREEN_OUT:
	push	ax
	mov	al,cs:reset	;restart von WDOS ?
	or	al,al
	jnz	SO0
	mov	ax,cs:TTYFLG	;Bildwiederholspeicher Vergleich erforderlich ?
	or	ah,ah		;TTYFLG1
	jnz	SO0
	or	al,al
	jz	SO0
	pop	ax
	ret

SO0:	mov	cs:TTYFLG1,0
	pop	ax
	db	60h		;pusha
	push	ds
	push	es
	cld
	mov	ax,0b000h
	mov	ds,ax
	mov	ax,0b100h
	mov	es,ax
	xor	dx,dx		;Zeile 0
	xor	bx,bx

	mov	al,cs:reset	;restart ?
	or	al,al
	jz	SO1
	xor	ax,ax		;zweiten Bildwiederholspeicher loeschen
	mov	cs:reset,al
	mov	di,ax
	mov	cx,800h
	rep	stosw

SO1:	mov	si,bx
	mov	di,bx
	mov	cx,80		;80 Zeichen pro	Zeile
	repe cmpsw		;1 Zeile vergleichen
	jz	SO2		;Zeile ist gleich: keine Ausgaben
	push	bx
	call	SO3
	mov	cs:CURSOR_TTY,dx
	pop	bx

SO2:	add	bx,2*80		;naechste Zeile
	inc	dh
	cmp	dh,25		;alle Zeilen verglichen	?
	jc	SO1

	pop	es
	pop	ds
	call	CURSOR_OUT
	db	61h		;popa
	ret

SO3:	dec	si
	dec	si
	inc	cx
	push	si
	mov	si,bx
	add	si,2*79		;Adresse Ende der Zeile
	mov	bx,cx
	mov	dl,80		;Nummer	der 1. ungleichen Spalte berechnen
	sub	dl,cl
	call	COUT		;Cursor	positionieren

	mov	di,si		;Zeile von hinten vergleichen
	std
	repe cmpsw
	cld
	inc	cl		;in cl Anzahl der ungleichen Zeichen
	mov	ch,bl		;in ch Anzahl der Zeichen ab si bis Zeilenende
	pop	si		;Adr erstes ungleiches Zeichen
	mov	di,si

	sub	bp,bp		;Video-Attribute + Zeichen ruecksetzen
	mov	bx,bp

SO4:	lodsw			;Zeichen aus 1.	Bildwiederholspeicher nach ax

	cmp	ah,bh		;neues + vorheriges Zeichen Attribute gleich ?
	jnz	SO11

				;altes Zeichen aus 2. Bildwiederholspeicher
	cmp	ah,es:[di]+1	;Attribute gleich ?
	jnz	SO11		;nein

	stosw
	call	SOUT
	inc	dl		;Spalten-Zaehler inc
	dec	ch
	dec	cl
	jnz	SO4
	jmp short ZOUTR		;restliche Zeichen ausgeben

SO10:	lodsw			;volle Anzahl Zeichen ausgeben
SO11:	stosw
	call	SOUT
	inc	dl
	dec	ch
	jnz	SO10
	jmp short ZOUTR		;restliche Zeichen ausgeben

;Zeichen ausgeben

SOUT:	or	bp,bp		;nur Attribute ausgeben ?
	jnz	SOUT4		;nein

SOUT1:	cmp	ah,bh		;Attribute gleich ?
	mov	bx,ax
	jz	SOUT2		;ja
	call	SATTR
SOUT2:	inc	bp
SOUT0:	ret

SOUT4:	cmp	ax,bx		;neues = letztes Zeichen ?
	jz	SOUT2		;ja
	call	ZOUT		;bisherige Zeichen ausgeben
	jmp short SOUT1		;neues Attribut testen

ZOUTR:	or	bp,bp		;restliche Zeichen ausgeben
	jz	ZOUT0		;keine Zeichen vorhanden
	or	bl,bl		;Blank-Zeichen ueber Clear to End
	jz	CLREND
	cmp	bl,20h
	jz	CLREND

ZOUT:	push	ax
	mov	ax,bx
	cmp	bl,20h
	jnc	ZOUT2
	mov	al,3eh
	cmp	bl,10h
	jz	ZOUT2
	mov	al,3ch
	cmp	bl,11h
	jz	ZOUT2

	mov	al,0
	call	PUTA

ZOUT1:	mov	al,20h

ZOUT2:	call	PUTA		;Zeichen einzeln ausgeben
	dec	bp
	jnz	ZOUT2
	pop	ax
ZOUT0:	ret


;Zeile von Cursor bis Ende loeschen

CLREND:	mov	al,1bh		;Zeile loeschen
	call	PUTA
	mov	al,'T'
	jmp short PUTA

;Video-Attribut ausgeben

SATTR:	mov	al,1bh
	call	PUTA
	mov	al,'G'
	call	PUTA
	mov	al,'p'		;normal
	test	ah,08h		;intensiv ?
	jz	SATTR1
	mov	al,'0'		;intensiv
SATTR1:	test	ah,80h		;blinkend ?
	jz	SATTR2
	add	al,2		;ja
SATTR2:	and	ah,77h
	cmp	ah,01h		;unterstrichen ?
	jnz	SATTR3
	add	al,8		;ja
	jmp short PUTA

SATTR3: cmp	ah,70h		;invers ?
	jnz	SATTR4
	add	al,4		;ja
	jmp short PUTA

SATTR4:	cmp	ah,0
	jnz	PUTA
	inc	al

;---------------------------------------------------------------
;PUTA
;---------------------------------------------------------------
PUTA:	push	ax
	push	bx
	push	di
	mov	di,offset ttybuf
PU1:	mov	bx, word ptr cs:ttyout+2
	xchg	bh,bl
	cmp	bx,800h
	jnc	PU1			;warten, da Puffer voll
PU2:	sti
	nop
	cli
	mov	ah,cs:ttyout+0		;outstop
	or	ah,ah
	jnz	PU2
	mov	bx, word ptr cs:ttyout+2
	xchg	bh,bl
	mov	cs:[bx+di],al
	inc	bx
	xchg	bh,bl
	mov	word ptr cs:ttyout+2,bx	;noutb
	cmp	bx,0100h
	jnz	PU4
PU3:	sti				;beim 1.Zeichen	INT geben
	nop
	cli
	mov	ah,cs:intout		;kann INT gegeben werden ?
	or	ah,ah
	jnz	PU3
	mov	cs:intout,1		;tty
	out	0,al			;INT geben
PU4:	sti
	pop	di
	pop	bx
	pop	ax
	ret


;--- INT 17 --------------------------------------------------------------
;PRINTER_IO
;	THIS ROUTINE PROVIDES COMMUNICATION WITH THE PRINTER
;INPUT
;	(AH)=0	PRINT THE CHARACTER IN (AL)
;		ON RETURN, (AH)	BITS SET AS ON NORMAL STATUS CALL
;	(AH)=1	INITIALIZE THE PRINTER PORT
;		RETURNS	WITH (AH) SET WITH PRINTER STATUS
;	(AH)=2	READ THE PRINTER STATUS	INTO (AH)
;
;	(DX) = PRINTER TO BE USED (0,1,2) - nur	ein Printer moeglich
;
;REGISTERS	AH IS MODIFIED
;		ALL OTHERS UNCHANGED
;-------------------------------------------------------------------------
PRINTER	;PROC FAR		;ENTRY POINT FOR ORG 0EFD2H
	STI			;INTERRUPTS BACK ON
	OR	AH,AH		;TEST FOR (AH)=0
	JZ	P0		;PRINT_AL
	DEC	AH		;TEST FOR (AH)=1
	JZ	P1		;INIT_PRT - nicht implementiert
	DEC	AH		;TEST FOR (AH)=2
	JZ	P1		;PRINTER STATUS
	JMP SHORT P3

;------	PRINT THE CHARACTER IN (AL)

P0:	push	bx
	push	di
	mov	di,offset lpbuf
P0A:	mov	bx, word ptr cs:lpout+2
	xchg	bh,bl
	cmp	bx,100h
	jnc	P0A			;warten	Uebergabepuffer	voll
P0B:	sti
	nop
	cli
	mov	ah,cs:lpout+0		;outstop
	or	ah,ah
	jnz	P0A
	mov	bx, word ptr cs:lpout+2
	xchg	bh,bl
	mov	cs:[bx+di],al
	inc	bx			;noutb
	xchg	bh,bl
	mov	word ptr cs:lpout+2,bx
	cmp	bx,0100h
	jnz	P0D
P0C:	sti				;beim 1.Zeichen	INT geben
	nop
	cli
	mov	ah,cs:intout		;kann INT gegeben werden ?
	or	ah,ah
	jnz	P0C
	mov	cs:intout,2		;lp
	out	0,al			;INT geben
P0D:	sti
	pop	di
	pop	bx

;------	PRINTER	STATUS

P1:	push	bx
	mov	bx, word ptr cs:lpout+2
	xchg	bh,bl
	cmp	bx,100h			;Puffer	voll ?
	jnc	P2
	mov	bh,cs:lpout+0		;outstop
	or	bh,bh
	jnz	P2
	mov	ah,0d0h			;bereit	fuer naechstes Zeichen
	pop	bx
	iret
P2:	mov	ah,50h			;nicht bereit fuer naechstes Zeichen
	pop	bx
P3:	iret


;------- INT_14	-----------------------------------------------------
;RS232_IO
;	THIS ROUTINE PROVIDES BYTE STREAM I/O TO THE COMMUNICATIONS
;	PORT ACCORDING TO THE PARAMETERS:
;	(AH)=0	INITIALIZE THE COMMUNICATIONS PORT
;		(AL) HAS PARMS FOR INITIALIZATION
;
;		7	6	5	4	3	2	1	0
;		----- BAUD RATE	--	-PARITY--     STOPBIT	--WORD LENGTH--
;
;		000 - 110		XO - NONE	0 - 1	 10 - 7	BITS
;		001 - 150		01 - ODD	1 - 2	 11 - 8	BITS
;		010 - 300		11 - EVEN
;		011 - 600
;		100 - 1200
;		101 - 2400
;		110 - 4800
;		111 - 9600
;		ON RETURN, CONDITIONS SET AS IN	CALL TO	COMMO STATUS (AH=3)
;	(AH)=1	SEND THE CHARACTER IN (AL) OVER	THE COMMO LINE
;		(AL) REGISTER IS PRESERVED.
;		ON EXIT, BIT 7 OF AH IS	SET IF THE ROUTINE HAS UNABLE TO
;			TO TRNASMIT THE	BYTE OF	DATA OVER THE LINE.
;			IF BIT 7 OF AH IS NOT SET, THE
;			REMAINDER OF AH	IS SET AS IN A STATUS REQUEST,
;			REFLECTING THE CURRENT STATUS OF THE LINE.
;	(AH)=2	RECEIVE	A CHARACTER IN (AL) FROM COMMO LINE BEFORE
;			RETURNIND TO CALLER
;		ON EXIT, AH HAS	THE CURRENT LINE STATUS, AS SET	BY THE
;			THE STATUS ROUTINE, EXCEPT THAT	THE ONLY BITS
;			LEFT ON	ARE THE	ERROR BITS (7,4,3,2,1)
;			IF AH HAS BIT 7	ON (TIME OUT) THE REMAINING
;			BITS ARE NOT PREDICTABLE.
;			THUS, AH IS NON	ZERO ONLY WHEN AH ERROR	OCCURRED.
;	(AH)=3	RETURN THE COMMO PORT STATUS IN	(AX)
;		AH CONTAINS THE	LINE CONTROL STATUS
;		BIT 7 =	TIME OUT
;		BIT 6 =	TRANS HOLDING REGISTER EMPTY
;		BIT 5 =	TRAN HOLDING REGISTER EMPTY
;		BIT 4 =	BREAK DETECT
;		BIT 3 =	FRANING	ERROR
;		BIT 2 =	PARITY ERROR
;		BIT 1 =	OVERRUN	ERROR
;		BIT 0 =	DATA READY
;		AL CONTAINS THE	MODEM STATUS
;		BIT 7 =	RECEVED	LINE SIGNAL DETECT
;		BIT 6 =	RING INDICATOR
;		BIT 5 =	DATA SET READY
;		BIT 4 =	CLEAR TO SEND
;		BIT 3 =	DELTA RECEIVE LINE SIGNAL DETECT
;		BIT 2 =	TRAILING EDCE RING DETECTOR
;		BIT 1 =	DELTA DATA SET READY
;		BIT 0 =	DELTA CLEAR TO SEND
;
;	(DX) = PARAMETER INDICATING WHICH RS232	CARD (0,1 ALLOWED)
;
;OUTPUT
;		AX MODIFIED ACCORDING TO PARMS OF CALL
;		ALL OTHERS UNCHANCED:
;----------------------------------------------------------------------
RS232 ;PROC FAR

;------	VECTOR TO APPROPRIATE ROUTINE

	STI			;INTERRUPTS BACK ON
	OR	AH,AH		;TEST FOR (AH)=0
	JZ	RS0		;COMMUN	INIT
	DEC	AH		;TEST FOR (AH)=1
	JZ	RS1		;SEND AL
	DEC	AH		;TEST FOR (AH)=2
	JZ	RS2		;RECEIVE INTO AL
	DEC	AH		;TEST FOR (AH)=3
	JZ	RS3		;COMMUNICATION STATUS
	JMP SHORT RS4

;------	INITIALIZE THE COMMUNICATIONS PORT
RS0:

;------	RECEIVE	CHARACTER FROM COMMO LINE
RS2:

;------	COMMO PORT STATUS ROUTINE
RS3:	MOV	AL,0

;------	SEND CHARACTER IN (AL) OVER COMMO LINE
RS1:	MOV	AH,80H		;INDICATE TIME OUT
RS4:	IRET

	END
