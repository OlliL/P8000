!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_brk

  Bearbeiter:	J. Zabel
  Datum:	16.11.1987
  Version:	3.0

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_brk
-----------------------------
Dieser Modul umfasst Monitorroutinen zur Programmtestung (R, B, G, N,)

Prozeduren
----------
REGISTER
WR_REGBEZ1
WR_REGBEZ2
WR_WERTE1
WR_WERTE2

SAVREG
GETREG

BREAK

GO
GO_NXT
GO_INT

NEXT
NXT_INT

RETI_CTC0_3
==============================================================================!


p_brk module

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
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE REGISTER
REGISTER-Routine (Monitorkommando)
Registerinhalte anzeigen und veraendern

Kommandoeingabe: R [ [R]0,...,[R]15; [R]L0,...,[R]L7; [R]H0,...,[R]H7;
                     RR0, RR2, RR4, RR6, RR8, RR10, RR12, RR14;
                     [R]PC, FC, [R]RF, [R]N4, [R]N5, [R]PS, [R]PO ]
(wenn keine Parameter angegeben, dann Anzeige aller Register)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    REGISTER procedure
      entry
	call	GET_SIGN	!rl0:=1.Zeichen ungleich Space aus INBFF!
	jp	z, ALL_RG	!keine Parameter angegeben -->
				 alle Registerinhalte ausgeben!
	cpb	rl0, #'R'
	jr	nz, RG1		!1. Zeichen beginnt nicht mit 'R'!

	call	GET_CHR		!rl0 := 2. Zeichen!
	jr	z, R_ERROR	!kein weiteres Zeichen angegeben!

	cpb	rl0, #'F'
	jr	nz, RG1		!2. Zeichen ungleich 'F'!

	ldb	rh1,#'R'	!1. Zeichen bei 'RF'!
	jr	RG6

RG1:
	clr	r6		!r6=0 bei 'RL'-Register!
	cpb	rl0, #'L'	
	jr	z, R_L
	inc	r6, #%02	!r6=2 bei 'RH'-Register!
	cpb	rl0, #'H'
	jr	z, R_L
	inc	r6, #%04	!r6=6 wenn nicht 'RL', 'RH' oder 'RR'-Reg.!
	cpb	rl0, #'R'
	jr	nz, RG5		!kein 'RL', 'RH', 'RR'-Register!
	dec	r6, #%02	!r6=4 bei 'RR'-Register!
R_L:
	call	GET_CHR		!rl0:=naechstes Zeichen
				 (=Registernummer 0-15, 1.Zeichen)!
	jr	z, R_ERROR	!keine Registernummer angegeben!

RG5:
	ldb	rh1, rl0	!rh1:=Registerbez. (siehe R_BEZ) 1. Zeichen!
	call	GET_CHR		!rl0 := 2. Zeichen der Registerbez.!
	jr	nz, RG6		!2. Zeichen des Registerbezeichnung vorh.!

	ldb	rl0, #' '	!nur einzeichige Registerbezeichnung!
RG6:
	ldb	rl1, rl0	!rl1:=Registerbez. (siehe R_BEZ) 2. Zeichen!
	ld	r7, #%0018
	ld	r8, #R_BEZ + 46
	cpdr	r1, @r8, r7, z	!Registerbez. (in r1) in Tabelle R_BEZ suchen!
	jr	nz, R_ERROR	!Falsche Registerbezeichnung!

	inc	r8, #%02	!r8 zeigt auf Reg.-bez.!
	add	r7, r7		!r7:= rel. Adr. bzgl. R_BEZ!
	cpb	rl6, #%04
	jr	c, RG17		!'RL' bzw. 'RH'-Reg.!
	jr	z, RG13		!'RR'-Reg.!

!Schleife fuer {kein RL*, RH*, RR* - Register}!

!ab aktuellen Register werden die Register entsprechend der Tabelle R_BEZ
 abgearbeitet (Anzeige Inhalt + Dateneingabe)!

RG8:
	ld	r1, #'R '
	call	PUT_2CHR
	dec	OUTPTR, #1
	ld	r1, @r8
	call	PUT_2CHR
	inc	OUTPTR, #1
	ld	r5, SV_R(r7)
	call	WR_R5_RDDATA
	ret	c		!'Q' eingegeben!
	jr	z, RG12
	ld	r2, @r8
	cp	r2, #'PC'
	jr	nz, RG9
	res	r3, #%0
	jr	RG11

RG9:
	cp	r2, # 'RF'
	jr	nz, RG10
	ldctl	REFRESH, r3
RG10:
	cp	r2, #'SG'
	jr	nz, RG11
	cpb	rh3, #%0
	jr	nz, RG11
	ldb	rh3, rl3
	clrb	rl3
RG11:
	ld	SV_R(r7), r3
RG12:
	inc	r7, #%02
	inc	r8, #%02
	cp	r8, #R_BEZ + 47
	jr	c, RG8
	ret			!alle Register abgearbeitet!

R_ERROR:
	jp	ERROR

!Schleife fuer RR* - Register!

!ab aktuellen Register werden alle Register in der Reihenfolge
 RR0 ... RR14 abgearbeitet!

RG13:
	cp	r8, #R_BEZ + 30
	jr	nc, R_ERROR

	cpb	rl1, #' '
	jr	z, RG14		!einzeichige Registerbezeichnung!
	bit	r1, #%0
	jr	RG_ERR
RG14:
	bit	r1, #%08
RG_ERR:
	jr	nz, R_ERROR	!Registerbezeichnung nicht gerade (0-14)!
RG15:
	ld	r1, #'RR'
	call	PUT_2CHR
	ld	r1, @r8
	call	PUT_2CHR
	inc	OUTPTR, #1
	ld	r5, SV_R(r7)
	call	BTOH16
	ld	r5, SV_R + 2(r7)
	call	WR_R5_RDDATA
	ret	c		!'Q' eingegeben!
	jr	z, RG16
	ld	SV_R(r7), r2
	ld	SV_R + 2(r7), r3
RG16:
	inc	r7, #%04
	inc	r8, #%04
	cp	r8, #R_BEZ + 31
	jr	c, RG15
	ret			!alle Register abgearbeitet!

!Schleife fuer RL* bzw. RH* - Register!

!ab aktuellen Register werden alle Register in der Reihenfolge
 RH0, RL0,...,RH7, RL7 abgearbeitet!

RG17:
	cp	r8, #R_BEZ + 16
	jr	nc, R_ERROR

	test	r6
	jr	nz, RG18
	inc	r7, #1
RG18:
	ld	r1, R_LH(r6)	!r1:=Registerbezeichnung (RH bzw. RL)!
	call	PUT_2CHR
	ld	r1, @r8
	call	PUT_2CHR
	ldb	rl5, SV_R(r7)
	call	BTOH8
	inc	OUTPTR, #1
	call	WR_OBF_RDDATA
	ret	c
	jr	z, RG19
	ldb	SV_R(r7), rl3
RG19:
	inc	r7, #1
	dec	r6, #%02
	jr	z, RG18
	inc	r6, #%04
	inc	r8, #%02
	cp	r8, #R_BEZ + 16
	jr	c, RG18
	ret			!alle Register angegeben!

!Alle Registerinhalte anzeigen!

ALL_RG:
	calr	WR_REGBEZ1
	CALR	WR_WERTE1
	CALR	WR_REGBEZ2
	jr	WR_WERTE2
    end REGISTER

!Registerbezeichnungen!

  INTERNAL
R_BEZ	 ARRAY [* BYTE] := '0 1 2 3 4 5 6 7 8 9 101112131415SGPCFCRFN4N5PSPO'
R_LH ARRAY [* BYTE] := 'RLRH'

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_REGBEZ1
Ausgabe der Registerbezeichnungen 1. Zeile
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_REGBEZ1 procedure
      entry
	ldk	r0, #%0C
	ld	r4, #R_BEZ
	ldk	r6, #%08
	ld	r8, #R_BEZ + 30
L_B1:
	ld	r3, OUTPTR
	ldb	OUTBFF(r3), #'R'
L_B2:
	inc	OUTPTR, #1
	ld	r1, @r4
	call	PUT_2CHR
	dec	r0, #1
	jp	z, WR_OUTBFF_CR
	dec	r6, #1
	jr	nz, L_B3
	ld	r4, r8
L_B3:
	inc	r4, #%02
	cp	r6, #0
	jr	gt, L_B4
	inc	OUTPTR, #%04
	jr	L_B2
L_B4:
	inc	OUTPTR, #%02
	jr	L_B1
    end WR_REGBEZ1

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_REGBEZ2
Ausgabe der Registerbezeichnungen 2. Zeile
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_REGBEZ2 procedure
      entry
	ldk	r0, #%0C
	ld	r4, #R_BEZ + 16
	ld	r8, #R_BEZ + 38
	ldk	r6, #%08
	jr	L_B1
    end WR_REGBEZ2
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_WERTE1
Ausgabe der Registerwerte 1. Zeile
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_WERTE1 procedure
      entry
	ldk	r2, #%0C
	ld	r6, #SV_R
	ldk	r8, #%08
	ld	r10, #SVSTK + 2
L_W1:
	ld	r5, @r6
	call	BTOH16
	dec	r2, #1
	jp	z, WR_OUTBFF_CR
	dec	r8, #1
	jr	nz, L_W2
	ld	r6, r10
L_W2:
	inc	r6, #%02
	cp	r8, #0
	jr	gt, L_W3
	inc	OUTPTR, #%03
	jr	L_W1
L_W3:
	inc	OUTPTR, #1
	jr	L_W1
    end WR_WERTE1

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_WERTE2
Ausgabe der Registerwerte 2. Zeile
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_WERTE2 procedure
      entry
	ldk	r2, #%0C
	ld	r6, #SV_R + %10
	ldk	r8, #%08
	ld	r10, #RF_CTR
	jr	L_W1
    end WR_WERTE2

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SAVREG
Registerrettung
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    SAVREG procedure
      entry
	ldl	RR14_V, rr14	!ORIGINAL STACKWERTE IN SAV BEREICH UMLADEN!
	ldl	rr14, SVSTK
	ld	r0, r15(#0)	!IDENTIFIER LADEN!
	ld	RR14_V + 4, r0
	ld	r0, r15(#%2)	!FCW LADEN!
	ld	FCW_, r0
	ld	r0, r15(#%4)	!SEGMENTADRESSE LADEN!
	ld	PC_SEG, r0
	ld	r0, r15(#%6)	!OFFSET LADEN!
	ld	PC_OFF, r0
	sc	#SC_SEGV
	ldctl	r0, NSPSEG 
	
	sc	#SC_NSEGV
	ld	RF_CTR + 2, r0
	ldctl	r0, NSPOFF
	
	ld	RF_CTR + 4, r0
	sc	#SC_SEGV
	ldctl	r0, PSAPSEG 
	
	sc	#SC_NSEGV
	ld	PS_, r0
	ldctl	r15, PSAP
	
	ld	PO_, r15
	ldctl	r15, REFRESH
	
	ld	RF_CTR, r15
	ld	r15, #SV_R + 2
	ldm	@r15, r1, #%0D	!REGISTERWERTE RETTEN!
	ldl	rr14, RR14_V
	ret	
    end SAVREG
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GETREG
Register-Lade-Routine
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    GETREG procedure
      entry
	ldl	RR14_V, rr14
	ldl	rr14, SVSTK
	dec	r15, #%08
	ld	r1, W23B4
	ld	r15(#%0000), r1
	ld	r1, FCW_
	ld	r15(#%0002), r1
	ld	r1, PC_SEG
	ld	r15(#%0004), r1
	ld	r1, PC_OFF
	ld	r15(#%0006), r1
	ld	r1, N4_
	sc	#SC_SEGV
	ldctl	NSPSEG, r1
	sc	#SC_NSEGV
	ld	r1, N5_
	ldctl	NSP, r1
	ld	r1, PS_
	sc	#SC_SEGV
	ldctl	PSAPSEG, r1
	sc	#SC_NSEGV
	ld	r1, PO_
	ldctl	PSAPOFF, r1
	ld	r15, #SV_R
	ldm	r0, @r15, #%0E
	ldl	rr14, RR14_V
	ret	
    end GETREG

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE BREAK
BREAKPOINT-Routine (Monitorkommando)
Setzen bzw. Loeschen eines Unterbrechungspunktes

Kommandoeingabe: B [<segnr>offset [count]]
                   (BREAK-Adresse Schleifenanzahl)
(wird kein Parameter angegeben, wird Unterbrechungspunkt geloescht)
Fehler, wenn Unterbrechungspunkt nicht im RAM liegt
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    BREAK procedure
      entry
	ldl	rr2, B_ADR_S	!rr2:=BREAK-Adresse!
	ld	r1, B_WORD	!r1:=urspruenglicher Inhalt auf BREAK-Adresse!
	sc	#SC_SEGV
	ld	@r2, r1		!BREAK_WORD ZURUECKLADEN!
	sc	#SC_NSEGV

	clr	B_ADR_S		!BREAK-Adresse loeschen!
	clr	B_ADR_O
	clr	B_WORD

	ld	NXTCTR, #1	!IMPLIZIT 1 ALS Schleifenzaehler FESTLEGEN!
	call	GET_SIGN	!rl0:=1. Zeichen ungleich Space nach 
				 naechstem Space im Eingabepuffer INBFF!
	RET	Z		!RUECKSPRUNG WENN kein Parameter angegeben!

	call	GET_ADR		!rr2 := BREAK_ADRESSE !
	ldl	rr4, rr2
	jr	z, BR1		!Schleifenzaehler NICHT ANGEGEBEN!

	call	GET_HEXZ	!r3:=Schleifenanzahl!
	test	r3
	jr	z, BR1		!angegebene Schleifenanzahl=0!

	ld	NXTCTR, r3	!eingegebene Schleifenanzahl in Schleifen-
				 zaehler NXTCTR laden!

BR1:
	ldl	rr2, rr4
	res	r3, #%00	!rr2:=gerade BREAK-Adresse!
	ldl	B_ADR_S, rr2	!BREAK_ADRESSE RETTEN!
	ld	r1, B_CODE	!Unterbrechungscode!
	sc	#SC_SEGV
	ex	r1, @r2		!r1:=Inhalt auf der BREAK-Adresse;
				 BREAK-Adresse mit Unterbrechungscode laden!
	ld	r4, r1
	ld	r1, @r2
	sc	#SC_NSEGV
	ld	B_WORD, r4	!urspruenglichen Inhalt auf BREAK-Adr. retten!
	cp	r1, B_CODE	!wurde Unterbrechungscode auf BREAK-Adresse
				 eingetragen, d.h. liegt BREAK-Adr. im RAM?!
	ret	z		!ja!
	
	ld	NXTCTR, #1
	clr	B_ADR_S		!BREAK-Adresse loeschen!
	clr	B_ADR_O
	clr	B_WORD
	jp	ERROR		!BREAK-Adresse liegt nicht im RAM!
    end BREAK

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GO
GO-Routine (Monitorkommando)
Programmabarbeitung bei eingegebener Ansprungadresse bzw. beim
aktuellen PC-Stand starten

Kommandoeingabe: GO [<segnr>offset]
                   (Ansprungadresse)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GO procedure
      entry
	call	GET_SIGN	!rl0:=1.Zeichen ungleich Space nach naechstem
				 Space im Eingabepuffer INBFF!
	jr	z, GO_PC	!keine Ansprungadresse vorhanden!

	call	GET_ADR		!rr2:=Ansprungadresse!
	res	r3, #%00	!Adresse muss gerade sein!
	ldl	PC_SEG, rr2	!PC_SEG/PC_OFF := Ansprungadresse!

GO_PC:
	ldl	rr2, PC_SEG	!rr2:=Startadresse!
	cpl	rr2, B_ADR_S	!=BREAK-Adresse?!
	jr	z, GO_NXT	!ja!

	calr	GETREG		!Register laden!
	ldl	rr14, SVSTK
	dec	r15, #8
	sc	#SC_SEGV
	iret			!Programm starten!
    end GO
	
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GO_NXT
Abarbeitung des naechsten Befehls, wenn auf diesem Befehl Unterbrechungspunkt
steht
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GO_NXT procedure
      entry
	ldl	rr2, B_ADR_S	!rr2:=BREAK-Adresse!
	ld	r1, B_WORD	!r1:=urspruenglicher Inhalt der BREAK-Adresse!
	sc	#SC_SEGV
	ld	@r2, r1		!BREAK-Inhalt wird zurueckgeladen, um den
				 Befehl abarbeiten zu koennen!
	sc	#SC_NSEGV
	ld	r2, #GO_INT	!Adresse der VI-Routine:
				 Interrup erfolgt nach Abarbeitung des Befehls;
				 in der GO_INT - Routine wird dann der Unter-
				 brechungspunkt wieder gesetzt!

G2:
	di	vi
	
	ld	VI_CTC0_3+2, r2	!Adresse der VI-Routine eintragen!
	calr	GETREG		!REGISTER ZURUECKLADEN!
	ldl	rr14, SVSTK
	dec	r15, #8
	sc	#SC_SEGV
	push	@r14, r1
	ld	r1, r14(#%0004)	!FCW_V AUS DEM STACK LADEN!
	sc	#SC_NSEGV
	ld	FCW_V, r1
	or	r1, #%1000	!VIE BIT IM FCW SETZEN!
	sc	#SC_SEGV
	ld	r14(#%0004), r1
	sc	#SC_NSEGV

!CTC als Zaehler programmieren - Interrupt nach Abarbeitung des kommenden
Befehls im zu testenden Programm!
	CLRB	RL1
	OUTB	CTC0_0, RL1	!Interruptvektor in den CTC laden!
	LDB	RL1, #%C7	!Zaehlermode!
	OUTB	CTC0_3, RL1
	LD	R1, STKCTR	!4 Stackoperationen!
	OUTB	CTC0_3,RL1	!Zeitkonstante!

	ldctl	r1, FCW
	set	r1, #%0F	!SEG BIT IM FCW SETZEN!
	ldctl	FCW, r1
	
	pop	r1, @r14
	iret			!Programmstart!
    end GO_NXT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GO_INT
VI-Routine fuer GO
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    GO_INT procedure
      entry
	push	@r15, r2
	push	@r15, r1
	calr	RETI_CTC0_3	!Interruptquelle ausschalten!
	ldl	rr2, B_ADR_S
	ld	r1, B_CODE
	sc	#SC_SEGV
	ld	@r2, r1		!Unterbrechungspunkt wieder setzen!
	sc	#SC_NSEGV
	bit	FCW_V, #%0C
	jr	nz, G_VIE	!VIE BIT GESETZT ? !
	ld	r2, r15(#%6)
	res	r2, #%0C
	ld	r15(#%0006), r2
G_VIE:
	pop	r1, @r15
	pop	r2, @r15
	sc	#SC_SEGV
	iret	
	
    end GO_INT
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE NEXT
NEXT-Routine (Monitorkommando)
Abarbeitung einer bestimmten Anzahl von Befehlen

Kommandoeingabe: N [count]
                   (Anzahl der Befehle (implizit %1))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    NEXT procedure
      entry
	setb	FLAG1, #%02	!Next-Betrieb!
				!dient in Kommandoeingabeschleife zur Kenn-
				 zeichnung, ob letztes Kommando NEXT-Kommando
				 war; wenn ja, kann Next-Kommando durch
				 Eingabe von nur CR wiederholt werden!

	call	GET_SIGN	!rl0:=1. Zeichen ungleich Space nach dem
				 naechsten Space im Eingabepuffer INBFF!
	jr	z, N1		!keine Anzahl eingegeben!
	
	call	GET_HEXZ	!r3:=Anzahl der Befehle!	
	test	r3
	jr	nz, N2		!ungleich 0!

N1:
	LDK	R3, #1		!implizit Einzelschrittbetrieb, wenn keine
				 Anzahl bzw. Anzahl=0 eingegeben wurde!
N2:
	ld	NXTCTR, r3	!Befehlszaehler!
N3:
	ld	r2, #NXT_INT	!Adresse der VI-Routine!
				!Interrupt erfolgt nach Abarbeitung des
				 naechsten Befehls des zu testenden Programms!
	ldl	rr4, B_ADR_S	!rr4:=BREAK-Adresse!
	cpl	rr4, PC_SEG	!=Adresse des naechsten Befehls?!	
	jr	nz, G2		!nein!

	ld	r1, B_WORD
	sc	#SC_SEGV
	ld	@r4, r1		!ehemaligen Inhalt der BREAK-Adresse zurueck-
				 laden, um Befehl abarbeiten zu koennen
				 (Unterbrechungspunkt wird nach Abarbeitung des
				 Befehls in VI-Routine NXT_INT wieder gesetzt)!
	sc	#SC_NSEGV
	jr	G2
    end NEXT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE NXT_INT
VI-Routine fuer NEXT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    NXT_INT procedure
      entry
	ldl	SVSTK, rr14
	ld	r15, #STACK
	ld	SV_R, r0
	calr	SAVREG		!Register retten!
	inc	SVSTK + 2, #8
	calr	RETI_CTC0_3	!Interrupt-Quelle loeschen!
	ldl	rr2, B_ADR_S
	ld	r1, B_CODE
	sc	#SC_SEGV
	ld	@r2, r1		!Unterbrechungspunkt wieder setzen!
	sc	#SC_NSEGV
	bit	FCW_V, #%0C
	jr	nz, N_I
	res	FCW_, #%0C
N_I:				!AUSGABE DER REGISTERBEZEICHNUNGEN UND WERTE!
	calr	WR_REGBEZ1
	calr	WR_WERTE1
	calr	WR_REGBEZ2
	calr	WR_WERTE2
	dec	NXTCTR, #1	!DURCHLAUFZAEHLER DEKREMENTIEREN!
	jr	nz, N3		!NAECHSTEN BEFEHL ABARBEITEN!

	ld	NXTCTR, #1	!Einzelschrittbetrieb bei Eingabe von nur CR
				 in Kommandoeingabeschleife!
	ei	vi
	jp	CMDLOP
    end NXT_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RETI_CTC0_3
Interruptquelle (CTC0_3) loeschen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RETI_CTC0_3 procedure
      entry
	LDB	RL1, #%ED	!U880-RETI (ED4D) ausgeben!
	OUTB	RETI_P, RL1
	LDB	RL1, #%4D
	OUTB	RETI_P, RL1
	LDB	RL1, #%03
	OUTB	CTC0_3, RL1	!CTC-Kanal ruecksetzen!
	ret	
    end RETI_CTC0_3

! Platzhalter !
internal
	array [%60 byte]

END p_brk
