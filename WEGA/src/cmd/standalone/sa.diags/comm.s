!******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
	P8000-PROM (MONITOR)	Modul: p_comm

	Bearbeiter:	J. Zabel
	Datum:		15.04.1987
	Version:	2.0

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_comm
------------------------------
Dieser Modul umfasst verschiedene Monitorkommandos
(D, C, F, M, PR, PW, PRS, PWS, HR, HW, FF, FR, FW).

Prozeduren
----------
DISPLAY
TST_WBL

COMPARE
PUT_ADR_INHALT

FILL

MOVE

PORT_READ
PORT_WRITE
PORT_READ_SPECIAL
PORT_WRITE_SPECIAL
PORT_RD
PORT_WR
P_IN
PS_IN
P_OUT
PS_OUT

HARD_DISK_READ
HARD_DISK_WRITE
HARD_DISK
FLOPPY_FORMAT
FLOPPY_READ
FLOPPY_WRITE
FLOPPY_BLOCK
==============================================================================!


p_comm module

$SECTION PROM

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

TEST_			procedure		!Modul p_test!

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

QUIT			procedure		!Modul p_ldsd!
LOAD			procedure		!Modul p_ldsd!
MCZ_INT			procedure		!Modul p_ldsd!
MCZ_ERR			procedure		!Modul p_ldsd!
TYWR_MCZ		procedure		!Modul p_ldsd!
WOCF_MCZ		procedure		!Modul p_ldsd!
SEND			procedure		!Modul p_ldsd!

MON16_1H		word			!Modul p.crc!
N_MON16_1H		word			!Modul p.crc!
MON16_1L		word			!Modul p.crc!
N_MON16_1L		word			!Modul p.crc!
MON16_2H		word			!Modul p.crc!
N_MON16_2H		word			!Modul p.crc!
MON16_2L		word			!Modul p.crc!
N_MON16_2L		word			!Modul p.crc!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Werte aus dem Arbeitsspeicher (Modul p_ram)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  EXTERNAL
RAM_ANF		ARRAY [%10 WORD]
NSP_OFF		WORD
		ARRAY [%3F WORD]	!Stackbereich!
STACK		WORD
		ARRAY [4 WORD]
_numterm	WORD 			!Anzahl der Terminals!
PTYPORT		BYTE			!Terminalkanal:
					 00 - SIO0_B (16-Bit-Teil)
					 01 - SIO0_B (8-Bit-Teil) ueber 
					      Koppelschnittstelle!
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
!Program Status Area (initialisiert bei Systemstart)!
PSAREA
!		                    ID    FCW   PCSEG PCOFF!
		ARRAY [4 WORD] !:= [%0000 %0000 %0000 %0000]
		               RESERVED!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 VI_ERR] 
		               UNIMLEMENTED INSTRUCTIONS!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 VI_ERR] 
		               PRIVILEGED INSTRUCTIONS!
		ARRAY [4 WORD] !:= [%0000 %C000 %8000 SC_ENTRY]
		               SYSTEM_CALL INSTRUCTIONS!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 MMU_ERR] 
		               SEGMENT TRAP!
PSA_NMI		ARRAY [4 WORD] !:= [%0000 %4000 %8000 AUTOBOOT bzw. NMI_INT] 
		               NONMASKABLE INTERRUPT
		               (AUTOBOOT vor erster Terminaleingabe;
		                NMI_INT nach erster Terminaleingabe)!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 VI_ERR] 
		               NONVECTORED INTERRUPT!
! VECTORED INTERRUPT!
		ARRAY [2 WORD] !:= [%0000 %4000] 
		               VECTORED INTERRUPT (ID, FCW)!
! VI - Sprung-Tabelle (128 Eintraege) !
!		                    SEG   OFFSET!
!CTC0!
VI_CTC0_0	ARRAY [2 WORD] !:= [%8000 VI_ERR] !
VI_CTC0_1	ARRAY [2 WORD] !:= [%8000 VI_ERR] !
VI_CTC0_2	ARRAY [2 WORD] !:= [%8000 VI_ERR] !
VI_CTC0_3	ARRAY [2 WORD] !:= [%8000 VI_ERR bzw. GO_INT bzw. NXT_INT]
		               fuer GO bzw. NEXT!
!PIO1!
VI_PIO1_B	ARRAY [2 WORD] !:= [%8000 KOPPEL_INT]
		               VI Zeichen empfangen (von Koppelschnittstelle)!
		ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 VI_ERR] !
!SIO0!
VI_SIO0_B	ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 PTY_INT]
		               VI Zeichen empfangen (fuer Terminaleingabe)!
		ARRAY [2 WORD] !:= [%8000 PTY_ERR]
		               VI Empfangssonderfall (fuer Empfangsfehler)!
VI_SIO0_A	ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 VI_ERR] !
		ARRAY [2 WORD] !:= [%8000 MCZ_INT]
		               VI Zeichen empfangen (fuer MCZ-Eingabe)!
		ARRAY [2 WORD] !:= [%8000 MCZ_ERR]
		               VI Empfangssonderfall (fuer Empfangsfehler)!
! Rest der 128 Eintraege der VI-Sprung-Tabelle!
VI_TAB_REST	ARRAY [224 WORD] !:= [%8000 VI_ERR] !
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

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
System Calls
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

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

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
spezielle ASCII-Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

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

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Hardwareadressen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

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

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Moegliche Statusausgaben an 8-Bit-Teil von 16-Bit-Teil 
fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO0_B)
(INT8-Bit jeweils = 0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  CONSTANT
STAT8_ENDE	:= %00	!Ende eines Write-Requests!
STAT8_READ	:= %02	!Read-Request an einen Peripheriebaustein 8-Bit-Teil!
STAT8_WRITE	:= %04	!Write-Request an einen Peripheriebaustein 8-Bit-Teil!

STAT8_UDOS	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers
			 bzw. eines Blocktransfers von/zu UDOS!
STAT8_QUIT	:= %1E	!Rueckkehr zum 8-Bit-Teil (Datenbyte "Q" oder "R")!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Moegliche Statuseingaben von 8-Bit-Teil an 16-Bit-Teil 
fuer 8-Bit-/16-Bit-Koppelschnittstelle (PIO1_B)
(INT16-Bit jeweils = 0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  CONSTANT
STAT16_ENDE	:= %00	!Beantwortung eines Read-Requests!

STAT16_SIO0_B_R	:= %04	!REC INT (SIO0_B-Empfaengerinterrupt)!
STAT16_SIO0_B_X	:= %24	!TRANS INT (SIO0_B-Interrupt Sendepuffer leer)!
STAT16_SIO0_B_E	:= %44	!EXT STAT INT (SIO0_B-Externer-Statusinterrupt)!
STAT16_SIO0_B_S	:= %64	!SPEC REC INT (SIO0_B-Interrupt Empfangssonderfall)!

STAT16_UDOS	:= %10	!Datenbyte ist Bestandteil eines Dateitransfers
			 bzw. eines Blocktransfers von/zu UDOS!
!ende prom.include!


!******************************************************************************
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE DISPLAY
DISPLAY-Routine (Monitorkommando)
Anzeigen eines Speicherbereiches bzw. Aenderung von Speicherzellen

Kommandoeingabe: D [[<segnr>]offset [count] [W,B,L]]
                   (Anfangsadresse, Datenanzahl, Datentyp)
Fehler, wenn anstelle 'W,B,L' ein anderes Zeichen eingegeben wurde bzw. wenn
Datenanzahl = 0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    DISPLAY procedure
      entry

!*** Parameterauswertung ***!

	resb	FLAG1, #1	!Flag: noch Parameter einlesen!
	call	GET_SIGN	!rl0:=1. Zeichen nach Space in INBFF!
	ld	r7, #%0202	!IMPLIZIT WORD AUSGEBEN!
	ld	r10, PC_SEG	!implizite Segmentnummer!
	jr	nz, D_PARA	!Parameter wurden angegeben!

	ld	r1, r3		!IMPLIZITER OFFSET!
	jr	D_WRITE		!keine Parameter angegeben!

D_PARA:
	call	GET_ADR		!rr2:=eingegebene Anfangsadresse / 
				 rl0:=naechstes Zeichen in INBFF!
	JR	NZ, D2		!weitere Parameter vorhanden!

	setb	FLAG1, #1	!MERKER: ALLE PARAMETER EINGELESEN!
D2:
	ldl	rr10, rr2	!rr10:=Anfangsadresse (r10-segnr/r11-offset)!
	bitb	FLAG1, #1
	jr	nz, D_WRITE0	!Sprung zu "Speicher schreiben" (keine
				 Datenanzahl und kein Datentyp angegeben!
	calr	TST_WBL		!Test: Datentyp (W,B,L)
				 r7 := %0202 bzw. %0101 bzw. %0404!
	jr	z, D_WRITE0	!Sprung zu "Speicher schreiben" (keine
				 Datenanzahl, aber Datentyp (W,B,L) angegeben)!
	call	GET_HEXZ		!Datenanzahl angegeben!
	ld	r13, r3		!r13 := Datenanzahl!
	jr	z, D_LEN0	!CR gelesen (kein Datentyp angegeben)!

	calr	TST_WBL		!Test: Datentyp (W,B,L)
				 r7 := %0202 bzw. %0101 bzw. %0404!
	jr	nz, D_ERROR	!falscher Datentyp angegeben (nicht W,B,L)!

D_LEN0:
	test	r13
	jr	z, D_ERROR	!Datenanzahl = 0!

!*** DISPLAY READ ***!
! Aufbau der Ausgabezeile !

D_READ:
	ld	r6, #%0010	!%10 Bytes je Ausgabezeile anzeigen!
	call	PUT_3C		!Zeichen '<' in OUTBFF eintragen!
	ld	r5, r10
	ldb	rl5, rh5
	call	PUT_SGNR	!'segnr>' in OUTBFF eintragen!
	ld	r5, r11
	call	BTOH16		!'offset' in OUTBFF eintragen!
	inc	OUTPTR, #1	!1 Leerzeichen in OUTBFF!
	ld	r8, #%003A
	ldb	OUTBFF(r8), #'*' !'*' auf Pos. %3A in OUTBFF eintragen!
	inc	r8, #1
D5:
	sc	#SC_SEGV
	ldb	rl5, @r10	!BYTE VOM SPEICHER LESEN!
	sc	#SC_NSEGV
	ldb	OUTBFF(r8), rl5
	cpb	rl5, #' '	!TEST UNTERE GRENZE ASCII-CODE!
	jr	c, D6
	cpb	rl5, #%7F	!TEST OBERE GRENZE ASCII-CODE!
	jr	c, D7
D6:	
	ldb	OUTBFF(r8), #'.' !kein ASCII-Code --> Punkt in OUTBFF eintr.!
D7:
	inc	r8, #1
	ldb	OUTBFF(r8), #'*' !'*' am Ende der ASCII-Zeichen in OUTBFF!
	call	BTOH8		!Datenbyte (im ASCII-Code) in OUTBFF eintragen!
	inc	r11, #1
	dec	r6, #1
	jr	nz, D8		!Zeile noch nicht vollst. aufgebaut (%10 Byte)!

! Ausgabe der vollstaendigen Zeile !

	inc	r8, #1
	ld	OUTPTR, r8	!Laenge der Ausgabezeile!
	call	WR_OUTBFF_CR	!Ausgabe OUTBFF auf Terminal (mit CR)!
	ldb	rl7, rh7	!WORD-BYTE-LONG MERKER NEULADEN!
	dec	r13, #1		!dec Datenanzahl!
	jr	nz, D_READ	!noch nicht alle Daten angezeigt!
	ret			!fertig!

! Steuerung der Leerzeichen zwischen den Dateneinheiten (W,B,L) !

D8:
	dbjnz	rl7, D5		!naechstes Byte, wenn Dateneinheit (W,L)
 				 noch nicht vollstaendig!
	inc	OUTPTR, #1	!1 Leerzeichen zwischen 2 Dateneinheiten!
	ldb	rl7, rh7	!Anz. der Bytes je Dateneinheit wieder setzen!
	dec	r13, #1		!dec Anzahl der Dateneinheiten!
	jr	nz, D5		!naechste Dateneinheit!

	inc	r8, #1		!Datenanzahl abgearbeitet!
	ld	OUTPTR, r8
	jp	WR_OUTBFF_CR	!Ausgabe der letzten Zeile auf Terminal!

!*** DISPLAY WRITE ***!
! Aufbau und Ausgabe der Ausgabezeile !

D_WRITE0:
	cpb	rl7, #1		!BYTE AUSGABE ?  !
	jr	z, D_WRITE
	res	r11, #%0	!gerade Offsetadresse bei 'W' und 'L'!
D_WRITE:
	ldl	rr8, rr10	!rr8 := Zeilenadresse!
	call	PUT_3C		!Zeichen '<' in OUTBFF eintragen!
	ld	r5, r10
	ldb	rl5, rh5
	call	PUT_SGNR	!'segnr>' in OUTBFF eintragen!
	ld	r5, r11
	call	BTOH16		!'offset' in OUTBFF eintragen!
	inc	OUTPTR, #1	!1 Leerzeichen nach Zeilenadresse in OUTBFF!
D11:
	sc	#SC_SEGV
	ldb	rl5, @r10	!BYTE VOM SPEICHER LESEN!
	sc	#SC_NSEGV
	call	BTOH8		!Datenbyte in OUTBFF eintragen (ASCII)!
	inc	r11, #1		!OFFSET INKREMENTIEREN!
	dbjnz	rl7, D11	!alle Bytes der Dateneinheit in OUTBFF eintr.;
				 r11:= Offsetadr. des folg. Datenwertes!
	inc	OUTPTR, #1	!1 Leerzeichen nach Dateneinheit!
	ldb	rl7, rh7	!Bytezaehler je Dateneinheit neu setzen!
	call	WR_OBF_RDDATA	!Ausgabe OUTBFF auf Terminal; Ausgabe '?';
				 einlesen des zu schreibenden Datenwertes!
	jr	nc, D12		!kein 'Q' oder '-' eingegeben!	

	cpb	rl0, #'Q'
	ret	z		!'Q' eingegeben --> Abbruch!

	push	@r15, r7	!'-' eingegeben!
	clrb	rh7
	sub	r11, r7
	sub	r11, r7		!Offsetadresse um 2 Dateneinheiten dec!
	pop	r7, @r15
	jr	D_WRITE		!vorherige Dateneinheit betrachten!

D12:
	jr	z, D_WRITE	!kein Datenwert wurde engegeben!

	decb	rl7, #1
	sc	#SC_SEGV	!Datenwert in Speicher schreiben!
	jr	nz, D13
	ldb	@r8, rl3	! Byte !
	jr	D15
D13:
	decb	rl7, #1
	jr	nz, D14
	ld	@r8, r3		! Word !
	jr	D15
D14:
	ldl	@r8, rr2	! Long !
D15:
	sc	#SC_NSEGV
	ldb	rl7, rh7
	jr	D_WRITE

D_ERROR:
	jp	ERROR
    end DISPLAY

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TST_WBL
Test WORD-BYTE-LONG
Input: 	rl0 - Datentyp ('W','B','L')
Output: rh7/rl7 - Anzahl der Byte des Datentyps
	
	} %0101		wenn rl0='B'
r7 =	} %0404		wenn rl0='L'
	} %0202, Z=1	wenn rl0=CR oder rl0='W'
	} %0202, Z=0	ansonsten (Fehler)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    TST_WBL procedure
      entry
	ld	r7, #%0202
	cpb	rl0, #CR
	ret	z
	cpb	rl0, #'W'
	ret	z
	ld	r7, #%0101
	cpb	rl0, #'B'
	ret	z
	ld	r7, #%0404
	cpb	rl0, #'L'
	ret	z
	ld	r7, #%0202
	ret	
    end TST_WBL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE COMPARE
COMPARE-Routine (Monitorkommando)
Vergleich zweier Speicherbereiche

Kommandoeingabe: C [<segnr1>]offset1 [<segnr2>]offset2 count
                   (1. Anfangsadresse, 2. Anfangsadresse, Bytezahl)
unterschiedliche Bytes werden ausgegeben:
	<segnr2>offset2= inhalt  <segnr1>offset1= inhalt
Fehler, wenn Bytezahl = 0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    COMPARE procedure
      entry
	call	GET_PAR		!Einlesen und Konvertierung der Eingabezeile!
				!rr6:=1. Adr./rr4:=2. Adr./r3:=Bytezahl!
	jr	c, C_ERROR	!falsche Parameterfolge!

	ldl	rr8, rr4	!rr8 := 2. Adresse!
	ld	r10, r3		!r10 := Bytezahl!
	test	r3
	jr	z, C_ERROR	!Bytezahl = 0!
C1:
	sc	#SC_SEGV
	ldb	rl1, @r6
	cpib	rl1, @r8, r10, z	!inc offset2, dec r10!
	sc	#SC_NSEGV
	jr	z, C2		!Vergleich: identisch!

	ldl	rr4, rr8	!rr4 := 2. Adresse!
	dec	r5, #1
	calr	PUT_ADR_INHALT	!Eintragung 2. Adr. und Inhalt in OUTBFF!
	ldl	rr4, rr6	!rr4 := 1. Adresse!
	calr	PUT_ADR_INHALT	!Eintragung 1. Adr. und Inhalt in OUTBFF!
	call	WR_OUTBFF_CR	!Ausgabe des Inhaltes von OUTBFF auf Terminal!
C2:
	test	r10
	ret	z		!alle Bytes verglichen!

	inc	r7, #1		!inc offset1!
	jr	C1
    
C_ERROR:
	jp	ERROR
end COMPARE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_ADR_INHALT
Eintragung von '<segnr>offset= inhalt' in Ausgabepuffer (OUTBFF)
Input: rr4 - Adresse
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PUT_ADR_INHALT procedure
      entry
	call	PUT_3C		!Zeichen '<' in OUTBFF eintragen!
	ld	r2, r5
	ldb	rl5, rh4
	call	PUT_SGNR	!'segnr>' in OUTBFF eintragen!
	ld	r5, r2
	call	BTOH16		!'offset' in OUTBFF eintragen!
	ld	r1, #'= '
	call	PUT_2CHR	!'= ' in OUTBFF eintragen!
	sc	#SC_SEGV
	ldb	rl5, @r4
	sc	#SC_NSEGV
	call	BTOH8		!Adresseninhalt in OUTBFF eintragen!
	inc	OUTPTR, #%02	!2 Leerzeichen in OUTBFF!
	ret	
    end PUT_ADR_INHALT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FILL
FILL-Routine (Monitorkommando)
Fuellen eines Speicherbereiches

Kommandoeingabe: F [<segnr>]offset1 offset2 word
                   (Anfangsadresse, Endadresse, Datenwort)
Fehler, wenn offset1 ungerade oder (offset2 < offset1)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    FILL procedure
      entry
	call	GET_PAR		!Einlesen und Konvertierung der Eingabezeile!
				!rr6:=1. Adr./rr4:=2. Adr./r3:=Datenwort!
	jr	c, F_ERROR	!falsche Parameterfolge!

	bit	r7, #%0
	jr	nz, F_ERROR

	sub	r5, r7		!r5 := Adressendifferenz!
	jr	c, F_ERROR      !2.Adr. < 1.Adr.!

	srl	r5, #1
	inc	r5, #1		!r5 := Wortanzahl!
	sc	#SC_SEGV
F2:
	ld	@r6, r3		!Speicherbereich fuellen!
	inc	r7, #%02
	djnz	r5, F2
	sc	#SC_NSEGV
	ret	

F_ERROR:
	jp	ERROR
    end FILL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MOVE
MOVE-Routine (Monitorkommando)
Verschieben eines Speicherbereiches

Kommandoeingabe: M [<segnr1>]offset1 [<segnr2>]offset2 count
                   (Adresse Quelle, Adresse Ziel, Bytezahl)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    MOVE procedure
      entry
	call	GET_PAR		!Einlesen und Konvertierung der Eingabezeile!
				!rr6:=1. Adr./rr4:=2. Adr./r3:=Bytezahl!
	jp	c, ERROR	!falsche Parameterfolge!

	cp	r4, r6		!Vergleich der Segmentnummern!
	jr	nz, M1		!ungleich!

	cp	r7, r5		!Vergleich der Offsetadressen, wenn Segment-
                                 nummern gleich sind!
	jr	c, M2		!1.Adr. < 2. Adr.!
M1:
	sc	#SC_SEGV
	ldirb	@r4, @r6, r3	!Speicherbereich verschieben (bei ungleichen
				 Segmentnummern bzw. bei gleichen Segment-
				 nummern und 1.Adr.>=2.Adr.)!
	sc	#SC_NSEGV
	ret	
M2:
	add	r7, r3
	dec	r7, #1
	add	r5, r3
	dec	r5, #1
	sc	#SC_SEGV
	lddrb	@r4, @r6, r3	!Speicherbereich verschieben (bei gleichen
				 Segmentnummern und 1.Adr.<2.Adr.)!
	sc	#SC_NSEGV
	ret	
    end MOVE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_READ
PORT Lese-Routine (Monitorkommando PR)

Kommandoeingabe: PR portadr [W]
                    (Portadresse, Datentyp)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PORT_READ procedure
      entry
	ld	r5, #P_IN	!r5:=Adresse der PORT-I/O-Routine!
	jr	PORT_RD
    end PORT_READ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_WRITE
PORT Schreib-Routine (Monitorkommando PW)

Kommandoeingabe: PW portadr [W] X1 [X2...Xn]
                    (Portadresse, Datentyp, Datenwerte)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PORT_WRITE procedure
      entry
	ld	r5, #P_OUT	!r5:=Adresse der PORT-I/O-Routine!
	jr	PORT_WR
    end PORT_WRITE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_READ_SPECIAL
SPECIAL-PORT Lese-Routine (Monitorkommando PRS)

Kommandoeingabe: PRS portadr [W]
                     (Portadresse, Datentyp)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PORT_READ_SPECIAL procedure
      entry
	ld	r5, #PS_IN	!r5:=Adresse der PORT-I/O-Routine!
	jr	PORT_RD
    end PORT_READ_SPECIAL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_WRITE_SPECIAL
SPECIAL-PORT Schreib-Routine (Monitorkommando PWS)

Kommandoeingabe: PWS portadr [W] X1 [X2...Xn]
                     (Portadresse, Datentyp, Datenwerte)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PORT_WRITE_SPECIAL procedure
      entry
	ld	r5, #PS_OUT	!r5:=Adresse der PORT-I/O-Routine!
	jr	PORT_WR
    end PORT_WRITE_SPECIAL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_RD
PORT/SPECIAL-PORT Lese-Routine
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PORT_RD procedure
      entry
	ld	PORT_IO, r5	!Adresse der PORT-I/O-Routine retten!

	call	GET_SIGN	!rl0:=1.Zeichen nach naechstem Space in INBFF
				 (= 1. Zeichen der Portadresse)!
	jp	z, ERROR	!Zeichen=CR, d.h. keine Portadr. angegeben!

	call	GET_HEXZ	!r3:=Hexazahl (binaer) aus INBFF (Portadr.);
				 rl0:=1.Zeichen nach Portadr. ungleich Space
				      (Datentyp)!
	ld	r6, r3		!r6:=Portadresse!
	ldk	r7, #1		!r7=Datentyp:=1 (implizit Datentyp 'BYTE')!
	jr	z, PORE1	!kein Datentyp angegeben-->Datentyp 'BYTE'!

	cpb	rl0, #'W'
	jp	nz, ERROR	!falscher Parameter (Datentyp)!
	clr	r7		!r7=Datentyp:=0 (Datentyp 'WORD')!

PORE1:
	ld	r5, #PORT_BF
	add	r5, r7		!r5:=Adresse, wo gelesener Portinhalt abge-
				 speichert werden soll (=#PORT_BF bei 'WORD'-
				 bzw. =#PORT_BF+1 bei 'BYTE'-Lesen)!
	ld	r3, PORT_IO	!r3:=Adresse der PORT-I/O-Routine!	
	call	@r3		!PORT LESEN!

	ld	r5, r6		!r5:=Portadresse!
	call	BTOH16		!Portadresse IN OUTBFF EINTRAGEN!
	inc	OUTPTR, #1	!1 Leerzeichen in OUTBFF eintragen!
	ld	r5, PORT_BF	!r5:=gelesener Portinhalt!
	bit	r7, #%0
	jr	nz, PORE2	!Datentyp 'BYTE'!
	call	BTOH16		!gelesenes Datenwort in OUTBFF eintragen!
	jr	PORE3
PORE2:
	call	BTOH8		!gelesenes Datenbyte in OUTBFF eintragen!
PORE3:
	jp	WR_OUTBFF_CR	!Inhalt OUTBFF auf Terminal ausgeben!
    end PORT_RD

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PORT_WR
PORT/SPECIAL-PORT Schreib-Routine
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PORT_WR procedure
      entry
	ld	PORT_IO, r5	!Adresse der PORT-I/O-Routine retten!

	call	GET_SIGN	!rl0:=1.Zeichen nach naechstem Space in INBFF
				 (=1. Zeichen der Portadresse)!
	jp	z, ERROR	!CR gelesen, d.h. keine Parameter angegeben!

	call	GET_HEXZ	!r3:=Hexazahl (binaer) aus INBFF (Portadr.);
				 rl0:=1.Zeichen nach Portadr. ungleich Space
				      (Datentyp bzw. 1. Datenwert)!
	jp	z, ERROR	!CR gelesen, d.h. keine weiteren Parameter!

	ld	r6, r3		!r6:=Portadresse!
	ldk	r7, #1		!r7=Datentyp:=1 (implizit Datentyp 'BYTE')!
	cpb	rl0, #'W'	
	jr	nz, POWR_LOOP	!kein Datentyp angegeben-->Datentyp 'BYTE'!
	clr	r7		!r7=Datentyp:=0 (Datentyp 'WORD')!
	call	GET_SIGN	!rl0:=1.Zeichen nach naechstem Space in INBFF
				 (=1. Zeichen des 1. Datenwertes)!
	jp	z, ERROR	!CR gelesen, d.h. keine weiteren Parameter
				 (Datenwerte) angegeben!

POWR_LOOP:
	call	GET_HEXZ	!r3:=Hexazahl (binaer) aus INBFF (Datenwert);
				 rl0:=1.Zeichen nach Datenwert ungleich Space
				      (naechster Datenwert)!
	ldctlb	rh0, FLAGS	!Stand Z-Flag retten!

	ld	r5, #PORT_BF
	ld	@r5, r3		!an Port auszugebenden Datenwert in #PORT_BF
				 abspeichern!
	add	r5, r7		!r5:=Adresse, wo auszugebender Datenwert steht
				 (=#PORT_BF bei 'WORD'/=#PORT_BF+1 bei 'BYTE')!
	ld	r3, PORT_IO	!r3:=Adresse der PORT-I/O-Routine!
	call	@r3		!PORT SCHREIBEN!
	
	bitb	rh0, #%06	!Z-Flag testen!
	ret	nz		!Z-Flag war nach GET_HEXZ gesetzt, d.h. CR
				 wurde gelesen, d.h. keine weiteren Daten-
				 werte vorhanden!
	jr	POWR_LOOP	!naechsten Datenwert an Port ausgeben!
    end PORT_WR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE P_IN
Normal-Port-Input
Input:	r7 - einzulesender Datentyp (0-'WORD' / 1-'BYTE')
	r5 - Adresse, wo gelesener Portinhalt abgespeichert werden soll
	r6 - Portadresse
Output:	auf urspruenglicher Adresse r5 - gelesener Portinhalt
zerstoerte Register:	r3, r5
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    P_IN procedure
      entry
	bit	r7, #%0
	jr	nz, P_INB
	ini	@r5, @r6, r3	!Normal Input ('WORD')!
	ret	
P_INB:
	inib	@r5, @r6, r3	!Normal Input ('BYTE')!
	ret	
    end P_IN

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PS_IN
Special-Port-Input
Input:	r7 - einzulesender Datentyp (0-'WORD' / 1-'BYTE')
	r5 - Adresse, wo gelesener Portinhalt abgespeichert werden soll
	r6 - Portadresse
Output:	auf urspruenglicher Adresse r5 - gelesener Portinhalt
zerstoerte Register:	r3, r5
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PS_IN procedure
      entry
	bit	r7, #%0
	jr	nz, PS_INB
	sini	@r5, @r6, r3	!Special Input ('WORD')!
	ret	
PS_INB:
	sinib	@r5, @r6, r3	!Special Input ('BYTE')!
	ret	
    end PS_IN

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE P_OUT
Normal-Port-Input
Input:	r7 - auszugebender Datentyp (0-'WORD' / 1-'BYTE')
	r5 - Adresse, auf der der auszugebende Datenwert abgespeichert ist
	r6 - Portadresse
zerstoerte Register:	r3, r5
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    P_OUT procedure
      entry
	bit	r7, #%0
	jr	nz, P_OUTB
	outi	@r6, @r5, r3	!Normal Output ('WORD')!
	ret	
P_OUTB:
	outib	@r6, @r5, r3	!Normal Output ('BYTE')!
	ret	
    end P_OUT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PS_OUT
Special-Port-Output
Input:	r7 - auszugebender Datentyp (0-'WORD' / 1-'BYTE')
	r5 - Adresse, auf der der auszugebende Datenwert abgespeichert ist
	r6 - Portadresse
zerstoerte Register:	r3, r5
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PS_OUT procedure
      entry
	bit	r7, #%0
	jr	nz, PS_OUTB
	souti	@r6, @r5, r3	!Special Output ('WORD')!
	ret	
PS_OUTB:
	soutib	@r6, @r5, r3	!Special Output ('BYTE')!
	ret	
    end PS_OUT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE HARD_DISK_READ
Lesen eines Blockes von Hard-Disk (Monitorkommando)

Kommandoeingabe: HR block [<segnr>]offset [dev]
                    (Blocknummer(32 Bit), Pufferadresse,
                                            Devicenummer (0-3, implizit 0))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    HARD_DISK_READ PROCEDURE
      entry
	ld	r7, #1
	jr	HARD_DISK
    end HARD_DISK_READ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE HARD_DISK_WRITE
Schreiben eines Blockes auf Hard-Disk (Monitorkommando)

Kommandoeingabe: HW block <segnr>offset [dev]
                    (Blocknummer(32 Bit), Pufferadresse,
                                            Devicenummer (0-3, implizit 0))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    HARD_DISK_WRITE PROCEDURE
      entry
	ld	r7, #2
	jr	HARD_DISK
    end HARD_DISK_WRITE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE HARD_DISK
Lesen bzw. Schreiben eines Blockes auf Hard-Disk 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    HARD_DISK PROCEDURE
      entry
	CALL	GET_DATA	!rr2:=eingeg. Blocknummer;
				 INPTR zeigt auf 2 Zeichen nach Blocknr.!
	ldl	rr4, rr2	!rr4:=Blocknummer!
	jp	z, ERROR	!keine Blocknummer angeg. (CR gelesen)!

	call	GET_SIGNW	!rl0:=naechstes Zeichen ungl. Space aus INBFF 
				 (=1.Zeichen der eingeg. Pufferadresse)!
	call	GET_ADR		!rr2:=eingeg. Pufferadresse;
				 rl0:=Zeichen ungl. Space nach Pufferadresse;
				 INPTR zeigt auf darauffolgendes Zeichen!
	jp	c, ERROR	!keine Pufferadresse angegeben!
	ldl	rr8, rr2	!rr8:=Pufferadresse!

	ld	r6, #0		!implizit DRIVE 0!
	jr	z, HD_E		!keine Devicenr. angeg.!

	subb	rl0, #%30	!DRIVE ADRESSE BILDEN!
	cpb	rl0, #4		!MAX. 4 DRIVES!
	jp	nc, ERROR	!Drivenr. >= 4!

	ldb	rl6, rl0	!r6:=Drivenr.!
HD_E:
	ld	r10, #%200	!r10:=Blocklaenge!
	sc	#SC_DISK	!Aufruf DISK!
	test	r2		!Test Returncode!
	ret	z		!kein Fehler aufgetreten!

	push	@r15, r2
	ld	r2, #T_HD_ERR
	sc	#SC_WR_MSG	!Textausgabe!
	pop	r5, @r15	!rl5 := Returncode!
	call	BTOH8		!rl5 (ASCII) in OUTBFF eintragen!
	sc	#SC_WR_OUTBFF_CR	!Ausgabe Returncode!
	ret
    end HARD_DISK

  INTERNAL
T_HD_ERR:
	WORD	:= %0010
	ARRAY [* BYTE] := 'HARD DISK ERROR '

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLOPPY_FORMAT
Formatieren einer blockorientierten Floppy-Disk (Monitorkommando)

Kommandoeingabe: FF typ/dev
                    (Floppytyp (0-F)/Devicenummer (0-3))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    FLOPPY_FORMAT PROCEDURE
      entry
	ld	r7, #3
	call	GET_SIGN	!rl0:=1. Zeichen nach naechsten Space in INBFF
				 (= Zeichen der Devicenummer)!
	jp	z, ERROR	!keine Devicenummer angegeben!
	jr	FLOPPY_BLOCK1
    end FLOPPY_FORMAT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLOPPY_READ
Lesen eines Blockes von blockorientierter Floppy-Disk (Monitorkommando)

Kommandoeingabe: FR block [<segnr>]offset [typ/dev]
                    (Blocknummer(16 Bit), Pufferadresse,
                    			   Floppytyp (0-F)/Devicenummer (0-3) 
								 (implizit 60))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    FLOPPY_READ PROCEDURE
      entry
	ld	r7, #1
	jr	FLOPPY_BLOCK
    end FLOPPY_READ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLOPPY_WRITE
Schreiben eines Blockes auf blockorientierte Floppy-Disk (Monitorkommando)

Kommandoeingabe: FW block [<segnr>]offset [typ/dev]
                    (Blocknummer(16 Bit), Pufferadresse,
                    			   Floppytyp (0-F)/Devicenummer (0-3) 
								 (implizit 60))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    FLOPPY_WRITE PROCEDURE
      entry
	ld	r7, #2
	jr	FLOPPY_BLOCK
    end FLOPPY_WRITE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLOPPY_BLOCK
Lesen bzw. Schreiben eines Blockes auf blockorientierte Floppy-Disk 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    FLOPPY_BLOCK PROCEDURE
      entry
	CALL	GET_DATA	!rr2:=eingeg. Blocknummer;
				 INPTR zeigt auf 2 Zeichen nach Blocknr.!
	ldl	rr4, rr2	!rr4:=Blocknummer!
	jp	z, ERROR	!keine Blocknummer angeg. (CR gelesen)!

	call	GET_SIGNW	!rl0:=naechstes Zeichen ungl. Space aus INBFF 
				 (=1.Zeichen der eingeg. Pufferadresse)!
	call	GET_ADR		!rr2:=eingeg. Pufferadresse;
				 rl0:=Zeichen ungl. Space nach Pufferadresse;
				 INPTR zeigt auf darauffolgendes Zeichen!
	jp	c, ERROR	!keine Pufferadresse angegeben!
	ldl	rr8, rr2	!rr8:=Pufferadresse!

	ld	r6, #%60	!implizit Floppytyp 6/Devicenummer 0!
	jr	z, FD_E		!keine Devicenr. angeg.!

FLOPPY_BLOCK1:
	call	GET_HEXZ	!r3:=eingeg. typ/dev!
	ld	r6, r3
	ldb	rh6, #0
	ldb	rl0, rl6
	andb	rl0, #%0F
	cpb	rl0, #4		!MAX. 4 DRIVES!
	jp	nc, ERROR	!Drivenr. >= 4!

FD_E:
	ld	r10, #%200	!r10:=Blocklaenge!
	sc	#SC_FLOPPY	!Aufruf Floppytreiber!
	test	r2		!Test Returncode!
	ret	z		!kein Fehler aufgetreten!

	push	@r15, r2
	ld	r2, #T_FLP_ERR
	sc	#SC_WR_MSG	!Textausgabe!
	pop	r5, @r15	!rl5 := Returncode!
	call	BTOH8		!rl5 (ASCII) in OUTBFF eintragen!
	sc	#SC_WR_OUTBFF_CR	!Ausgabe Returncode!
	ret
    end FLOPPY_BLOCK

  INTERNAL
T_FLP_ERR:
	WORD	:= 13
	ARRAY [* BYTE] := 'FLOPPY ERROR '

end p_comm
