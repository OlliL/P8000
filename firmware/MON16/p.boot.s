!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_boot

  Bearbeiter:	J. Zabel
  Datum:	16.11.1987
  Version:	3.0

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_boot
------------------------------
Dieser Modul umfasst die Monitorroutine zum manuellen Start des WEGA-Systems
(O) sowie die NMI-Routine zum automatischen Start des WEGA-Systems.

Prozeduren
----------
AUTOBOOT
OS_BOOT
DSK_BOOT
FLP_BOOT
UDOS_BOOT
RM_BOOT
==============================================================================!


p_boot module

$SECTION PROM

!anfang mon16.include!
!******************************************************************************
allgemeine Vereinbarungen (Stand: 16.11.1987)
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

  INTERNAL
_serial 	LONG := %00000000
  
  INTERNAL
T_DSK_BOOT
	WORD := %0017
	ARRAY [* BYTE] := 'BOOTING FROM HARD DISK%0D'
T_FLP_BOOT
	WORD := %0014
	ARRAY [* BYTE] := 'BOOTING FROM FLOPPY%0D'
T_UDOS_BOOT
	WORD := %0019
	ARRAY [* BYTE] := 'BOOTING FROM UDOS FLOPPY%0D'
T_RM_BOOT
	WORD := %000F
	ARRAY [* BYTE] := 'BOOTING REMOTE%0D'
 
  CONSTANT
BOOT0_START	:= %8000	!Lade- und Startadresse fuer "boot0"!
BOOT0_FILETYP	:= %01		!binary file/keine Uebertragung des Datei-
				 descriptors!

  INTERNAL
BOOT0_UDOS_FILENAME ARRAY [* BYTE] := 'boot0.ud%00'
BOOT0_RM_FILENAME  ARRAY [* BYTE] := 'boot0.rm%00'

TXT_ABO
	WORD	:= 6
	ARRAY [* BYTE] := 'ABORT%0D'


!******************************************************************************
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE AUTOBOOT
automatisches Laden und Starten "boot0" von DISK (Block 0)
NMI-Routine (vor erster Terminaleingabe)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    AUTOBOOT procedure
      entry
	call	TEST_		!Hardwareeigentest!
	ld	r4, r2		!R4=0 =AUTOMATIC BOOT  /  R4=1  MANUAL BOOT!
	test	r2
	jr	z, DSK_BOOT	!r4=0!
	
!Hardwareeigentest war fehlerhaft --> kein WEGA-Start:
 NMI-Entry in PSAREA aendern, PROMT setzen und Rueckkehr in 
 Kommandoeingabeschleife!
	ld	PSA_NMI+6, #NMI_INT	!NMI_ENTRY in PSAREA:
					- nach RESET steht dort #AUTOBOOT, d.h.
					  bei NMI erfolgt Abarbeitung von
					  #AUTOBOOT
					- nach Eingabe des ersten Zeichens vom
					  Terminal (Abarbeitung von PTY_INT oder
					  KOPPEL_INT) bzw. nach Abarbeitung
					  von #AUTOBOOT wird #AUTOBOOT durch
					  #NMI_INT ersetzt, d.h. bei NMI erfolgt
					  jetzt Abarbeitung von #NMI_INT !
	
	ld	PROMT, #'* '
	ei	vi
	jp	CMDLOP
 end AUTOBOOT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE OS_BOOT
OS_BOOT-Routine (Monitorkommando)
manuelles BOOT von HARD DISK, FLOPPY, UDOS FLOPPY bzw. REMOTE

Kommandoeingabe: O D/F/U/R
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    OS_BOOT procedure
      entry
	call	GET_SIGN	!rl0:=naechstes Zeichen ungleich Space nach
				 einem Space im Eingabepuffer INBFF!
	jp	z, ERROR	!CR eingegeben!
	
	cpb	rl0, #'D'	!FROM HARD DISK ? !
	jr	z, OS_BOOT_DSK
	cpb	rl0, #'F'	!FROM FLOPPY ? !
	jr	z, OS_BOOT_FLP
	cpb	rl0, #'U'	!FROM UDOS FLOPPY ? !
	jr	z, OS_BOOT_UDOS
	cpb	rl0, #'R'	!FROM REMOTE ? !
	jr	z, OS_BOOT_REM
	jp	ERROR		!nicht 'D','F','U','R' als Parameter angegeben!

OS_BOOT_DSK:
	ld	r2, #T_DSK_BOOT
	call	WR_MSG
	ld	r4, #1		!MANUEL BOOT FROM DISK!
	jr	DSK_BOOT

OS_BOOT_FLP:
	ld	r2, #T_FLP_BOOT
	call	WR_MSG
	ld	r4, #1		!MANUEL BOOT FROM FLOPPY!
	jp	FLP_BOOT

OS_BOOT_UDOS:
	ld	r2, #T_UDOS_BOOT
	call	WR_MSG
	ldl	rr6, _serial
	ld	r5, _numterm	!Anzahl der Terminals!
	ld	r4, #1		!MANUEL BOOT FROM UDOS FLOPPY!
	jr	UDOS_BOOT

OS_BOOT_REM:
	ld	r2, #T_RM_BOOT
	call	WR_MSG
	ldl	rr6, _serial
	ld	r5, _numterm	!Anzahl der Terminals!
	ld	r4, #1		!MANUEL BOOT FROM REMOTE!
	jp	RM_BOOT
    end OS_BOOT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE DSK_BOOT
Laden und Starten "boot0" von DISK (Block 0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    DSK_BOOT procedure
      entry
	ld	r0, #%4000
	ldctl	FCW, r0
	PUSH	@R15, R4	!MANUELL OR AUTO BOOT MERKEN!
	LD	R7, #1		!READ_REQUEST!
	LD	R6, #0		!DRIVE NR.!
	ld	r8, #%8000	!Pufferadresse - Segment 0!
	ld	r9, #BOOT0_START!Pufferadresse - Offset!
	LDL	RR4, #0		!BLOCKNR.!
	LD	R10, #%200	!BLOCKLAENGE!
	SC	#SC_DISK	!r2 - Rueckgabeparameter!
 
	LDL	RR6, _serial
	LD	R5, _numterm	!ANZAHL DER TERMINALS!
	POP	R4, @R15
 
	TEST	r2	       	!r2=0 --> fehlerfrei!
	jp	z, BOOT0_START	!"boot0" starten!

	push	@r15, r2
	ld	r2, #T_HD_ERR
	sc	#SC_WR_MSG	!Textausgabe!
	pop	r5, @r15	!rl5:=Returncode!
	call	BTOH8		!rl5 (ASCII) in OUTBFF eintragen!
	sc	#SC_WR_OUTBFF_CR !Ausgabe des Returncodes!
	jp	CMDLOP
    end DSK_BOOT

T_HD_ERR:
	WORD := %10
	ARRAY [* BYTE] := 'HARD DISK ERROR '

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLP_BOOT
Laden und Starten "boot0" von Floppy 
(blockorientierte Diskette, Block 0, Drive 0, Floppytyp 6)
(ueber 8-Bit-/16-Bit-Koppelschnittstelle)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    FLP_BOOT procedure
      entry
	cpb	PTYPORT, #%02	!8-Bit-Teil aktiv und UDOS verfuegbar?!
	jp	nz, ERROR	!nein --> Kommando nicht ausfuehrbar!

	ld	r0, #%4000
	ldctl	FCW, r0
	PUSH	@R15, R4	!MANUELL BOOT MERKEN!
	LD	R7, #1		!READ_REQUEST!
	ld	r6, #%60	!Floppytyp 6/Drive 0!
	ld	r8, #%8000	!Pufferadresse - Segment 0!
	ld	r9, #BOOT0_START!Pufferadresse - Offset!
	LDL	RR4, #0		!BLOCKNR.!
	LD	R10, #%200	!BLOCKLAENGE!
	SC	#SC_FLOPPY	!r2 - Rueckgabeparameter!
 
	LDL	RR6, _serial
	LD	R5, _numterm	!ANZAHL DER TERMINALS!
	POP	R4, @R15
 
	TEST	r2	       	!r2=0 --> fehlerfrei!
	jp	z, BOOT0_START	!"boot0" starten!

	push	@r15, r2
	ld	r2, #T_FLP_ERR
	sc	#SC_WR_MSG	!Textausgabe!
	pop	r5, @r15	!rl5:=Returncode!
	call	BTOH8		!rl5 (ASCII) in OUTBFF eintragen!
	sc	#SC_WR_OUTBFF_CR !Ausgabe des Returncodes!
	jp	CMDLOP
    end FLP_BOOT

T_FLP_ERR:
	WORD := 13
	ARRAY [* BYTE] := 'FLOPPY ERROR '

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE UDOS_BOOT
Laden und Starten  "boot0.ud" von UDOS-Floppy
(ueber 8-Bit-/16-Bit-Koppelschnittstelle)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

!rr4 und rr6 enthalten Parameter nicht benutzen!

  INTERNAL
    UDOS_BOOT procedure
      entry
	cpb	PTYPORT, #%02	!8-Bit-Teil aktiv und UDOS verfuegbar?!
	jp	nz, ERROR	!nein --> Kommando nicht ausfuehrbar!

	clr	COUNT_UDOS
	clr	INPTR_UDOS
	clr	OUTPTR_UDOS

!Startmeldung an 8-Bit-Teil senden (Datenbyte: "S")!
	ldb	rl0, #'S'	!Datenbyte: 8-Bit-Teil soll Datei senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Filetyp an 8-Bit-Teil! 
	ldb	rl0, #BOOT0_FILETYP
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Dateinamen an 8-Bit-Teil!
	ldb	rh1, #10	!retries!
flpo0:
	ldb	rl1, #0		!Checksumme init!
	ld	r8, #BOOT0_UDOS_FILENAME
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR_UDOS_INT
flpo1:
	ldb	rl0 ,@r8	!Zeichen von filename!
	inc	r8, #1
	cpb	rl0, #0		!ende filename ?!
	jr	z, flpo2	!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR_UDOS_INT
	jr	flpo1
flpo2:
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR_UDOS_INT
	exb	rl0, rl1
	sc	#SC_TYWR_UDOS_INT	!Checksumme ausgeben!
	sc	#SC_TYRD_UDOS_INT	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, flpread	!filename richtig uebertragen
			 	 file vorhanden!
	cpb	rl3, #CAN	!Abbrechen ?!
	jr	z, flperror
	dbjnz	rh1, flpo0	!naechster Versuch!
!Uebertragungsfehler! 
flperror:
	ld	r2, #TXT_ABO
	sc	#SC_WR_MSG	!Textausgabe!
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT
	ret	

!Einlesen aller Bloecke mit den Daten der Datei! 
flpread:
	ld	r9, #BOOT0_START	!Speicheranfangsadresse fuer "boot0"!
flpread1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
flpr0:				!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Buffer Pointer retten!
	sc	#SC_TYRD_UDOS_INT
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, flpr01	!nein!
	sc	#SC_TYRD_UDOS_INT	!noch 1 Zeichen im Puffer!
	jp	BOOT0_START	!file geladen Sprung zum boot0!
flpr01:
	cpb	rl3, #CAN
	jr	z, flperror	!Abbruch von 8-Bit-Teil!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, flpr2	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD_UDOS_INT
	ldb	rh2, rl3
	sc	#SC_TYRD_UDOS_INT
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
flpr1:				!Scheife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD_UDOS_INT	!Daten lesen!
	ldb	@r9, rl3	!in Buffer!
	inc	r9, #1
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, flpr1
flpr2:				!Schleife: Warten auf ETX!
	sc	#SC_TYRD_UDOS_INT
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, flpr2
 
	sc	#SC_TYRD_UDOS_INT	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, flpr3	!Fehler!
 
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR_UDOS_INT
	jr	flpread1	!und naechsten Block einlesen!
flpr3:
	decb	rh1, #1
	jr	z, flperror	!Abbruch!
 
	ld	r9, r8		!Buffer Pointer auf Anfang zurueck!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR_UDOS_INT
	jr	flpr0		!Wiederholung der Blockuebertragung!
    end UDOS_BOOT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RM_BOOT
Laden und Starten "boot0.rm" ueber "remote" des lokalen Systems
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

!rr4 und rr6 enthalten Parameter nicht benutzen!

  INTERNAL
    RM_BOOT procedure
      entry
	setb	FLAG0, #0	!Abschalten xon xoff!

!Startmeldung (SOH, ESC, 'S') an local senden!
	ldk	r0, #SOH	!Kommando Send file an local!
	sc	#SC_TYWR
	ldb	rl0, #ESC
	sc	#SC_TYWR
	ldb	rl0, #'S'
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge von local!
!Ausgabe Filetyp an local! 
	ldb	rl0, #BOOT0_FILETYP
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge von local!
!Ausgabe Dateinamen an local! 
	ldb	rh1, #10	!retries!
rmo0:
	ldb	rl1, #0		!Checksumme init!
	ld	r8, #BOOT0_RM_FILENAME
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR
rmo1:
	ldb	rl0, @r8	!Zeichen von filename!
	inc	r8, #1
	cpb	rl0, #0		!ende filename ?!
	jr	z, rmo2		!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR
	jr	rmo1
rmo2:
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR
	exb	rl0, rl1
	sc	#SC_TYWR	!Checksumme ausgeben!
	sc	#SC_TYRD	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, rmread	!filename richtig uebertragen
			 	 file vorhanden!
	cpb	rl3, #CAN	!Abbrechen ?!
	jr	z, rmerror
	dbjnz	rh1, rmo0	!naechster Versuch!
!Uebertragungsfehler! 
rmerror:
	ld	r2, #TXT_ABO
	sc	#SC_WR_MSG	!Textausgabe!
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR
	sc	#SC_TYRD
	resb	FLAG0, #0
	ret

!Einlesen aller Bloecke mit den Daten der Datei! 
rmread:
	ld	r9, #BOOT0_START	!Speicheranfangsadresse fuer "boot0"!
rmread1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
rmr0:				!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Buffer Pointer retten!
	sc	#SC_TYRD
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, rmr01	!nein!
	sc	#SC_TYRD	!noch 1 Zeichen im Puffer!
	resb	FLAG0, #0
	jp	BOOT0_START	!file geladen Sprung zum boot0!
rmr01:
	cpb	rl3, #CAN
	jr	z,rmerror	!local gibt Abbruch!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, rmr2	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD
	ldb	rh2, rl3
	sc	#SC_TYRD
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
rmr1:				!Schleife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD	!Daten lesen!
	ldb	@r9, rl3	!in Buffer!
	inc	r9, #1
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, rmr1
rmr2:
	sc	#SC_TYRD	!warten auf ETX!
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, rmr2
 
	sc	#SC_TYRD	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, rmr3	!Fehler!
 
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR
	jr	rmread1		!und naechsten Block einlesen!
rmr3:
	decb	rh1, #1
	jr	z, rmerror	!Abbruch!
 
	ld	r9, r8		!Buffer Pointer auf Anfang zurueck!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR
	jr	rmr0
    end RM_BOOT
end p_boot
