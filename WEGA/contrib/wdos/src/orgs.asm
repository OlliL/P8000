;**************************************************************************
;**************************************************************************
;
;	WDOS-BIOS - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
;	Programm: orgs.asm
;
;	Bearbeiter	: P. Hoge
;	Datum		: 24.1.89
;	Version		: 1.1
;
;**************************************************************************
;
;	System-Einspruenge und Vereinbarungen
;
;**************************************************************************
;**************************************************************************


.EXTERNAL DISK_SETUP;NEAR
.EXTERNAL SEEK;NEAR
.EXTERNAL START;NEAR
.EXTERNAL NMIINT;NEAR
.EXTERNAL BOOT;NEAR
.EXTERNAL KBOARD;NEAR
.EXTERNAL KBINT;NEAR
.EXTERNAL DISK;NEAR
.EXTERNAL DISKINT;NEAR
.EXTERNAL PRINTER;NEAR
.EXTERNAL VIDEO;NEAR
.EXTERNAL MEMSIZE;NEAR
.EXTERNAL EQUIP;NEAR
.EXTERNAL CASSETTE;NEAR
.EXTERNAL TOFDAY;NEAR
.EXTERNAL TIMERINT;NEAR
.EXTERNAL RS232;NEAR
.EXTERNAL PRSCREEN;NEAR

.public connect
.public reset
.public intin
.public ttyin
.public clkin
.public diskin
.public intout
.public ttyout
.public lpout
.public diskout
.public hdtyp
.public date
.public ttybuf
.public lpbuf
.public diskrq
.PUBLIC	VECTOR_TABLE
.PUBLIC	DUMMY_RET
.PUBLIC FD_TBL
.PUBLIC FD1_TBL
.PUBLIC FD2_TBL
.PUBLIC DISK_BASE


	ORG	0D000H
connect:			;---- Uebergabe-Bereich
reset:	db	-1		;reset
intin:	db	0		;intin
	db	0		;dummy in
	db	0
	db	0
	db	0
ttyin:	db	9		;vector Tastatur
	db	0		;instop
	db	0		;inbyte
	db	0		;dummy
clkin:	db	8		;vector Timer
	db	0		;instop
	db	0		;inbyte
	db	0		;dummy
diskin:	db	0eh		;vector Disk
	db	0		;instop
	db	0		;inbyte
	db	0		;dummy
intout:	db	0		;intout
	db	0		;dummy
	db	0		;dummy out
	db	0
	db	0
	db	0
ttyout:	db	0		;outstop TTY
	db	0		;dummy
	dw	0		;noutb
lpout:	db	0		;outstop LP
	db	0		;dummy
	dw	0		;noutb
diskout	db	0		;outstop Disk
	db	0		;dummy
	dw	0		;noutb
hdtyp:	db	0		;Harddisk 0 Typ
	db	0		;Harddisk 1 Typ
date:	db	0		;sec Date/Time
	db	0		;min
	db	0		;hour
	db	0		;day
	db	0		;mon
	db	0		;year
ttybuf:	blkb	800h
lpbuf:	blkb	100h
diskrq:	blkb	10h


	ORG	0E05BH
RESET:	JMP	START

	ORG	0E2C3H		;----- NMI ENTRY
NMI_INT:
	JMP	NMIINT

;------ FIXED DISK PARAMETER TABLE

FD_TBL:				;Drive 5 MByte
	DW	500		;CYLINDERS
	DB	2		;HEADS
	DW	0		;nicht benutzt
	DW	-1		;(WRITE PRE-COMPENSATION CYL)
	DB	0		;nicht benutzt
	DB	0		;(CONTROL BYTE)
	DB	0,0,0		;nicht benutzt
	DW	0		;(LANDING ZONE) nicht benutzt
	DB	10		;SECTORS/TRACK
	DB	0		;nicht benutzt

FD1_TBL:			;Drive 10 MByte
	DW	500		;CYLINDERS
	DB	4		;HEADS
	DW	0		;nicht benutzt
	DW	-1		;(WRITE PRE-COMPENSATION CYL)
	DB	0		;nicht benutzt
	DB	0		;(CONTROL BYTE)
	DB	0,0,0		;nicht benutzt
	DW	0		;(LANDING ZONE) nicht benutzt
	DB	10		;SECTORS/TRACK
	DB	0		;nicht benutzt

FD2_TBL:			;Drive 20 MByte
	DW	500		;CYLINDERS
	DB	8		;HEADS
	DW	0		;nicht benutzt
	DW	-1		;(WRITE PRE-COMPENSATION CYL)
	DB	0		;nicht benutzt
	DB	0		;(CONTROL BYTE)
	DB	0,0,0		;nicht benutzt
	DW	0		;(LANDING ZONE) nicht benutzt
	DB	10		;SECTORS/TRACK
	DB	0		;nicht benutzt

	ORG	0E6F2H		;------ BOOT LOADER INTERRUPT
BOOTSTRAP:
	JMP	BOOT

	ORG	0E739H		;----- RS232
RS232_IO:
	JMP	RS232

	ORG	0E82EH		;------ KEYBOARD
KBOARD_IO:
	JMP	KBOARD

	ORG	0E987H		;------ KEYBOARD INT
KB_INT:
	JMP	KBINT

	ORG	0EC59H		;------ DISKETTE I/O
DKETTE_IO:
	JMP	DISK

	ORG	0EF57H		;------ DISKETTE INT
DISK_INT:
	JMP	DISKINT

	ORG	0EFC7H		;------ DISK BASE
DISK_BASE:
	DB	0
	DB	0
	DB	0
	DB	2		;512 Byte/Sector
	DB	9		;letzter Sector
	DB	1BH		;GPL
	DB	0FFH
	DB	54H		;GPL fuer Format
	DB	0F6H		;Datenbyte fuer Format
	DB	0
	DB	0

	ORG	0EFD2H		;------ PRINTER IO
PRINTR_IO:
	JMP	PRINTER

	ORG	0F045H		;------ VIDEO IO

.EXTERNAL SET_MODE;NEAR
.EXTERNAL SET_CTYPE;NEAR
.EXTERNAL SET_CPOS;NEAR
.EXTERNAL READ_CURSOR;NEAR
.EXTERNAL READ_LPEN;NEAR
.EXTERNAL ACT_DISP_PAGE;NEAR
.EXTERNAL SCROLL_UP;NEAR
.EXTERNAL SCROLL_DOWN;NEAR
.EXTERNAL READ_AC_CURRENT;NEAR
.EXTERNAL WRITE_AC_CURRENT;NEAR
.EXTERNAL WRITE_C_CURRENT;NEAR
.EXTERNAL SET_COLOR;NEAR
.EXTERNAL WRITE_DOT;NEAR
.EXTERNAL READ_DOT;NEAR
.EXTERNAL WRITE_TTY;NEAR
.EXTERNAL VIDEO_STATE;NEAR

M1:	DW	SET_MODE	;TABLE OF ROUTINES WITHIN VIDEO I/O
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
M1L	EQU	$-M1

	ORG	0F065H
VIDEO_IO:
	JMP	VIDEO

	ORG	0F0A4H
VIDEO_PARMS:
	DB	0


	ORG	0F841H		;------ MEMORY SIZE
MEMSIZE_D:
	JMP	MEMSIZE

	ORG	0F84DH		;------ EQUIPMENT DETERMINE
EQUIPMENT:
	JMP	EQUIP

	ORG	0F859H		;------ CASSETTE (NO BIOS SUPPORT)
CASSET_IO:
	JMP	CASSETTE

	ORG	0FE6EH		;------ TIME OF DAY
T_OF_DAY:
	JMP	TOFDAY

	ORG	0FEA5H		;------ TIMER INTERRUPT
TIMER_INT:
	JMP	TIMERINT

	ORG	0FEE3H		;------ VECTOR TABLE
VECTOR_TABLE:
	DW	DUMMY_RET	;INT 0
	DW	DUMMY_RET	;INT 1
	DW	NMI_INT		;INT 2
	DW	DUMMY_RET	;INT 3
	DW	DUMMY_RET	;INT 4
	DW	PR_SCREEN	;INT 5
	DW	DUMMY_RET	;INT 6
	DW	DUMMY_RET	;INT 7
	DW	TIMER_INT	;INT 8
	DW	KB_INT		;INT 9
	DW	DUMMY_RET	;INT A
	DW	DUMMY_RET	;INT B
	DW	DUMMY_RET	;INT C
	DW	DUMMY_RET	;INT D
	DW	DISK_INT 	;INT E
	DW	DUMMY_RET	;INT F

;------ SOFTWARE INTERRUPTS

	DW	VIDEO_IO 	;INT 10H
	DW	EQUIPMENT	;INT 11H
	DW	MEMSIZE_D	;INT 12H
	DW	DKETTE_IO	;INT 13H
	DW	RS232_IO 	;INT 14H
	DW	CASSET_IO	;INT 15H
	DW	KBOARD_IO	;INT 16H
	DW	PRINTR_IO	;INT 17H
	DW	DUMMY_RET	;INT 18H
	DW	BOOTSTRAP	;INT 19H
	DW	T_OF_DAY	;INT 1AH
	DW	DUMMY_RET	;INT 1BH -- KEYBOARD BREAK ADDR
	DW	DUMMY_RET	;INT 1CH -- TIMER BREAK ADDR
	DW	VIDEO_PARMS	;INT 1DH -- VIDEO PARAMETERS
	DW	DISK_BASE	;INT 1EH -- DISK PARMS
	DW	0		;INT 1FH -- POINTER TO VIDEO EXT

	ORG	0FF53H		;------ DUMMY INT HANDLER
DUMMY_RET:
	IRET

	ORG	0FF54H		;------ PRINT SCREEN
PR_SCREEN:
	JMP	PRSCREEN

	ORG	0FFF0H		;------ POWER ON RESET

P_O_R:	DB	0EAH		;HARD CODE JUMP
	DW	RESET		;OFFSET
	DW	0F000H		;SEGMENT

	db	'01/10/88'	;RELEASE MARKER
				;bei 01/10/84 (Original) wird Harddisk-
				;Treiber im Disketten-BIOS benutzt

	ORG	0FFFEH
	DB	0FCH		;THIS PC'S ID

	END
