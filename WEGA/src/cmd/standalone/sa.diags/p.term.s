!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_term

  Bearbeiter:	J. Zabel
  Datum:	27.01.1989
  Version:	3.1

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_term
------------------------------
Dieser Modul umfasst:
- Routinen zur Terminalein- und -ausgabe,
- Routinen zur Ein- und Ausgabe fuer den Dateitransfer von/zu UDOS ueber
  die 8-Bit-/16-Bit-Koppelschnittstelle,
- Konvertierungsroutinen (Binaer <--> ASCII-Hexa)
- Routinen zur Arbeit mit dem Eingabepuffer INBFF und dem Ausgabepuffer OUTBFF.

Routinen, die direkte Eingaben von den Interrupt-Eingabepuffern PTYBFF/UDOSBFF
bzw. direkte Ausgaben auf den Terminal-Kanal/Koppelschnittstelle durchfuehren,
sind i.A. mit RD bzw. WR gekennzeichnet.
Routinen, die Eingaben vom Eingabepuffer INBFF bzw. Ausgaben in den
Ausgabepuffer OUTBFF durchfuehren, sind i.A. mit GET bzw. PUT gekennzeichnet.

Prozeduren
----------
PTY_INT
PTY_ERR
KOPPEL_INT
INCPTR
TYRD
TYWR
TYRD_UDOS_INT
TYRD_UDOS_POL
TYWR_UDOS_INT
TYWR_UDOS_POL
TYWR_KOPPEL_INT
TYWR_KOPPEL_POL
KP_OUTB
KP_INB
TYRDWR
RD_LINE_BFF
RD_LINE_INBFF
CLRCHR
WR_CRLF
WR_NULL
WR_MSG
WR_OUTBFF_CR
WR_OUTBFF

HTOB
BTOH16
BTOH8
BTOH4

GET_CHR
GET_SIGN
GET_SIGNW
GET_SGNR
GET_HEXZ
GET_ADR
GET_PAR
RD_DATA
GET_DATA
WR_R5_RDDATA
WR_OBF_RDDATA
PUT_CHR
PUT_3C
PUT_1CHR
PUT_2CHR
PUT_SEGNR
PUT_SGNR
==============================================================================!


p_term module

$SECTION PROM

!anfang mon16.include!
!******************************************************************************
allgemeine Vereinbarungen (Stand: 27.01.1989)
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
externe Prozeduren und Marken
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  EXTERNAL
!INIT_CTC0_0		procedure		 Modul p_init!
INIT_CTC0_1		procedure		!Modul p_init!
!INIT_SIO0_A		procedure		 Modul p_init!
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

!
QUIT			procedure		Modul p_ldsd
LOAD			procedure		Modul p_ldsd
MCZ_INT			procedure		Modul p_ldsd
MCZ_ERR			procedure		Modul p_ldsd
TYWR_MCZ		procedure		Modul p_ldsd
WOCF_MCZ		procedure		Modul p_ldsd
SEND			procedure		Modul p_ldsd
!

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
MCZBFF		ARRAY [%0EA BYTE]	!Ein-/Ausgabepuffer fuer LOAD/SEND!
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
PSA_SC		ARRAY [4 WORD] !:= [%0000 %C000 %8000 SC_ENTRY]
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
		ARRAY [2 WORD] !:= [%8000 VI_ERR]   (vorher MCZ_INT)
		               VI Zeichen empfangen (fuer MCZ-Eingabe)!
		ARRAY [2 WORD] !:= [%8000 VI_ERR]   (vorher MCZ_ERR)
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
ERR_WDC		WORD	!Fehlerzaehler fuer WDC-Test!
MAX_SEGNR	WORD	!Merker: hoechste vorhandene Segmentnummer!
REM_PARNMI	WORD	!Merker: Paritaets-NMI aufgetreten
		                 (%0000-->nein / %FFFF-->ja)!
ERRPAR_ID	WORD	!Spezifizierung der 4 Parameter der auszugebenden
			 Fehlerzeile (Prozedur MSG_ERROR)!
REM_MMU1	WORD
REM_MMU2	WORD
REM_INT		WORD	!Merker: Interrupt aufgetreten (0-->nein)!
REM_WDC1	WORD	!Speicherplatz fuer Testdatenwert!
REM_WDC2	WORD	!Speicherplatz fuer rueckgelesenen datenwert!

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
!
SC_WOCF_MCZ		:= %18		Modul p.ldsd
SC_TYWR_MCZ		:= %1A		Modul p.ldsd
!
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

SIO0	:= %FF81	!Kanal A (XB1/TTY4): unbenutzt (vorher LOAD/SEND/QUIT);
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

LON	:= %FFD9	!RUN16-Lampe ein!
LOFF	:= %FFB9	!RUN16-Lampe aus!
  
S_BNK	:= %FFC1	!SWITCH-BANK:
			 Bit0-3 Ausgabebits; Bit4-7 Eingabebits (DIL-Schalter):
			 Bit 0: =0 - Monitor PROM+RAM einschalten
				=1 - Monitor PROM+RAM ausschalten
			 Bit 1: =0 - MMU's ausschalten; =1 - MMU's einschalten
			 Bit 2: Seg. Usr.
			 Bit 3: =0 - Paritaetspruefung ausgeschaltet bzw.
                                     Paritaetsfehler loeschen
                                =1 - Paritaetspruefung eingeschaltet
			 Bit 5,4: Baudrate Terminal (unbenutzt)
			 Bit 7,6: Autoboot Device (unbenutzt)!

SBREAK	:= %FFC9	!System Break Register!
NBREAK	:= %FFD1	!Normal Break Register!
RETI_P	:= %FFE1	!RETI-Port fuer Schaltkreise des U880-Systems!

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
!ende mon16.include!


!******************************************************************************
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PTY_INT
VI-Routine fuer Dateneingabe vom Terminalkanal SIO0_B des 16-Bit-Teils;
Abspeicherung des Zeichens im Interrupt-Eingabepuffer PTYBFF;
Aktualisierung von INPTR_PTY und COUNT_PTY;
Test auf Sonderzeichen (^S, ^Q, ESC) bei Terminaleingaben
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PTY_INT procedure
      entry
	push	@r15, r0
	push	@r15, r1
	push	@r15, r2
!bei erster TerminaleingabeNMI-Entry in PSAREA aendern und PROMT setzen!
	cp	PSA_NMI+6, #AUTOBOOT	!NMI_ENTRY in PSAREA:
					- nach RESET steht dort #AUTOBOOT, d.h.
					  bei NMI erfolgt Abarbeitung von
					  #AUTOBOOT
					- nach Eingabe des ersten Zeichens vom
					  Terminal (Abarbeitung von PTY_INT oder
					  KOPPEL_INT) bzw. nach Abarbeitung
					  von #AUTOBOOT wird #AUTOBOOT durch
					  #NMI_INT ersetzt, d.h. bei NMI erfolgt
					  jetzt Abarbeitung von #NMI_INT !
	jr	nz, PTY_RD
	ld	PROMT, #'* '	!Promt-Zeichen eintragen (d.h. die Zelle PROMT
				 wird erst nach der 1. Zeicheneingabe geladen)!
	ld	PSA_NMI+6, #NMI_INT	!#AUTOBOOT durch #NMI_INT ersetzen in
					 PSAREA!
!Zeichen vom SIO einlesen!
PTY_RD:
	inb	rl1, SIO0D_B	!Zeichen vom SIO einlesen!
!Test, ob eingelesenes Zeichen vom Terminalkanal als ASCII-Zeichen oder
 als beliebiges Datenbyte zu interpretieren ist!	
	bitb	FLAG0, #%00
	jr	nz, PTY_WR	!keine ASCII-Zeichen einzulesen
				--> Zeichen in PTYBFF abspeichern!
	resb	rl1, #%07	!RES BIT7 DES ASCII-ZEICHENS!
!Test auf ^Q!
	cpb	rl1, XONCHR	!Test auf ^Q (Terminalausgabe nicht gesperrt)!
	jr	nz, PTY1	!Zeichen nicht ^Q!
	resb	FLAG0, #%02	!Bit2/FLAG0=0: Terminalausgabe nicht gesperrt!
	jr	PTYEND
!Test auf ^S!
PTY1:
	cpb	rl1, XOFCHR	!Test auf ^S (Terminalausgabe gesperrt)!
	jr	nz, PTY2	!Zeichen nicht ^S!
	setb	FLAG0, #%02	!Bit2/FLAG0=1: Terminalausgabe nicht gesperrt!
	jr	PTYEND
!Test auf ESC!
PTY2:
	cpb	rl1, #ESC	!Test auf ESC (Abbruchtaste)!
	jr	nz, PTY_WR	!Zeichen nicht ESC!
	bitb	FLAG0, #%03	!wenn Bit3/FLAG0 und Bit4/FLAG0 = 0, dann
				 wird ESC als normales ASCII-Zeichen in PTYBFF
				 abgespeichert und nicht als Abbruchtaste
				 behandelt!
	jr	nz, PTY3
	bitb	FLAG0, #%04
	jr	z, PTY_WR	!ESC in PTYBFF eintragen!
!ESC als Abbruchtaste behandeln!
PTY3:	setb	FLAG0, #%05	!Bit5/FLAG0=1: Abbruchtaste gedrueckt!
	jr	PTYEND
!Zeichen in PTYBFF abspeichern!
PTY_WR:
	ld	r0, INPTR_PTY
	calr	INCPTR		!r2:=r0; r0:=ro+1!
	ld	INPTR_PTY, r0	!incr Eingabezeiger (Zeiger zum naechsten
				 freien Platz in PTYBFF)!
	ldb	PTYBFF(r2), rl1	!Zeichen in PTYBFF abspeichern!
	incb	COUNT_PTY, #%01	!Zeichenzaehler incr.!
!SIO ruecksetzen!
PTYEND:
	ldb	rl1, #%38	!RETI an SIO ausgeben!
	outb	SIO0C_A, rl1
	
	pop	r2, @r15
	pop	r1, @r15
	pop	r0, @r15
	sc	#SC_SEGV
	nop	
	iret	
    end PTY_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PTY_ERR
VI-Routine fuer Empfangsfehler von Terminalkanal SIO0_B des 16-Bit-Teils
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PTY_ERR procedure
      entry
	push	@r15, r1
	ldb	rl1, #%30
	outb	SIO0C_B, rl1	!SIO-Error-Reset!
	ldb	rl1, #%38
	outb	SIO0C_A, rl1	!RETI an SIO senden!
	pop	r1, @r15
	sc	#SC_SEGV
	iret	
    end PTY_ERR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE KOPPEL_INT
VI-Routine fuer Dateneingabe von Koppelschnittstelle PIO1 von Terminalkanal
SIO0_B des 8-Bit Teils bzw. bei Datei-/Blocktransfer von/zu UDOS;
Abspeicherung des Zeichens im Interrupt-Eingabepuffer PTYBFF bzw. UDOSBFF;
Aktualisierung von INPTR_PTY/COUNT_PTY bzw. INPTR_UDOS/COUNT_UDOS;
Test auf Sonderzeichen (^S, ^Q, ESC) bei Terminaleingaben
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    KOPPEL_INT procedure
      entry
	push	@r15, r0
	push	@r15, r1
	push	@r15, r2

!Warten, bis Datenbyte anliegt!
KP_0:
	inb	rl1, PIO0D_B
	bitb	rl1, #5		!DATEN DA?!
	jr	nz, KP_0	!nein!
!Status/Datenbyte von Koppelschnittstelle einlesen!
	inb	rh1, PIO1D_B	!Statuseingabe!
	andb	rh1, #%7E	!Statusbits sind Bit1-6!
	inb	rl1, PIO1D_A	!Dateneingabe!
!Statusauswertung!
	cpb	rh1, #STAT16_SIO0_B_R	!REC INT!
					!(SIO0_B_8 Empfaengerinterrupt)!
	jr	z, KOPPEL_PTY		!Datenbyte fuer PTY_BFF!

	cpb	rh1, #STAT16_SIO0_B_X	!TRANS INT!
	jr	nz, KP_1		!(SIO0_B_8 Interrupt Sendepuffer leer)!
	ldb	XEMPTY, #0		!Kennzeichnung: Sendepuffer leer!
	jr	KOPPEL_END

KP_1:
	cpb	rh1, #STAT16_SIO0_B_E	!EXT STAT INT!
	jr	z, KOPPEL_END		!(SIO0_B_8 externer Status-Interrupt)!

	cpb	rh1, #STAT16_SIO0_B_S	!SPEC REC COND INT!
	jr	z, KOPPEL_END		!(SIO0_B_8 Int. Empfangssonderfall)!

	cpb	rh1, #STAT16_UDOS	!Dateitransfer bzw. Blocktransfer!
	jr	z, KOPPEL_UDOS		!Datenbyte fuer UDOS_BFF!

	jr	KOPPEL_END	!falscher Status --> Zeichen wird uebergangen!

!*** Datenbyte kam vom Terminal und ist in PTY_BFF abzuspeichern ***!
KOPPEL_PTY:
!bei erster Terminaleingabe NMI-Entry in PSAREA aendern und PROMT setzen!
	cp	PSA_NMI+6, #AUTOBOOT	!NMI_ENTRY in PSAREA:
					- nach RESET steht dort #AUTOBOOT, d.h.
					  bei NMI erfolgt Abarbeitung von
					  #AUTOBOOT
					- nach Eingabe des ersten Zeichens vom
					  Terminal (Abarbeitung von PTY_INT oder
					  KOPPEL_INT) bzw. nach Abarbeitung
					  von #AUTOBOOT wird #AUTOBOOT durch
					  #NMI_INT ersetzt, d.h. bei NMI erfolgt
					  jetzt Abarbeitung von #NMI_INT !
	jr	nz, KP_PTY_0
	ld	PROMT, #'* '	!Promt-Zeichen eintragen (d.h. die Zelle PROMT
				 wird erst nach der 1. Zeicheneingabe geladen)!
	ld	PSA_NMI+6, #NMI_INT	!#AUTOBOOT durch #NMI_INT ersetzen in
					 PSAREA!
!Test, ob eingelesenes Zeichen vom Terminalkanal als ASCII-Zeichen oder
 als beliebiges Datenbyte zu interpretieren ist!	
KP_PTY_0:	
	bitb	FLAG0, #%00
	jr	nz, KP_PTY_WR	!keine ASCII-Zeichen einzulesen
				--> Zeichen in PTYBFF abspeichern!
	resb	rl1, #%07	!RES BIT7 DES ASCII-ZEICHENS!
!Test auf ^Q!
	cpb	rl1, XONCHR	!Test auf ^Q (Terminalausgabe nicht gesperrt)!
	jr	nz, KP_PTY_1	!Zeichen nicht ^Q!
	resb	FLAG0, #%02	!Bit2/FLAG0=0: Terminalausgabe nicht gesperrt!
	jr	KOPPEL_END
!Test auf ^S!
KP_PTY_1:
	cpb	rl1, XOFCHR	!Test auf ^S (Terminalausgabe gesperrt)!
	jr	nz, KP_PTY_2	!Zeichen nicht ^S!
	setb	FLAG0, #%02	!Bit2/FLAG0=1: Terminalausgabe nicht gesperrt!
	jr	KOPPEL_END
!Test auf ESC!
KP_PTY_2:
	cpb	rl1, #ESC	!Test auf ESC (Abbruchtaste)!
	jr	nz, KP_PTY_WR	!Zeichen nicht ESC!
	bitb	FLAG0, #%03	!wenn Bit3/FLAG0 und Bit4/FLAG0 = 0, dann
				 wird ESC als normales ASCII-Zeichen in PTYBFF
				 abgespeichert und nicht als Abbruchtaste
				 behandelt!
	jr	nz, KP_PTY_3
	bitb	FLAG0, #%04
	jr	z, KP_PTY_WR	!ESC in PTYBFF eintragen!
!ESC als Abbruchtaste behandeln!
KP_PTY_3:
	setb	FLAG0, #%05	!Bit5/FLAG0=1: Abbruchtaste gedrueckt!
	jr	KOPPEL_END
!Zeichen in PTYBFF abspeichern!
KP_PTY_WR:
	ld	r0, INPTR_PTY
	calr	INCPTR		!r2:=r0; r0:=r0+1!
	ld	INPTR_PTY, r0	!incr Eingabezeiger (Zeiger zum naechsten
				 freien Platz in PTYBFF)!
	ldb	PTYBFF(r2), rl1	!Zeichen in PTYBFF abspeichern!
	incb	COUNT_PTY, #%01	!Zeichenzaehler incr!
	jr	KOPPEL_END

!*** Datenbyte kam vom Dateitransfer bzw. Blocktransfer von/zu UDOS
			 	       und ist in UDOS_BFF abzuspeichern ***!
KOPPEL_UDOS:
!Zeichen in UDOSBFF abspeichern!
	ld	r0, INPTR_UDOS
	calr	INCPTR		!r2:=r0; r0:=r0+1!
	ld	INPTR_UDOS, r0	!incr Eingabezeiger (Zeiger zum naechsten
				 freien Platz in UDOSBFF)!
	ldb	UDOSBFF(r2), rl1 !Zeichen in UDOSBFF abspeichern!
	incb	COUNT_UDOS, #%01 !Zeichenzaehler incr!
	jr	KOPPEL_END

KOPPEL_END:
!U880-RETI (ED4D) an PIO ausgeben!
	ldb	rl1, #%ED
	outb	RETI_P, rl1
	ldb	rl1, #%4D
	outb	RETI_P, rl1
	
	pop	r2, @r15
	pop	r1, @r15
	pop	r0, @r15
	sc	#SC_SEGV
	nop	
	iret	
	
    end KOPPEL_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INCPTR
Pufferzeiger (der Puffer PTYBFF bzw. UDOSBFF) inkrementieren 
und auf Ueberlauf testen
Input:	r0 - Pufferzeiger 
Output: r2 := r0 (alter Stand des Pufferzeigers)
	r0 := r0 + 1, wenn C=1, d.h. r0+1 < 256
	r0 := 0, wenn C=0, d.h. r0+1 >= 256
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    INCPTR procedure
      entry
	ld	r2, r0
	inc	r0, #%01
	cp	r0, #%0100
	ret	c
	clr	r0
	ret	
    end INCPTR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYRD
Einlesen eines Zeichens aus Interrupt-Eingabepuffer PTYBFF
Output:	rl0 - eingelesenes Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYRD procedure
      entry
	outb	LOFF, rl0	!RUN16-Lampe aus!
TYRD1:	testb	COUNT_PTY	!Zeichen in PTYBFF vorhanden?!
	jr	z, TYRD1	!nein --> warten bis Zeichen empfangen wird!
	push	@r15, r2	
	ld	r0, OUTPTR_PTY
	calr	INCPTR		!Ausgabezeiger fuer PTYBFF incr.!
	ld	OUTPTR_PTY, r0
	ldb	rl0, PTYBFF(r2)	!rl0 := Zeichen aus PTYBFF!
	decb	COUNT_PTY, #%01 !Zeichenzaehler decr.!
	pop	r2, @r15
	outb	LON, rl0	!RUN16-Lampe ein!
	ret	
    end TYRD

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR
Ausgabe eines Zeichens auf Terminalkanal 
(im Pollingbetrieb - Interruptbetrieb moeglich)
(ueber Koppelschnittstelle PIO0 (an SIO0_B des 8-Bit-Teils) bzw.
ueber SIO0_B des 16-Bit-Teils)
Input:	rl0 - Zeichen
Output:	Z=1, wenn Zeichen = CR
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR procedure
      entry
	push	@r15, r0
	testb	PTYPORT
	jr	nz, TYWR_PIO	!Terminalbetrieb ueber Koppelschnittstelle!

!Ausgabe ueber SIO0_B!
TYWR_SIO:
	bitb	FLAG0, #%02	!Terminal gesperrt?!
	jr	nz, TYWR_SIO	!ja!
 
	inb	rh0, SIO0C_B
	bitb	rh0, #%02	!Sendepuffer leer?!
	jr	z, TYWR_SIO	!nein!

	outb	SIO0D_B, rl0	!Datenbyte ausgeben!
	jr	TYWR_END
	
!Ausgabe ueber Koppelschnittstelle (PIO0) an SIO0_B des 8-Bit-Teils!
TYWR_PIO:
	bitb	FLAG0, #%02	!Terminal gesperrt?!
	jr	nz, TYWR_PIO	!ja!
	
	!Test, ob Sendepuffer SIO0_B des 8-Bit-Teils leer ist!
	! 1. bei Interruptbetrieb!
	!
	testb	XEMPTY		|Sendepuffer leer?
				(XEMPTY wird in KOPPEL_INT auf %00 gesetzt
				 wenn Int. "Sendepuffer leer" erfolgte)|
	jr	nz, TYWR_PIO	|nein|
	!
	! 2. bei Pollingbetrieb!
	ldb	rl0, #SIO0C_B_8	!Adresse SIO0C_B auf 8-Bit-Teil!
	calr	KP_INB		!rh0 := eingelesenes Byte von SIO0C_B!
	bitb	rh0, #%02	!Sendepuffer leer?!
	jr	z, TYWR_PIO	!nein!

	!Datenbyte an SIO0D_B des 8-Bit-Teils ausgeben!
	ldb	XEMPTY, #%FF	!Kennzeichnung: Sendepuffer nicht leer!
	
	pop	r0, @r15
	push	@r15, r0
	ldb	rh0, rl0	!rh0:=auszugebendes Zeichen!
	ldb	rl0, #SIO0D_B_8	!Adresse SIO0D_B auf 8-Bit-Teil!
	calr	KP_OUTB		

TYWR_END:
	pop	r0, @r15
	cpb	rl0, #CR	!Z=1, wenn Zeichen CR war!
	ret	
    end TYWR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYRD_UDOS_INT
Einlesen eines Zeichens von Dateitransfer von/zu UDOS bzw. Floppy-Blocktransfer
aus Interrupt-Eingabepuffer UDOSBFF (Status STAT16_UDOS)
Output:	rl0 - eingelesenes Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYRD_UDOS_INT procedure
      entry
	testb	COUNT_UDOS	!Zeichen in UDOSBFF vorhanden?!
	jr	z, TYRD_UDOS_INT !nein --> warten bis Zeichen empfangen wird!
	push	@r15, r2	
	ld	r0, OUTPTR_UDOS
	calr	INCPTR		!Ausgabezeiger fuer UDOSBFF incr.!
	ld	OUTPTR_UDOS, r0
	ldb	rl0, UDOSBFF(r2) !rl0 := Zeichen aus UDOSBFF!
	decb	COUNT_UDOS, #%01 !Zeichenzaehler decr.!
	pop	r2, @r15
	ret	
    end TYRD_UDOS_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYRD_UDOS_POL
Einlesen eines Zeichens von Dateitransfer von/zu UDOS bzw. Floppy-Blocktransfer
im Polling (Status STAT16_UDOS)
Output:	rl0 - eingelesenes Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYRD_UDOS_POL procedure
      entry
	di	vi
	inb	rh0, PIO0D_B
	bitb	rh0, #5			!DATEN DA?!
	jr	nz, TYRD_UDOS_POL	!nein!

	inb	rh0, PIO1D_B	!Statuseingabe!
	andb	rh0, #%7E
	cpb	rh0, #STAT16_UDOS
	jr	nz, TYRD_UDOS_POL !falscher Status!

	inb	rl0, PIO1D_A	!Dateneingabe!
	ei	vi
	ret
    end TYRD_UDOS_POL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR_UDOS_INT
Ausgabe eines Zeichens fuer Dateitransfer von/zu UDOS bzw. Floppy-Blocktransfer
(ueber Koppelschnittstelle PIO0)
--- mit Interruptausloesung auf 8-Bit-Teil (INT8-Bit=1) ---
Input:	rl0 - Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR_UDOS_INT procedure
      entry
	di	vi
	ldb	rh0, #STAT8_UDOS !Status!
	calr	TYWR_KOPPEL_INT	!Zeichen+Status/INT8 an Koppelschnittstelle 
				 senden!
	ei	vi
	ret	
    end TYWR_UDOS_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR_UDOS_POL
Ausgabe eines Zeichens fuer Dateitransfer von/zu UDOS bzw. Floppy-Blocktransfer
(ueber Koppelschnittstelle PIO0)
--- ohne Interruptausloesung auf 8-Bit-Teil (INT8-Bit=0) ---
Input:	rl0 - Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR_UDOS_POL procedure
      entry
	di	vi
	ldb	rh0, #STAT8_UDOS !Status!
	calr	TYWR_KOPPEL_POL	!Zeichen+Status an Koppelschnittstelle senden!
	ei	vi
	ret	
    end TYWR_UDOS_POL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR_KOPPEL_INT
allgemeine Ausgabe eines Zeichens und zugehoerigen Status an
8-Bit-/16-Bit-Koppelschnittstelle (PIO0)
--- mit Interruptausloesung auf 8-Bit-Teil (INT8-Bit=1) ---
Input:	rl0 - Zeichen
	rh0 - Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR_KOPPEL_INT procedure
      entry
	push	@r15, r0
TKI1:
	inb	rh0, PIO0D_B
	bitb	rh0, #%06	!ENABLE OUTPUT?!
	jr	nz, TKI1	!nein!

	outb	PIO0D_A, rl0	!Datenbyte ausgeben!
	pop	r0, @r15
	resb	rh0, #0		!Bit0=0: INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rh0
	setb	rh0, #0		!Bit0=1: INT8-Bit auf 1 setzen!
	outb	PIO0D_B, rh0	!INT8/Status ausgeben!
	ret	
    end TYWR_KOPPEL_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR_KOPPEL_POL
allgemeine Ausgabe eines Zeichens und zugehoerigen Status an
8-Bit-/16-Bit-Koppelschnittstelle (PIO0)
--- ohne Interruptausloesung auf 8-Bit-Teil (INT8-Bit = 0) ---
Input:	rl0 - Zeichen
	rh0 - Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR_KOPPEL_POL procedure
      entry
	push	@r15, r0
TKP1:
	inb	rh0, PIO0D_B
	bitb	rh0, #%06	!ENABLE OUTPUT?!
	jr	nz, TKP1	!nein!

	pop	r0, @r15
	resb	rh0, #0		!Bit0=0: INT8-Bit auf 0 setzen!
	outb	PIO0D_B, rh0	!Status ausgeben!
	outb	PIO0D_A, rl0	!Datenbyte ausgeben!
	ret	
    end TYWR_KOPPEL_POL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE KP_OUTB
Ausgabe eines Bytes an einen I/O-Port des 8-Bit-Teils
(ueber 8-Bit-/16-Bit-Koppelschnittstelle (PIO0))
Input:	rl0 - Portadresse
	rh0 - auszugebendes Datenbyte
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    KP_OUTB procedure
      entry
	di	vi
	push	@r15, r0

	ldb	rh0, #STAT8_WRITE
	calr	TYWR_KOPPEL_INT	!Portadresse und Write-Status ausgeben!

	pop	r0, @r15
	push	@r15, r0
	ldb	rl0, rh0	!auszugebendes Datenbyte!
	ldb	rh0, #STAT8_ENDE !Ende-Status!
	calr	TYWR_KOPPEL_POL	!Datenbyte und Ende-Status ausgeben!

	pop	r0, @r15
	ei	vi
	ret
    end KP_OUTB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE KP_INB
Eingabe eines Bytes von einem I/O-Port des 8-Bit-Teils
(ueber 8-Bit-/16-Bit-Koppelschnittstelle (PIO1))
Input:	rl0 - Portadresse
Output:	rh0 - eingelesenes Datenbyte
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    KP_INB procedure
      entry
	di	vi
	ldb	rh0, #STAT8_READ
	calr	TYWR_KOPPEL_INT	!Portadresse und Read-Status ausgeben!

KPINB1:
	inb	rh0, PIO0D_B
	bitb	rh0, #5		!DATEN DA?!
	jr	nz, KPINB1	!nein!

	inb	rh0, PIO1D_B	!Status-Eingabe!
	andb	rh0, #%7E
	cpb	rh0, #STAT16_ENDE
	jr	nz, KPINB1

	inb	rh0, PIO1D_A	!eingelesenes Datenbyte!
	ei	vi
	ret
    end KP_INB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYRDWR
Einlesen eines Zeichens aus Interrupt-Eingabepuffer (PTYBFF) und
Ausgabe auf Terminal
Output:	rl0 - eingelesenes Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    TYRDWR procedure
      entry
	calr	TYRD
	calr	TYWR
	ret
    end TYRDWR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RD_LINE_BFF
Zeile aus Interrupt-Eingabepuffer PTYBFF lesen, in Zwischenpuffer abspeichern
und auf Terminal ausgeben
Input:	r1 - max. Zeilenlaenge
	r2 - Anfangsadresse Zwischenpuffer
Output: r1 - Anzahl der uebertragenen Zeichen
	r2 - zeigt auf Zeilenende
	Z=1, wenn max. Zeilenlaenge erreicht wurde
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RD_LINE_BFF procedure
      entry
	ld	MAXLEN, r1
RDL_NEU:
	clr	r1
RDL_LOOP:
	calr	TYRDWR		!ZEICHEN  LESEN UND AUSGEBEN!
	bitb	FLAG1, #%03	!NUR GROSSBUCHSTABEN ERLAUBT ?!
	jr	nz, RDL_TST	!nein!
	cpb	rl0, #'a'
	jr	c, RDL_TST
	cpb	rl0, #'{'
	jr	nc, RDL_TST
	resb	rl0, #%05	!KLEINBUCHST. IN GROSSBUCHST. WANDELN !
 
RDL_TST:
	ldb	r2(r1), rl0	!Zeichen in Zwischenpuffer abspeichern!
	cpb	rl0, CHRDEL
	jr	z, _CL
	cpb	rl0, LINDEL
	jr	z, _CR
	cpb	rl0, #CR
	jr	z, RDL_END
	inc	r1, #%01
	cp	r1, MAXLEN	!MAXIMALE ZEILENLAENGE ERREICHT ?!
	JR	C,RDL_LOOP	!SPRUNG WENN NICHT!
	setflg	z
	ret	
_CL:
	calr	CLRCHR
	jr	pl, RDL_LOOP
	ldb	rl0, PROMT	!PROMT-Zeichen wurde durch CHRDEL geloescht
				 --> PROMT-Zeichen neu ausgeben!
	calr	TYWR
	jr	RDL_NEU		!ZEILE NEU EINGEBEN!
_CR:
	test	r1		!EINGEGEBEN ZEICHENKETTE LOESCHEN!
	jr	z, RDL_LOOP
	ldb	rl0, #BS
	calr	TYWR
	calr	CLRCHR
	jr	_CR
RDL_END:
	ldb	rl0, #LF
	calr	TYWR
	lda	r2, r2(r1)	!R2 AUF Zeilenende STELLEN!
	resflg	z
	ret	
    end RD_LINE_BFF

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RD_LINE_INBFF
Einlesen einer Kommandozeile bis CR aus Interrupt-Eingabepuffer (PTYBFF) in
Eingabepuffer (INBFF) und Ausgabe auf Terminal
Output: rl0 - 1. Zeichen der Eingabezeile ungleich Space
	INPTR zeigt auf dieses Zeichen
	Z=1, wenn dieses Zeichen = CR
Fehler, wenn Zeile laenger als 128 Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RD_LINE_INBFF procedure
      entry
	ld	r2, #INBFF
	ld	r1, #%80	!MAXLEN LADEN!
	ld	INPTR, #0	!EINGABEZEIGER LOESCHEN!
	calr	RD_LINE_BFF	!Zeile aus PTYBFF in INBFF speichern und
				 auf Terminal ausgeben!
	jp	z, ERROR	!Pufferende erreicht!

	calr	GET_SIGNW	!rl0:=1.Zeichen aus INBFF ungleich Space!
	dec	INPTR, #%01	!INPTR zeigt auf dieses Zeichen!
	cpb	rl0, #CR	!Z=1, wenn Zeichen = CR!
	ret	
    end RD_LINE_INBFF

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CLRCHR
Loeschen eines Zeichens
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    CLRCHR procedure
      entry
	ldb	rl0, #' '
	calr	TYWR
	ldb	rl0, #BS
	calr	TYWR
	dec	r1, #%01
	ret	
    end CLRCHR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_CRLF
Ausgabe CR LF auf Terminal
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_CRLF procedure
      entry
	ldb	rl0, #CR
	calr	TYWR
	ldb	rl0, #LF
	calr	TYWR
	calr	WR_NULL
	ret
    end WR_CRLF

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_NULL
Ausgabe einer bestimmten Anzahl Nullen auf Terminal (entsprechend NULLCT)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_NULL procedure
      entry
	test	NULLCT
	ret	z
	ld	r1, NULLCT
	clr	r0
WR_NULL1:
	calr	TYWR
	dec	r1, #1
	jr	nz, WR_NULL1
	ret	
    end WR_NULL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_MSG
Zeichenketten-Ausgabe auf Terminal
Ausgabe wird bei CR beendet
Input:	r2 - Adresse der Zeichenkette
	     (Zeichenkette beginnt mit Angabe der Laenge (WORD) gefolgt
	     von der Zeichenkette selbst)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_MSG procedure
      entry
	ld	r0, @r2
	cp	r0, #%0		!LAENGE = 0? !
	ret	z
	ld	OUTPTR, r0
	inc	r2, #%02	!ADRESSE DES 1. CHAR.!
	ld	r4, #OUTBFF	!OUTBFF_ADRESSE LADEN!
	ldirb	@r4, @r2, r0	!MESSAGE TRANSFERIEREN!
	calr	WR_OUTBFF
	ret
    end WR_MSG

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_OUTBFF_CR
Ausgabe des Inhaltes des Ausgabepuffers (OUTBFF) mit CR auf Terminal;
Loeschen des Ausgabepuffers
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_OUTBFF_CR procedure
      entry
	ld	r3, OUTPTR
	ldb	OUTBFF(r3), #%0D
	inc	OUTPTR, #1
	calr	WR_OUTBFF
	ret
    end WR_OUTBFF_CR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_OUTBFF
Ausgabe des Inhaltes des Ausgabepuffers (OUTBFF) auf Terminal;
Loeschen des Ausgabepuffers
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_OUTBFF procedure
      entry
	clr	r3		!ZEICHENZAEHLER LOESCHEN!
TX_1:
	ldb	rl0, OUTBFF(r3)
	inc	r3, #1
	calr	TYWR
	jr	z, TX_2		!SPRUNG WENN CR AUSGEGEBEN WURDE!
	cp	r3, OUTPTR	!ALLE ZEICHEN AUSGEBEN ? !
	jr	c, TX_1		!SPRUNG WENN PUFFER NICHT LEER!
	jr	BF_EMPTY
TX_2:
	ldb	rl0, #LF	!LINE FEED NACH MSG SENDEN!
	calr	TYWR
	calr	WR_NULL		!NULLCT Nullen senden!
	bitb	FLAG1, #%04	!AUSGABE BEI CR ABBRECHEN ? !
	jr	nz, TX_1
BF_EMPTY:
	ld	OUTPTR, #0	! OUTPTR RUECKSETZTEN !
	ld	r0, #%0040
	ld	OUTBFF, #'  '
	ld	r2, #OUTBFF	!OUTBFF LOESCHEN !
	ld	r4, #OUTBFF + 2
	ldir	@r4, @r2, r0
	ret	
    end WR_OUTBFF

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE HTOB
Konvertierung Hexaziffer (ASCII) in Binaerwert
Input:	rl0 - Hexaziffer (ASCII)
Output:	rl0 - Binaerwert
	C=1 --> keine Hexaziffer (Fehler)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    HTOB procedure
      entry
	cpb	rl0, #'0'
	ret	c		!FEHLER := KEINE ZAHL!
	cpb	rl0, #'9' + 1
	jr	c, HTOB1	!SPRUNG WENN ZAHL!
	cpb	rl0, #'A'
	ret	c		!FEHLER := KEIN BUCHSTABE!
	cpb	rl0, #'F' + 1
	jr	nc, HTOB2	!FEHLER := KEIN BUCHSTABE A...F!
	subb	rl0, #%07
HTOB1:
	andb	rl0, #%0F	!FEHLERFREI!
	resflg	c
	ret	
HTOB2:
	setflg	c		!FEHLER := KEINE HEXADEZIMALZAHL!
	ret	
    end HTOB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE BTOH16
Konvertierung Binaer in Hexaziffern (ASCII) und Eintragung in
Ausgabepuffer (OUTBFF)
Input:	r5 - 16-Bit-Wert (--> 4 Hexaziffern)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    BTOH16 procedure
      entry
	ldb	rh0, rh5	!HIGH ADRESSE LADEN!
	calr	BTH
	calr	BTOH8
	ret
    end BTOH16

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE BTOH8
Konvertierung Binaer in Hexaziffern (ASCII) und Eintragung in
Ausgabepuffer (OUTBFF)
Input:	 rl5 - 8-Bit-Wert (--> 2 Hexaziffern)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    BTOH8 procedure
      entry
	ldb	rh0, rl5	!LOW ADRESSE ODER 8-BIT WERT LADEN!
BTH:
	rldb	rl0, rh0
	calr	BTOH4		!BINAER TO HEX KONVERTIERUNG!
	rldb	rl0, rh0
	calr	BTOH4
	ret
    end BTOH8

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE BTOH4
Konvertierung Binaer in Hexaziffer (ASCII) und Eintragung in
Ausgabepuffer (OUTBFF)
Input:	rl0 (Bit0-3) - 4-Bit-Wert (--> 1 Hexaziffer)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    BTOH4 procedure
      entry
	push	@r15, r1
	andb	rl0, #%0F
	ldb	rl1, KONV
	addb	rl1, rl0
	ldb	KONV, rl1
	cpb	rl0, #%0A
	jr	c, BTOH41
	addb	rl0, #%07
BTOH41:
	addb	rl0, #%30
	push	@r15, r6
	ld	r6, OUTPTR
	ldb	OUTBFF(r6), rl0
	inc	OUTPTR, #1
	pop	r6, @r15
	pop	r1, @r15
	ret	
    end BTOH4

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_CHR
aus Eingabepuffer INBFF das Zeichen in rl0 bereitstellen, auf das INPTR zeigt
Output:	rl0 - Zeichen
	INPTR zeigt auf darauffolgendes Zeichen
	Z=1 --> Zeichen=CR
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_CHR procedure
      entry
	push	@r15, r2
	ld	r2, INPTR
	ldb	rl0, INBFF(r2)
	inc	INPTR, #1	!INPTR := INPTR + 1!
	cpb	rl0, #CR
	pop	r2, @r15
	ret	
    end GET_CHR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_SIGN
aus Eingabepuffer INBFF ab Position INPTR (einschl.) Space suchen und naechstes
Zeichen ungleich Space in rl0 bereitstellen
Output: rl0 - Zeichen
	INPTR zeigt auf darauffolgendes Zeichen
	Z=1 --> CR gelesen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_SIGN procedure
      entry
	calr	GET_CHR
	ret	z		!RUECKSPRUNG WENN ENDEKENNUNG (CR)!
	cpb	rl0, #' '
	jr	nz, GET_SIGN	!1. SPACE NACH DEM KOMMANDO SUCHEN!
	calr	GET_SIGNW
	ret
    end GET_SIGN

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_SIGNW
aus Eingabepuffer INBFF ab Position INPTR (einschl.) erstes Zeichen ungleich
Space in rl0 bereitstellen
Output: rl0 - Zeichen
	INPTR zeigt auf darauffolgendes Zeichen
	Z=1 --> CR gelesen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_SIGNW procedure
      entry
	calr	GET_CHR		!1. ZEICHEN DER PARAMETERFOLGE LESEN!
	ret	z		!Zeichen=CR!
	cpb	rl0, #' '
	jr	z, GET_SIGNW
	ret			!Z=0!
    end GET_SIGNW

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_SGNR
Einlesen (aus Eingabepuffer INBFF) und Konvertierung der Segmentnummer
Input:	rl0 - 1. Zeichen der eingegebenen Adresse
	INPTR zeigt auf darauffolgendes Zeichen
Output:	r2 - Segmentnummer (Bit0-7=0 / Bit8-14=segnr / Bit15=1)
	     (falls kein Segmentnummer angegeben, dann r2=PC_SEG)
	rl0 - 1. Zeichen der Offsetadresse
	INPTR zeigt auf darauffolgendes Zeichen
Fehler, wenn unzulaessige Segmentnummer angegeben
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_SGNR procedure
      entry
	ld	r2, PC_SEG	!implizite segnr!
	set	r2, #%0F	!Setzen Bit15!
	resb	FLAG1, #%0	!Flag=0 --> keine Segmentnummer angegeben ???!
	cpb	rl0, #CR
	ret	z		!Ende der Eingabezeile!
	cpb	rl0, #'<'	!#SEGMENT IDENTIFIER ANF. !
	ret	nz		!keine Segmentnummer angegeben!
	clr	r3
EKSN_N:
	calr	GET_CHR		!rl0:=naechstes Zeichen aus INBFF!
	jp	z, ERROR	!Zeichen=CR!
	calr	HTOB		!Konvertierung rl0 hexa (ASCII)-->binaer!
	jr	c, EKSN_NHZ	!keine Hexaziffer!
	rldb	rl0, rl3	!Binaerwert in rl3 schieben!
	jr	EKSN_N		!naechstes Zeichen!

EKSN_NHZ:
	cpb	rl0, #'>'	!#SEGMENT IDENTIFIER ENDE!
	jp	nz, ERROR	!segnr enthaelt unzulaessiges Zeichen!
	calr	GET_SIGNW	!rl0:=Zeichen nach segnr ungleich Space!
	ldb	rh2, rl3
	clrb	rl2
	set	r2, #%0F	!r2 := segnr!
	setb	FLAG1, #%0	!Flag=1 --> Segmentnummer angegeben ???!
	ret	
    end GET_SGNR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_HEXZ
Einlesen (aus Eingabepuffer INBFF) und Konvertierung einer Hexazahl
Input:	rl0 - 1. Ziffer der eingegebenen Hexazahl
	INPTR zeigt auf darauffolgendes Zeichen
Output:	r3  - Hexazahl (binaer) - wurden mehr als 4 Ziffern angegeben, so
                                  werden die letzten 4 Ziffern ausgewertet
	rl0 - naechstes Zeichen aus Eingabepuffer ungleich Space nach
              ausgewerteter Hexazahl
	INPTR zeigt auf darauffolgendes Zeichen
	C=1 --> keine Hexazahl angegeben
 	Z=1 --> CR gelesen
Fehler, wenn unzulaessige Hexazahl angegeben
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_HEXZ procedure
      entry
	clr	r3
	cpb	rl0, #CR
	setflg	c
	ret	z
RD_HX:
	calr	HTOB		!Konvertierung rl0 in Binaerwert!
	jp	c, ERROR	!unzulaessige Hexaziffer!
	rldb	rl0, rl3
	rldb	rl0, rh3
	calr	GET_CHR		!rl0 := naechste Ziffer!
	ret	z		!CR gelesen (C=0 aus GET_CHR)!
	cpb	rl0, #' '
	jr	nz, RD_HX	!naechste Ziffer!

	calr	GET_SIGNW	!rl0:= naechstes Zeichen ungleich Space!
	resflg	c
	ret			!Z=1, wenn CR gelesen; sonst Z=0 
				 (aus GET_SIGNW)!
    end GET_HEXZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_ADR
Einlesen (aus Eingabepuffer INBFF) und Konvertierung einer Adresse
(eingegebene Adresse muss folgenden Aufbau haben: '<segnr>offset', wobei
die Segmentnummer fehlen kann)
Input:	rl0 - 1. Zeichen der Adresse
	INPTR zeigt auf darauffolgendes Zeichen
Output:	rr2 - Adresse (binaer)
	rl0 - naechstes Zeichen ungleich Space aus Eingabepuffer nach
	      ausgewerteter Adresse
	INPTR zeigt auf darauffolgendes Zeichen
	C=1 --> keine Adresse angegeben
	Z=1 --> CR gelesen
Fehler, wenn ungueltige Hexaziffer angegeben
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_ADR procedure
      entry
	calr	GET_SGNR	!r2 := Segmentnummer!
	calr	GET_HEXZ	!r3 := Offset!
	ret
    end GET_ADR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_PAR
Einlesen (aus Eingabepuffer INBFF) und Konvertierung der Parameterfolge:
'<segnr1>offset1 <segnr2>offset2 word', wobei <segnr> fehlen kann
Output:	c=1 wenn Parameterfolge unvollst. oder fehlerhaft
	rr6 - 1. Adresse (binaer)
	rr4 - 2. Adresse (binaer)
	r3  - word (binaer)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_PAR procedure
      entry
	calr	GET_SIGN	!1. ASCII-ZEICHEN IN RL0!
	calr	GET_ADR		!rr2 := 1. Adresse!
	jr	z, RD_P2	!CR wurde gelesen!
	ldl	rr6, rr2	!rr6 := 1. Adresse!
	calr	GET_ADR		!rr2 := 2. Adresse!
	jr	z, RD_P2	!CR wurde gelesen!
	ldl	rr4, rr2	!rr4 := 2. Adresse!
	calr	GET_HEXZ		!r3 := word!
	ret	nc		!kein Fehler!

RD_P2:
	setflg	c		!C=1, d.h. Fehler!
	ret	
    end GET_PAR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RD_DATA
Dateneingabe (PROMT-Zeichen (':') ausgeben, eingegebene Zeile aus PTYBFF 
             in INBFF speichern, auf Terminal ausgeben und 1. Datenwert der 
             Zeile auswerten)
Output:	rr2  enthaelt Datenwert
	Z=1, wenn keine Dateneingabe erfolgte bzw. 'Q' oder '-' eingegeben
	C=1, wenn 'Q' oder '-' eingegeben (rl0 enthaelt dann 'Q' oder '-')
	Z=0, wenn Dateneingabe erfolgte (rr2 enthaelt dann Datenwert) mit:
	     P/V=1, wenn CR nach Datenwert,
	     C=0,   wenn Space nach Datenwert
	INPTR zeigt auf 2 Zeichen nach der letzten Ziffer des Datenwertes
Fehler, wenn ungueltige Hexazahl eingegeben wurde
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RD_DATA procedure
      entry
	ldb	rl0, #':'
	calr	TYWR		!Ausgabe ':' auf Terminal!
	calr	RD_LINE_INBFF	!Zeile aus PTYBFF in INBFF speichern und
				 auf Terminal ausgeben!
	calr	GET_DATA
	ret
    end RD_DATA

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET_DATA
Dateneingabe (Einlesen des naechsten Datenwertes aus Eingabepuffer INBFF)
Input:	INPTR zeigt auf das erste auszuwertende Zeichen des Datenwertes
	bzw. auf Space davor
Output:	rr2  enthaelt Datenwert
	Z=1, wenn keine Dateneingabe erfolgte bzw. 'Q' oder '-' eingegeben
	C=1, wenn 'Q' oder '-' eingegeben (rl0 enthaelt dann 'Q' oder '-')
	Z=0, wenn Dateneingabe erfolgte (rr2 enthaelt dann Datenwert) mit:
	     P/V=1, wenn CR nach Datenwert,
	     C=0,   wenn Space nach Datenwert
	INPTR zeigt auf 2 Zeichen nach der letzten Ziffer des Datenwertes
Fehler, wenn ungueltige Hexazahl eingegeben wurde
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET_DATA procedure
      entry
	clr	r2
	clr	r3
	calr	GET_SIGNW	!rl0:=1. Zeichen des Datenwertes!
	ret	z		!Z=1, kein Datenwert (CR)!

	cpb	rl0, #'Q'	!QUIT ? !
	jr	z, K_Q
	cpb	rl0, #'-'
	jr	nz, K_LOOP
K_Q:
	setflg	c
	ret			!Z=1, C=1: 'Q' oder '-' eingegeben!

K_LOOP:
	calr	HTOB		!Konv. rl0 (Hexa-ASCII) --> rl0 (binaer)!
	jp	c, ERROR	!keine Hexaziffer!

	rldb	rl0, rl3
	rldb	rl0, rh3
	rldb	rl0, rl2
	rldb	rl0, rh2	!Hexaziffer (binaer) in rr2 schieben!
	calr	GET_CHR		!rl0:=naechstes Zeichen aus INBFF!
	jr	nz, K_1		!kein CR!

	setflg	p, v
	jr	K_END		!Z=0, P/V=1, wenn CR nach Datenwert!
K_1:
	cpb	rl0, #' '
	jr	nz, K_LOOP	!naechste Ziffer des Datenwertes!

	resflg	c		!Z=0, C=1, wenn Space nach Datenwert!
K_END:
	resflg	z
	ret	
    end GET_DATA

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_R5_RDDATA
Eintragung des ASCII-konvertierten Inhaltes von r5 in Ausgabepuffer (OUTBFF);
Ausgabe des Ausgabepuffers auf Terminal; Ausgabe ':'; Eingabe des Datenwertes
Input:	r5 - Adresse
Output:	siehe Prozedur RD_DATA
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_R5_RDDATA procedure
      entry
	calr	BTOH16		!Eintragung r5 (ASCII) in OUTBFF!
	inc	OUTPTR, #1	!1 Leerzeichen!
	calr	WR_OBF_RDDATA
	ret
    end WR_R5_RDDATA

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_OBF_RDDATA
Ausgabe des Ausgabepuffers auf Terminal; Ausgabe ':'; Eingabe des Datenwertes
Output:	siehe Prozedur RD_DATA
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WR_OBF_RDDATA procedure
      entry
	calr	WR_OUTBFF	!Ausgabe Inhalt OUTBFF!
	calr	RD_DATA		!AUSGABE ':' + EINGABE NEUER INHALT!
	ret
    end WR_OBF_RDDATA

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_CHR
Eintragung eines Zeichens in Ausgabepuffer (OUTBFF)
Input: rl1 - einzutragendes Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PUT_CHR procedure
      entry
	ld	r2, OUTPTR
	ldb	OUTBFF(r2), rl1
	inc	OUTPTR, #1
	ret	
    end PUT_CHR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_3C
Eintragung des Zeichens '<' in Ausgabepuffer (OUTBFF)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PUT_3C procedure
      entry
	ld	r1, #'< '
	calr	PUT_1CHR
	ret
    end PUT_3C

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_1CHR
Eintragung eines Zeichens in Ausgabepuffer (OUTBFF)
Input: r1 - Zeichenkette aus 'Zeichen, Space'
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PUT_1CHR procedure
      entry
	calr	PUT_2CHR	!2 ZEICHEN IN DEN OUTPUT_PUFFER LADEN!
	dec	OUTPTR, #1	!PUFFERLAENGE DEKREMENTIEREN!
	ret	
    end PUT_1CHR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_2CHR
2 Zeichen in Ausgabepuffer (OUTBFF) eintragen
Input:	rh1 - 1. Zeichen
	rl1 - 2. Zeichen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PUT_2CHR procedure
      entry
	push	@r15, r4
	ld	r4, OUTPTR
	ldb	OUTBFF(r4), rh1
	inc	r4, #1
	ldb	OUTBFF(r4), rl1
	inc	OUTPTR, #%02
	pop	r4, @r15
	ret	
    end PUT_2CHR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_SEGNR
Konvertierung der Segmentnummer in HEXA-ASCII-Zahl (xx);
Eintragung von '<xx>' in Ausgabepuffer OUTBFF
Input: rl5 - Segmentnummer (binaer)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PUT_SEGNR PROCEDURE
      ENTRY
	calr	PUT_3C		!Zeichen '<' in OUTBFF eintragen!
	calr	PUT_SGNR
	ret
    end PUT_SEGNR		!Weiterlauf bei PUT_SGNR!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PUT_SGNR
Konvertierung der Segmentnummer in HEXA-ASCII-Zahl (xx);
Eintragung von 'xx>' in Ausgabepuffer OUTBFF
Input: rl5 - Segmentnummer (binaer)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    PUT_SGNR PROCEDURE
      ENTRY
	resb	rl5, #%07
	calr	BTOH8		!Segmentnummer in OUTBFF eintragen!
	ld	r1, #'> '
	calr	PUT_1CHR	!Zeichen '>' in OUTBFF eintragen!
	ret
    end PUT_SGNR

end p_term

