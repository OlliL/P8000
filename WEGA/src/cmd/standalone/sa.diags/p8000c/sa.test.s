!******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1987/88
	P8000			Modul: sa.test (p_test)

	Bearbeiter:	J. Zabel, J.Kubisch, M.Klatte
	Datum:		1/87 12/87 11/88 7/89
	Version:	1.2

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul sa.test
------------------------------
Dieser Modul umfasst die U8000-Hardwareeigentestprogramme.

Prozeduren
----------
_TEST_
RAM_TEST_SEG0
MMU_TEST
RAM_TEST_SEG
PRIVINSTR_TRAP
SEGMENT_TRAP
CODE_TRAP
CLR_CTL_REG
LD_SDR
RD_SDR
LD_3SDR
SEGMENT_TRAP_TEST
BY_WO_CMP
FAT_ERR
WR_TESTNR
WR_2SP
WR_2BS
MSG_ERROR
_MSG_MAXSEG
CRC_TEST
PIO_TEST1
PIO_ISR
CTC_TEST1
CTC_TEST2
CTC_TEST3
CTC_INIT
CTC_ISR
SIO_TEST1
SIO0_A_TEST2
SIO0_B_TEST2
SIO1_A_TEST2
SIO1_B_TEST2
SIO_INIT
SIO_ISR
==============================================================================!


satest module


!anfang prom.include!
!******************************************************************************
allgemeine Vereinbarungen (Stand: 12.12.1986)
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
externe Prozeduren und Marken
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  EXTERNAL
INIT_CTC0_0		procedure		!Modul p_init!
INIT_CTC0_1		procedure		!Modul p_init!
INIT_SIO0_A		procedure		!Modul p_init!
INIT_SIO0_B		procedure		!Modul p_init!
CMDLOP			procedure		!Modul p_init!
ERROR			procedure		!Modul p_init!
NMI_INT			procedure		!Modul p_init!

PTY_INT			procedure		!Modul p_term!
PTY_ERR			procedure		!Modul p_term!
KOPPEL_INT		procedure		!Modul p_term!
INCPTR			procedure		!Modul p_term!
TYRD			procedure		!Modul p_term!
TYWR			procedure		!Modul p_term!
TYRD_UDOS_INT		procedure		!Modul p_term!
TYRD_UDOS_POL		procedure		!Modul p_term!
TYWR_UDOS_INT		procedure		!Modul p_term!
TYWR_UDOS_POL		procedure		!Modul p_term!
TYWR_KOPPEL_INT		procedure		!Modul p_term!
TYWR_KOPPEL_POL		procedure		!Modul p_term!
KP_OUTB			procedure		!Modul p_term!
KP_INB			procedure		!Modul p_term!
RD_LINE_BFF		procedure		!Modul p_term!
RD_LINE_INBFF		procedure		!Modul p_term!
WR_CRLF			procedure		!Modul p_term!
WR_MSG			procedure		!Modul p_term!
WR_OUTBFF_CR		procedure		!Modul p_term!
WR_OUTBFF		procedure		!Modul p_term!
HTOB			procedure		!Modul p_term!
BTOH16			procedure		!Modul p_term!
BTOH8			procedure		!Modul p_term!
GET_CHR			procedure		!Modul p_term!
GET_SIGN		procedure		!Modul p_term!
GET_SIGNW		procedure		!Modul p_term!
GET_SGNR		procedure		!Modul p_term!
GET_HEXZ		procedure		!Modul p_term!
GET_ADR			procedure		!Modul p_term!
GET_PAR			procedure		!Modul p_term!
RD_DATA			procedure		!Modul p_term!
GET_DATA		procedure		!Modul p_term!
WR_R5_RDDATA		procedure		!Modul p_term!
WR_OBF_RDDATA		procedure		!Modul p_term!
PUT_CHR			procedure		!Modul p_term!
PUT_3C			procedure		!Modul p_term!
PUT_2CHR		procedure		!Modul p_term!
PUT_SEGNR		procedure		!Modul p_term!
PUT_SGNR		procedure		!Modul p_term!

DISPLAY			procedure		!Modul p_comm!
COMPARE			procedure		!Modul p_comm!
FILL			procedure		!Modul p_comm!
MOVE			procedure		!Modul p_comm!
PORT_READ		procedure		!Modul p_comm!
PORT_WRITE		procedure		!Modul p_comm!
PORT_READ_SPECIAL	procedure		!Modul p_comm!
PORT_WRITE_SPECIAL	procedure		!Modul p_comm!
HARD_DISK_READ		procedure		!Modul p_comm!
HARD_DISK_WRITE		procedure		!Modul p_comm!
FLOPPY_FORMAT		procedure		!Modul p_comm!
FLOPPY_READ		procedure		!Modul p_comm!
FLOPPY_WRITE		procedure		!Modul p_comm!

REGISTER		procedure		!Modul p_brk!
SAVREG			procedure		!Modul p_brk!
BREAK			procedure		!Modul p_brk!
GO			procedure		!Modul p_brk!
GO_NXT			procedure		!Modul p_brk!
NEXT			procedure		!Modul p_brk!
RETI_CTC0_3		procedure		!Modul p_brk!


GET			procedure		!Modul p_gesa!
SAVE			procedure		!Modul p_gesa!
QUIT8			procedure		!Modul p_gesa!
QUIT8_RESET		procedure		!Modul p_gesa!
FLOPPY			procedure		!Modul p.gesa!
RM_GET			procedure		!Modul p_gesa!
RM_SAVE			procedure		!Modul p_gesa!
RM_QUIT			procedure		!Modul p_gesa!

OS_BOOT			procedure		!Modul p_boot!
AUTOBOOT		procedure		!Modul p_boot!

DISK			procedure		!Modul p_disk!
wdc			procedure		!Modul wdc   !

QUIT			procedure		!Modul p_ldsd!
LOAD			procedure		!Modul p_ldsd!
MCZ_INT			procedure		!Modul p_ldsd!
MCZ_ERR			procedure		!Modul p_ldsd!
TYWR_MCZ		procedure		!Modul p_ldsd!
WOCF_MCZ		procedure		!Modul p_ldsd!
SEND			procedure		!Modul p_ldsd!


! Werte aus dem Arbeitsspeicher (Modul p_ram) !

  EXTERNAL
RAM_ANF		ARRAY [%10 WORD]
NSP_OFF		WORD
		ARRAY [%3F WORD]	!Stackbereich!
STACK		WORD
		ARRAY [4 WORD]
_numterm	WORD 			!Anzahl der Terminals!
XEMPTY		BYTE			!fuer interruptgesyeuerten Terminal-
					 betrieb ueber Koppelschnittstelle:
					 00 - Sendepuffer SIO0_B (8-Bit-Teil)
									leer
					 FF - Sendepuffer SIO0_B (8-Bit-Teil)
								  nicht leer!
		BYTE
KONV		BYTE			!Hilfsbyte zur Konvertierung!
INBFF		ARRAY [%80 BYTE]	!Eingabepuffer!
OUTBFF		ARRAY [%80 BYTE]	!Ausgabepuffer!
PTYBFF		ARRAY [%100 BYTE]	!Eingabepuffer fuer interrupt-
					 gesteuerte Eingabe vom Terminal-
					 kanal!
MCZBFF		ARRAY [%0EA BYTE]	!Ein-/Ausgabepuffer fuer ZLOAD/ZSEND!
MAXLEN		WORD			!maximale Zeilenlaenge!
INPTR		WORD			!Zeiger fuer Eingabepuffer INBFF!
OUTPTR		WORD			!Zeiger fuer Ausgabepuffer OUTBFF 
		                         (Pufferlaenge)!
COUNT_PTY	WORD			!aktuelle Anzahl der Zeichen in PTYBFF!
INPTR_PTY	WORD			!Eingabezeiger fuer PTYBFF
					 (naechster freier Platz)!
OUTPTR_PTY	WORD			!Ausgabezeiger fuer PTYBFF
					 (naechstes auszulesende Zeichen)!
INPTR_MCZ	WORD			!Eingabezeiger fuer MCZBFF!
OUTPTR_MCZ	WORD			!Ausgabezeiger fuer MCZBFF!
B_WORD		WORD			!Speicherinhalt auf der BREAK-Adresse!
B_ADR_S		WORD			!Segmentnummer der BREAK-Adresse!
B_ADR_O		WORD			!Offset der BREAK-Adresse!
RR14_V		LONG			!VARIABLER STACK, FUER NEXT UND BREAK!
W23B4		WORD
FCW_V		WORD			!VARIABLES FCW FUER NEXT UND BREAK!
PORT_BF		WORD			!Puffer fuer gelesenen bzw. zu
					 schreibenden PORT-Datenwert!	
PORT_IO		WORD			!Adresse des PORT-I/O-Programms!
		WORD			!unbenutzt!
SV_R		ARRAY [14 WORD]		!Registerrettungsbereich!
SVSTK		LONG
PC_SEG		WORD			!Programmcounter (Segmentnummer)!
PC_OFF		WORD			!Programmcounter (Offsetadresse)!
FCW_		WORD			!FCW im Registerrettungsbereich!
RF_CTR		WORD			!Merker: Refresh Control Register!
N4_		WORD			!Normalstack R14!
N5_		WORD			!Normalstack R15!
PS_		WORD			!Program-Status-Area Segment!
PO_	 	WORD			!Program-Status-Area Offset!
		WORD			!unbenutzt!
!Flags!
FLAG0	BYTE
FLAG1	BYTE
!Variablen (initialisiert bei Systemstart)!
CHRDEL	BYTE		!Zeichen loeschen (Backspace %08)!
LINDEL	BYTE		!Zeile loeschen (Line delete %7F)!
XONCHR	BYTE		!Terminalausgabe fortsetzen (^Q %11)!
XOFCHR	BYTE		!Terminalausgabe anhalten (^S %13)!
NULLCT	WORD		!Anzahl der Nullen nach CR bei Terminalausgabe (%0000)!
B_CODE	WORD		!Unterbrechungscode Z8000 (7F00)!
STKCTR	WORD		!Zaehler der Stackoperationen fuer NEXT (%0004)!
NXTCTR	WORD		!SCHRITTZAEHLER FUER NEXT (%0001)!
PROMT	WORD		!Promt-Zeichen (%2000)!
!---------------------------------------------------!
!Arbeitsspeicher fuer Kommando "T" (Hardwareeigentest)!
RG_FELD		ARRAY [%1A WORD]	!Feld zur Registerrettung!
REM_MMU_BCSR	BYTE	!Merker: Inhalt BUS CYCLE STATUS REGISTER!
REM_MMU_VTR	BYTE	!Merker: Inhalt VIOLATION TYPE REGISTER!
REM_MMU_ID	WORD	!Merker: MMU-Identifikation!
REM_ERR_CNT	WORD	!Merker: Stand Fehlerzaehler!
REM_SEGMENT_TRAP WORD	!Merker: urspruengliche Offsetadresse fuer Segmenttrap!
REM_PRIVINSTR_TRAP WORD	!Merker: urspruengl. Offsetadr. fuer Priv.-Instr.-Trap!
REM_MMU_SINOUT	WORD	!Merker: MMU-Ein-/Ausgabewerte!
ERR_CNT		WORD	!Fehlerzaehler!
MAX_SEGNR	WORD	!Merker: hoechste vorhandene Segmentnummer!
PUDC_PTR	WORD	!Absoluter Zeiger fuer Zeichenkette T_PUDC!
REM_PUDC	WORD	!Merker: Text T_PUDC vollstaendig ausgegeben
		                 (%0000-->nein / %FFFF-->ja)!
ERRPAR_ID	WORD	!Spezifizierung der 4 Parameter der auszugebenden
			 Fehlerzeile (Prozedur MSG_ERROR)!
REM_WDC2	WORD	!Speicherplatz fuer rueckgelesenen Datenwert!
REM_WDC1	WORD	!Speicherplatz fuer Testdatenwert!
REM_MMU1	WORD
REM_MMU2	WORD
REM_INT		WORD	!Merker: Interrupt aufgetreten (0-->nein)!
		ARRAY [%36 WORD] !unbenutzt!
!Arbeitsspeicher fuer Kommandos "GE"/"S"/"O U" und Floppytreiber!
FILENAME_PTR	WORD			!Anfangsadresse Dateiname!
COUNT_UDOS	WORD			!akt. Anzahl der Zeichen in UDOSBFF!
INPTR_UDOS	WORD			!Eingabezeiger fuer UDOSBFF 
					 (naechster freier Platz)!
OUTPTR_UDOS	WORD			!Ausgabezeiger fuer UDOSBFF
			 		 naechstes auszulesende Zeichen)!
UDOSBFF		ARRAY [%100 BYTE]	!Eingabepuffer fuer interrupt-
					 gesteuerte Eingabe bei Dateitransfer
					 von/zu UDOS bzw. Blocktransfer!


! System Calls !

  CONSTANT
SC_SEGV			:= %01		!Modul p.term!
SC_NSEGV		:= %02		!Modul p.term!
SC_TYRD			:= %04		!Modul p.term!
SC_TYWR			:= %06		!Modul p.term!
SC_RD_LINE_BFF		:= %08		!Modul p.term!
SC_WR_CRLF		:= %0A		!Modul p.term!
SC_WR_MSG		:= %0C		!Modul p.term!
SC_BTOH16		:= %0E		!Modul p.term!
SC_WR_OUTBFF_CR		:= %10		!Modul p.term!
SC_BTOH8		:= %12		!Modul p.term!
SC_PUT_SEGNR		:= %14		!Modul p.term!
SC_PUT_CHR		:= %16		!Modul p.term!
SC_WOCF_MCZ		:= %18		!Modul p.ldsd!
SC_TYWR_MCZ		:= %1A		!Modul p.ldsd!
SC_CMDLOP		:= %1C		!Modul p.init!
SC_DISK			:= %1E		!Modul p.disk!
SC_TYRD_UDOS_INT	:= %20		!Modul p.term!
SC_TYWR_UDOS_INT	:= %22		!Modul p.term!
SC_FLOPPY		:= %24		!Modul p.gesa!
SC_WR_OUTBFF		:= %26		!Modul p.term!

! Program Status Area (initialisiert bei Systemstart) !

  CONSTANT
PTYPORT			:= %40AC

PSAREA			:= %4400	
!		                    ID    FCW   PCSEG PCOFF!
	       !ARRAY [4 WORD]  := [%0000 %0000 %0000 %0000]
		               RESERVED!
	       !ARRAY [4 WORD]  := [%0000 %4000 %8000 VI_ERR] 
		               UNIMLEMENTED INSTRUCTIONS!
	       !ARRAY [4 WORD]  := [%0000 %4000 %8000 VI_ERR] 
		               PRIVILEGED INSTRUCTIONS!
	       !ARRAY [4 WORD]  := [%0000 %C000 %8000 SC_ENTRY]
		               SYSTEM_CALL INSTRUCTIONS!
	       !ARRAY [4 WORD]  := [%0000 %4000 %8000 MMU_ERR] 
		               SEGMENT TRAP!
PSA_NMI			:= %4428 	
	       !ARRAY [4 WORD]  := [%0000 %4000 %8000 AUTOBOOT bzw. NMI_INT] 
		               NONMASKABLE INTERRUPT
		               (AUTOBOOT vor erster Terminaleingabe;
		                NMI_INT nach erster Terminaleingabe)!
               !ARRAY [4 WORD]  := [%0000 %4000 %8000 VI_ERR] 
		               NONVECTORED INTERRUPT!

! VECTORED INTERRUPT!

               !ARRAY [2 WORD]  := [%0000 %4000] 
		               VECTORED INTERRUPT (ID, FCW)!

! VI - Sprung-Tabelle (128 Eintraege) !
!		                    SEG   OFFSET!

!CTC0!
VI_CTC0_0		:= %443C
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
VI_CTC0_1		:= %4440
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
VI_CTC0_2		:= %4444
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
VI_CTC0_3		:= %4448	
	       !ARRAY [2 WORD]  := [%8000 VI_ERR bzw. GO_INT bzw. NXT_INT]
		               fuer GO bzw. NEXT!

!PIO1!
VI_PIO1_B		:= %444C
	       !ARRAY [2 WORD]  := [%8000 KOPPEL_INT]
		               VI Zeichen empfangen (von Koppelschnittstelle)!
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
               !ARRAY [2 WORD]  := [%8000 VI_ERR] !
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !

!SIO0!
VI_SIO0_B		:= %445C
       	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
	       !ARRAY [2 WORD]  := [%8000 PTY_INT]
		               VI Zeichen empfangen (fuer Terminaleingabe)!
	       !ARRAY [2 WORD]  := [%8000 PTY_ERR]
		               VI Empfangssonderfall (fuer Empfangsfehler)!
VI_SIO0_A		:= %446C
       	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
	       !ARRAY [2 WORD]  := [%8000 VI_ERR] !
	       !ARRAY [2 WORD]  := [%8000 MCZ_INT]
		               VI Zeichen empfangen (fuer MCZ-Eingabe)!
	       !ARRAY [2 WORD]  := [%8000 MCZ_ERR]
		               VI Empfangssonderfall (fuer Empfangsfehler)!

! Rest der 128 Eintraege der VI-Sprung-Tabelle!
VI_TAB_REST		:= %447C
               !ARRAY [224 WORD]  := [%8000 VI_ERR] !
 
! Adressvereinbarungen fuer EPROM-Test !
   CONSTANT
MON16_1H		:= %3FF0
N_MON16_1H		:= %3FF2
MON16_1L		:= %3FF4
N_MON16_1L		:= %3FF6
MON16_2H		:= %3FF8
N_MON16_2H		:= %3FFA
MON16_2L		:= %3FFC
N_MON16_2L		:= %3FFE

! spezielle ASCII-Zeichen !

  CONSTANT
SOH	:= %01
STX	:= %02
ETX	:= %03
EOT	:= %04
ACK	:= %06
BEL	:= %07
BS	:= %08
LF	:= %0A
CR	:= %0D
NAK	:= %15
CAN	:= %18
ESC	:= %1B

! Hardwareadressen !

  CONSTANT

!8-Bit-Teil!

SIO0D_B_8	:= %26	!SIO fuer Terminal!
SIO0C_B_8	:= %27

!16-Bit-Teil!

SIO0	:= %FF81	!Kanal A (XB1/TTY4): LOAD/SEND/QUIT;
			 Kanal B (XB2/TTY5): Terminal, wenn 8-Bit-Teil inaktiv!
SIO0D_A	:= SIO0 + 0
SIO0D_B	:= SIO0 + 2
SIO0C_A	:= SIO0 + 4
SIO0C_B	:= SIO0 + 6

SIO1	:= %FF89	!Kanal A (XB3/TTY6): unbenutzt
			 Kanal B (XB4/TTY7): unbenutzt!
SIO1D_A	:= SIO1 + 0
SIO1D_B	:= SIO1 + 2
SIO1C_A	:= SIO1 + 4
SIO1C_B	:= SIO1 + 6

PIO0	:= %FF91	!Koppel-PIO zum 8-Bit-Teil 
			 fuer Terminal und Kommandos "GE", "S", 
			 "Q", "QRES", "O U" und Floppytreiber
			 Kanal A: Datenausgabe
			 Kanal B: Statusausgabe/Steuerport!
PIO0D_A	:= PIO0 + 0
PIO0D_B	:= PIO0 + 2
PIO0C_A	:= PIO0 + 4
PIO0C_B	:= PIO0 + 6

PIO1	:= %FF99	!Koppel-PIO zum 8-Bit-Teil
			 fuer Terminal und Kommandos "GE", "S",
			 "Q", "QRES", "O U" und Floppytreiber
                         Kanal A: Dateneingabe
			 Kanal B: Statuseingabe/Steuerport!
PIO1D_A	:= PIO1 + 0
PIO1D_B	:= PIO1 + 2
PIO1C_A	:= PIO1 + 4
PIO1C_B	:= PIO1 + 6

PIO2	:= %FFA1	!(XB5): Winchester
                         Kanal A: Datenein-/-ausgabe; Kanal B: Steuerport!
PIO2D_A	:= PIO2 + 0
PIO2D_B	:= PIO2 + 2
PIO2C_A	:= PIO2 + 4
PIO2C_B	:= PIO2 + 6

CTC0	:= %FFA9
CTC0_0	:= CTC0 + 0	! Baud 0 - SIO0, Kanal A!
CTC0_1	:= CTC0 + 2	! Baud 1 - SIO0, Kanal B!
CTC0_2	:= CTC0 + 4 	! Baud 2 - SIO1, Kanal A!
CTC0_3	:= CTC0 + 6	! Next !

CTC1	:= %FFB1
CTC1_0	:= CTC1 + 0	! Baud 3 - SIO1, Kanal B!
CTC1_1	:= CTC1 + 2
CTC1_2	:= CTC1 + 4
CTC1_3	:= CTC1 + 6

S_BNK	:= %FFC1	!SWITCH-BANK:
			 Bit0-3 Ausgabebits; Bit4-7 Eingabebits (DIL-Schalter):
			 Bit 0: =0 - Monitor PROM+RAM einschalten
				=1 - Monitor PROM+RAM ausschalten
			 Bit 1: =0 - MMU's ausschalten; =1 - MMU's einschalten
			 Bit 2: Seg. Usr.
			 Bit 3: =0 - Clear Parity
			 Bit 5,4: Baudrate Terminal (unbenutzt)
			 Bit 7,6: Autoboot Device (unbenutzt)!

SBREAK	:= %FFC9	!System Break Register!
NBREAK	:= %FFD1	!Normal Break Register!
RETI_P	:= %FFE1	!RETI-Port fuer Schaltkreise des U800-Systems!

ALL_MMU	  := %F0
STACK_MMU := %F6
DATA_MMU  := %FA
CODE_MMU  := %FC

!
Moegliche Statusausgaben an 8-Bit-Teil von 16-Bit-Teil 
fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO0_B)
(INT8-Bit jeweils = 0)
!

  CONSTANT
STAT8_ENDE	:= %00	!Ende eines Write-Requests!
STAT8_READ	:= %02	!Read-Request an einen Peripheriebaustein 8-Bit-Teil!
STAT8_WRITE	:= %04	!Write-Request an einen Peripheriebaustein 8-Bit-Teil!

STAT8_UDOS	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers
			 bzw. eines Blocktransfers von/zu UDOS!
STAT8_QUIT	:= %1E	!Rueckkehr zum 8-Bit-Teil (Datenbyte "Q" oder "R")!

!
Moegliche Statuseingaben von 8-Bit-Teil an 16-Bit-Teil 
fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO1_B)
(INT16-Bit jeweils = 0)
!

  CONSTANT
STAT16_ENDE	:= %00	!Beantwortung eines Read-Requests!

STAT16_SIO0_B_R	:= %04	!REC INT (SIO0_B-Empfaengerinterrupt)!
STAT16_SIO0_B_X	:= %24	!TRANS INT (SIO0_B-Interrupt Sendepuffer leer)!
STAT16_SIO0_B_E	:= %44	!EXT STAT INT (SIO0_B-Externer-Statusinterrupt)!
STAT16_SIO0_B_S	:= %64	!SPEC REC INT (SIO0_B-Interrupt Empfangssonderfall)!

STAT16_UDOS	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers
			 bzw. eines Blocktransfers von/zu UDOS!
!ende prom.include!

! modulspezifische Vereinbarungen !

!++++++++++++
SIO-Kommandos
++++++++++++!
 
!Command Identifiers and Values
Includes all control bytes for asynchronous and synchronous I/O !
 
  CONSTANT
!WR0 Commands!
SIO_R0	:=	%00	!SIO register pointers!
SIO_R1	:=	%01
SIO_R2	:=	%02
SIO_R3	:=	%03
SIO_R4	:=	%04
SIO_R5	:=	%05
!SIO_R6	:=	%06!
!SIO_R7	:=	%07!
 
!COMM0	:=	%00!	!Null Code!
!COMM1	:=	%08!	!Send Abort (SDLC)!
COMM2	:=	%10	!Reset Ext/Stat Int!
COMM3	:=	%18	!Channel Reset!
!COMM4	:=	%20!	!Enable Int On Next Rx Char!
!COMM5	:=	%28!	!Reset Tx Int Pending!
!COMM6	:=	%30!	!Error Reset!
 
RFI	:=	%38	!Return from Int!
!RRCC	:=	%40!	!Reset Rx CRC Checker!
!RTCG	:=	%80!	!Reset Tx CRC Generator!
!RTUEL	:=	%C0!	!Reset Tx Under/EOM Latch!
 
!WR1 Commands!
!WAIT	:=	%00!	!Wait function!
DRCVRI	:=	%00	!Disable Receive Interrupts!
!EXTIE	:=	%01!	!External interrupt enable!
XMTRIE	:=	%02	!Transmit Int Enable!
SAVECT	:=	%04	!Status affects vector!
!FIRSTC	:=	%08!	!Rx interrupt on first character!
!PAVECT	:=	%10!	!Rx interrupt on all characters!
			!(parity affects vector)!
PDAVCT	:=	%18	!Rx interrupt on all characters!
			!(parity doesn't affect vector)!
!WRONRT	:=	%20!	!Wait/ready on receive!
!RDY	:=	%40!	!Ready function!
!WRDYEN	:=	%80!	!Wait/Ready enable!
 
!WR2 Commands!
!IV	:=	%00!
 
!WR3 Commands!
!B5	:=	%00!	!Receive 5 bits/character!
!RENABLE :=	%01!	!Receiver enable!
!SCLINH	:=	%02!	!Sync character load inhibit!
!ADSRCH	:=	%04!	!Address search mode!
!RCRCEN	:=	%08!	!Receive CRC enable!
!HUNT	:=	%10!	!Enter hunt mode!
!AUTOEN	:=	%20!	!Auto enable!
!B7	:=	%40!	!Receive 7 bits/character!
!B6	:=	%80!	!Receive 6 bits/character!
!B8	:=	%C0!	!Receive 8 bits/character!
 
!WR4 Commands!
!SYNC	:=	%00!	!Sync modes enable!
NOPRTY	:=	%00	!Disable partity!
!ODD	:=	%00!	!Odd parity!
!MONO	:=	%00!	!8 bit sync character!
!C1	:=	%00!	!x1 clock mode!
!PARITY	:=	%01!	!Enable Parity!
!EVEN	:=	%02!	!Even parity!
!S1	:=	%04!	!1 stop bit/character!
!S1HALF	:=	%08!	!1+1/2 stop bit/character!
S2	:=	%0C	!2 stop bits/character!
!BISYNC	:=	%10!	!16 bit sync character!
!SDLC	:=	%20!	!SDLC mode!
!ESYNC	:=	%30!	!External sync mode!
!C16	:=	%40!	!x16 clock mode!
C32	:=	%80	!x32 clock mode!
!C64	:=	%C0!	!x64 clock mode!
 
!WR5 Commands!
!T5	:=	%00!	!Transmit 5 bits/character!
!XCRCEN	:=	%01!	!Transmit CRC enable!
!RTS	:=	%02!	!Request to send!
!SELCRC	:=	%04!	!Select CRC-16 polynomial!
XENABLE	:=	%08	!Transmitter enable!
!XBREAK	:=	%10!	!Send break!
!T7	:=	%20!	!Transmit 7 bits/character!
!T6	:=	%40!	!Transmit 6 bits/character!
T8	:=	%60	!Transmit 8 bits/character!
!DTR	:=	%80!	!Data terminal ready!

!+++++++
Texte
+++++++!

  INTERNAL

T_ERR	WORD := %000A
	ARRAY [* BYTE] :='*** ERROR '
T_FERR	WORD := %0010
	BYTE := BEL
	ARRAY [* BYTE] := '*** FATAL ERROR'
T_SEG	WORD := %0009
	ARRAY [* BYTE] := 'SEGMENTED'
T_NSEG	WORD := %000D
	ARRAY [* BYTE] := 'NON-SEGMENTED'
T_JUM	WORD := %0008
	ARRAY [* BYTE] := ' JUMPERS'
T_MAXS	WORD := %0007
	ARRAY [* BYTE] := 'MAXSEG='

T_CUM_ERR1:
	WORD := %001F
	ARRAY [* BYTE]  := 'Seit Start von sa.diags traten '
T_CUM_ERR2:
	WORD := %0006
	ARRAY [* BYTE]  := 'keine '
T_CUM_ERR3:

	WORD := %000B
	ARRAY [* BYTE] := 'Fehler auf.'  

!+++++++++
MMU-Listen
+++++++++!

 INTERNAL
MMU_LISTE1:			!MMU-Adressen!
	WORD	:= CODE_MMU
	WORD	:= DATA_MMU
	WORD	:= STACK_MMU
	WORD	:= CODE_MMU
	WORD	:= DATA_MMU
MMU_LISTE2:			!Adressen der SDR-Tabellen!
	WORD	:= SDRT0
	WORD	:= SDRT1
	WORD	:= SDRT1

MMU_LISTE3:			!1. Byte: Ausgabewert fuer SAR;
				 2. Byte: Ausgabewert fuer DSCR;
 				 3. Byte: Datenwert, der rueckgelesen werden
					  muesste !
	ARRAY [3 BYTE] := [%00 %00 %00]
	ARRAY [3 BYTE] := [%00 %01 %01]
	ARRAY [3 BYTE] := [%00 %02 %02]
	ARRAY [3 BYTE] := [%00 %03 %03]
	ARRAY [3 BYTE] := [%01 %00 %04]
	ARRAY [3 BYTE] := [%02 %00 %08]
	ARRAY [3 BYTE] := [%04 %00 %10]
	ARRAY [3 BYTE] := [%08 %00 %20]
	ARRAY [3 BYTE] := [%10 %00 %40]
	ARRAY [3 BYTE] := [%20 %00 %80]

! Segmentdeskriptoren 
				     ___________Segmentbasisadresse
				     |    ______Limitfeld
				     |    | ____Attributfeld
				     |    | |				!
MMU_LISTE4:	array [2 word] := [%0000 %FF00] ! 256 Bloecke		!
MMU_LISTE5:	array [2 word] := [%0000 %0000] !   1 Block		!
MMU_LISTE6:	array [2 word] := [%0000 %FF20] !   1 Block  /down write!
MMU_LISTE7:	array [2 word] := [%0000 %FF01] ! 256 Bloecke/ read only!
MMU_LISTE8:	array [2 word] := [%0100 %0000] !   1 Block		!
MMU_LISTE9:	array [2 word] := [%0101 %0000] !   1 Block		!
MMU_LISTE10:	array [2 word] := [%0000 %0020] ! 256 Bloecke/down write!

! SDR - Tabellen !

SDRT0:	array [64 long]
SDRT1:	array [64 long]
SDRT2:	array [64 long]
SDRT3:	array [64 long]
SDRT4:	array [64 long]

!*******************************
Speicherbereich fuer Fehlerliste
*******************************!

   INTERNAL
PAR_CNT_ERR:
	ARRAY [4 BYTE] := [%25 %64 %20 %0]   !Parameter fuer _printf!
CNT_ERR_LST:
	WORD 			!Zaehler fuer eingetragene Fehler (max. 20)!
CUM_CNT_ERR:
	WORD			!Kumulativer Zaehler!
PTR_ERR_LST:
	WORD			!Zeiger auf Fehlerliste!
ERR_LST:
	ARRAY [120 WORD]	!Speicherbereich fuer 20 Fehlermeldungen!


!*****************************************************
Speicherreservierungen fuer WDC-Test (Procedure: _WDC_)
******************************************************!

TYPE
BUFFER ARRAY [%200 BYTE]	! Sektorpuffer  !

INTERNAL
TDAT BUFFER			! Testdatenfeld !
WDC_BUFF BUFFER			! Sektorpuffer  !

!*********
Prozeduren
*********!

!------------------------------------------------------------------------------
In diesem Modul wird das Register rh1 als Flagbyte verwendet. Die einzelnen
Bits haben dabei folgende Bedeutung:
Bit 0 = 0  RAM-Test nicht in Segment 0
      = 1  RAM-Test in Segment 0
           (gesetzt in RAM_TEST_SEG0;
            getestet in BY_WO_CMP, WR_TESTNR)
Bit 1 = 0  Byte-Vergleich
      = 1  Word-Vergleich
           (gesetzt in RAM_TEST_SEG, MMU_TEST;
            getestet in BY_WO_CMP)
Bit 2 = 0  ERR_CNT wird in MSG_ERROR inkrementiert
      = 1  ERR_CNT wird in MSG_ERROR nicht inkrementiert
           (getestet und rueckgesetzt in MSG_ERROR)
Bit 6 = 0  MSG_ERROR wurde seit dem letzten Loeschen des Bits nicht
           schon einmal durchlaufen
      = 1  MSG_ERROR wurde bereits vorher einmal durchlaufen
           (gesetzt und getestet in MSG_ERROR)
Bit 7 = 0  MSG_ERROR wurde seit dem letzten Loeschen des Bits nicht
           zweimal durchlaufen
      = 1  MSG_ERROR wurde zweimal durchlaufen
           (gesetzt in MSG_ERR;
            getestet in RAM_TEST_SEG, MMU_TEST, SEGMENT_TRAP_TEST)
------------------------------------------------------------------------------!

!+++++++++++++++++++++++++++
PROCEDURE PLACE_
platzreservierung fuer %8000
+++++++++++++++++++++++++++!

   INTERNAL
     PLACE_   procedure
	entry
		 wval	%0000
	end	PLACE_

!++++++++++++++++++++++++++++
PROCEDURE TEST_
TEST-Routine (Monitorkommando)
++++++++++++++++++++++++++++!

  GLOBAL
     _EPROM_ procedure
      entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7

	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
!-------------------------
Testschritt 40  (EPROM)
Test der EPROM-Pruefsummen
-------------------------!

!Fuer jeden EPROM wird zuerst getestet, ob die im Modul p_crc abgelegte
Pruefsumme fehlerfrei ist (durch Vergleich mit ihrem ebenfalls in p_crc
abgelegten negierten Wert) - danach wird die Pruefsumme des EPROM's berechnet
und mit dem in p_crc abgelegten Wert verglichen
--- Fehler 40 bei fehlerhafter Pruefsumme ---!

	ld	r8, #%40	!Fehler 40: falsche EPROM-Pruefsumme!
	ld	ERRPAR_ID, #%0000 !Fehlerparameter fuer Fehler 40:
				 keine Parameter!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r2, #%0000	!Anfangsadresse fuer Pruefsummenbildung!
	ld	r3, #%1000	!Bytezahl!
	ld	r4, MON16_1H	!erwartete Pruefsumme (aus Modul p_crc)!
	ld	r5, N_MON16_1H	!erwartete negierte Pruefsumme (aus p_crc)!
	calr	CRC_TEST	!Pruefsummentest fuer EPROM MON16_1H; 
				 bei Fehler Abbruch des Testes (FATAL ERROR)!

	ld	r2, #%0001	!Anfangsadresse fuer Pruefsummenbildung!
	ld	r3, #%1000	!Bytezahl!
	ld	r4, MON16_1L	!erwartete Pruefsumme (aus Modul p_crc)!
	ld	r5, N_MON16_1L	!erwartete negierte Pruefsumme (aus p_crc)!
	calr	CRC_TEST	!Pruefsummentest fuer EPROM MON16_1L; 
				 bei Fehler Abbruch des Testes (FATAL ERROR)!

	ld	r2, #%2000	!Anfangsadresse fuer Pruefsummenbildung!
	ld	r3, #%1000-8	!Bytezahl (letzten 8 Byte nicht in CRC-Bildung
				 einbeziehen, da dort die geraden Werte der
				 EPROM-Pruefsummen abgespeichert sind)!
	ld	r4, MON16_2H	!erwartete Pruefsumme (aus Modul p_crc)!
	ld	r5, N_MON16_2H	!erwartete negierte Pruefsumme (aus p_crc)!
	calr	CRC_TEST	!Pruefsummentest fuer EPROM MON16_2H; 
				 bei Fehler Abbruch des Testes (FATAL ERROR)!

	ld	r2, #%2001	!Anfangsadresse fuer Pruefsummenbildung!
	ld	r3, #%1000-8	!Bytezahl (letzten 8 Byte nicht in CRC-Bildung
				 einbeziehen, da dort die ungeraden Werte der
				 EPROM-Pruefsummen abgespeichert sind)!
	ld	r4, MON16_2L	!erwartete Pruefsumme (aus Modul p_crc)!
	ld	r5, N_MON16_2L	!erwartete negierte Pruefsumme (aus p_crc)!
	calr	CRC_TEST	!Pruefsummentest fuer EPROM MON16_2L; 
				 bei Fehler Abbruch des Testes (FATAL ERROR)!

	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret
   end	_EPROM_
!---------------------------------------------------
Testschritt 46  (SRAM)
Test des statischen RAM auf fehlerhafte Datenleitung
---------------------------------------------------!

!Die Adresse %4000 (1. Adr. des statischen RAM) wird nacheinander mit den Werten
0001,0002,0004,0008,0010,0020,0040,0080,0100,0200,0400,0800,1000,2000,4000,
8000,0001 (jeweils ein Bit gesetzt) beschrieben und wieder rueckgelesen.
--- Fehler 46, wenn ein Wert fehlerhaft rueckgelesen wurde ---!

  GLOBAL
     _STAT_RAM_   procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r8, #%46	!Fehler 46: fehlerhafte Datenleitung!
	ld	ERRPAR_ID, #%0029 !Fehlerparameter fuer Fehler 46,47,48:
				 				r7, r5, r4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #%01	!Word-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r7, #%4000	!r7=Fehlerpar. (Testadresse)!
	ld	r5, #%0001	!r5=Fehlerpar. (Testdatenwert)!
	ldb	rl0, #%11	!rl0:=Anzahl der Testdatenwerte!
TSRAM1:	ld	r6, @r7		!urspruengl. Wert auf Testadresse retten!
	ld	@r7, r5		!Testdatenwert auf Testadr. eintragen!
	ld	r4, @r7		!r4=Fehlerpar. (rueckgel. Datenwert)!
	ld	@r7, r6		!urspruengl. Wert wieder eintragen!
	calr	BY_WO_CMP	!Vergleich r4/r5; ggf. Fehler 46!
	bitb	rh1, #7		!Fehler aufgetreten?!
	jr	nz, TSRAM2	!ja --> naechster Testschritt!
	rl	r5, #1		!nein --> neuer Testdatenwert!
	dbjnz	rl0, TSRAM1
	
!-----------------------------------------------
Testschritt 47/48  (SRAM)
Test auf fehlerhafte '%AAAA'- bzw. '%5555'-Daten
-----------------------------------------------!

!Der statische RAM wird mit dem Datenwert %AAAA bzw. %5555 
beschrieben und wieder rueckgelesen.
--- Fehler 47, wenn ein %AAAA-Wert fehlerhaft rueckgelesen wurde ---
--- Fehler 48, wenn ein %5555-Wert fehlerhaft rueckgelesen wurde ---!

TSRAM2:
	ld	r8, #%47	!Fehler 47: fehlerhafte '%AAAA'-Daten!
	ld	r5, #%AAAA	!r5=Fehlerpar. (Testdatenwert)!
TSRAM3:
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #%01	!Word-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!
	ld	r7, #%4000	!r7=Fehlerpar. (Testadresse)!
	ld	r0, #%400	!r0:=Word-Anzahl statischer RAM!
TSRAM4:	ld	r6, @r7		!urspruengl. Wert auf Testadresse retten!
	ld	@r7, r5		!Testdatenwert auf Testadr. eintragen!
	ld	r4, @r7		!r4=Fehlerpar. (rueckgel. Datenwert)!
	ld	@r7, r6		!urspruengl. Wert wieder eintragen!
	calr	BY_WO_CMP	!Vergleich r4/r5; ggf. Fehler 47 bzw. 48!
	bitb	rh1, #7		!Fehler aufgetreten?!
	jr	nz, TSRAM5	!ja --> naechster Testschritt!
	inc	r7, #2		!nein --> naechste Testadresse!
	djnz	r0, TSRAM4
TSRAM5:
	cp	r5, #%5555	!Testschritt 48 abgearbeitet?!
	jr	z,TSRAM6	!ja, Test beenden!


	ld	r8, #%48	!Fehler 48: fehlerhafte '%5555'-Daten!
	ld	r5, #%5555	!r5=Fehlerpar. (Testdatenwert)!
	jr	TSRAM3

TSRAM6:

	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret
   end   _STAT_RAM_
!----------------------------------------
Testschritt 50  (PIO2)
Test auf fehlerhafte Daten 'AA' oder '55'
----------------------------------------!

!An die Datenkanaele A und B des PIO2 werden die Daten 'AA' bzw. '55'
ausgegeben und rueckgelesen
--- Fehler 50, wenn ein Wert fehlerhaft rueckgelesen wurde ---!

!An diesem PIO ist der Winchestercontroller angeschlossen. Deshalb sind
beim PIO-Test bestimmete Restriktionen einzuhalten, die sonst nicht
noetig waeren!

GLOBAL
  _PIO_  procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r8, #%50         !Fehler 50: fehlerhafte '%AA' bzw. '%55'-Daten!
	ld	ERRPAR_ID, #%0085 !Fehlerparameter fuer Fehler 50:
				 				r6, rl5, rl4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen,
				 Byte-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

				!Restriktion!
	ldb	rl0, #%20	!Bit5=1; BIT6=0!
	outb	PIO2D_B, rl0
	ldb	rl0, #%0F	!Ausgabemode fuer Kanal B!
	outb	PIO2C_B, rl0

	ld	r6, #PIO2D_A	!r6=Fehlerpar. (PIO-Adresse)!
	ld	r7, #PIO2C_A
	calr	PIO_TEST1
	ld	r6, #PIO2D_B	!r6=Fehlerpar. (PIO-Adresse)!
	ld	r7, #PIO2C_B
	calr	PIO_TEST1


	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret
   end	_PIO_


!---------------------------------------------------------
Testschritt 52/53 (WDC)
Test auf fehlerhafte Daten 'AAAA' oder '5555' fuer WDC-RAM
---------------------------------------------------------!

!Der WDC-RAM wird auf der Adresse %2E00 mit den Datenwerten %AAAA bzw.
%5555 beschrieben und wieder rueckgelesen
--- Fehler 52, wenn beim Schreiben ein Fehler vom WDC gemeldet wurde ---
--- Fehler 53, wenn beim Ruecklesen ein Fehler vom WDC gemeldet wurde bzw.
               wenn der rueckgesene Wert nicht mit dem geschriebenen Wert
               uebereinstimmt ---!

GLOBAL
  _WDC_  procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!
	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	ld	r5, #%AAAA	!r5=Fehlerpar. (Testdatenwert)!
TWDC1:	
	ld	r8, #%52	!Fehler 52: Schreiben in WDC-RAM fehlerhaft!
	ld	ERRPAR_ID, #%48	!Fehlerpar. fuer Fehler 52: rl6, r5!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	push	@r15, r5
	push	@r15, r8
	push	@r15, r1
	ld	REM_WDC1, r5	!Testdatenwert abspeichern!
	ld	r7, #4		!Requestcode fuer WDC: Write WDC-RAM!
	ldb	rl6, #%00	!Low-Teil WDC-RAM-Adresse (%2E00)!
	ldb	rl5, #%2E	!High-Teil WDC-RAM-Adresse (%2E00)!
	ld	r8, #%00	!Pufferadresse (Segment)!
	ld	r9, #REM_WDC1	!Pufferadresse (Offset)!
	ld	r10, #2		!Laenge (2 Byte)!
	sc	#SC_DISK
	pop	r1, @r15
	pop	r8, @r15
	pop	r5, @r15
	test	r2		!Returncode!
	jr	z, TWDC2	!kein Fehler aufgetreten!

	ldb	rl6, rl2	!rl6=Fehlerpar. := Returncode!
	push	@r15, r5
	calr	MSG_ERROR	!Fehler 52!
	pop	r5, @r15

TWDC2:
	ld	r8, #%53	!Fehler 53: Lesen aus WDC-RAM fehlerhaft!
	ld	ERRPAR_ID, #%4A	!Fehlerpar. fuer Fehler 53: rl6, r5, r4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #01	!Word-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	push	@r15, r5
	push	@r15, r8
	push	@r15, r1
	ld	REM_WDC2, #0	!Platz fuer rueckgelesenen Datenwert loeschen!
	ld	r7, #3		!Requestcode fuer WDC: Read WDC-RAM!
	ldb	rl6, #%00	!Low-Teil WDC-RAM-Adresse (%2E00)!
	ldb	rl5, #%2E	!High-Teil WDC-RAM-Adresse (%2E00)!
	ld	r8, #%00	!Pufferadresse (Segment)!
	ld	r9, #REM_WDC2	!Pufferadresse (Offset)!
	ld	r10, #2		!Laenge (2 Byte)!
	sc	#SC_DISK
	pop	r1, @r15
	pop	r8, @r15
	pop	r5, @r15
	ldb	rl6, rl2	!rl6=Fehlerpar. := Returncode!
	ld	r4, REM_WDC2	!r4=Fehlerpar. := rueckgelesener Datenwert!
	test	r2		!Returncode!
	jr	z, TWDC3	!kein Fehler aufgetreten!

	push	@r15, r5
	calr	MSG_ERROR	!Fehler 53!
	pop	r5, @r15
	jr	TWDC_END
TWDC3:	
	push	@r15, r5
	calr	BY_WO_CMP	!Vergleich r4/r5, ggf. Fehler 53!
	pop	r5, @r15
TWDC4:
	cp	r5, #%5555
	jr	z, TWDC5	!naechster Testschritt!
	ld	r5, #%5555	!Test mit Datenwert %5555!
	jr	TWDC1

!-------------------------------------------
Testschritt 54 (WDC)
Test auf richtige Kommandoauswertung des WDC
-------------------------------------------!

!An den WDC wird ein ungueltiger Kommandokode ausgegeben; als Returnkode
muesste daruafhin %01 zurueckgegeben werden
--- Fehler 54, wenn der Returncode ungleich %01 ist ---!

TWDC5:
	ld	r8, #%54	!Fehler 54: falsche Kommandoauswertung des WDC!
	ld	ERRPAR_ID, #%40	!Fehlerpar. fuer Fehler 54: rl6!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!
	
	push	@r15, r8
	push	@r15, r1
	ld	r7, #5		!Requestcode fuer WDC: ungueltiger Code!
	sc	#SC_DISK
	pop	r1, @r15
	pop	r8, @r15
	cp	r2, #%01	!Returncode =  %01?!
	jr	z, TWDC6	!ja, d.h. kein Fehler!

	ldb	rl6, #%C1	!WDC-Fehlerkode %C1 ausgeben!
	calr	MSG_ERROR	!Fehler 54!
	jr	TWDC_END

TWDC6:

! Testdaten auf Zylinder 0, Kopf 1, Sektor 1 schreiben. 
  Meldet der WDC kein Status 0 (r2>0), dann wird der
  Fehlercode %58 mit Statusbyte rl2 ausgegeben !

! Testdatenbereich TDAT initialisieren !
	ld	r7,#TDAT
	ld	r6,r7
	inc	r6,#1
	ld	r5, #sizeof BUFFER
	ldb	@r7,#%55	! Erstes Byte laden !
	ldirb	@r6,@r7,r5	! Bereich fuellen   !

	ld	ERRPAR_ID,#%40	! Fehlerparameter (rl6) !

	ldb	rh7,#2		! WDC-Kommandocode !
	ld	r3,#TDAT	! Programmbereich der auf HD geschrieben wird!
	calr	DO_WDC

	test	r2		! Returncode = 0 ? !
	jr	z,TWDC10

	ld	r8,#%58		! Fehlercode !
	ldb	rl6,rl2		! rl6=Fehlerpar. := Returncode !
	calr	MSG_ERROR	! Fehlerausgabe !
	jr	TWDC_END

TWDC10:
! Zylinder 0, Kopf 1, Sektor 1 in Datenpuffer lesen.
  Meldet der WDC kein Status 0 (r2>0), dann wird der
  Fehlercode %59 mit Statusbyte rl2 ausgegeben !

	ldb	rh7,#1		! WDC-Kommandocode !
	ld	r3,#WDC_BUFF	! Zieladresse !
	calr	DO_WDC		! WDC action !

	test	r2
	jr	z,TWDC20

	ld	r8,#%59		! Code fuer Schreibfehler HD !
	ldb	rl6,rl2		! Parameter = Statusbyte !
	calr	MSG_ERROR
	jr	TWDC_END

TWDC20:
! Datenpuffer mit Solldaten vergleichen. Sind die Daten des
  gelesenen Sektors nicht mit den Daten beim Schreiben ueberein,
  wird der Fehlercode %51 gemeldet.
  Parameter sind:   Sollbyte    Istbyte     Lfd.-Nr.          !

	ld	ERRPAR_ID,#%58	! Ausgabe rl6,rl7,r5 !

	ld	r9,#WDC_BUFF	! Istdatenbereich !
	ld	r10,#TDAT	! Solldatenbereich !
	ld	r4,#sizeof BUFFER	! Datenlaenge !
	ld	r5,r4
	cpsirb	@r9,@r10,r4,NE	! Vergleich bis ungleichheit !
	jr	nz,TWDC30	! JP wenn keine Fehler !

	ld	r8,#%51		! Fehler beim schreiben/lesen eines Sektors !
	dec	r9,#1		! Adresse zurueckstellen !
	ldb	rl7,@r9		! Ist-Byte laden !
	dec	r10,#1		! Adsresse zurueckstellen !
	ldb	rl6,@r10	! Soll-Byte laden !
	sub	r5,r4		! Lfd.-Nr. := %200 - counter r4 !
	calr	MSG_ERROR
	jr	TWDC_END

TWDC30:
! Zylinder 0, Kopf 1, Sektor 1 loeschen. !

	ld	r7,#WDC_BUFF
	ld	r6,r7
	inc	r6,#1
	ld	r5, #sizeof BUFFER
	ldb	@r7,#0		! Erstes Byte laden !
	ldirb	@r6,@r7,r5	! Sektorpuffer loeschen !

	ldb	rh7,#2		! WDC-Kommandocode (write) !
	ld	r3,#WDC_BUFF
	calr	DO_WDC		! Sektor loeschen !


TWDC_END:
	ldb	rl0, #%20	!Bit5=1; BIT6=0!
	outb	PIO2D_B, rl0	! RESET an WDC !

	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret
   end	_WDC_


!--------------------------------------------------------------------
Testschritt 55  (CTC0/CTC1)
Test auf fehlerhafte Daten 'AA' oder '55' fuer Zeitkonstantenregister
--------------------------------------------------------------------!

!An die Zeitkonstantenregister der einzelnen CTC-Kanaele die Daten 
'AA' bzw. '55' ausgegeben und rueckgelesen
--- Fehler 55, wenn ein Wert fehlerhaft rueckgelesen wurde ---!

GLOBAL
  _CTC_  procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r8, #%55         !Fehler 55: fehlerhafte '%AA' bzw. '%55'-Daten!
	ld	ERRPAR_ID, #%0085 !Fehlerparameter fuer Fehler 55:
				 				r6, rl5, rl4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen,
				 Byte-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%FFFF	!Wartezeit, um Ausgabe der Testschritt-
				 nummer zu garantieren im Falle, dass die
				 Ausgabe ueber Terminalkanal SIO0_B / CTC0_1 
				 erfolgt und nicht ueber Koppelschnittstelle!
WARTE1:	djnz	r0, WARTE1

	ld	r6, #CTC0_0	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC0_1	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC0_2	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC0_3	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC1_0	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC1_1	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC1_2	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1
	ld	r6, #CTC1_3	!r6=Fehlerpar. (CTC-Kanal-Adresse)!
	calr	CTC_TEST1

!--------------------------
Testschritt 56  (CTC0/CTC1)
Test, ob CTC-Kanaele zaehlen
---------------------------!

!Die CTC-Kanaele werden im Zeitgebermode programmiert, es wird eine
Zeitkonstante ausgegeben und diese nach einer bestimmten Wartezeit rueckgelesen
--- Fehler 56, wenn sich die Zeitkonstante nicht gaendert hat, d.h. wenn
    der Kanal nicht zaehlt ---!

	ld	r8, #%56         !Fehler 56: CTC-Kanal zaehlt nicht!
	ld	ERRPAR_ID, #%0080 !Fehlerparameter fuer Fehler 56: r6!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%FFFF	!Wartezeit, um Ausgabe der Testschritt-
				 nummer zu garantieren im Falle, dass die
				 Ausgabe ueber Terminalkanal SIO0_B / CTC0_1 
				 erfolgt und nicht ueber Koppelschnittstelle!
WARTE2:	djnz	r0, WARTE2

	ld	r6, #CTC0_0	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC0_1	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC0_2	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC0_3	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC1_0	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC1_1	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC1_2	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2
	ld	r6, #CTC1_3	!r6=Fehlerpar. (CTC-Kanaladresse)!
	calr	CTC_TEST2

!--------------------------
Testschritt 57  (CTC0/CTC1)
Test des Interruptsystems
--------------------------!

!Die CTC-Kanaele werden auf ihre Interruptfaehigkeit getestet, indem sie
als Zeitgeber programmiert werden (mit Interrupt) und eine Zeitkonstante
ausgegeben wird
--- Fehler 57, wenn kein Interrupt erfolgt ---!

	ld	r8, #%57         !Fehler 57: CTC-Kanal interruptet nicht!
	ld	ERRPAR_ID, #%0080 !Fehlerparameter fuer Fehler 57: r6!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%FFFF	!Wartezeit, um Ausgabe der Testschritt-
				 nummer zu garantieren im Falle, dass die
				 Ausgabe ueber Terminalkanal SIO0_B / CTC0_1 
				 erfolgt und nicht ueber Koppelschnittstelle!
WARTE3:	djnz	r0, WARTE3

	push	@r15, VI_TAB_REST+2	!urspruengl. Wert retten!
	push	@r15, VI_TAB_REST+6	!urspruengl. Wert retten!
	push	@r15, VI_TAB_REST+10	!urspruengl. Wert retten!
	push	@r15, VI_TAB_REST+14	!urspruengl. Wert retten!

	ld	VI_TAB_REST+2, #CTC_ISR	!Adresse ISR in PSAREA eintr.!
	ld	VI_TAB_REST+6, #CTC_ISR	!Adresse ISR in PSAREA eintr.!
	ld	VI_TAB_REST+10, #CTC_ISR !Adresse ISR in PSAREA eintr.!
	ld	VI_TAB_REST+14, #CTC_ISR !Adresse ISR in PSAREA eintr.!

	ldb	rl0, #%20	!Interruptvektor (VI_TAB_REST)!
	outb	CTC0_0, rl0
	outb	CTC1_0, rl0

	ld	r6, #CTC0_0
	calr	CTC_TEST3
	ld	r6, #CTC0_1
	calr	CTC_TEST3
	ld	r6, #CTC0_2
	calr	CTC_TEST3
	ld	r6, #CTC0_3
	calr	CTC_TEST3
	ld	r6, #CTC1_0
	calr	CTC_TEST3
	ld	r6, #CTC1_1
	calr	CTC_TEST3
	ld	r6, #CTC1_2
	calr	CTC_TEST3
	ld	r6, #CTC1_3
	calr	CTC_TEST3

	pop	VI_TAB_REST+14, @r15	!urspruengl. Wert zurueckspeichern!
	pop	VI_TAB_REST+10, @r15	!urspruengl. Wert zurueckspeichern!
	pop	VI_TAB_REST+6, @r15	!urspruengl. Wert zurueckspeichern!
	pop	VI_TAB_REST+2, @r15	!urspruengl. Wert zurueckspeichern!


	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret	
   end	_CTC_

!---------------------------------------------------------------
Testschritt 60  (SIO0/SIO1)
Test auf fehlerhafte Daten 'AA' oder '55' fuer Interruptregister
---------------------------------------------------------------!

!An die Interruptregister der SIO's werden die Daten 'AA' bzw. '55'
ausgegeben und rueckgelesen
--- Fehler 60, wenn ein Wert fehlerhaft rueckgelesen wurde ---!

GLOBAL
  _SIO_	  procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r8, #%60         !Fehler 60: fehlerhafte '%AA' bzw. '%55'-Daten!
	ld	ERRPAR_ID, #%0085 !Fehlerparameter fuer Fehler 60:
				 				r6, rl5, rl4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen,
				 Byte-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%FFFF	!Wartezeit, um Ausgabe der Testschritt-
				 nummer zu garantieren im Falle, dass die
				 Ausgabe ueber Terminalkanal SIO0_B / CTC0_1 
				 erfolgt und nicht ueber Koppelschnittstelle!
WARTE4:	djnz	r0, WARTE4

	ld	r6, #SIO0D_B	!r6=Fehlerpar. (SIO-Adresse)!
	ld	r7, #SIO0C_B
	calr	SIO_TEST1
	ld	r6, #SIO1D_B	!r6=Fehlerpar. (SIO-Adresse)!
	ld	r7, #SIO1C_B
	calr	SIO_TEST1

!--------------------------
Testschritt 61  (SIO0/SIO1)
Test des Interruptsystems
--------------------------!

!Die SIO-Ports werden auf ihre Interruptfaehigkeit getestet, indem sie
auf Senderinterrupt programmiert werden und ein Byte ausgegeben wird
--- Fehler 61, wenn kein Interrupt erfolgt ---!

	ld	r8, #%61         !Fehler 61: SIO-Port interruptet nicht!
	ld	ERRPAR_ID, #%0080 !Fehlerparameter fuer Fehler 61: r6!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%FFFF	!Wartezeit, um Ausgabe der Testschritt-
				 nummer zu garantieren im Falle, dass die
				 Ausgabe ueber Terminalkanal SIO0_B / CTC0_1 
				 erfolgt und nicht ueber Koppelschnittstelle!
WARTE5:	djnz	r0, WARTE5

	push	@r15, VI_TAB_REST+2	!urspruengl. Wert retten!
	push	@r15, VI_TAB_REST+18	!urspruengl. Wert retten!

	ld	VI_TAB_REST+2, #SIO_ISR	!Adresse ISR in PSAREA eintr.!
	ld	VI_TAB_REST+18, #SIO_ISR !Adresse ISR in PSAREA eintr.!

	calr	SIO0_A_TEST2
	calr	SIO0_B_TEST2
	calr	SIO1_A_TEST2
	calr	SIO1_B_TEST2

	pop	VI_TAB_REST+18 @r15	!urspruengl. Wert zurueckspeichern!
	pop	VI_TAB_REST+2, @r15	!urspruengl. Wert zurueckspeichern!


	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	
	ret
   end   _SIO_
!---------------------------------------------
Testschritt 70  (0000 'P')
Test auf externen Speicher (hoechstes Segment)
---------------------------------------------!

!Beginnend bei Segment 0 wird jeweils bei den Offsetadressen %8000 und %FFFE
getestet, ob RAM vorhanden ist (auf der Adresse wird die Segmentnummer des
betrachteten Segments eingetragen und rueckgelesen). Wenn auf beiden Adressen
kein RAM vorhanden ist, ist die vorherige Segmentadresse die hoechste.
--- Fehler 70, wenn kein Segment vorhanden ist ---!

GLOBAL  
  _DYN_RAM_  procedure
   entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r8, #%70	!Fehler 70: NO EXTERNAL MEMORY!
	ld	ERRPAR_ID, #%0000 !Fehlerparameter fuer Fehler 70:
				 			keine Parameter!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	xor	r6, r6		!r6 (rh6) := Segmentnummer (=0)!
	ld	MAX_SEGNR, r6	!hoechste Segmentnummer initialisieren (:=0)!

        ldb	rl5, #%A5	
C_SEG:
	ldb	rl0, #%02	!Schleifenzaehler (2 Adressen testen)!
	ldb	rh5, rh6	!rh5:=Segmentnummer des zu testenden Segments!
	ld	r7, #%8000	!1. betrachtete Offsetadresse des Segments!
C_SEGL:
	sc	#SC_SEGV
	ld	@r6, r5		!r5 (rh5=akt.Segnr./rl5=%A5) in RAM eintragen!
	ld	r4, @r6		!ruecklesen!
	sc	#SC_NSEGV
	cp	r5, r4		!ruecklesbar!
	jr	z, INC_SEG	!Sprung wenn Segment vorhanden!
	ld	r7, #%FFFE	!2. betrachtete Offsetadresse des Segments!	
	dbjnz	rl0, C_SEGL

	decb	rh6, #%01	!betrachtetes Segment nicht vorhanden;
				 rh6:=hoechstes vorhandenes Segment!
	cpb	rh6, #0
	jr	ge, _SEG_IO	!mindestens Segment 0 vorhanden!
	jp	FAT_ERR		!Fehler: kein Segment vorhanden!

INC_SEG:
	ld	MAX_SEGNR, r6	!hoechste Segmentnummer retten!
	incb	rh6, #1		!Segmentadresse erhoehen!
	jr 	C_SEG		!naechstes Segment testen!

!------------------------
Testschritt 71 (0001 'O')
Test Segmentadressen
------------------------!

!Beginnend beim hoechsten Segment wird bis zum Segment 0 auf den Offset-
adressen %8000 und %FFFE die im vorherigen Testschritt auf diesen Adressen
eingetragenen Segmentnummern rueckgelesen und mit der Segmentnummer des
betrachteten Segments verglichen.
--- Fehler 71, wenn der Vergleich auf beiden Adressen eines Segments
                 					negativ ausfaellt ---!

_SEG_IO:
	ld	r8, #%71	!Fehler 71: SEG. ADDR. FAULT!
	ld	ERRPAR_ID, #%00E1 !Fehlerparameter fuer Fehler 71:
				   <rh6>, r7, rl4 !
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!
T_SEGR:
	ldb	rl0, #%02	!Schleifenzaehler (2 Adressen testen)!
	ld	r7, #%8000	!1. betrachtete Offsetadresse des Segments!
SEGR_LOOP:
	sc	#SC_SEGV
	ldb	rl4, @r6	!ruecklesen der im vorherigen Testschritt in
				 RAM eingetragene Segmentnummer!
	sc	#SC_NSEGV
	cpb	rh6, rl4	!ruecklesbar ? !
	jr	z, DEC_SEG	!Segment i.O.!
	ld	r7, #%FFFE	!2. betrachtete Offsetadresse des Segments!
	dbjnz	rl0, SEGR_LOOP
	jp	FAT_ERR		!Fehler: falsche Segmentadresse!

DEC_SEG:
	decb	rh6, #%01	!Segmentnummer decrementieren!
	cpb	rh6, #%00
	jr	ge, T_SEGR	!naechstes Segment testen!

!------------------------------------------------------------------
Testschritte  72, 73, 74, 75, 76 (0100, 0101, 0102, 0103, 0104 'W')
RAM-Test (ausser Segment 0)
------------------------------------------------------------------!

!
--- Fehler 72, 73, 74, 75 - siehe RAM_TEST_SEG ---
--- Fehler 76 wenn kein fehlerfreies Segment groesser 0 vorhanden ist ---
!

	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	ld	REM_ERR_CNT, #0	!Merker: Stand Fehlerzaehler loeschen!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	ld	r6, MAX_SEGNR	!r6:=hoechste Segmentnummer!
	ldctl	r9, FCW		!r9:=Stand FCW!

RAM_CHK:
	calr	RAM_TEST_SEG	!RAM-Test im Segment r6!

	ld	r2, ERR_CNT	!r2:=Stand Fehlerzaehler nach Aufruf von
				     RAM_TEST_SEG!
	cp	r2, REM_ERR_CNT	!Vergleich mit Stand Fehlerzaehler vor
				 Aufruf von RAM_TEST_SEG!
	jr	z, T_LOOP	!kein Fehler aufgetreten --> Segment r6 i.O.!

				!Segment r6 nicht i.O.!
	ld	REM_ERR_CNT, r2	!neuen Stand Fehlerzaehler merken!
	cp	r6, MAX_SEGNR	!war fehlerhaftes Segment das hoechste?!
	jr	nz, T_LOOP	!nein --> naechstes Segment testen!

				!ja!
	ld	r8, #%76	!Fehler 76: No Good Segments Above Zero!
	ld	ERRPAR_ID, #%0000 !Fehlerparameter fuer Fehler 76:
				   keine Parameter!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	decb	rh6, #%01	!hoechste SEGMENTNR. DEKREMENTIEREN!
	testb	rh6
	jp	z, FAT_ERR	!hoechste Segmentnummer war 1 und Segment 1 ist
				 fehlerhaft --> Fehler 76!
	ld	MAX_SEGNR, r6	!hoechste Segmentnummer neu laden, da
				 bisheriges hoechstes Segment fehlerhaft ist!
	incb	rh6, #%01

T_LOOP:				!naechtes Segment testen!
	decb	rh6, #%01	!SEGMENTNR. DEKREMENTIEREN!
	cpb	rh6, #%00
	jr	gt, RAM_CHK	!alle Segmente > 0 testen!

!------------------------------------------------------
Testschritt 72, 73, 74, 75 (0100, 0101, 0102, 0103 'E')
RAM-Test Segment 0
------------------------------------------------------!

!
--- Fehler 72,73,74,75 - siehe RAM_TEST_SEG ---
!
	xor	r6, r6		!Segmentnummer 0!
	ld	r3, #S_BNK	!Adresse S_BNK fuer Monitor PROM+RAM
				 ein- und ausschalten!

	ld	r10, r6		!Quell-Segment=0!
	ld	r12, MAX_SEGNR	!Ziel-Segment=hoechstes Segment!
	ld	r11, r6		!Quell-Offset=0!
	ld	r13, r6		!Ziel-Offset=0!
	ld	r9, #%8000	!Transferlaenge=ganzes Segment!
	sc	#SC_SEGV
	ldir	@r12, @r10, r9 	!Transfer des Monitors in hoechstes Segment!

	ld	r13, #RAM_TEST_SEG0 !Adresse fuer Programmfortsetzung laden
				     (RAM_TEST_SEG0 im hoechsten Segment)!
	ld	r11, #CONTINUE	!Folgeadresse nach RAM_TEST_SEG0 laden!
				
	jp	@r12		!Sprung in hoechstes Segment zu RAM_TEST_SEG0!

CONTINUE:
	ldctl	r9,FCW
	res	r9,#%0F
	ldctl	FCW,r9		!Nichtsegmentierten Mode einstellen!
	
	add	r3,ERR_CNT	!Fehler aus RAM_TEST_SEG0 addieren!
	ld	ERR_CNT,r3	


	calr	WR_2BS		!2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret
   end   _DYN_RAM_

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RAM_TEST_SEG0
RAM-Test im Segment 0 ausfuehren
Input:	r6 - Segmentnummer des zu testenden Segments (= %0000)
Output:	r3 - Stand Fehlerzaehler (ERR_CNT)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    RAM_TEST_SEG0 procedure
      entry
	ldctl	r9, FCW		!r9:=Stand FCW!
	res	r9, #%0F
	ldctl	FCW, r9		!nichtsegmentierten Mode einstellen!

	push	@r15,r12	!r12 retten!

!Terminalinterrupts sperren!
	testb	PTYPORT		!8-Bit-Teil aktiv?!
	jr	z, RATE01	!nein!

				!Terminal 8-Bit-Teil:
				 Empfaenger- und Senderinterrupts sperren!
	ldb	rl0, #SIO0C_B_8	!Adresse SIO0C_B auf 8-Bit-Teil!
	ldb	rh0, #SIO_R1
	call	KP_OUTB		!Datenbyte rh0 an SIO-Adr. rl0 ausgeben!
	ldb	rh0, #DRCVRI	!Empfaengerinterrupt aus!
	call	KP_OUTB		!Datenbyte rh0 an SIO-Adr. rl0 ausgeben!
	jr	RATE02
RATE01:				!Terminal 16-Bit-Teil:
				 Empfaengerinterrupts sperren!
	ld	r2, #SIO0C_B	
	ldb	rh4, #SIO_R1
	outb	@r2, rh4
	ldb	rl4, #DRCVRI	!Empfaengerinterrupt aus!
	outb	@r2, rl4
	
RATE02:
	inb	rh2, S_BNK
	setb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM ausschalten!

	setb	rh1, #%00	!Test in Segment 0!
	calr	RAM_TEST_SEG	!Ramtest des Segment 0!

	inb	rh2, S_BNK
	resb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM einschalten!

!Terminalinterrupts wieder zulassen!
	testb	PTYPORT		!8-Bit-Teil aktiv?!
	jr	z, RATE03	!nein!

				!Terminal 8-Bit-Teil:
				 Empfaenger- und Senderinterrupts zulassen!
	ldb	rl0, #SIO0C_B_8	!Adresse SIO0C_B auf 8-Bit-Teil!
	ldb	rh0, #SIO_R1
	call	KP_OUTB		!Datenbyte rh0 an SIO-Adr. rl0 ausgeben!
	ldb	rh0, #PDAVCT+SAVECT+XMTRIE
	call	KP_OUTB		!Datenbyte rh0 an SIO-Adr. rl0 ausgeben!
	jr	RATE04
RATE03:				!Terminal 16-Bit-Teil:
				 Empfaengerinterrupts zulassen!
	ld	r2, #SIO0C_B	
	ldb	rh4, #SIO_R1
	outb	@r2, rh4
	ldb	rl4, #PDAVCT+SAVECT
	outb	@r2, rl4

RATE04:
	ld	r3, ERR_CNT	!r3:=Anzahl der Fehler bei RAM_TEST_SEG0
				 (RAM-Zelle ERR_CNT liegt hier im hoechsten
				 Segment)!
	pop	r12,@r15	!r12 wiederherstellen!

	set	r9, #%0F
	ldctl	FCW, r9		!segmentierten Mode einstellen!

	ld	r2,r11		!Folgeadresse nach RAM_TEST_SEG0!
	xor	r6,r6
	ld	r11,r6		!Zieloffset!
	ld	r13,r6		!Quelloffset!
	ld	r9,#%8000	!Anzahl!
	ldir	@r10,@r12,r9	!Rueckspeichern Segment 0!
	ld	r11,r2

	jp	@r10		!Ruecksprung ins Segment 0 zu CONTINUE!
    end RAM_TEST_SEG0

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MMU_TEST
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    _MMU_ procedure
      entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!

	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT,#%0000	!Fehlerzaehler loeschen!

	xorb	rh1, rh1	!Flagbyte rh1 loeschen!

!MMU-ID in Mode-Register eintragen!
	ldb	rl2,#%02
	soutb	ALL_MMU, rl2	!Fuer alle 3 MMU: ID=02!
	SOUTB	ALL_MMU + %1100, RL2	!Reset Violation-Type-Register!

!Adresse fuer Segment-Trap und Priv.-Instr.-Trap aus PSAREA retten!
	ldctl	r9, PSAP	!=PSAPOFF!
	ld	r4, %26(r9)	!Offsetadresse fuer Segment-Trap!
	ld	REM_SEGMENT_TRAP, r4
	ld	r4, %16(r9)	!Offsetadresse fuer Priv.-Instr.-Trap!
	ld	REM_PRIVINSTR_TRAP, r4
!Adresse fuer Segment-Trap und Priv. Instr. Trap in PSAREA neu setzen!
	ld	%22(r9), #%4000	!FCW=nonsegmented!
	ld	%24(r9), #%0000	!Segmentadresse=0!
	ld	%26(r9), #SEGMENT_TRAP	!Offsetadresse!
	ld	%12(r9), #%4000	!FCW=nonsegmented!
	ld	%14(r9), #%0000	!Segmentadresse=0!
	ld	%16(r9), #PRIVINSTR_TRAP !Offsetadresse!

!SDR-Tabellen fuer Test laden! 
	ld	r10, #SDRT1	!SDRT1 ... SDRT1+%FF mit %00 laden!
	ld	r11, #SDRT1 + 1
	xor	r5, r5
	ld	r0, #255
	ldb	@r10, rl5
	ldirb	@r11, @r10, r0
 
	ld	r10, #SDRT0	!SDRT0 ... SDRT0+%FF mit %00 BIS %FF laden!
LD_INC:
	ldb	@r10, rl5
	inc	r10, #1
	inc	r5, #1
	bitb	rh5, #0
	jr	z, LD_INC
 
	ld	r10, #SDRT2	!SDRT2 ... SDRT2+%FF mit %AA laden!
	ld	r11, #SDRT2 + 1
	ld	r5, #%AAAA
	ld	r0, #255
	ldb	@r10, rl5
	ldirb	@r11, @r10, r0
 
	ld	r10, #SDRT3	!SDRT3 ... SDRT3+%FF mit %55 laden!	
	ld	r11, #SDRT3 + 1
	ld	r5, #%5555
	ld	r0, #%00FF
	ldb	@r10, rl5
	ldirb	@r11, @r10, r0

!Schleife L14E2 (Testschritt 80, 81, 82, 83) wird 3x durchlaufen mit 
r2=0,2,4 (r2-Zeiger auf 1. MMU-Adresse in MMU_LISTE1 fuer den entsprechenden
Durchlauf!
	xor	r2, r2		!r2=0 (1. Schleifendurchlauf)
				 Reihenfolge der MMU's:
				 1. CODE-, 2. DATA-, 3. STACK-MMU!
				!r2=2 (2. Schleifendurchlauf)
				 Reihenfolge der MMU's:
				 1. DATA-, 2. STACK-, 3. CODE-MMU!
				!r2=4 (3. Schleifendurchlauf)
				 Reihenfolge der MMU's:
				 1. STACK-, 2. CODE-, 3. DATA-MMU!
L14E2:

!---------------------------------------------------------
Testschritt 80 (0300)
Test auf individuelle Adressierbarkeit der einzelnen MMU's
---------------------------------------------------------! 
!
--- Fehler 80, wenn MMU's nicht einzeln addressierbar ---
!

	ld	r8, #%80	!Fehler 80: MMU's Not Individually Adressable!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	xor	r3, r3	

!64 SDR der 3 MMU's fuellen!
	ld	r6, MMU_LISTE1(r2) !Adresse der 1. MMU des entsprechenden
				    Durchlaufes: CODE-MMU (1. Durchlauf)
			                    bzw. DATA-MMU (2. Durchlauf)
				            bzw. STACK-MMU (3. Durchlauf) !
	ld	r11, #SDRT0	!Anfangsadresse der SDR-Tabelle im Speicher!
	calr	LD_SDR		!64 SDR der 1. MMU  des Durchlaufes fuellen mit
 			       Inhalt des Speicherbereiches SDRT0 ... SDRT0+%FF!

	incb	rl2, #%02
	ld	r6, MMU_LISTE1(r2) !Adresse der 2. MMU des entsprechenden
				    Durchlaufes: DATA-MMU (1. Durchlauf)
					    bzw. STACK-MMU (2. Durchlauf)
					    bzw. CODE-MMU (3. Durchlauf) !
	ld	r11, #SDRT1	!Anfangsadresse der SDR-Tabelle im Speicher!
	calr	LD_SDR		!64 SDR der 2. MMU des Durchlaufes fuellen mit
			       Inhalt des Speicherbereiches SDRT1 ... SDRT1+%FF!

	incb	rl2, #%02
	ld	r6, MMU_LISTE1(r2) !Adresse der 3. MMU des entsprechenden
				    Durchlaufes: STACK-MMU (1.Durchlauf)
					    bzw. CODE-MMU (2. Durchlauf)
					    bzw. DATA-MMU (3. Durchlauf)!
	calr	LD_SDR		!64 SDR der 3. MMU des Durchlaufes fuellen mit
			       Inhalt des Speicherbereiches SDRT1 ... SDRT1+%FF!

	decb	rl2, #%04	!Zeiger in MMU_LISTE1 wieder auf 1. MMU des
				 entsprechenden Durchlaufes stellen!

!Vergleich: geschriebener mit rueckgelesener SDR-Tabelle fuer die
3 MMU's des entsprechenden Durchlaufes
(r3=0: 1. MMU / r3=2: 2. MMU / r3=4: 3. MMU des Durchlaufes)!
L150E:
	ld	r9, r3
	add	r9, r2
	ld	r6, MMU_LISTE1(r9) !Adresse der 1. bzw. 2. bzw. 3. MMU
				    entsprechend r3!
	ld	r11, MMU_LISTE2(r3) !Anfangsadresse der zugehoerigen SDR-
				     Tabelle im Speicher!
	calr	RD_SDR		!Vergleich: geschriebene und rueckgelesene
				 SDR-Tabelle; ggf. Fehler 80!
	incb	rl3, #2		! Pointer auf naechste Adresse stellen !
	cpb	rl3, #6
	jr	nz, L150E	!naechste MMU!

!----------------------------------
Testschritt 81 (0301)
Test der SAR- bzw. DSCR-Indizierung
----------------------------------!
!
--- Fehler 81, wenn die SAR- oder DSCR-Indizierung fehlerhaft ist ---
!

	ld	r8, #%81	!Fehler 81: SAR Or DSCR Indexing Fault!
	andb	rh1, #%01	!Flagbyte Bit 1-7 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%001E	!Anzahl der Bytes der MMU_LISTE3!
				!(10 x 3 Bytes)!
	ld	r6, MMU_LISTE1(R2) !Adresse der 1. MMU des entsprechenden
				    Durchlaufes: CODE-MMU (1. Durchlauf)
					    bzw. DATA-MMU (2. Durchlauf)
					    bzw. STACK-MMU (3. Durchlauf) !
	ld	r3, #MMU_LISTE3
!Schleife L1538 wird 10 mal durchlaufen, wobei bei jedem Durchlauf 3 Bytes
 der MMU_LISTE3 benutzt werden (1.Byte: write SAR; 2.Byte: write DSCR; 
 3.Byte: Datenwert, der rueckgelesen werden muesste)!
L1538:
	ld	r10, r6		!Adresse der 1. MMU des Durchlaufes!
	add	r10, #%0100	!Write Segment-Adress-Register (SAR)!
	ldb	rl7, @r3
	sllb	rl7, #%02	!Shift 2 nach links!
	soutib	@r10, @r3, r0	!Beschreiben SAR mit akt. Byte der MMU_LISTE3!
				!incr r3 / decr r0!

	ld	r10, r6		!Adresse der 1. MMU des Durchlaufes!
	add	r10, #%2000	!Write Descriptor-Selector-Counter-Reg. (DSCR)!
	addb	rl7, @r3	!rl7=Fehlerpar. (SDR FELD#)!
	soutib	@r10, @r3, r0	!Beschreiben DSCR mit akt. Byte der MMU_LISTE3!
				!incr r3 / decr r0!

	ld	r10, r6		!Adresse der 1. MMU des Durchlaufes!
	add	r10, #%0F00	!Read Descriptor and increment SAR!
	ldb	rl5, @r3	!rl5=Fehlerpar. (Testdatenwert (Datenwert, der
				 rueckgelesen werden muesste))!
	ld	r9, #REM_MMU_SINOUT !Speicherplatz fuer rueckgelesenen Wert!
	sinib	@r9, @r10, r0	!Ruecklesen!
				!incr r9 / decr r0!
	dec	r9, #%01
	ldb	rl4, @r9	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	calr	BY_WO_CMP	!Vergleich rl4/rl5; ggf. Fehler 81!
	bitb	rh1, #%07
	jr	nz, L1578	!Fehler aufgetreten, naechster Testschritt!
	inc	r3, #%01	!r3 auf naechstes Byte in MMU_LISTE3 stellen!
	testb	rl0		!MMU_LISTE3 abgearbeitet?!
	jr	nz, L1538	!nein!

!--------------------
Testschritt 82 (0302)
Test der SDR-Daten
--------------------!
!
--- Fehler 82, wenn SDR-Daten 'AA' oder '55' fehlerhaft sind ---
!

	ld	r8, #%82	!Fehler 82: SDR 'As' Or '5s' Data Fault!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

L1578:
	ld	r11, #SDRT2	!Anfangsadresse der SDR-Tabelle im Speicher!
	calr	LD_SDR		!64 SDR der 1. MMU des Durchlaufes fuellen
			 mit 'AA' (Inhalt Speicherbereich SDRT2 ... SDRT2+%FF)!
	calr	RD_SDR		!Vergleich geschriebene mit rueckgelesene
				 SDR-Tabelle; ggf. Fehler 82!
	ld	r11, #SDRT3	!Anfangsadresse der SDR-Tabelle im Speicher!
	calr	LD_SDR		!64 SDR der 1. MMU des Durchlaufes fuellen
			 mit '55' (Inhalt Speicherbereich SDRT3 ... SDRT3+%FF)!
	calr	RD_SDR		!Vergleich geschriebene mit rueckgelesene
				 SDR-Tabelle; ggf. Fehler 82!

!----------------------------------
Testschritt 83 (0303)
Test der MMU-Control-Register-Daten
----------------------------------!
!
--- Fehler 83, wenn MMU-Control-Register-Daten 'AA' oder '55' 
                 fehlerhaft sind ---
!

	ld	r8, #%83	!Fehler 83: MMU Control Register 'As' Or '5s'
					      Fault!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	ld	ERRPAR_ID, #%0025 !Fehlerparameter fuer Fehler 83:
							   r7, rl5, rl4!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r9, #REM_MMU_SINOUT !Speicherplatz fuer Testdaten!
	ld	@r9, #%AAAA	!Test mit Daten 'AA'!
	ldb	rl0, #%02	!2 Durchlaeufe fuer Schleife L15A4
				 (fuer Daten 'AA' bzw. '55')!
L15A4:
!Beschreiben der Control-Register aller MMU's!
	ldb	rl5, @r9	!rl5=Fehlerpar. (Testdatenwert)!
	soutb	ALL_MMU + %0000, rl5	!Write Mode-Register!
	soutb	ALL_MMU + %0100, rl5	!Write Segment-Adress-Register (SAR)!
	soutb	ALL_MMU + %2000, rl5	!Write Descriptor-Selector-Counter-
					 Register (DSCR)!
!Ruecklesen der Control-Register der 1. MMU des Durchlaufes und Vergleich!
	ld	r10, r6		!Adresse der 1. MMU des Durchlaufes!
	add	r10, #%0000	!Read Mode-Register!
	sinib	@r9, @r10, r11	!Ruecklesen Mode-Register!
	dec	r9, #%01
	ldb	rl4, @r9	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	ld	r7, r6		!r7=Fehlerpar. (MMU CMD#)
				 rl7 - MMU-Adresse
				 rh7 - Kommando %00: R/W Mode-Register!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 83!

	ld	r10, r6
	add	r10, #%0100	!Read Segment-Adress-Register (SAR)!
	sinib	@r9, @r10, r11	!Ruecklesen SAR!
	dec	r9, #%01
	ldb	rl4, @r9	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	andb	rl5, #%3F	!rl5=Fehlerpar. (Testdatenwert (Datenwert, der
				 rueckgelesen werden muesste))!
	incb	rh7, #%01	!r7=Fehlerpar. (MMU CMD#)
				 rl7 - MMU-Adresse
				 rh7 - Kommando %01: R/W SAR!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 83!

	ld	r10, r6
	add	r10, #%2000	!Read Descriptor-Selector-Counter-Register!
	sinib	@r9, @r10, r11	!Ruecklesen DSCR!
	dec	r9, #%01
	ldb	rl4, @r9	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	andb	rl5, #%03	!rl5=Fehlerpar. (Testdatenwert (Datenwert, der
				 rueckgelesen werden muesste))!
	ldb	rh7, #%20	!r7=Fehlerpar. (MMU CMD#)
				 rl7 - MMU-Adresse
				 rh7 - Kommando %20: R/W DSCR!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 83!

	ld	@r9, #%5555	!Test mit Daten '55'!
	dbjnz	rl0, L15A4

	incb	rl2, #%02
	cpb	rl2, #%06
	jp	nz, L14E2	!r2=2 (2. Schleifendurchlauf)!
				!bzw.!
				!r2=4 (3. Schleifendurchlauf)!


!-------------------------------------------
Testschritt 84 (0304)
Test der Normal-Break-Register-Daten
-------------------------------------------!
!
--- Fehler 84, wenn Normal-Break-Register-Daten 'AA' oder '55' 
                 fehlerhaft sind ---
!

	ld	r8, #%84	!Fehler 84: System/Normal Break Register
				              'As' Or '5s' Fault!
	ld	ERRPAR_ID, #%0085 !korrigiert (original %0094)!
				  !Fehlerparameter fuer Fehler 84:
					 		r6, rl5, rl4 !
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!
	
	ld	r0,#%0001
	ld	REM_MMU1, r0	!Segmented Jumpers!
	ld	REM_MMU2, r0	!Systemmode (?)!
	
	ld	r6, #NBREAK	!Test Normal-Break-Register!
L1616:
	ld	r5, #%AAAA	!rl5=Fehlerpar. (Testdatenwert)!
	outb	@r6, rl5	!Break-Reg. beschreiben!
	inb	rl4, @r6	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 84!

	ld	r5, #%5555	!rl5=Fehlerpar. (Testdatenwert)!
	outb	@r6, rl5	!Break-Reg. beschreiben!
	inb	rl4, @r6	!rl4=Fehlerpar. (rueckgelesener Datenwert)!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 84!


!----------------------------------------------
Testschritt 85 (0305 'I')
Test, ob STACK-MMU beim Limit-Test Trap erzeugt
----------------------------------------------!
!
--- Fehler 85, wenn STACK-MMU beim Limit-Test keinen Trap erzeugt ---
!

LB_18A8:
	ld	r8, #%85	!Fehler 85: Stack MMU Did Not Trap On
					      Limit Test!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r2, #%9000
	outb	NBREAK, rh2	!Normal-Break-Register mit %90 laden!

	ld	r3, #MMU_LISTE4	!r3 - Zeiger auf SDR-Feld fuer CODE-MMU!
	ld	r4, r3		!r4 - Zeiger auf SDR-Feld fuer DATA-MMU!
	ld	r5, #MMU_LISTE5	!r5 - Zeiger auf SDR-Feld fuer STACK-MMU!
LB_18CE:
	calr	LD_3SDR		!Alle 64 SDR der 3 MMU's programmieren!

	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	ldb	rh0, #%04	!MMU_ID: Stack-MMU muss Trap ausloesen!
	ldb	rl4, #%D2	!Fuer Mode-Reg.: STACK-MMU!
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 0305!

!-------------------------
Testschritt 86 (0305 'A')
Test auf unerwarteten Trap
-------------------------!
!
--- Fehler 86, wenn ein Trap ausgeloest wird ---
!

	ld	r8, #%86	!Fehler 86: Unexpected Trap!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!
	
	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	xorb	rh0, rh0	!kein Trap darf ausgeloest werden!
	ldb	rl4, #%D1	!Fuer Mode-Reg.: DATA-MMU!
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 86!

!-------------------------
Testschritt 87 (0305 'G')
Test auf unerwarteten Trap
-------------------------!
!
--- Fehler 87, wenn ein Trap ausgeloest wird ---
!

	ld	r3, #MMU_LISTE4	!r3 - Zeiger auf SDR-Feld fuer CODE-MMU!
	ld	r4, #MMU_LISTE6	!r4 - Zeiger auf SDR-Feld fuer DATA-MMU!
	ld	r5, r3		!r5 - Zeiger auf SDR-Feld fuer STACK-MMU!
LB_18FE:
	calr	LD_3SDR		!SDR der 3 MMU's programmieren!

	xor	r7, r7
	calr	CLR_CTL_REG	!SAR:=0, DSCR:=0!
	ld	r7,  #DATA_MMU+%0B00	!Write SDR0 !
	ld	r4, #MMU_LISTE4
	ld	r2, #%0004
	sotirb	@r7, @r4, r2	

!Damit SDR 0 von DATA_MMU ohne Limitbegrenzung , sonst wird bei Testschritt 88 
 schon durch Befehlslesen ueber SDR0 der DATA_MMU Trap ausgeloest. Test be-
 ginnt erst bei SDR 2! 
	ld	r8, #%87	!Fehler 87: Unexpected Trap!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	xorb	rh0, rh0	!kein Trap darf ausgeloest werden!
	ldb	rl4, #%D2	!Fuer Mode-Reg.: STACK-MMU!
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 0305!

!----------------------------------------------
Testschritt 88 (0305 'N')
Test, ob DATA-MMU beim Limit-Test Trap erzeugt
----------------------------------------------!
!
--- Fehler 88, wenn DATA-MMU beim Limit-Test keinen Trap erzeugt ---
!

	ld	r8, #%88	!Fehler 88: Data MMU Did Not Trap On
					      Limit Test!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	ldb	rh0, #%02	!MMU_ID: DATA-MMU muss Trap ausloesen!
	ldb	rl4, #%D1
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 88!

!--------------------------------------------------
Testschritt 89 (0305 'O')
Test, ob STACK-MMU beim Read-Only-Test Trap erzeugt
--------------------------------------------------!
!
--- Fehler 89, wenn STACK-MMU beim Read-Only-Test keinen Trap erzeugt ---
!

	ld	r3, #MMU_LISTE4	!r3 - Zeiger auf SDR-Feld fuer CODE-MMU!
	ld	r4, #MMU_LISTE7	!r4 - Zeiger auf SDR-Feld fuer DATA-MMU!
	ld	r5, r4		!r5 - Zeiger auf SDR-Feld fuer STACK-MMU!
	calr	LD_3SDR		!SDR der 3 MMU's programmieren!

	ld	r8, #%89	!Fehler 89: Stack MMU Did Not Trap On
					      Read-Only Test!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	ldb	rh0, #%04	!MMU_ID: STACK-MMU muss Trap ausloesen!
	ldb	rl4, #%D2	!Mode-Reg.: STACK-MMU!
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 89!

!--------------------------------------------------
Testschritt 90 (0305 'S')
Test, ob DATA-MMU beim Read-Only-Test Trap erzeugt
--------------------------------------------------!
!
--- Fehler 90, wenn DATA-MMU beim Read-Only-Test keinen Trap erzeugt ---
!

	ld	r8, #%90 	!Fehler 90: Data MMU Did Not Trap On
					      Read-Only Test!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r3, #SDRT0	!Offsetadresse fuer Trap-Test!
	ldb	rh0, #%02	!MMU_ID: DATA-MMU muss Trap ausloesen!
	ldb	rl4, #%D1
	calr	SEGMENT_TRAP_TEST !Trap-Test; ggf. Fehler 90!

!------------------------------------------------
Testschritt 91/92 (0306/0307 'T')
Test, ob Uebersetzung der DATA-MMU fehlerfrei ist
------------------------------------------------!
!
--- Fehler 91, wenn Uebersetzung der DATA-MMU fehlerhaft ist ---
--- Fehler 92, wenn ein Trap ausgeloest wurde ---
!

	ld	r8, #%91	!Fehler 91: Translation Fault On Data MMU!

	ld	ERRPAR_ID, #%009A !Fehlerparameter fuer Fehler 91:
							r6, rl7, r5, r4 !
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #%01	!WORD-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r2, #%FFFF
	outb	NBREAK, rl2	!Normal-Break-Register mit %FF laden!

	ld	r3, #MMU_LISTE4	!r3 - Zeiger auf SDR-Feld fuer CODE-MMU!
	ld	r4, r3		!r4 - Zeiger auf SDR-Feld fuer DATA-MMU!
	ld	r5, r3		!r5 - Zeiger auf SDR-Feld fuer STACK-MMU!
	calr	LD_3SDR		!SDR der 3 MMU's programmieren!

	xor	r2, r2		!rh2=logische Segmentnummer (0-63)!
	ld	r3, #SDRT0	!Offsetadresse fuer Test!
	ld	r5, #%AAAA	!r5=Fehlerpar. (Testdatenwert (auf Offset-
				 adresse r3 zu schreiben))!
	ld	r6, #DATA_MMU	!r6=Fehlerpar. (MMU PORT#)!

!Schleife fuer die Segmente 0-63 (r2)!
TST_DATA_MMU:
	ld	@r3, #%0000	!Speicheradresse SDRT0 loeschen!
	ld	REM_MMU_ID, #%0000 !Merker: MMU-ID loeschen -
				    wird bei Segment-Trap gesetzt!
LB_199E:	
	ld	r0, #%00D1
	soutb	DATA_MMU,rl0	!Mode-Register mit %D1 laden (ID=1), d.h.
				 DATA-MMU aktivieren!	
	ldb	rl0, #%D0
	soutb	CODE_MMU, rl0	!Mode-Reg.: MSEN,TRNS,MST,ID!

	ldb	rl4, #%06
	outb	S_BNK, rl4	!MMU-Treiber ein!

	ldctl	r9, FCW		
	set	r9, #%0F	!Segmented Mode ein!
	test	REM_MMU2	!Normal-Mode eingestellt?!
	jr	z, LB_19B2	!ja!
	res	r9, #%0E	!Normal-Mode einstellen!
LB_19B2:
	ldctl	FCW, r9
	ld	@r2, r5		!Speicheradresse rr2 (Segment rh2/Offset SDRT0)
				 mit Datenwert %AAAA beschreiben!

	mbit
!Privilegierter Befehl im Normal-Mode, fuehrt zu Priv. Instr. Trap. 
 In der Service-Routine wird in System-Mode umgeschaltet.!

	outb	S_BNK, rh0	!MMU-Treiber aus!
	soutb	ALL_MMU, rh0	!MMUs ausschalten!

	sc	#SC_NSEGV

	ldb	rl7, rh2	!rl7=Fehlerpar. (SDR#) =log. Segmentnummer!
	test	REM_MMU_ID	!Segment-Trap (Routine SEGMENT_TRAP)
				 aufgetreten?!
	jr	nz, L1738	!ja, d.h. Fehler 92!

	ld	r4, @r3		!r4=Fehlerpar. (rueckgelesener Datenwert!
	calr	BY_WO_CMP	!Vergleich r5/r4; ggf. Fehler 91!

	bitb	rh1, #%07	!Fehler aufgetreten?!
	jr	nz, L1748	!ja, d.h. naechster Testschritt!

	incb	rh2, #%01
	cpb	rh2, #%40
	jr	nz, TST_DATA_MMU !naechste logische Segmentnummer!
	jr	L1748		!alle Segmentnummern abgearbeitet, d.h.
				 naechster Testschritt!

!Fehler 92!
L1738:
	ld	r8, #%92	!Fehler 92: Unexpected Trap!
	ld	ERRPAR_ID, #%0098 !Fehlerparameter fuer Fehler 92:
							r6, rl7, r5 !

	ld	r5, REM_MMU_BCSR !r5=Fehlerpar. (VDAT) - Werte der Status-
                                  Register BCSR und VTR nach Segment-Trap:
 				  rh5= Bus Cycle Status Register (BCSR) / 
				  rl5= Violation Type Register (VTR) !
	calr	MSG_ERROR	!Fehlerausgabe!

!-------------------------------------------------
Testschritt 93/94 (0308/0309 'I')
Test, ob Uebersetzung der STACK-MMU fehlerfrei ist
-------------------------------------------------!
!
--- Fehler 93, wenn Uebersetzung der STACK-MMU fehlerhaft ist ---
--- Fehler 94, wenn ein Trap ausgeloest wurde ---
!

L1748:
	ld	r8, #%93	!Fehler 93: Translation Fault On Stack MMU!
	ld	ERRPAR_ID, #%009A !Fehlerparameter fuer Fehler 93:
							r6, rl7, r5, r4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #%01	!WORD-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r0, #%00D2
	outb	NBREAK, rh0	!Normal-Break-Register mit %00 laden!

	ld	r2, #%4000	!Zugriff auf STACK_MMU im seg. Normalmode!
LB_1A16:
	ld	r6, #STACK_MMU	!r6=Fehlerpar. (MMU PORT#)!

!Schleife fuer die Segmente 64-127 (r2)!
TST_STACK_MMU:
	ld	@r3, #%0000	!Speicheradresse SDRT0 loeschen!
	ld	REM_MMU_ID, #%0000 !Merker: MMU-ID loeschen -
				    wird bei Segment-Trap gesetzt!

	ldb	rl0, #%D2
	soutb	STACK_MMU, rl0	!Mode-Reg.:MSEN,TRNS,MST,ID!
	ldb	rl0, #%D0
	soutb	CODE_MMU, rl0	!Mode-Reg.:MSEN,TRNS,MST,ID!
	ldb	rl0, #%D1
	soutb	DATA_MMU, rl0	!Mode-Reg.:MMSEN,TRNS,MST,ID!

	ldb	rl4, #%06
	outb	S_BNK, rl4	!MMU-Treiber ein!

	ldctl	r9, FCW
	set	r9, #%0F	!Segmented Mode ein!
	test	REM_MMU2	!Normalmode eingestellt?!
	jr	z, LB_1A42	!Ja!
	res	r9, #%0E	!Normalmode einstellen!
LB_1A42:
	ldctl	FCW, r9
	ld	@r2, r5		!Speicheradresse rr2 (Segment rh2/Offset SDRT0)
				 mit Datenwert %AAAA beschreiben!
	mbit
!Privilegierter Befehl im Normal-Mode, fuehrt zu Priv. Instr.Trap.
In der Service-Routine wird in System-Mode umgeschaltet.!

	outb	S_BNK, rh0	!MMU-Treiber aus!
	soutb	ALL_MMU, rh0	!MMUs ausschalten!
	sc	#SC_NSEGV	

	ldb	rl7, rh2	!rl7=Fehlerpar. (SDR#) =log. Segmentnummer!
	test	REM_MMU_ID	!Segment-Trap (Routine SEGMENT_TRAP)
				 aufgetreten?!
	jr	nz, L17AA	!ja, d.h. Fehler 94!

	ld	r4, @r3		!r4=Fehlerpar. (rueckgelesener Datenwert!
	calr	BY_WO_CMP	!Vergleich r5/r4; ggf. Fehler 93!

	bitb	rh1, #%07	!Fehler aufgetreten?!
	jr	nz, L17BA	!ja, d.h. naechster Testschritt!

	incb	rh2, #%01
	test	REM_MMU1
	jr	z, LB_1A72
	cpb	rh2, #%80
	jr	nz, TST_STACK_MMU
	jr	L17BA
LB_1A72:
	cpb	rh2, #%40
	jr	nz, TST_STACK_MMU !naechste logische Segmentnummer!
	jr	L17BA		!alle Segmentnummern abgearbeitet, d.h.
				 naechster Testschritt!

!Fehler 94!
L17AA:
	ld	r8, #%94	!Fehler 0309: Unexpected Trap!
	ld	ERRPAR_ID, #%0098 !Fehlerparameter fuer Fehler 94:
							r6, rl7, r5 !

	ld	r5, REM_MMU_BCSR !r5=Fehlerpar. (VDAT) - Werte der Status-
                                  Register BCSR und VTR nach Segment-Trap:
 				  rh5= Bus Cycle Status Register (BCSR) / 
				  rl5= Violation Type Register (VTR) !
	calr	MSG_ERROR	!Fehlerausgabe!

!------------------------------------------------
Testschritt 95/96 (0310/0311 'C')
Test, ob Uebersetzung der CODE-MMU fehlerfrei ist
------------------------------------------------!
!
--- Fehler 95, wenn Uebersetzung der CODE-MMU fehlerhaft ist ---
--- Fehler 96, wenn ein Trap ausgeloest wurde ---
!

L17BA:

	ld	r8, #%95	!Fehler 95: Translation Fault On Code MMU!
	ld	ERRPAR_ID, #%009A !Fehlerparameter fuer Fehler 95:
							r6, rl7, r5, r4 !
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	setb	rh1, #%01	!WORD-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	clr	REM_MMU2

	ldctl	r9, PSAP
	ld	%26(r9), #CODE_TRAP !neue Trap-Routine fuer Segment-Trap
				     in PSAREA eintragen!
	ld	r4, #CODE_FELD
	ld	r5, #SDRT0
	ld	r3, #%0004
	ldirb	@r5, @r4, r3	!CODE_FELD NACH SDRT0 MOVEN!

	ld	r0, #%00D0
	ld	r6, #CODE_MMU	!r6=Fehlerpar. (MMU PORT#)!

	ldb	rl3, #%02	!Kennzeichnung Testschritt 95 (rl3=2)!
	ldb	rh3, #%06	!Fuer S_BNK!
LB_1ACC:
	xor	r2, r2		!rh2=logische Segmentnummer (0-63)!
	ld	r5, r2		!r5=Fehlerpar. (Testdatenwert) = %0000
				 (r4 soll durch Abarbeitung von CODE_FELD
				 auf diesen Wert gesetzt werden)!

	ld	r10, r2		!r10/r11 - Ansprungadresse Marke FKT_TST!
	ld	r11, #FKT_TST	!(Segment 0, Offset #FKT_TST)!

	ld	r13, #SDRT0	!Ansprungadresse (Offset) fuer CODE_FELD!

!fuer Testschritt 95 und 97!
!Schleife fuer die Segmente 0(1)-63 (r2)!
TST_CODE_MMU:
	ld	r12, r2		!Ansprungadresse (Segment) fuer CODE_FELD
				 (Segment 0(1)-63)!
	ld	REM_MMU_ID, #%0000 !Merker: MMU-ID loeschen -
				    wird bei Segment-Trap gesetzt!
	ld	r4, #%AAAA	!r4=Fehlerpar. (zurueckgegebener Datenwert)
				 setzen (wird bei Abarbeitung von
				 CODE_FELD auf %0000 gesetzt)!

	ldb	rl0, #%92
	soutb	STACK_MMU, rl0	!Mode-Reg.:MSEN,ID!
	ldb	rl0, #%91
	soutb	DATA_MMU, rl0	!Mode-Reg.:MSEN,ID!
	ldb	rl0, #%D0
	soutb	CODE_MMU, rl0	!Mode-Reg.:MSEN,TRNS,ID!

	outb	S_BNK, rh3	!MMU-Treiber ein!

	sc	#SC_SEGV
	jp	@r12		!Sprung zu CODE_FELD auf Offsetadresse SDRT0
				 im aktuellen Segment (r2) - r4 wird in
				 CODE_FELD auf den Wert %0000 gesetzt;
				 dann Fortsetzung bei FKT_TST!
			!oder!
				!Segment-Trap (Routine CODE_TRAP) -
				 r4 wird nicht veraendert;
				 dann Fortsetzung bei FKT_TST!
FKT_TST:
	sc	#SC_NSEGV
	outb	S_BNK, rh0	!MMU-Treiber aus!
	ldb	rh0, #%80
	soutb	CODE_MMU, rh0	!Mode-Register mit %00 laden, d.h.
				 CODE-MMU deaktivieren!
	xorb	rh0, rh0

	ldb	rl7, rh2	!rl7=Fehlerpar. (SDR#) =log. Segmentnummer!

	bitb	rl3, #%00	!rl3=2 bei Testschritt 95;
				 rl3=1 bei Testschritt 97!
	jr	nz, L1866	!Programmfortsetzung fuer Testschritt 97!

!fuer Testschritt 95!
	test	REM_MMU_ID	!Segment-Trap (Routine CODE_TRAP) aufgetreten?!
	jr	nz, L1874	!ja, d.h. Fehler 96!

	calr	BY_WO_CMP	!Vergleich r5/r4, d.h. ob CODE_FELD abge-
				 arbeitet wurde und dabei r4 auf 0 gesetzt
				 wurde; ggf. Fehler 95!

	bitb	rh1, #%07	!Fehler aufgetreten?!
	jr	nz, L183C	!ja, d.h. naechster Testschritt!
				!(korrigiert: im Original L1842)!

!fuer Testschritt 95 und 97!
L1834:
	incb	rh2, #%01
	cpb	rh2, #%40
	jr	nz, TST_CODE_MMU !naechste logische Segmentnummer!

				!alle Segmentnummern abgearbeitet, d.h.
				 naechster Testschritt!

!---------------------------------------------
Testschritt 97 (0312 'S')
Test, ob CODE-MMU beim Limit-Test Trap erzeugt
---------------------------------------------!
!
--- Fehler 97, wenn CODE-MMU beim Limit-Test keinen Trap erzeugt ---
!

L183C:
	decb	rl3, #%01	!rl3=1, d.h. Kennzeichnung Testschritt 97;
				 rl3=0, d.h. Testschritt 97 beendet!
	testb	rl3
	jr	z, MMU_TEST_END	!Ende des Testes!

L1842:
	ld	r8, #%97	!Fehler 97: No Trap On Code MMU Limit Test!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	push	@r15, r3	!r3 retten!
	ld	r3, #MMU_LISTE5	!r3 - Zeiger auf SDR-Feld fuer CODE-MMU!
	ld	r4, #MMU_LISTE4	!r4 - Zeiger auf SDR-Feld fuer DATA-MMU!
	ld	r5, r4		!r5 - Zeiger auf SDR-Feld fuer STACK-MMU!
	calr	LD_3SDR		!SDR der 3 MMU's programmieren!

	xor	r7, r7
	calr	CLR_CTL_REG	!SAR:=0, DSCR:=0!
	ld	r5, #CODE_MMU+%0B00	!Write SDR0!
	ld 	r4, #MMU_LISTE4
	ld	r2, #%0004
	sotirb	@r5, @r4, r2

!Da auch die Programmabarbeitung ueber die CODE_MMU (SDR 0) erfolgt,  
 darf dort keine Limitbegrenzung vorliegen. Test beginnt bei SDR 1!

	pop	r3, @r15	!r3 rueckretten!

	ld	r2, #%0100	!rh2=logische Segmentnummer (1-63)!
	ld	r5, #%AAAA	!ueberfluessig ???!
	jr	TST_CODE_MMU

L1866:
	test	REM_MMU_ID	!Segment-Trap (Routine CODE_TRAP) aufgetreten?!
	jr	nz, L1834	!ja, d.h. kein Fehler;
				 naechste Segmentnummer!

	ld	ERRPAR_ID, #%0090 !Fehlerparameter fuer Fehler 97: r6, rl7 !

	jr	L1880		!Fehlerausgabe!

!Fehler 96!
L1874:
	ld	r8, #%96	!Fehler 96: Unexpected Trap!
	ld	ERRPAR_ID, #%0098 !Fehlerparameter fuer Fehler 96:
							r6, rl7, r5 !
	ld	r5, REM_MMU_BCSR !r5=Fehlerpar. (VDAT) - Werte der Status-
                                  Register BCSR und VTR nach Segment-Trap:
 				  rh5= Bus Cycle Status Register (BCSR) / 
				  rl5= Violation Type Register (VTR) !

L1880:
	calr	MSG_ERROR	!Fehlerausgabe!

MMU_TEST_END:
	ldctl	r9, PSAP
	ld	r4, REM_SEGMENT_TRAP
	ld	%26(r9), r4	!Adresse der urspruenglichen Segment-Trap-
				 Routine wieder in PSAREA abspeichern!
	ld	r4, REM_PRIVINSTR_TRAP
	ld	%16(r9), r4	!Adresse der urspruenglichen Priv.-Instr.-Trap-
				 Routine wieder in PSAREA abspeichern!
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MMU-Treiber aus!
	soutb	ALL_MMU, rl0	!Alle MMUs aus!

	calr	WR_2BS		! 2x Backspace, um letzte Testschritt-
				 nummer zu ueberschreiben!
	calr	WR_2SP		!2x Space ausgeben!
	calr	WR_2BS		!Zurueck zum Zeilenanfang!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret

CODE_FELD:
	xor	r4,r4		!r4=Fehlerpar. (zurueckgegebener Datenwert)=
				 %0000 (Kennzeichnung, dass CODE_FELD
				 abgearbeitet wurde)!
	jp	@r10		!Segment. Sprung zu FKT_TST!

    end _MMU_

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RAM_TEST_SEG
RAM-Test eines Segmentes
Input:	r6 - Segmentnummer des zu testenden Segments
	rh1 Bit0=0 - Test nicht in Segment 0
	        =1 - Test in Segment 0
	r9 - Stand FCW
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    RAM_TEST_SEG procedure
      entry

!-----------------------------------
Testschritt 72 (0100)
Test auf fehlerhafte Speicheradresse
-----------------------------------!

!Die 15 Adressen (0003,0006,000C,0018,0030,0060,00C0,0180,0300,0600,0C00,1800,
3000,6000,C000) des betrachteten Segments werden beschrieben und wieder
rueckgelesen.
--- Fehler 72, wenn eine Speicherzelle fehlerhaft rueckgelesen wurde ---!

	ld	r8, #%72	!Fehler 72: MEM. ADDR. FAULT!

	ld	ERRPAR_ID, #%00E5 !Fehlerparameter fuer Fehler 72:
							<r6>, r7, rl5, rl4 !
	andb	rh1, #%01	!Loeschen Bit1-7(rh1);
				 Bit1(rh1):=0, d.h. Bytevergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

!15 Adresse des Segments beschreiben!

	ld	r7, #%0003	!Anfangs-Offset-Adresse!
	ldb	rl5, #%0F	!Adressenzaehler: 15 Adresse testen!
RATE1:
	set	r9, #%0F
	ldctl	FCW, r9		!segmentierten Mode einstellen!
	ldb	@r6, rl5	!Speicherzelle beschreiben
				 (mit akt. Stand des Adressenzaehlers rl5)!
	res	r9, #%0F
	ldctl	FCW, r9		!nichtsegmentierten Mode einstellen!
	rl	r7, #%01	!Offsetadresse um 1 Bit nach links verschieben!
	dbjnz	rl5, RATE1	!naechste Adresse!

!Ruecklesen der beschriebenen Adressen!

	rl	r7, #%01	!Anfangs-Offset-Adressse := %0003!
	ldb	rl5, #%0F	!Adressenzaehler!
RATE2:
	set	r9, #%0F
	ldctl	FCW, r9		!segmentierten Mode einstellen!
	ldb	rl4, @r6	!Speicherzelle ruecklesen!
	res	r9, #%0F
	ldctl	FCW, r9		!nichtsegmentierten Mode einstellen!
	calr	BY_WO_CMP	!Vergleich eingetragener Wert (rl5) mit
				 rueckgelesenen Wert (rl4); 
				 ggf. Fehler 72: Mem Addr Fault!
	bitb	rh1, #%07	!Fehler aufgetreten?!
	jr	nz, RATE3	!ja --> naechster Testschritt!

	rl	r7, #%01	!Offsetadresse um 1 Bit nach links verschieben!
	dbjnz	rl5, RATE2	!naechste Adresse testen!

!--------------------------------
Testschritt 73 (0101)
Test auf fehlerhafte Datenleitung
--------------------------------!

!Die Adresse %8000 des betrachteten Segments wird nacheinander mit den Werten
0001,0002,0004,0008,0010,0020,0040,0080,0100,0200,0400,0800,1000,2000,4000,
8000,0001 (jeweils ein Bit gesetzt) beschrieben und wieder rueckgelesen.
--- Fehler 73, wenn ein Wert fehlerhaft rueckgelesen wurde ---!

RATE3:
	ld	r8, #%73	!Fehler 73: Data Line Fault!
	ld	ERRPAR_ID, #%00EA !Fehlerparameter fuer Fehler 73, 74, 75:
							<r6>, r7, r5, r4 !
	andb	rh1, #%01	!Loeschen Bit1-7(rh1)!
	setb	rh1, #%01	!Bit1(rh1):=1,d.h. WORD-Vergleich in BY_WO_CMP!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r7, #%8000	!r7:=Offsetadresse!
	ld	r5, #1		!r5:=in RAM einzutragender Datenwert 
				 (Anfangswert - Bit0 gesetzt)!
	ldb	rl0, #%11	!rl0:=Anzahl der Schritte!
RATE4:
	set	r9, #%0F
	ldctl	FCW, r9		!segmentierten Mode einstellen!
	ld	@r6, r5		!r5 in RAM eintragen!
	ld	r4, @r6		!r4:=rueckgelesenen Wert!
	res	r9, #%0F
	ldctl	FCW, r9		!nichtsegmentierten Mode einstellen!

	calr	BY_WO_CMP	!Vergleich r4/r5; ggf. Fehler 73!
	bitb	rh1, #%07	!Fehler aufgetreten?!
	jr	nz, RATE5	!ja!

	rl	r5, #%01	!nein --> naechstes Bit testen!
	dbjnz	rl0, RATE4

!-----------------------------------------------
Testschritt 74, 75 (0102, 0103)
Test auf fehlerhafte '%AAAA'- bzw. '%5555'-Daten
-----------------------------------------------!

!Das ganze betrachtete Segment wird mit dem Datenwert %AAAA bzw. %5555 
beschrieben und wieder rueckgelesen.
--- Fehler 74, wenn ein %AAAA-Wert fehlerhaft rueckgelesen wurde ---
--- Fehler 75, wenn ein %5555-Wert fehlerhaft rueckgelesen wurde ---!

RATE5:
	ld	r8, #%74 	!Fehler 74: 'A' Data Fault!
	ld	r5, #%AAAA	!zuerst %AAAA in RAM eintragen!
RATE6:
	andb	rh1, #%03	!Loeschen Bit2-7(rh1)!
	calr	WR_TESTNR	!Ausgabe der Testschrittnummer!

	ld	r12, r6		!r12=Segnr. Zieladresse:=Segnr. Quelladresse!
	ld	r13, #%0002	!r13=Offset Zieladresse:= %0002!
	xor	r7, r7		!r7=Offset Quelladresse:= %0000!
	ld	r0, #%7FFF	!r0=WORD-Anzahl fuer ldir!
	set	r9, #%0F
	ldctl	FCW, r9		!segmentierten Mode einstellen!
	ld	@r6, r5
	ldir	@r12, @r6, r0	!ganzes Segment mit r5 beschreiben!

	xor	r7, r7		!r7=Anfangs-Offset-Adresse fuer Vergleich:=%0!
	ld	r0, #%8000	!r0=Anzahl der Worte fuer Vergleich!
	cpir	r5, @r6, r0, nz	!Test ab Adr. 0 (max. bis Segmentende), bis
				 zu einer Stelle, wo r5 nicht im RAM steht!
	dec	r7, #%02	!r7 zeigt auf fehlerhafte RAM-Adresse!
	ld	r4, @r6		!fehlerhafte Adresse nochmal lesen!
	res	r9, #%0F
	ldctl	FCW, r9		!nichtsegmentierten Mode einstellen!
	calr	BY_WO_CMP	!Vergleich r4/r5; ggf. Fehler 74 bzw. 75!
	cpb	rl5, #%55
	ret	z
	ld	r8, #%75 	!Fehler 75: '5' Data Fault!
	ld	r5, #%5555	!Test mit Datenwort %5555!
	jr	RATE6
    end RAM_TEST_SEG

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PRIVINSTR_TRAP
Trap-Routine bei Priv.-Instr.-Trap
Umschaltung auf Systemmode
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PRIVINSTR_TRAP procedure
      entry
	set	%02(r15), #%0E
	sc	#SC_SEGV
	iret
    end PRIVINSTR_TRAP

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SEGMENT_TRAP
Trap-Routine bei Segment-Trap
Output:	REM_MMU_BCSR - Inhalt Bus-Cycle-Status-Register (BCSR) der MMU bzw. 
						0 bei Fehler
	REM_MMU_VTR  - Inhalt Violation-Type-Register (VTR) der MMU bzw.
						0 bei Fehler
	REM_MMU_ID   - MMU_ID (1:CODE-MMU; 2:DATA-MMU; 3:STACK-MMU) bzw.
						High-Teil=%FF bei Fehler
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SEGMENT_TRAP procedure
      entry
	pushl	@r15, rr2
	ld	r2, 4(r15)
	exb	rl2, rh2
	and	r2, #%0007	!rl2=MMU_ID!
	
	cpb	rl2, #%01
	jr	z, TRAP_CODE	!Trap durch CODE-MMU ausgeloest!
	cpb	rl2, #%02
	jr	z, TRAP_DATA	!Trap durch DATA-MMU ausgeloest!
	cpb	rl2, #%04
	jr	z, TRAP_STACK	!Trap durch STACK-MMU ausgeloest!
!Fehler: falscher MMU_ID!
	ldb	rh2, #%FF	!High-Teil von REM_MMU_ID auf %FF setzen!
	xor	r3, r3		!REM_MMU_BCSR/REM_MMU_VTR auf %00 setzen!
	jr	TRAP_OUT

TRAP_CODE:
	sinb	rl3, CODE_MMU + %0200	!Read VTR!
	sinb	rh3, CODE_MMU + %0500	!Read BCSR!
	jr	TRAP_OUT

TRAP_DATA:
	sinb	rl3, DATA_MMU + %0200	!Read VTR!
	sinb	rh3, DATA_MMU + %0500	!Read BCSR!
	jr	TRAP_OUT

TRAP_STACK:
	sinb	rl3, STACK_MMU + %0200	!Read VTR!
	sinb	rh3, STACK_MMU + %0500	!Read BCSR!

TRAP_OUT:
	ld	REM_MMU_ID, r2	!Trap-Werte abspeichern!
	ld	REM_MMU_BCSR, r3

	soutb	ALL_MMU + %1100, rl2 	!Reset VTR!

	popl	rr2, @r15
	ldctl	r9, FCW
	set	r9, #%0F	!Segment-Bit im FCW setzen!
	ldctl	FCW, r9
	iret	
    end SEGMENT_TRAP

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CODE_TRAP
Trap-Routine fuer Segment-Trap fuer Test der CODE-MMU
Input:	rh0 - %00
	r11 - Ruecksprungadresse (FKT_TST)
Output:	REM_MMU_BCSR - Inhalt Bus-Cycle-Status-Register (BCSR) der CODE-MMU
	REM_MMU_VTR  - Inhalt Violation-Type-Register (VTR) der CODE-MMU
	REM_MMU_ID   - %FFFF
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CODE_TRAP procedure
      entry
	outb	S_BNK, rh0	!MMU's ausschalten!
	soutb	CODE_MMU, rh0	!Mode-Register der CODE-MMU mit %00 laden
				 (CODE-MMU deaktivieren)!
	add	r15, #%0008	!(im Original %0009)!
	push	@r15, r2

	sinb	rl2, CODE_MMU + %0200	!Read VTR!
	sinb	rh2, CODE_MMU + %0500	!Read BSCR!
	ld	REM_MMU_BCSR, r2
	ld	REM_MMU_ID, #%FFFF
	soutb	CODE_MMU + %1100, rl2	!Reset VTR!

	ld	r2, #%5000
	ldctl	FCW, r2		!FCW setzen (nonseg./Systemmode/VI=EI/NVI=DI)!
	pop	r2, @r15
	jp	@r11		!Sprung zu FKT_TST!
    end CODE_TRAP

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CLR_CTL_REG
Beschreiben der 2 Control-Register SAR (Segment-Adress-Register) und
DSCR (Descriptor-Selector-Counter-Register)
Input:	rl7 - Wert fuer SAR
	rh7 - Wert fuer DSCR
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CLR_CTL_REG procedure
      entry
	soutb	ALL_MMU + %0100, rl7	!Write SAR!
	soutb	ALL_MMU + %2000, rh7	!Write DSCR!
	ret	
    end CLR_CTL_REG

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE LD_SDR
Fuellen der 64 SDR einer MMU
Input:	r6  - MMU-Adresse
	r11 - Zeiger auf Anfang des Speicherbereiches (%100 Byte lang) mit
	      dessen Inhalt die 64 SDR gefuellt werden sollen
	      (64 SDR * 4 Byte je SDR = %100 Byte)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    LD_SDR procedure
      entry
	ldm	RG_FELD, r0, #%0F
	xor	r7, r7
	calr	CLR_CTL_REG	!Loeschen der Control-Register SAR und DSCR!

	add	r6, #%0F00	!Write Descriptor and increment SAR!
	ld	r0, #%0100
	sotirb	@r6, @r11, r0	!alle 64 SDR (zu je 4 Byte) laden!

	xor	r7, r7
	calr	CLR_CTL_REG	!Loeschen der Control-Register SAR und DSCR!
	ldm	r0, RG_FELD, #%0F
	ret	
    end LD_SDR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RD_SDR
Ruecklesen der 64 SDR einer MMU und Vergleich mit urspruenglich geladener SDR
ggf. Fehler 80 bzw. 82
Input:	r6  - MMU-Adresse
	r11 - Zeiger auf Anfang des Speicherbereiches, mit dessen Inhalt die
	      64 SDR gefuellt wurden
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    RD_SDR procedure
      entry
	pushl	@r15, rr2
	push	@r15, r11
	ld	ERRPAR_ID, #%0095 !Fehlerparameter fuer Fehler 80,81,82:
							r6, rl7, rl5, rl4!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!

	add	r6, #%0F00	!Read Descriptor and increment SAR!
	ld	r10, #SDRT4	!Anfangsadresse Puffer fuer rueckgelesene SDR!
	ld	r0, #%0100
	sinirb	@r10, @r6, r0	!64 SDR der MMU zuruecklesen!
 
	ld	r10, #SDRT4
	ld	r0, #%0100
	cpsirb	@r11, @r10, r0, nz !Vergleich!
				   !Befehl cpsirb wird beendet, bei Ungleich-
				    heit von @r11/@10 bzw. wenn Bytezahl r0
				    abgearbeitet ist!
	dec	r10, #%01
	dec	r11, #%01
	ldb	rl4, @r10	!rl4=Fehlerpar. (zurueckgelesener Datenwert)!
	ldb	rl5, @r11	!rl5=Fehlerpar. (Testdatenwert)!
	sub	r10, #SDRT4
	ld	r7, r10		!rl7=Fehlerpar. (SDR FELD#)!
	sub	r6, #%0F00	!r6=Fehlerpar. (MMU PORT#)!
	calr	BY_WO_CMP	!Vergleich rl5/rl4; ggf. Fehler 80 bzw. 82!
	pop	r11, @r15
	popl	rr2, @r15
	ret	
    end RD_SDR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE LD_3SDR
Fuellen der 64 SDR (zu je 4 Byte) der CODE-, DATA- und STACK-MMU
Input:	r3 - Zeiger auf Anfangsadresse des Speicherbereiches (4 Byte), mit
	     dessen Inhalt die 64 SDR der CODE-MMU gefuellt werden sollen
	r4 - Zeiger auf Anfangsadresse des Speicherbereiches (4 Byte), mit
	     dessen Inhalt die 64 SDR der DATA-MMU gefuellt werden sollen
	r5 - Zeiger auf Anfangsadresse des Speicherbereiches (4 Byte), mit
	     dessen Inhalt die 64 SDR der STACK-MMU gefuellt werden sollen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    LD_3SDR procedure
      entry
	ldm	RG_FELD, r0, #%0F
	xor	r7, r7
	calr	CLR_CTL_REG	!Loeschen der Control-Register SAR und DSCR!

	ldb	rl0, #%40		!Anzahl der SDR (64)!
	ld	r9, #CODE_MMU + %0F00	!Write Descriptor and increment SAR!
	ld	r10, #DATA_MMU + %0F00 	!Write Descriptor and increment SAR!
	ld	r11, #STACK_MMU + %0F00	!Write Descriptor and increment SAR!
LD_3LOOP:
	ld	r2, #4			!Descriptorlaenge (4 Byte je SDR)!
	sotirb	@r9, @r3, r2		!SDR CODE_MMU programmieren!
	dec	r3, #%04		!Zeiger auf Anfangsadr. zurueckstellen!

	ld	r2, #4			!Descriptorlaenge (4 Byte je SDR)!
	sotirb	@r10, @r4, r2		!SDR DATA_MMU programmieren!
	dec	r4, #%04		!Zeiger auf Anfangsadr. zurueckstellen!

	ld	r2, #4			!Descriptorlaenge (4 Byte je SDR)!
	sotirb	@r11, @r5, r2		!SDR STACK_MMU programmieren!
	dec	r5, #%04		!Zeiger auf Anfangsadr. zurueckstellen!
	dbjnz	rl0, LD_3LOOP		!naechstes SDR!

	calr	CLR_CTL_REG	!Loeschen der Control-Register SAR und DSCR!
	ldm	r0, RG_FELD, #%0F
	ret	
    end LD_3SDR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SEGMENT_TRAP_TEST
Test, ob beim Beschreiben der Offsetadresse (r3) in jedem Segment ein Trap
ausgeloest wird
Input:	r3  - Offsetadresse fuer Trap-Test (Adresse, die in jedem Segment
	      beschrieben werden soll)
	rh0 - MMU_ID fuer MMU, die Trap ausloesen muss (2:DATA-MMU;4:STACK-MMU)
	      bzw. rh0=0, wenn kein Segment-Trap erwartet wird
	rl4 - ???
Fehler 85, wenn
- Segment-Trap nicht von der erwarteten MMU ausgeloest wurde (bei rh0=2,4)
- Segment-Trap ausgeloest wurde, obwohl nicht erwartet (bei rh0=0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SEGMENT_TRAP_TEST procedure
      entry
	pushl	@r15, rr2	!r2, r3 retten!
	push	@r15, r9	!r9 retten!
	xorb	rh1, rh1	!Flagbyte rh1 loeschen!
	xor	r2, r2		!rh2=logische Segmentnummer (0-63)!
	ldb	rh2, #%02
	bitb	rl4, #%01	!ID=2? (STACK-MMU)!
	jr	z, L1ABE	!nein!
	ldb	rh2, #%42
L1ABE:
	ld	ERRPAR_ID, #%0050 !Fehlerparameter fuer Fehler 85: rl6, rl7!
	ld	REM_MMU_ID, #%0000 !Merker: MMU-ID loeschen -
				    wird bei Segment-Trap gesetzt!
	xorb	rh4, rh4
	bitb	rl4, #%01	!ID=2? (STACK-MMU)!
	jr	z, LB_24D0	!nein!
	soutb	STACK_MMU, rl4	!Mode-Reg.: MSEN,TRNS,MST,ID!
	ldb	rh4, #%80
	soutb	CODE_MMU, rh4	!Mode-Reg.: MSEN,ID!
	ldb	rh4, #%81
	soutb	DATA_MMU, rh4	!Mode-Reg.: MSEN,ID!
	jr	LB_250E
LB_24D0:
	bitb	rl4, #%00	!ID=1 ? (DATA-MMU)!
	jr	z, LB_24E6	!nein!
	soutb	DATA_MMU, rl4	!Mode-Reg.: MSEN,TRNS,MST,ID!
	ldb	rh4, #%80
	soutb	CODE_MMU, rh4	!Mode-Reg.: MSEN,ID!
	ldb	rh4, #%82
	soutb	STACK_MMU, rh4	!Mode-Reg.: MSEN, ID!
	jr	LB_250E
LB_24E6:			!CODE-MMU!
	soutb	CODE_MMU, rl4	!Mode-Reg.: MSEN,TRNS,MST,ID!
	ldb	rh4, #%81
	soutb	DATA_MMU, rh4	!Mode-Reg.: MSEN,ID!
	ldb	rh4, #%82
	soutb	STACK_MMU, rh4	!Mode-Reg.: MSEN,ID!

LB_250E:
	ldb	rl0, #%06	
	outb	S_BNK, rl0	!MMU-Treiber ein; Segmentierter User!

	ldctl	r9, FCW
	set	r9, #%0F	!Segmented Mode ein!
	test	REM_MMU2	!Normal-Mode eingstellt?!
	jr	z, LB_251A	!ja!
	res	r9, #%0E	!Normal-Mode ein, um Zugriff auf STACK-MMU zu 
 				 ermoeglichen!
LB_251A:
	ldctl	FCW, r9

	ld	@r2, r5		!Speicherzelle (Segment r2 / Offset r3)
				 beschreiben!

!Segmentverletzung soll erfolgen und zu Segment-Trap fuehren !

	mbit

!Priviligierter Befehl im Normal-Mode, fuehrt zu Priv. Instr. Trap. In der 
Service-Routine wird in System-Mode umgeschaltet.!
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MMU-Treiber aus!
	soutb	ALL_MMU, rl0	!MMUs ausschalten!

	sc	#SC_NSEGV	!Nonsegment. Mode ein!

	ld	r6, REM_MMU_ID	!rl6=Fehlerpar. (MMU ID#)!
	testb	rh6	
	jr	nz, L1B12	!Fehler 85 mit Fehlerpar. rl6, rl7:
				 Segment-Trap mit falschem MMU_ID aufgetreten
				 (nicht 1,2,4 - siehe SEGMENT_TRAP)!
	cpb	rh0, rl6	!Vergleich MMU_ID des Segment-Traps mit dem
				 erwarteten MMU_ID!
	jr	nz, L1B08	!Fehler 85 mit Fehlerpar. rl6, rl7, r5:
				 Segment-Trap erfolgte nicht mit erwarteten
				 MMU_ID (bei rh0=2,4)
				 bzw.
				 Segment-Trap aufgetreten, obwohl keiner
				 auftreten durfte (bei rh0=0)!
L1AFE:
	incb	rh2, #%01	!Naechstes SDR!
	bitb	rl4, #%01	!STACK-MMU ?!
	jr	z, LB_254C	!nein!
	cpb	rh2, #%80	!Alle SDR (66-128) behandelt ?!
	jr	nz, L1ABE	!nein!
	jr	L1B1C		!ja!
LB_254C:
	cpb	rh2, #%40	!Alle SDR (2-63) behandelt ?!
	jr	nz, L1ABE	!naechste logische Segmentnummer!
	jr	L1B1C		!alle Segmentnummern abgearbeitet,d.h. Ende!

!Fehlereinsprung!
L1B08:
	ld	ERRPAR_ID, #%0058 !Fehlerparameter fuer Fehler 85:
							rl6, rl7, r5 !
	ld	r5, REM_MMU_BCSR !r5=Fehlerpar. (VDAT) - Werte der Status-
                                  Register BCSR und VTR nach Segment-Trap:
 				  rh5= Bus Cycle Status Register (BCSR) / 
				  rl5= Violation Type Register (VTR) !
!Fehlereinsprung!
L1B12:
	ldb	rl7, rh2	!rl7=Fehlerpar. (SDR#)!
	calr	MSG_ERROR	!Fehlerausgabe!
	bitb	rh1, #%07	!MSG_ERROR seit dem letzen Loeschen von rh1
				 (am Anfang dieser Prozedur) bereits zweimal
				 durchlaufen? !
	jr	z, L1AFE	!nein, d.h. naechste logische Segmentnummer!

L1B1C:
	pop	r9, @r15
	popl	rr2, @r15
	ret	
    end SEGMENT_TRAP_TEST

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE BY_WO_CMP
Vergleich zweier Byte-Register (rl4, rl5) bzw. zweier Word-Register (r4, r5); 
bei Ungleichheit Fehlermeldung (MSG_ERROR)
Input:	rl4/rl5 bzw. r4/r5 - zu vergleichende Register
	rh1 Bit0=0 - Test nicht in Segment 0
	        =1 - Test in Segment 0 
	    Bit1=0 - Byte-Vergleich (rl4,rl5)
	        =1 - Word-Vergleich (r4,r5)
	r8,r7,r6,ERRPAR_ID - siehe Prozedur MSG_ERROR
OUTPUT: siehe Prozedur MSG_ERROR (nur bei Abarbeitung von MSG_ERROR)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    BY_WO_CMP procedure
      entry
	bitb	rh1, #%01
	jr	z, BYWO_BY	!Byte-Vergleich!
	cp	r5, r4		!Word-Vergleich!
	ret	z		!kein Fehler!
	jr	BYWO_ERR	!Fehler!
BYWO_BY:
	cpb	rl5, rl4	!Byte-Vergleich!
	ret	z		!kein Fehler!

BYWO_ERR:
	bitb	rh1, #%00	!Test in Segment 0?!
	jr	z, MSG_ERROR	!nein!

	inb	rh2, S_BNK	!ja!
	resb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM einschalten!
	calr	MSG_ERROR	!Fehlerausgabe!
	inb	rh2, S_BNK
	setb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM ausschalten!
	ret	
    end BY_WO_CMP

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FAT_ERR
Ausgabe von T_ERR, Fehlerzeile und T_FERR auf Terminal
Input:	siehe Prozedur MSG_ERROR
Output: r2 - Stand Fehlerzaehler (ERR_CNT)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    FAT_ERR procedure
      entry
	calr	MSG_ERROR	!Ausgabe T_ERR und Fehlerzeile auf Terminal!

FAT_ERR1:
	calr	WR_2BS		!2x Backspace ausgeben, um letzte Testschritt-
				 nummer zu ueberschreiben!
	ld	r2, #T_FERR
	sc	#SC_WR_MSG	!Ausgabe T_FERR auf Terminal!
	sc	#SC_WR_OUTBFF_CR !Ausgabe CR auf Terminal!
	ld	r2, ERR_CNT
	ret
    end FAT_ERR
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_TESTNR
Ausgabe der Testschrittnummer auf Terminal
Input:	r8 - Testschrittnummer
	rh1 Bit0=0 - Test nicht in Segment 0
	        =1 - Test in Segment 0 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_TESTNR procedure
      entry
	ldm	RG_FELD, r0, #%0F
	bitb	rh1, #%00	!Test in Segment 0?!
	jr	z, WRTST1	!nein!

	inb	rh2, S_BNK	!ja!
	resb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM einschalten!

WRTST1:
	calr	WR_2BS		!2x Backspace ausgeben, um letzte Testschritt-
				 nummer zu ueberschreiben!
	ld	r5, r8
	sc	#SC_BTOH8	!Testschrittnummer (aus rl5) in Ausgabepuffer 
				 OUTBFF eintragen!
	sc	#SC_WR_OUTBFF	!Inhalt von OUTBFF auf Terminal ausgeben!

	bitb	rh1, #%00	!Test in Segment 0?!
	jr	z, WRTST2	!nein!

	inb	rh2, S_BNK
	setb	rh2, #%00
	outb	S_BNK, rh2	!Monitor PROM+RAM ausschalten!

WRTST2:
	ldm	r0, RG_FELD, #%0F
	ret
    end WR_TESTNR	

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_2SP
Ausgabe von 2 Space auf Terminal
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_2SP procedure
      entry
	pushl	@r15, rr0
	pushl	@r15, rr2

	ldb	rl0, #' '
	sc	#SC_TYWR
	ldb	rl0, #' '
	sc	#SC_TYWR

	popl	rr2, @r15
	popl	rr0, @r15
	ret
    end WR_2SP
	
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_2BS
Ausgabe von 2 Backspace auf Terminal
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_2BS procedure
      entry
	pushl	@r15, rr0
	pushl	@r15, rr2

	ldb	rl0, #BS
	sc	#SC_TYWR
	ldb	rl0, #BS
	sc	#SC_TYWR

	popl	rr2, @r15
	popl	rr0, @r15
	ret
    end WR_2BS

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MSG_ERROR
Ausgabe T_ERR und Fehlerzeile (Fehlernummer und max. 4 Parameter) auf Terminal
Input:	r8 - Fehlernummer
	r6 - 1. Parameter der Fehlerzeile
	r7 - 2. Parameter der Fehlerzeile
	r5 - 3. Parameter der Fehlerzeile
	r4 - 4. Parameter der Fehlerzeile
	ERRPAR_ID - Spezifizierung, als was die 4 Fehlerparameter auszugeben
	            sind:
 		    Bit 7,6 (ERRPAR_ID) fuer Parameter 1 (r6)
		    Bit 5,4 (ERRPAR_ID) fuer Parameter 2 (r7)
		    Bit 3,2 (ERRPAR_ID) fuer Parameter 3 (r5)
		    Bit 1,0 (ERRPAR_ID) fuer Parameter 4 (r4)
		    mit
		    Bit x,x = 00 --> Parameter ist nicht auszugeben
		    Bit x,x = 01 --> Ausgabe des rl-Reg. des Parameters
		    Bit x,x = 10 --> Ausgabe des r-Reg. des Parameters
		    Bit x,x = 11 --> Ausgabe des rh-Reg. des Parameters als
				     Segmentnummer ('<yy>')
Output:	ERR_CNT - Fehlerzaehler wurde incr., wenn Bit2(rh1)=0 war
	Bit2(rh1) := 0
	Bit6(rh1) := 1 
	Bit7(rh1) := 1 , wenn Bit6(rh1) vorher =1 war
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    MSG_ERROR procedure
      entry
	ldm	RG_FELD, r0, #%0F	!Registerinhalte r0-r14 retten!
	calr	MEM_MSG_ERR	!Fehlermeldung in ERR_LST eintragen!
	ldm	r0, RG_FELD, #%0F

MSG_ERR1:
	ldm	RG_FELD, r0, #%0F

	push	@r15, r4	!4. Parameter!
	push	@r15, r5	!3. Parameter!
	push	@r15, r7	!2. Parameter!
	push	@r15, r6	!1. Parameter!
	push	@r15, r8	!Fehlernummer!

	calr	WR_2BS		!2x Backspace ausgeben, um letzte Testschritt-
				 nummer zu ueberschreiben!
	
	ld	r2, #T_ERR
	call	WR_MSG		!Ausgabe Text T_ERR auf Terminal!
!Aufbau der Fehlerzeile im Ausgabepuffer OUTBFF und Ausgabe auf Terminal!
	pop	r5, @r15
	sc	#SC_BTOH8	!Fehlernummer in Ausgabepuffer OUTBFF eintr.!
	ldb	rl1, #' '
	sc	#SC_PUT_CHR	!Space in OUTBFF eintragen!
	ldb	rl1, #' '
	sc	#SC_PUT_CHR	!Space in OUTBFF eintragen!
	ldb	rl1, #' '
	sc	#SC_PUT_CHR	!Space in OUTBFF eintragen!
	
	ld	r7, ERRPAR_ID	!rl7 := Parameter-Identifier fuer Fehlerzeile!	
	ld	r6, #%0004	!r6 := Anzahl der Parameter (=4)!
MSER2:
	xorb	rh7, rh7	!rh7 loeschen!
	rl	r7, #%02	!Parameter-Identifier fuer betrachteten
				 Parameter in rh7 schieben (Bit 0,1)!
	pop	r5, @r15	!r5 := betrachteter Parameter!	
	cpb	rh7, #%01	!Parameter-Identifier = 1 ?!
	jr	nz, MSER3	!nein!

	sc	#SC_BTOH8	!ja --> rl-Reg. des Par. in OUTBFF eintr.!
	jr	MSER5		!naechster Parameter!

MSER3:
	cpb	rh7, #%02	!Parameter-Identifier = 2 ?!
	jr	nz, MSER4	!nein!

	sc	#SC_BTOH16	!ja --> r-Reg. des Par. in OUTBFF eintr.!
	jr	MSER5		!naechster Parameter!

MSER4:
	cpb	rh7, #%03	!Parameter-Identifier = 3 ?!
	jr	nz, MSER5	!nein --> Parameter nicht ausgeben!

	ldb	rl5, rh5	!ja --> rh-Reg. des Parameters als!
	sc	#SC_PUT_SEGNR	!Segmentnummer in OUTBFF eintragen!

MSER5:
	ldb	rl1, #' '
	sc	#SC_PUT_CHR	!Space in OUTBFF eintragen!
	djnz	r6, MSER2	!naechster Parameter!

	sc	#SC_WR_OUTBFF_CR !Ausgabe Inhalt OUTBFF mit CR auf Terminal!
	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!

	ldm	r0, RG_FELD, #%0F	!gerettete Registerinhalte r0-r14
					 zurueckspeichern!	

	bitb	rh1, #%02
	jr	nz, LB_2286
	inc	ERR_CNT, #%01	!Fehlerzaehler incr!
LB_2286:
	resb	rh1, #%02

	bitb	rh1, #%06
	jr	z, MSER6
	setb	rh1, #%07	!SET Bit7(rh1), wenn Bit6(rh1)=1!
MSER6:
	setb	rh1, #%06	!SET Bit6(rh1), wenn Bit6(rh1)=0!
	ret	
    end MSG_ERROR
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE _MSG_MAXSEG
Ausgabe  T_SEG/NSEG, TJUM, T_MAXS und hoechste Segmentnummer auf Terminal
Output: r2 - Stand Fehlerzaehler (ERR_CNT)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    _MSG_MAXSEG procedure
      entry
	dec	r15,#14		!Retten der Saferegister r8-r14!
	ldm	@r15,r8,#7
	
	xorb	rl0, rl0
	outb	S_BNK, rl0	!MONITOR PROM & RAM EINSCHALTEN!


	calr 	WR_2SP		!2 Space ausgeben, da Testschrittausgabe
				 mit 2x Backspace beginnt!
	ld	ERR_CNT, #%0000	!Fehlerzaehler loeschen!
	
	ld	r2, #T_MAXS
	sc	#SC_WR_MSG	!Ausgabe Text T_MAXS auf Terminal!
	ldb	rl5, MAX_SEGNR
	sc	#SC_PUT_SEGNR	!hoechste Segmentnr. in OUTBFF eintragen!
	sc	#SC_WR_OUTBFF_CR !Ausgabe der hoechsten Segmentnummer auf
				  Terminal (mit CR)!

	ldm	r8,@r15,#7	!Rueckretten der Safe-Register r8-r14!
	inc	r15,#14
	ret	
    end _MSG_MAXSEG

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CRC_TEST
CRC-Berechnung und Vergleich mit vorgegebenen Sollwert
--- bei Fehler Sprung zu FAT_ERR und Ruecksprung zu Monitor ---

(bei der CRC-Berechnung wird beginnend bei der angeg. Anfangsadresse nur
jedes zweite Byte in die CRC-Berechnung einbezogen, da der Maschinenkode
auf den EPROM's separiert ist (even/odd))

Input:	r2 - Speicheranfangsadresse
	r3 - Anzahl der in die CRC-Bildung einzubeziehenden Bytes
	r4 - erwartete Pruefsumme
	r5 - negierte erwartete Pruefsumme
Output: r6 - Pruefsumme
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CRC_TEST procedure
      entry
!Vergleich erwartete Pruefsumme mit negierter erwarteter Pruefsumme!
	xor	r5, r4
	cp	r5, #%FFFF
	jr	nz, CRCERR	!Fehler!

!Berechnung der Pruefsumme!
	ld	r6, #-1
CRC:	ldb	rl0, @r2
	xorb	rl0, rh6
	ldb	rh6, rl0
	rrb	rl0
	rrb	rl0
	rrb 	rl0
	rrb	rl0
	andb	rl0, #%0F
	xorb	rl0, rh6
	ldb	rh6, rl0
	rrb	rl0
	rrb	rl0
	rrb	rl0
	ldb	rh0, rl0
	andb	rl0, #%1F
	xorb	rl0, rl6
	ldb	rl6, rl0
	ldb	rl0, rh0
	rrb	rl0
	andb	rl0, #%F0
	xorb	rl0, rl6
	ldb	rl6, rl0
	ldb	rl0, rh0
	andb	rl0, #%E0
	xorb	rl0, rh6
	ldb	rh6, rl6
	ldb	rl6, rl0

	inc	r2, #2		!naechstes Byte!
	djnz	r3, CRC

!Vergleich der berechneten mit der erwarteten Pruefsumme!
	cp	r6, r4
	ret	z		!kein Fehler!
CRCERR:
	pop	r5, @r15	!UP-RET aufheben!
	jp	FAT_ERR		!Fehlerabsprung!
    end CRC_TEST

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PIO_TEST1

Input:	r6 - PIO-Datenadresse
	r7 - PIO-Controladresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PIO_TEST1 procedure
      entry
	ldb	rl5, #%55	!rl5=Fehlerpar. (Testdatenwert)!
				!Restriktion!
	cp	r6, #PIO2D_B
	jr	nz, PIOTST11
	ldb	rl5, #%75	!BIT5=1!

PIOTST11:			!Restriktion: fuer Kanal kein Ausgabemode!
	ldb	rl0, #%CF	!PIO-Bitmode!
	outb	@r7, rl0
	ldb	rl0, #%00	!alle Bits auf Ausgabe!
	outb	@r7, rl0
	ldb	rl0, #%07	!kein Interrupt!
	outb	@r7, rl0

	outb	@r6, rl5	!Ausgabe des Testdatenwertes!
	inb	rl4, @r6	!rl4=Fehlerpar. (rueckgel. Datenwert)!
	calr	BY_WO_CMP	!Vergleich rl4/rl5; ggf. Fehler 50!

				!Restriktion!
	ldb	rl0, #%4F	!PIO-Eingabemode!
	outb	@r7, rl0

	cpb	rl5, #%AA
	ret	z
	ldb	rl5, #%AA	!rl5=Fehlerpar. (naechster Testdatenwert)!
	jr	PIOTST11
    end PIO_TEST1

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CTC_TEST1

Input:	r6 - CTC-Kanaladresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CTC_TEST1 procedure
      entry
	ldb	rl5, #%AA	!rl5=Fehlerpar. (Testdatenwert)!
CTCTST11:
	di	vi
	ldb	rl0, #%27	!Zeitgeber, Vorteiler 256!
	outb	@r6, rl0
	outb	@r6, rl5	!Ausgabe des Testdatenwertes (Zeitkonstante)!
	inb	rl4, @r6	!rl4=Fehlerpar. (rueckgel. Datenwert)!
	ei	vi
	calr	CTC_INIT	!CTC-Kanal ruecksetzen,ggf. neu initialisieren!
	calr	BY_WO_CMP	!Vergleich rl4/rl5; ggf. Fehler 55!
	cpb	rl5, #%55
	ret	z
	ldb	rl5, #%55	!rl5=Fehlerpar. (naechster Testdatenwert)!
	jr	CTCTST11
    end CTC_TEST1

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CTC_TEST2

Input:	r6 - CTC-Kanaladresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CTC_TEST2 procedure
      entry
	ldb	rl0, #%07	!Zeitgeber, Vorteiler 16!
	outb	@r6, rl0
	ldb	rl5, #%FF	!Zeitkonstante!
	outb	@r6, rl5

	ldb	rl0, #2		!bestimmte Zeit warten!
CTCTST21:
	dbjnz	rl0, CTCTST21
	
	inb	rl4, @r6	!Zeitkonstante ruecklesen!
	calr	CTC_INIT	!CTC-Kanal ruecksetzen,ggf. neu initialisieren!

	cpb	rl4, rl5
	jp	z, MSG_ERROR	!Fehler 56: Kanal zaehlt nicht!
	ret		
    end CTC_TEST2

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CTC_TEST3

Input:	r6 - CTC-Kanaladresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CTC_TEST3 procedure
      entry
	ldb	rl0, #%87	!Zeitgeber, Vorteiler 16, Int!
	outb	@r6, rl0

	ld	REM_INT, #0	!Interruptmerkerzelle loeschen!

	ldb	rl0, #%20	!Zeitkonstante!
	outb	@r6, rl0

	ldb	rl0, #%FF	!bestimmte Zeit auf Interrupt warten!
CTCTST31:
	test	REM_INT		!Interrupt aufgetreten?!
	jr	nz, CTC_INIT	!ja --> CTC-Kanal rueckssetzen,
				        ggf. neu initialisieren!
	dbjnz	rl0, CTCTST31

	calr	CTC_INIT	!CTC-Kanal ruecksetzen,ggf. neu initialisieren!
	jp	MSG_ERROR	!Fehler 57!
    end CTC_TEST3

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CTC_INIT
CTC-Kanal ruecksetzen;
ggf. Neuprogrammierung von bereits programmierten Kanaelen (CTC0_0/CTC0_1)

Input:	r6 - CTC-Kanaladresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CTC_INIT procedure
      entry
	ldb	rl0, #%03	!CTC-Reset!
	outb	@r6, rl0

	cp	r6, #CTC0_0
	jp	z, INIT_CTC0_0	!CTC0_0 fuer LOAD/SEND wieder initialisieren!

	cp	r6, #CTC0_1
	ret	nz
	testb	PTYPORT		!8-Bit-Teil aktiv?!
	ret	nz		!ja!
	jp	INIT_CTC0_1	!CTC0_1 fuer Terminalkanal SIO0_B wieder init.!
    end CTC_INIT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISR CTC_ISR

Input:  r6 - CTC-Kanaladresse
Output:	REM_INT := %FFFF
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CTC_ISR procedure
      entry
	di	vi
	push	@r15, r0
	ld	REM_INT, #%FFFF
	ldb	rl0, #%03	!CTC-Reset!
	outb	@r6, rl0
	ld	r0, #%ED4D	!U880-RETI ausgeben!
	outb	RETI_P, rh0
	outb	RETI_P, rl0
	pop	r0, @r15
	ei	vi
	sc	#SC_SEGV
	nop
	iret
    end CTC_ISR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SIO_TEST1

Input:	r6 - SIO-Datenadresse (nur Kanal B des entsprechenden SIO's)
	r7 - SIO-Controladresse (nur Kanal B des entsprechenden SIO's)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SIO_TEST1 procedure
      entry
	ldb	rl5, #%AA	!rl5=Fehlerpar. (Testdatenwert)!
SIOTST11:
	ldb	rl0, #SIO_R0+COMM3 !SIO-Kanal ruecksetzen!
	outb	@r7, rl0

	ldb	rl0, #SIO_R1+COMM2
	ldb	rh0, #%00	!Status veraendert Vektor nicht!
	outb	@r7, rl0
	outb	@r7, rh0

	ldb	rl0, #SIO_R2+COMM2
	outb	@r7, rl0	!Schreibregister 2 adressieren!
	outb	@r7, rl5	!Ausgabe des Testdatenwertes an Int.-Reg.!

	outb	@r7, rl0	!Leseregister 2 adressieren!
	inb	rl4, @r7	!rl4=Fehlerpar. (rueckgel. Datenwert)!

	calr	SIO_INIT	!SIO-Port ggf. neu initialisieren!
	calr	BY_WO_CMP	!Vergleich rl4/rl5; ggf. Fehler 55!
	cpb	rl5, #%55
	ret	z
	ldb	rl5, #%55	!rl5=Fehlerpar. (naechster Testdatenwert)!
	jr	SIOTST11
    end SIO_TEST1

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SIO0_A_TEST2 
          SIO0_B_TEST2
          SIO1_A_TEST2
          SIO1_B_TEST2
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SIO0_A_TEST2 procedure
      entry
	ld	r6, #SIO0D_A	!SIO-Datenadresse!
	ld	r7, #SIO0C_A	!SIO-Controladresse!
	ld	r5, #CTC0_0	!zugehoeriger CTC-Kanal fuer Baudrate!
	ld	r4, #SIO0C_B	!SIO-Controladresse f. Ausgabe des INT-Vektors
				 (Ausgabe nur an Kanal B des SIO's)!
	ld	r3, #SIO0C_A	!SIO-Controladresse fuer RETI-Ausgabe
				 (Ausgabe nur an Kanal A des SIO's)!
	jr	SIO_TEST2

SIO0_B_TEST2:
	ld	r6, #SIO0D_B	!SIO-Datenadresse!
	ld	r7, #SIO0C_B	!SIO-Controladresse!
	ld	r5, #CTC0_1	!zugehoeriger CTC-Kanal fuer Baudrate!
	ld	r4, #SIO0C_B	!SIO-Controladresse f. Ausgabe des INT-Vektors
				 (Ausgabe nur an Kanal B des SIO's)!
	ld	r3, #SIO0C_A	!SIO-Controladresse fuer RETI-Ausgabe
				 (Ausgabe nur an Kanal A des SIO's)!
	jr	SIO_TEST2

SIO1_A_TEST2:
	ld	r6, #SIO1D_A	!SIO-Datenadresse!
	ld	r7, #SIO1C_A	!SIO-Controladresse!
	ld	r5, #CTC0_2	!zugehoeriger CTC-Kanal fuer Baudrate!
	ld	r4, #SIO1C_B	!SIO-Controladresse f. Ausgabe des INT-Vektors
				 (Ausgabe nur an Kanal B des SIO's)!
	ld	r3, #SIO1C_A	!SIO-Controladresse fuer RETI-Ausgabe
				 (Ausgabe nur an Kanal A des SIO's)!
	jr	SIO_TEST2

SIO1_B_TEST2:
	ld	r6, #SIO1D_B	!SIO-Datenadresse!
	ld	r7, #SIO1C_B	!SIO-Controladresse!
	ld	r5, #CTC1_0	!zugehoeriger CTC-Kanal fuer Baudrate!
	ld	r4, #SIO1C_B	!SIO-Controladresse f. Ausgabe des INT-Vektors
				 (Ausgabe nur an Kanal B des SIO's)!
	ld	r3, #SIO1C_A	!SIO-Controladresse fuer RETI-Ausgabe
				 (Ausgabe nur an Kanal A des SIO's)!
	jr	SIO_TEST2

SIO_TEST2:
	ldb	rh0, #%57	!zugehoerigen CTC-Kanal fuer Baudrate progr.!
	ldb	rl0, #%02	!9600 Baud!
	outb	@r5, rh0
	outb	@r5, rl0

	ld	r2, #ITAB_SIO	!SIO-Kanal programmieren!
	ld	r0, #9
	otirb	@r7, @r2, r0
	ld	r2, #ITAB_SIO_INTVEK
	ld	r0, #2
	otirb	@r4, @r2, r0

	ld	REM_INT, #0	!Interruptmerkerzelle loeschen!

	ldb	rl0, #%00	
	outb	@r6, rl0	!Datenausgabe!

	ld	r0, #%1000	!bestimmte Zeit auf Interrupt warten!
SIOTST21:
	test	REM_INT		!Interrupt aufgetreten?!
	jr	nz, SIO_INIT	!ja!
	djnz	r0, SIOTST21

	calr	SIO_INIT	!ggf. SIO-Port neu initialisieren!
	jp	MSG_ERROR	!Fehler 61!
    end SIO0_A_TEST2

  INTERNAL
ITAB_SIO ARRAY [* BYTE] := [SIO_R0+COMM3
                            SIO_R4 + COMM2
                            S2 + C32 + NOPRTY
                            SIO_R3
                            0			!Empfaenger aus!
                            SIO_R5
                            XENABLE + T8	!Sender ein!
                            SIO_R1 + COMM2
                            XMTRIE]		!Senderinterrupt ein!

ITAB_SIO_INTVEK ARRAY [* BYTE] := [SIO_R2
				   %20]		!VI_TAB_REST in PSAREA!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SIO_INIT
Ruecksetzen des SIO- und CTC-Kanals;
ggf. Neuprogrammierung von bereits programmierten Kanaelen 
(SIO0_A/SIO0_B und CTC0_0/CTC0_1)

Input:	r7 - SIO-Controladresse
	r5 - zugeh. CTC-Kanaladresse fuer Baudrate
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SIO_INIT procedure
      entry
	ldb	rl0, #SIO_R0+COMM3 !SIO-Kanal ruecksetzen!
	outb	@r7, rl0
	ldb	rl0, #%03	!CTC-Kanal ruecksetzen!
	outb	@r5, rl0

	cp	r7, #SIO0C_A	!SIO0_A?!
	jr	nz, SIOINIT1	!nein!

	call	INIT_CTC0_0	!CTC0_0 fuer SIO0_A initialisieren!
	jp	INIT_SIO0_A	!SIO0_A fuer LOAD/SEND wieder initialisieren!

SIOINIT1:
	cp	r7, #SIO0C_B	!SIO0_B!
	ret	nz		!nein!

	testb	PTYPORT		!8-Bit-Teil aktiv?!
	jp	nz, INIT_SIO0_A	!ja --> SIO0_B nicht initialisieren, aber
				 Interruptvektor fuer SIO0_A wieder init., da
				 er bei Test von SIO0_B zerstoert wurde!

	call	INIT_CTC0_1	!nein --> Terminalkanal SIO0_B wieder init.!
	jp	INIT_SIO0_B	
    end SIO_INIT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISR SIO_ISR

Input:  r7 - SIO-Controladresse
	r3 - zugeh. SIO-Controladresse fuer RETI-Ausgabe (Kanal A des SIO's)
Output:	REM_INT := %FFFF
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SIO_ISR procedure
      entry
	push	@r15, r0
	ld	REM_INT, #%FFFF

	ldb	rl0, #SIO_R0+COMM3
	outb	@r7, rl0	!SIO-Kanal zuruecksetzen!

	ldb	rl0, #SIO_R0+RFI
	outb	@r3, rl0	!RETI an SIO ausgeben!
	
	pop	r0, @r15
	sc	#SC_SEGV
	nop
	iret
    end SIO_ISR

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DO_WDC
Fuer WDC-Test lese/schreibzugriff auf Sec.1, Kopf1, Zyl.0, Drive0
Inputs:	rh7= Kommandocode
	 r3= Pufferadresse
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

INTERNAL
   DO_WDC procedure
	entry

	ldb	rl7,#0			! Laufwerks-Nr. !
	ld	r6,#0			! Zylinder-Nr.  !
	ldb	rh5,#1			! Kopf-Nr.      !
	ldb	rl5,#1			! Sektor-Nr.    !
	ld	r4,#sizeof BUFFER	! Sektorlaenge  !
	call	wdc			! WDC-Treiber   !
	ret

end DO_WDC


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_INI_ERR_LST
Initialisierung fuer Fehlerlistenausgabe
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

GLOBAL
  _INI_ERR_LST procedure
      entry
	dec	r15, #14	!Retten der Safereg. r8-r14!
	ldm	@r15,r8,#7

	xor	r0, r0		!Loeschen r0!
	outb	S_BNK, rl0	!Monitor PROM & RAM einschalten!

	ld	CNT_ERR_LST, #0	!Loeschen Zaehler fuer Eintrag. in ERR_LST!
	ld	CUM_CNT_ERR, r0	!Loeschen des kumulat. Zaehlers!

	ld	r1, #ERR_LST
	ld	r2, #ERR_LST+2
	ld 	r3, #119
	ld	@r1, r0		!Erstes WORD loeschen!
	ldir	@r2, @r1, r3	!Ganzen Bereich ERR_LST LOESCHEN!

	ld	PTR_ERR_LST, #ERR_LST-2	!Pointer fuer Ausgabe von ERR_LST!

	ldm	r8, @r15, #7	!Rueckretten der Saferegister!
	inc	r15, #14
	ret
      end  _INI_ERR_LST


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MEM_MSG_ERR   
Speichern der Fehlermeldung in ERR_LST.
Aufruf aus MSG_ERROR (Eingabeparameter siehe dort).
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    MEM_MSG_ERR   procedure
      entry
	ld	r10, #ERR_LST+238	!Letzte Eintragung in ERR_LST!  
	ld	r11, #ERR_LST+226	!Vorletzte Eintragung in ERR_LST!
	ld	r12, #114		!Zaehler:=114, d.h. 19 Eintragungen!
	lddr	@r10, @r11, r12		!Ganze Tabelle um eine Eintragung nach 
					 unten schieben!
	ld	r10, #ERR_LST+13	!Fuer oberste Eintragung in Liste!
	push	@r10, r4		!4. Parameter!
	push	@r10, r5		!3. Parameter!
	push	@r10, r7		!2. Parameter!
	push	@r10, r6		!1. Parameter!
	push	@r10, r8		!Fehler-Nummer!
	push	@r10, ERRPAR_ID		!Spezifikation fuer Parameter!

	inc	CUM_CNT_ERR, #01	!Kumulativer Fehlerzaehler!
	cp	CNT_ERR_LST, #20	!Schon 20 Fehler eingetragen?!
	ret	z			!Ja!

	inc	CNT_ERR_LST, #01	!Fehlerzaehler erhoehen!
	inc	PTR_ERR_LST, #12	!Pointer weist auf letzten Parameter der
					 ersten Eintragung in ERR_LST!
	ret
      end MEM_MSG_ERR


!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_MEM_MSG_OUT
Ausgabe der gesamten Fehlerliste. Aufruf aus sa.diags nach Kommando "d". 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
     _MEM_MSG_OUT   procedure
       entry
	dec	r15, #14		!Retten der Saferegister r8-r14!
	ldm	@r15, r8, #7

	xorb	rh1,rh1			!Flagbyte loeschen!
	outb	S_BNK, rh1		!MONITOR PROM & RAM EINSCHALTEN!
	ld	r2, #T_CUM_ERR1
	call	WR_MSG			!Text 1 ausgeben!

	cp	CNT_ERR_LST, #0		!Schon Fehler aufgetreten?!
	jr	z, OUT1

	ld	r7, #PAR_CNT_ERR	!Adresse fuer Druckparameter!
	ld	r6, CUM_CNT_ERR		!ZAehler!
	call	_printf			!Ausgabe der Gesamtfehleranzahl!

	ld	r2, #T_CUM_ERR3
	call	WR_MSG
	call	WR_CRLF
	call	WR_2SP

	ld	r2, PTR_ERR_LST		 
	ld 	r3, CNT_ERR_LST

OUT3:					!Alle Parameter bereitstellen!
	ld	r4, @r2			!4. Parameter!
	dec	r2, #02
	ld	r5, @r2			!3. Parameter!
	dec	r2, #02			 
	ld	r7, @r2			!2. Parameter!
	dec	r2, #02			 
	ld	r6, @r2			!1. Parameter!
	dec	r2, #02			 
	ld	r8, @r2			!Fehler-Nummer!
	dec	r2, #02
	ld	r9, @r2			!Spezifikatin fuer Parameter!
	dec	r2, #02
	ld	ERRPAR_ID, r9		
	
	push	@r15, r2		!r2 ablegen!
	push	@r15, r3		!r3 ablegen!

	calr	MSG_ERR1		!Ausgabe einer Fehlermitteilung!
	pop	r3, @r15
	pop	r2, @r15
	dec	r3, #01			!CNT_ERR_LST!
	jr	z, OUT2			!Alle Fehler ausgegeben!

	jr	OUT3

OUT1:	ld 	r2, #T_CUM_ERR2
	call	WR_MSG
	ld	r2, #T_CUM_ERR3
	call	WR_MSG

OUT2:	ldm	r8, @r15, #7 
	inc	r15, #14
	ret

 end  _MEM_MSG_OUT
			

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TEST_
DUMMY-Routine fuer Monitorkommando
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

   GLOBAL
     TEST_    procedure
	entry

	nop
	ret
     end  TEST_

end satest

