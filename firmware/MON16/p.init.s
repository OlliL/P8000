!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1989
  U8000-Softwaremonitor (MON16)        Modul: p_init

  Bearbeiter:	J. Zabel
  Datum:	14.04.1989
  Version:	3.1

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_init
------------------------------
Dieser Modul umfasst:
- die U8000-Initialisierung, 
- den Test und die Initialisierung der Koppelschnittstelle 
  (fuer Terminalbetrieb ueber SIO0_B des 8-Bit-Teils und Dateitransfer 
  von/zu UDOS und Blocktransfer zur Floppy), 
- falls die Koppelschnittstelle inaktiv ist, die Initialisierung des seriellen
  Kanals fuer das Terminal (SIO0_B),
- die Initialisierung des seriellen Kanals fuer das LOAD/SEND-System (SIO0_A),
- die Initialisierung des Arbeitsspeichers,
- die Ausgabe der Systemmeldung und
- die Kommandoeingabeschleife.

Prozeduren
----------
ENTRY_
CMDLOP
INIT_CTC0_0
INIT_CTC0_1
INIT_SIO0_A
INIT_SIO0_B
ERROR
UINSTR_ERR
PINSTR_ERR
SC_ENTRY
MMU_ERR
NMI_INT
NVI_INT
VI_ERR
==============================================================================!


p_init module

$SECTION PROM

!anfang mon16.include!
!******************************************************************************
allgemeine Vereinbarungen (Stand: 13.04.1989)
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

QUIT			procedure		!Modul p_ldsd!
LOAD			procedure		!Modul p_ldsd!
MCZ_INT			procedure		!Modul p_ldsd!
MCZ_ERR			procedure		!Modul p_ldsd!
TYWR_MCZ		procedure		!Modul p_ldsd!
WOCF_MCZ		procedure		!Modul p_ldsd!
SEND			procedure		!Modul p_ldsd!
 
OS_BOOT			procedure		!Modul p_boot!
AUTOBOOT		procedure		!Modul p_boot!

DISK			procedure		!Modul p_disk!

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
XEMPTY		BYTE			!fuer interruptgesteuerten Terminal-
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
modulspezifische Vereinbarungen
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
	WORD := 0000
	WORD := %C000	! FCW !
	WORD := %8000	! PC Segment-Nr. !
	WORD := ENTRY_	! PC Offset !
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Copyright
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
  INTERNAL
COPYRIGHT
	ARRAY	[* BYTE]	:= '(C) ZFT/KEAW Berlin, DATE 14.04.1989'
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Systemmeldung
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
  INTERNAL
SYSMSG	WORD := 46
	ARRAY [* BYTE] := 'U8000-Softwaremonitor Version 3.1 - '
	ARRAY [* BYTE] := 'Press NMI%0D'
	
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Monitorkommandoliste
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
CMD_LISTE:
	WORD	:= 1
	ARRAY [* BYTE] := 'D'
	WORD	:= DISPLAY

	WORD	:= 1
	ARRAY [* BYTE] := 'C'
	WORD	:= COMPARE

	WORD	:= 1
	ARRAY [* BYTE] := 'F'
	WORD	:= FILL

	WORD	:= 1
	ARRAY [* BYTE] := 'M'
	WORD	:= MOVE

	WORD	:= 2
	ARRAY [* BYTE] := 'PR'
	WORD	:= PORT_READ

	WORD	:= 3
	ARRAY [* BYTE] := 'PRS'
	WORD	:= PORT_READ_SPECIAL

	WORD	:= 3
	ARRAY [* BYTE] := 'PSR'
	WORD	:= PORT_READ_SPECIAL

	WORD	:= 2
	ARRAY [* BYTE] := 'PW'
	WORD	:= PORT_WRITE

	WORD	:= 3
	ARRAY [* BYTE] := 'PWS'
	WORD	:= PORT_WRITE_SPECIAL

	WORD	:= 3
	ARRAY [* BYTE] := 'PSW'
	WORD	:= PORT_WRITE_SPECIAL

	WORD	:= 2
	ARRAY [* BYTE] := 'HR'
	WORD	:= HARD_DISK_READ

	WORD	:= 2
	ARRAY [* BYTE] := 'HW'
	WORD	:= HARD_DISK_WRITE

	WORD	:= 2
	ARRAY [* BYTE] := 'FF'
	WORD	:= FLOPPY_FORMAT

	WORD	:= 2
	ARRAY [* BYTE] := 'FR'
	WORD	:= FLOPPY_READ

	WORD	:= 2
	ARRAY [* BYTE] := 'FW'
	WORD	:= FLOPPY_WRITE

	WORD	:= 1
	ARRAY [* BYTE] := 'R'
	WORD	:= REGISTER

	WORD	:= 1
	ARRAY [* BYTE] := 'B'
	WORD	:= BREAK

	WORD	:= 1
	ARRAY [* BYTE] := 'G'
	WORD	:= GO

	WORD	:= 1
	ARRAY [* BYTE] := 'N'
	WORD	:= NEXT

	WORD	:= 1
	ARRAY [* BYTE] := 'T'
	WORD	:= TEST_

	WORD	:= 2
	ARRAY [* BYTE] := 'GE'
	WORD	:= GET

	WORD	:= 1
	ARRAY [* BYTE] := 'S'
	WORD	:= SAVE

	WORD	:= 1
	ARRAY [* BYTE] := 'Q'
	WORD	:= QUIT8

	WORD	:= 4
	ARRAY [* BYTE] := 'QRES'
	WORD	:= QUIT8_RESET

	WORD	:= 3
	ARRAY [* BYTE] := 'RGE'
	WORD	:= RM_GET

	WORD	:= 2
	ARRAY [* BYTE] := 'RS'
	WORD	:= RM_SAVE

	WORD	:= 2
	ARRAY [* BYTE] := 'RQ'
	WORD	:= RM_QUIT

	WORD	:= 4
	ARRAY [* BYTE] := 'LOAD'
	WORD	:= LOAD

	WORD	:= 4
	ARRAY [* BYTE] := 'SEND'
	WORD	:= SEND

	WORD	:= 4
	ARRAY [* BYTE] := 'QUIT'
	WORD	:= QUIT

	WORD	:= 1
	ARRAY [* BYTE] := 'O'
	WORD	:= OS_BOOT

	WORD	:= %FFFF

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PIO/SIO/CTC-Initialisierungstabellen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
  INTERNAL

ITAB_PIO0_B ARRAY [* BYTE] := [%CF	!Bit-Ein-Ausgabemode!
			       %60]	!Bit 0-4: Ausgabe; Bit 5,6: Eingabe!

ITAB_PIO1_B ARRAY [* BYTE] := [%08	!VI_PIO1_B in PSAREA!
			       %CF	!Bit-Ein-Ausgabemode!
			       %FF	!Bit 0-6: Eingabe! 
			       %B7	!Interruptsteuerwort:
					 EI/OR/HIGH/MASK folgt!
			       %FE]	!Bit 0 loest Interrupt aus!

ITAB_SIO0_A ARRAY [* BYTE] := [COMM3
                               SIO_R4 + COMM2
                               S2 + C32 + NOPRTY
                               SIO_R3
                               RENABLE + B8
                               SIO_R5
                               XENABLE + T8 + DTR + RTS
                               SIO_R1 + COMM2
                               PDAVCT]

ITAB_SIO0_B ARRAY [* BYTE] := [COMM3
                               SIO_R4 + COMM2
                               S2 + C32 + NOPRTY
                               SIO_R3
                               RENABLE + B8
                               SIO_R5
                               XENABLE + T8 + DTR + RTS
                               SIO_R1 + COMM2
                               PDAVCT + SAVECT]

ITAB_SIO0_INTVEK ARRAY [* BYTE] := [SIO_R2
				    %10]		!VI_SIO0_B in PSAREA!

  CONSTANT
BAUD9600 := %02

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SIO-Kommandos
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
!Command Identifiers and Values
Includes all control bytes for asynchronous and synchronous I/O !
 
  CONSTANT
!WR0 Commands!
!SIO_R0	:=	%00!	!SIO register pointers!
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
 
!RFI	:=	%38!	!Return from Int!
!RRCC	:=	%40!	!Reset Rx CRC Checker!
!RTCG	:=	%80!	!Reset Tx CRC Generator!
!RTUEL	:=	%C0!	!Reset Tx Under/EOM Latch!
 
!WR1 Commands!
!WAIT	:=	%00!	!Wait function!
!DRCVRI	:=	%00!	!Disable Receive Interrupts!
!EXTIE	:=	%01!	!External interrupt enable!
!XMTRIE	:=	%02!	!Transmit Int Enable!
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
IV	:=	%00
 
!WR3 Commands!
!B5	:=	%00!	!Receive 5 bits/character!
RENABLE	:=	%01	!Receiver enable!
!SCLINH	:=	%02!	!Sync character load inhibit!
!ADSRCH	:=	%04!	!Address search mode!
!RCRCEN	:=	%08!	!Receive CRC enable!
!HUNT	:=	%10!	!Enter hunt mode!
!AUTOEN	:=	%20!	!Auto enable!
!B7	:=	%40!	!Receive 7 bits/character!
!B6	:=	%80!	!Receive 6 bits/character!
B8	:=	%C0	!Receive 8 bits/character!
 
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
RTS	:=	%02	!Request to send!
!SELCRC	:=	%04!	!Select CRC-16 polynomial!
XENABLE	:=	%08	!Transmitter enable!
!XBREAK	:=	%10!	!Send break!
!T7	:=	%20!	!Transmit 7 bits/character!
!T6	:=	%40!	!Transmit 6 bits/character!
T8	:=	%60	!Transmit 8 bits/character!
DTR	:=	%80	!Data terminal ready!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Variablenliste zur RAM-Initialisierung fuer CHRDEL-PROMT
(siehe Modul p_ram)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
  INTERNAL
!VARIABLENLISTE: CHRDEL,LINDEL/^Q,^S/N_CNT/B_CODE/STACK_CT/N_/PROMT!
VAR_LISTE_PROM
	ARRAY [7 WORD] := [%087f %1113 %0000 %7f00 %0004 %0001 %2000]
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Initialisierungsfeld zur RAM-Initialisierung 
fuer Program Status Area
(siehe Modul p_ram)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
 
  INTERNAL
PSAREA_PROM:
!			   ID    FCW   PCSEG PCOFF!
	ARRAY [4 WORD] := [%0000 %0000 %0000 %0000] !RESERVED!
	ARRAY [4 WORD] := [%0000 %4000 %8000 UINSTR_ERR] 
						    !UNIMLEMENTED INSTRUCTIONS!
	ARRAY [4 WORD] := [%0000 %4000 %8000 PINSTR_ERR] 
						    !PRIVILEGED INSTRUCTIONS!
	ARRAY [4 WORD] := [%0000 %C000 %8000 SC_ENTRY] 
						    !SYSTEM CALL INSTRUCT.!
	ARRAY [4 WORD] := [%0000 %4000 %8000 MMU_ERR] !SEGMENT TRAP!
	ARRAY [4 WORD] := [%0000 %4000 %8000 AUTOBOOT] !NONMASKABLE INTERRUPT!
	ARRAY [4 WORD] := [%0000 %4000 %8000 NVI_ERR]  !NONVECTORED INTERRUPT!

	ARRAY [2 WORD] := [%0000 %4000]             !VECTORED INTERRUPT!
 
!VECTORED INTERRUPT JUMP TABLE!
!                          SEG   OFFSET!
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 KOPPEL_INT]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 PTY_INT]
	ARRAY [2 WORD] := [%8000 PTY_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 VI_ERR]
	ARRAY [2 WORD] := [%8000 MCZ_INT]
	ARRAY [2 WORD] := [%8000 MCZ_ERR]
 
 
!******************************************************************************
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE ENTRY_
Startprozedur
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    ENTRY_ procedure
      entry
!PSAPSEG laden (Segment 00)!
	clr	r14
	ldctl	PSAPSEG, r14	!Program Status Area Pointer Segment Register!
!Umschaltung auf nichtsegmentierten Mode!
	ld	r1, #%4000
	ldctl	FCW, r1		!Flag and Control Word Register!
!MMU's ausschalten! 
	clr	R3
	sout	ALL_MMU, r3	!MMU in Tristate bringen!
!RAM loeschen!
	ld	r4, #RAM_ANF
	ld	r2, #RAM_ANF+2
	ld	@r4, #0
	ld	r1, #%0400 
	ldir	@r2, @r4, r1
!Refresh-Register laden!
	ld	r1, #%9E00	!REFRESH-Anpassung!
	ldctl	REFRESH, r1	!Refresh Control Register!
	ld	RF_CTR, r1
!Eingabepuffer (INBFF) und Ausgabepuffer (OUTBFF) mit Space initialisieren!
	ld	INBFF, #%2020
	ld	r4, #INBFF
	ld	r2, #INBFF+2
	ld	r1, #%80
	ldir	@r2, @r4, r1
!Variablen-Transfer in den RAM!
	ld	r4, #VAR_LISTE_PROM
	ld	r2, #CHRDEL
	ldk	r1, #%07
	ldir	@r2, @r4, r1
!Program Status Area im RAM initialisieren!
	ld	r4, #PSAREA_PROM
	ld	r2, #PSAREA
	ld	r1, #%3E
	ldir	@r2, @r4, r1
!Rest der VI-Sprungtabelle in PSAREA fuellen!
	ld	r6, #%8000	!Segmentadresse!
	ld	r7, #VI_ERR
	ld	r3, #128-16	!Zaehler!
	ld	r2, #VI_TAB_REST
E1:	ldl	@r2, rr6
	inc	r2, #%0004
	djnz	r3, E1
!Stack-Bereich initialisieren!
	ld	r14, #%8000	!Normal-Stackpointer (Segment)!
	ld	r15, #NSP_OFF	!Normal-Stackpointer (Offset)!
	ldl	SVSTK, rr14
	ld	r15, #STACK
!PSAPOFF laden!
	ld	r1, #PSAREA
	ldctl	PSAP, r1	!Program Status Area Pointer Offset Register!
!Registerbereich laden!	
	ld	PS_, r14
	ld	PC_SEG, r14
	ld	PO_, r1
	ld	PC_OFF, #%8000	!PC im Registerbereich initialisieren
				 (in Vs.1.7: %5000)!
	ld	FCW_, #%C000	!FCW im Registerbereich initialisieren!
!Flags setzen!
	setb	FLAG1, #3	!Flag=1 --> Klein-/Grossbuchstaben erlaubt!
	resb	FLAG1, #4	!Flag=0 --> Ausgabe bei CR abbrechen!

!------------------------------------------------------------------------------
Test, ob Koppelschnittstelle aktiv ist und fehlerfrei arbeitet
ja   --> PTYPORT:=1
	 Terminalbetrieb ueber Koppelschnittstelle;
	 UDOS auf 8-Bit-Teil nicht verfuegbar;
	 Kommandos "Q", "QRES" ausfuehrbar;
	 Kommandos "GE", "S", "O U" und Floppytreiber ("O F", "FF", "FR", "FW")
         nicht ausfuehrbar;
     bzw.
         PTYPORT:=2
	 Terminalbetrieb ueber Koppelschnittstelle;
	 UDOS auf 8-Bit-Teil verfuegbar;
	 Kommandos "GE", "S", "Q", "QRES", "O U" und Floppytreiber 
         ("O F", "FF", "FR", "FW") ausfuehrbar;
nein --> PTYPORT:=0
	 Terminalbetrieb ueber SIO0_B des 16-Bit-Teils;
	 Kommandos "GE", "S", "Q", "QRES", "O U" und Floppytreiber
         ("O F", "FF", "FR", "FW") nicht ausfuehrbar
------------------------------------------------------------------------------!

	di	vi

!PIO0/PIO1 initialisieren (fuer Koppelschnittstelle)!
    !PIO0_A: Byteausgabe!
	ldb	rl0, #%0F	!Byteausgabe!
	outb	PIO0C_A, rl0
    !PIO0_B: Bit-Ein-Ausgabe (Bit 0-4: Ausgabe, Bit 5,6: Eingabe)!
	ld	r2, #ITAB_PIO0_B
	ld	r0, #2
	ld	r1, #PIO0C_B
	otirb	@r1, @r2, r0
    !PIO1_A: Byteeingabe!
	ldb	rl0, #%4F	!Byteeingabe!
	outb	PIO1C_A, rl0
	inb	rl0, PIO1D_A	!Scheineingabe!
    !PIO1_B: Bit-Eingabe (Bit 0-6: Eingabe, Bit 0 loest Interrupt aus)!
 	!Programmierung erst nach Test der Koppelschnittstelle (s. KP_AKTIV:)!

!Test Datentransfer ueber Koppelschnittstelle:
 1. Empfangen Status-INT16 0101010/Datenbyte %AA von 8-Bit-Teil
    Zuruecksenden empfangenen Status-INT8/Datenbyte
 2. Empfangen Status-INT16 1010101/Datenbyte %55 von 8-Bit-Teil
    Zuruecksenden empfangenen Status-INT8/Datenbyte
 3. Empfangen von 8-Bit-Teil:
    Status-INT16 0000000/
    Datenbyte %01, wenn Start des U8000-Softwaremonitors von 
                   U880-Softwaremonitor aus erfolgte
                   (d.h. UDOS ist nicht verfuegbar)
    bzw.
    Datenbyte %02, wenn Start des U8000-Softwaremonitors von 
                   UDOS aus erfolgte
                   (d.h. UDOS ist verfuegbar)!


    !Initialisierung PIO1_B (nur fuer Test der Koppelschnittstelle):
     Bit-Eingabe (Bit 0-6: Eingabe, Bit 0 loest keinen Interrupt aus)!
	ld	r2, #ITAB_PIO1_B+1
	ld	r0, #2
	ld	r1, #PIO1C_B
	otirb	@r1, @r2, r0

! 1. !
	ld	r2, #%FFFF	!bestimmte Zeit auf Datenbyte von 8-Bit-Teil
				 warten!
KP1:	inb	rl1, PIO0D_B
	bitb	rl1, #5		!DATEN DA?!
	jr	z, KP2		!ja!
	djnz	r2, KP1
	jr	KP_INAKTIV	!Wartezeit abgelaufen!

KP2:	inb	rl0, PIO1D_B	!Statuseingabe!
	andb	rl0, #%7F	!Bit 7 unbenutzt!
	inb	rh0, PIO1D_A	!Datenbyte einlesen!

	ld	r2,#%FFFF	!bestimmte Zeit auf ENABLE OUTPUT warten!
KP3:	inb	rl1, PIO0D_B
	bitb	rl1, #6		!ENABLE OUTPUT?!
	jr	z, KP4		!ja!
	djnz	r2, KP3
	jr	KP_INAKTIV	!Wartezeit abgelaufen!
KP4:	outb	PIO0D_B, rl0	!empf. Status-INT8 zuruecksenden (nur Bit 0-4)!
	outb	PIO0D_A, rh0	!empf. Datenbyte zuruecksenden!

	cpb	rl0, #%2A	!Status-INT16 0101010!
	jr	nz, KP_INAKTIV	!Statusuebertragung fehlerhaft!
	cpb	rh0, #%AA
	jr	nz, KP_INAKTIV	!Datenbyteuebertragung fehlerhaft!

! 2. !
	ld	r2, #%FFFF	!bestimmte Zeit auf Datenbyte von 8-Bit-Teil
				 warten!
KP5:	inb	rl1, PIO0D_B
	bitb	rl1, #5		!DATEN DA?!
	jr	z, KP6		!ja!
	djnz	r2, KP5
	jr	KP_INAKTIV	!Wartezeit abgelaufen!

KP6:	inb	rl0, PIO1D_B	!Statuseingabe!
	andb	rl0, #%7F	!Bit 7 unbenutzt!
	inb	rh0, PIO1D_A	!Datenbyte einlesen!

	ld	r2,#%FFFF	!bestimmte Zeit auf ENABLE OUTPUT warten!
KP7:	inb	rl1, PIO0D_B
	bitb	rl1, #6		!ENABLE OUTPUT?!
	jr	z, KP8		!ja!
	djnz	r2, KP7
	jr	KP_INAKTIV	!Wartezeit abgelaufen!
KP8:	outb	PIO0D_B, rl0	!empf. Status-INT8 zuruecksenden (nur Bit 0-4)!
	outb	PIO0D_A, rh0	!empf. Datenbyte zuruecksenden!

	cpb	rl0, #%55	!Status-INT16 1010101!
	jr	nz, KP_INAKTIV	!Statusuebertragung fehlerhaft!
	cpb	rh0, #%55
	jr	nz, KP_INAKTIV	!Datenbyteuebertragung fehlerhaft!

! 3. !
	ld	r2, #%FFFF	!bestimmte Zeit auf Datenbyte von 8-Bit-Teil
				 warten!
KP9:	inb	rl1, PIO0D_B
	bitb	rl1, #5		!DATEN DA?!
	jr	z, KP10		!ja!
	djnz	r2, KP9
	jr	KP_INAKTIV	!Wartezeit abgelaufen!

KP10:	inb	rl0, PIO1D_B	!Statuseingabe!
	andb	rl0, #%7F	!Bit 7 unbenutzt!
	inb	rh0, PIO1D_A	!Datenbyte einlesen!

	cpb	rl0, #%00	!Status-INT16 0000000!
	jr	nz, KP_INAKTIV	!Statusuebertragung fehlerhaft!
	cpb	rh0, #%01	!Kennzeichen: Koppelschnittstelle aktiv;
				              UDOS nicht verfuegbar!
	jr	z, KP_AKTIV
	cpb	rh0, #%02  	!Kennzeichen: Koppelschnittstelle aktiv;
				              UDOS verfuegbar!
	jr	nz, KP_INAKTIV	!Datenbyteuebertragung fehlerhaft!

!Koppelschnittstelle aktiv:
 - SIO0_B deaktivieren
 - PIO1_B (Koppelschnittstelle) fuer Interruptbetrieb aktivieren!
KP_AKTIV:
	ldb	PTYPORT, rh0	!Kennzeichnung: Koppelschnittstelle aktiv
					   01 - UDOS nicht verfuegbar
				           02 - UDOS verfuegbar!
	ldb	rl0, #%08	!Anzahl der Terminals!
	ld	_numterm, r0

	ldb	rl0, #COMM3
	outb	SIO0C_B, rl0	!SIO0_B Reset!

    !PIO1_B: Bit-Eingabe (Bit 0-6: Eingabe, Bit 0 loest Interrupt aus)!
	ld	r2, #ITAB_PIO1_B
	ld	r0, #5
	ld	r1, #PIO1C_B
	otirb	@r1, @r2, r0

    !RETI-Port initialisieren (fuer Koppelschnittstelle)!
	ldb	rl0, #0
	outb	RETI_P, rl0

	jr	E0

!Koppelschnittstelle inaktiv:
 - PIO1_B (Koppelschnittstelle) deaktivieren
 - SIO0_B fuer Terminalbetrieb aktivieren!
KP_INAKTIV:
	ldb	rh0, #%00
	ldb	PTYPORT, rh0	!Kennzeichnung: Koppelschnittstelle inaktiv!
	ldb	rl0, #%04	!Anzahl der Terminals!
	ld	_numterm, r0

	ldb	rl0, #%07
	outb	PIO1C_B, rl0	!Interrupt Koppelschnittstelle aus!

	calr	INIT_CTC0_1
	calr	INIT_SIO0_B
!-----------------------------------------------------------------------------!

E0:
!SIO0_A initialisieren (fuer LOAD/SEND/QUIT)!
	calr	INIT_CTC0_0
	calr	INIT_SIO0_A

!Ausgabe der Systemmeldung!
	ei	vi
	call	WR_CRLF		!Ausgabe CRLF!
	ld	r2, #SYSMSG
	call	WR_MSG		!Ausgabe der Systemmeldung!
    end ENTRY_			!Weiterlauf bei CMDLOP!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE CMDLOP
Kommandoeingabeschleife
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    CMDLOP procedure
      entry
	ei	vi		!zusaetzlich!
	ld	r15, #STACK
	push	@r15, #CMDLOP	!Rueckkehradresse fuer die Monitorroutinen!
	ldb	rl1, FLAG0
	and	r1, #%0006
	ldb	FLAG0, rl1	!set Bit1,2!
	ldb	rl0, PROMT
	call	TYWR		!PROMT ausgeben!
	call	RD_LINE_INBFF	!Einlesen der eingegebenen Zeile in INBFF;
				 rl0:=1. Zeichen der Eingabezeile ungl. Space;
				 INPTR zeigt auf dieses Zeichen!
	jr	nz, CMDL1	!rl0 ungleich CR!

	bitb	FLAG1, #%02	!Bit 2 = NEXT-Flag!
	jr	z, CMDL1	!kein NEXT-Betrieb!

!NEXT-Flag gesetzt und nur CR als Kommando eingegeben!
	ldb	rl0, #'N'	!nach Abarbeitung des NEXT-Kommandos kann
				 durch Eingabe von CR in der Kommandoeingabe-
				 schleife das NEXT-Kommando wiederholt werden!
	ld	r1, INPTR
	ldb	INBFF(r1), rl0	!Next-Kommando in Eingabepuffer eintragen!

!Berechnen der Zeichenzahl des eingeg. Kommandonamens!
CMDL1:
	resb	FLAG1, #%02	!Flag: NEXT-Betrieb ruecksetzen!
	ld	r11, INPTR	!Stand INPTR retten (zeigt auf 1. Zeichen)!
	ld	r8, #0		!r8 - Zeichenzaehler des eingeg. Kommandos!
CMDL2:
	call	GET_CHR		!rl0 := Zeichen, auf das INPTR zeigt,
				 INPTR := INPTR + 1!
	jr	z, CMDL3	!Zeichen = CR!
	cpb	rl0, #' '
	jr	z, CMDL3	!Zeichen = SPACE!
	inc	r8, #1		!Zeichenzaehler erhoehen!
	jr	CMDL2		!naechstes Zeichen!
!Kommando in Kommandoliste suchen!
CMDL3:
	ld	r10, #CMD_LISTE	!r10 - Zeiger auf akt. Pos. in CMD_LISTE!
CMDL4:
	ld	INPTR, r11	!INPTR zeigt auf 1. Zeichen des eingegebenen
				 Kommandos,
				 r8 = Laenge des eingegebenen Kommandos!
	ld	r9, @r10	!r9:=Laenge des akt. Kommandos in CMD_LISTE!
	cp	r9, #%FFFF
	jr	z, ERROR	!Ende Kommandoliste, Kommando nicht gefunden!
	inc	r10, #2		!r10 zeigt auf 1. Zeichen des akt. Kommandos
				 in CMD_LISTE!
	cp	r9, r8	
	jr	z, CMDL6	!Kommandolaengen sind gleich!
CMDL5:
	add	r10, r9		!Kommandonamen ueberspringen!
	inc	r10, #3
	and	r10, #%FFFE	!Kommandoadresse ueberspringen,
				 r10 zeigt auf naechstes Kommando in CMD_LISTE!
	jr	CMDL4		!naechstes Kommando vergleichen!

CMDL6:
	call	GET_CHR		!rl0 := Zeichen, auf das INPTR zeigt,
				 INPTR := INPTR + 1!
	cpb	rl0, @r10	!Vergleich der Kommandozeichen!
	jr	nz, CMDL5	!Kommandonamen nicht identisch!
	inc	r10, #1
	djnz	r9, CMDL6	!naechstes Zeichen!

	inc	r10, #1
	and	r10, #%FFFE	!r10 zeigt auf Adresse der Monitorroutine!
	ld	r2, @r10
	jp	@r2		!Absprung in die Monitorroutine!
    end CMDLOP

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INIT_CTC0_0
Initialisierung CTC0_0 fuer Baudrate SIO0_A
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    INIT_CTC0_0 procedure
      entry
	push	@r15, r0
	ldb	rh0, #%57	!kein Int., Zaehler, Vorteiler 16, Zeitkonst.!
	ldb	rl0, #BAUD9600	!Baudrate SIO0 Kanal A!
	outb	CTC0_0, rh0
	outb	CTC0_0, rl0
	pop	r0, @r15
	ret
    end INIT_CTC0_0 
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INIT_CTC0_1
Initialisierung CTC0_1 fuer Baudrate SIO0_B
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    INIT_CTC0_1 procedure
      entry
	push	@r15, r0
	ldb	rh0, #%57	!kein Int., Zaehler, Vorteiler 16, Zeitkonst.!
	ldb	rl0, #BAUD9600	!Baudrate SIO0 Kanal B!
	outb	CTC0_1, rh0
	outb	CTC0_1, rl0
	pop	r0, @r15
	ret
    end INIT_CTC0_1 
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INIT_SIO0_A
Initialisierung SIO0_A fuer LOAD/SEND/QUIT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    INIT_SIO0_A procedure
      entry
	push	@r15, r0
	push	@r15, r1
	push	@r15, r2

	ld	r2, #ITAB_SIO0_A
	ld	r0, #9
	ld	r1, #SIO0C_A
	otirb	@r1, @r2, r0

	ld	r2, #ITAB_SIO0_INTVEK
	ld	r0, #2
	ld	r1, #SIO0C_B
	otirb	@r1, @r2, r0

	pop	r2, @r15
	pop	r1, @r15
	pop	r0, @r15
	ret
    end INIT_SIO0_A 
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INIT_SIO0_B
Initialisierung SIO0_B fuer Terminalbetrieb
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    INIT_SIO0_B procedure
      entry
	push	@r15, r0
	push	@r15, r1
	push	@r15, r2

	ld	r2, #ITAB_SIO0_B
	ld	r0, #9
	ld	r1, #SIO0C_B
	otirb	@r1, @r2, r0

	ld	r2, #ITAB_SIO0_INTVEK
	ld	r0, #2
	ld	r1, #SIO0C_B
	otirb	@r1, @r2, r0

	pop	r2, @r15
	pop	r1, @r15
	pop	r0, @r15
	ret
    end INIT_SIO0_B 

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE ERROR
allgemeine Fehlerprozedur
Ausgabe von '?' und Sprung in Kommandoeingabeschleife
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    ERROR procedure
      entry
	ldb	rl0, #'?'
	call	TYWR
	ld	OUTPTR, #0
	ld	r15, #STACK
	jp	CMDLOP
    end ERROR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE UINSTR_ERR
Trap-Routine fuer nichtimplementierte Befehle
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    UINSTR_ERR procedure
      entry
	ld	r2, #UINSTR_TXT
	jp	_ERR
    end UINSTR_ERR

UINSTR_TXT:
	WORD := UINSTR_END - UINSTR_TXT - 2
	ARRAY [* BYTE] := 'UNIMPLEMENTED INSTRUCTION TRAP  ID= '
UINSTR_END:

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE PINSTR_ERR
Trap-Routine fuer priviligierte Befehle
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    PINSTR_ERR procedure
      entry
	ld	r2, #PINSTR_TXT
	jr	_ERR
    end PINSTR_ERR

PINSTR_TXT:
	WORD := PINSTR_END - PINSTR_TXT - 2
	ARRAY [* BYTE] := 'PRIVILEGED INSTRUCTION TRAP  ID= '
PINSTR_END:
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SC_ENTRY
SYSTEM-CALL-ENTRY

Bei Aufruf eines Unterprogramms ueber System Call:
Input:	r3 darf keine Eingabewerte fuer das Unterprogramm enthalten
Output: r3 enthaelt Wert von r0 nach Ausfuehrung des Unterprogramms
	   (d.h. evtl. in r0 vom Unterprogramm zurueckgegebene Werte werden
	   bei Aufruf ueber System Call in r3 zurueckgegeben)
zerstoerte Register : r3
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SC_ENTRY procedure
      entry
	push	@r14, r0
	ldctl	r0, FCW
	res	r0, #%0F
	ldctl	FCW, r0
	cpb	3(r15), #1	!SC_01 = segmentierte Arbeitsweise!
	jr	nz, SC_02
 
	set	4(r15), #15	!Segment-Bit im FCW setzen!
	jr	SC_END
	
SC_02:
	cpb	3(r15), #2	!SC_02 = nichtsegmentierte Arbeitsweise!
	jr	nz, SC_X3
	res	%04(r15), #15	!Segment-Bit im FCW ruecksetzen!
	jr	SC_END
	
SC_X3:
	pop	r0, @r15
	testb	1(r15)		!SC_00 = BREAK ?!
	jr	nz, NO_BRK
	ld	SV_R, r0
	ldl	SVSTK, rr14
	ld	r15, #STACK
	call	SAVREG
	inc	SVSTK + 2, #8
	dec	PC_OFF, #%02
	dec	NXTCTR, #%01
	jp	nz, GO_NXT
	ld	NXTCTR, #1
	call	RETI_CTC0_3
	ei	vi
	
	ld	r2, #T_BRK	!Text: BREAK AT ....!
	call	WR_MSG
	ld	r5, PC_OFF
	call	BTOH16
	call	WR_OUTBFF_CR
	jp	CMDLOP

NO_BRK:
	push	@r15, r0
	ldb	rh0, 3(r15)	!Nummer des SC laden!
	ldb	rl3, rh0
	clrb	rh3
	sub	r3, #4		!SC_04 = 1. Adresse in SC_ADR-Liste!
	ld	r3, SC_ADR(r3)	!Adresse des UP zur Ausfuehrung des SC laden!
	ei	vi
	
	call	@r3		!Aufruf des UP!
	ld	r3, r0
SC_END:
	ldctl	r0, FCW
	
	set	r0, #%0F	!segmentierter Mode einstellen!
	ldctl	FCW, r0
	
	pop	r0, @r14
	iret	
    end SC_ENTRY

  INTERNAL
T_BRK:	WORD := %0009
	ARRAY [* BYTE] := 'BREAK AT  '

!------------------------------------------------------------------------------
Tabelle der Monitorroutinen, die ueber System Calls aufrufbar sind
------------------------------------------------------------------------------!
 
  INTERNAL
SC_ADR	ARRAY [* WORD] := [TYRD TYWR RD_LINE_BFF WR_CRLF WR_MSG BTOH16
                           WR_OUTBFF_CR BTOH8 PUT_SEGNR PUT_CHR
			   WOCF_MCZ TYWR_MCZ CMDLOP DISK
			   TYRD_UDOS_INT TYWR_UDOS_INT FLOPPY WR_OUTBFF]

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MMU_ERR
Segment-Trap-Routine fuer MMU-Fehler
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    MMU_ERR procedure
      entry
	ld     	r2, #MMU_TXT
_ERR:
	ld	r5, @r15
	call	WR_MSG
	call	BTOH16
	call	WR_OUTBFF_CR
	jp	CMDLOP
    end MMU_ERR 
 
MMU_TXT:
	WORD := MMU_END - MMU_TXT - 2
	ARRAY [* BYTE] := 'MMU TRAP ID= '
MMU_END:
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE NMI_INT
NMI-Routine
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    NMI_INT procedure		!Routine wird nach dem ersten Tastendruck
				 in PSAREA aktiviert!
      entry
	push	@r15, r0
	inc	r15, #2		!Stackpointer wie vor dem push stellen!
	ld	r0, @r15	!NMI-Identifier lesen!
	dec 	r15, #2
	bitb	rl0, #0		!manuelles NMI? !
	jr	nz, NMIman	!ja!
	bitb	rl0, #2		!Paritaets-NMI? !
	jr	nz, NMIpar	!ja!
!------!

NMIman: pop	r0, @r15
	bitb	FLAG0, #%00
	jr	nz, NMI1	!Bit0/Flag0 gesetzt:--> ruecksetzen
				 (empf. Zeichen ist ASCII-Zeichen)!
	ldl	SVSTK, rr14
	ld	r15, #STACK
	ld	SV_R, r0
	call	SAVREG		!Register im Rettungsbereich eintragen!
	inc	SVSTK + 2, #8
	jr	NMI2

NMI1:
	resb	FLAG0, #%00	!ruecksetzen!
	ld	r15, #STACK
NMI2:
	ei	vi
	
	call	WR_CRLF		!Ausgabe CR LF an Terminal!
	ld	r2, #T_NMI	!Ausgabe: 'NMI'!
	call	WR_MSG
	jp	CMDLOP

!------!
NMIpar:	
	inb	rl0, S_BNK
	ldb	rh0, rl0
  	resb	rl0, #3
	outb	S_BNK, rl0	!Paritaetsfehler ruecksetzen!
	outb	S_BNK, rh0	!Paritaetskontrolle wieder einschalten!
	ld	REM_PARNMI, #%FFFF !Merkerzelle  'Paritaets-NMI aufgetreten'
   				    setzen!
	pop	r0, @r15
	sc	#SC_SEGV
	iret

    end NMI_INT

  INTERNAL
T_NMI:	WORD := %0005
	ARRAY [* BYTE] := 'NMI %0D'

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE NVI_ERR
Interrupt-Routine fuer uninitialisierten nichtvektorisierten Interrupt (NVI)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    NVI_ERR procedure
      entry
	ld	r2, #NVI_TXT
	jr	_ERR
    end NVI_ERR

NVI_TXT:
	WORD := NVI_END - NVI_TXT - 2
	ARRAY [* BYTE] := 'UNINITIALIZED NONVECTORED INTERRUPT ENTRY  ID= '
NVI_END:
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE VI_ERR
Interrupt-Routine fuer uninitialisierten vektorisierten Interrupt (VI)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    VI_ERR procedure
      entry
	ld	r2, #VI_TXT
	jr	_ERR
    end VI_ERR

VI_TXT:
	WORD := VI_END - VI_TXT - 2
	ARRAY [* BYTE] := 'UNINITIALIZED VECTORED INTERRUPT ENTRY  ID= '
VI_END:
 
END p_init
