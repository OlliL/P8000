!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1987
  U8000-Softwaremonitor (MON16)        Modul: p_gesa

  Bearbeiter:	J. Zabel
  Datum:	16.11.1987
  Version:	3.0

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_gesa
------------------------------
Dieser Modul umfasst Programme  fuer die Arbeit mit der
8-Bit-/16-Bit-Koppelschnittstelle (PIO-Schnittstelle) - 
(Monitorroutinen GE, S, Q, QRES und den Floppytreiber fuer blockorientierte
 Disketten)

Weiterhin umfasst dieser Modul Monitorroutinen fuer die Dateiarbeit mit
einem lokalen System - (RGE, RS, RQ).
Dazu ist ein entsprechender serieller Kanal (SI) des lokalen Systems mit dem
Systemterminalein-/-ausgang des P8000 zu verbinden. Auf dem lokalen System
muss ein entsprechendes "remote"-Programm laufen.

I-------I                           I----------------I             I----------I
I P8000 I(T) ------------------ (SI)I lokales System I(T) -------- I Terminal I
I-------I                           I----------------I             I----------I

(T) - Terminalein-/-ausgang

Prozeduren
----------
GET
SAVE
QUIT8
QUIT8_RESET

FLOPPY

RM_GET
RM_SAVE
RM_QUIT
==============================================================================!


p_gesa module

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

  CONSTANT
FILETYP		:= %05	!binary file/mit Uebertragung des Dateidescriptors!
BLOCKLEN	:= %40	!Laenge eines Blockes bei SAVE!

  INTERNAL
TXT_ILA
	WORD	:= 23
	ARRAY [* BYTE] := 'INCORRECT LOAD ADDRESS%0D'
TXT_ABO
	WORD	:= 6
	ARRAY [* BYTE] := 'ABORT%0D'
TXT_EPNT
	WORD	:= 12
	ARRAY [* BYTE] := 'ENTRY POINT '


!******************************************************************************
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE GET
GET-Routine "8-Bit-Teil" (Monitorkommando)
Laden einer UDOS-Procedure-Datei (von 8-Bit-Teil) in RAM
(ueber 8-Bit-/16-Bit-Koppelschnittstelle)

Kommandoeingabe: GE filename [<segnr>]
		    (Dateiname Segmentnummer(implizit PC_SEG))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    GET procedure
      entry
	cpb	PTYPORT, #%02	!8-Bit-Teil aktiv und UDOS verfuegbar?!
	jp	nz, ERROR	!nein --> Kommando nicht ausfuehrbar!

	clr	COUNT_UDOS
	clr	INPTR_UDOS
	clr	OUTPTR_UDOS

!Auswertung der eingegebenen Parameter!
	call	GET_SIGN	!INPTR zeigt auf 2. Zeichen des Dateinamens!
	jp 	z, ERROR	!kein Dateinamen angegeben!
	ld	r2, INPTR
	dec	r2, #1		!r2 zeigt auf 1. Zeichen des Dateinamens!
	lda	r2, INBFF(r2)	!r2 := Adresse des Dateinamens!
	ld	FILENAME_PTR, r2 !Adresse des Dateinamens merken!

	push	@r15, PC_SEG
	call	GET_SIGN	!rl0 := 1. Zeichen der Segmentangabe ('<')!
	call	GET_SGNR	!r2 := Segmentnummer (implizit PC_SEG)!
	ld	PC_SEG, r2	!Segmentnummer merken!

!Startmeldung an 8-Bit-Teil senden (Datenbyte: "S")!
	ldb	rl0, #'S'	!Datenbyte: 8-Bit-Teil soll Datei senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Filetyp an 8-Bit-Teil! 
	ldb	rl0, #FILETYP
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Dateinamen an 8-Bit-Teil!
	ldb	rh1, #10	!retries!
GET_OPEN1:			!Schleife: Ausgabe des Blockes mit dem
					   Dateinamen!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, FILENAME_PTR !Anfangsadr. des Dateinamens in INBFF!
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR_UDOS_INT
GET_OPEN2:			!Schleife: Ausgabe der Zeichen des Dateinamens!
	ldb	rl0 ,@r8	!Zeichen von filename!
	inc	r8, #1
	cpb	rl0, #' '	!ende filename ?!
	jr	z, GET_OP1	!ja!
	cpb	rl0, #CR	!ende filename ?!
	jr	z, GET_OP1	!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR_UDOS_INT
	jr	GET_OPEN2
GET_OP1:			
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR_UDOS_INT
	exb	rl0, rl1
	sc	#SC_TYWR_UDOS_INT	!Checksumme ausgeben!
	sc	#SC_TYRD_UDOS_INT	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, GET_DESCR	!filename richtig uebertragen
			 	 file vorhanden!
	cpb	rl3, #CAN	!Abbrechen ?!
	jp	z, GET_ERROR	!ja!
	dbjnz	rh1, GET_OPEN1	!naechster Versuch!
	jp	GET_ERROR	!Abbruch!

!UDOS-Dateidescriptor (Laenge %80 Byte) empfangen und in INBFF abspeichern!
GET_DESCR:
	ld	r9, #INBFF	!Speicheranfangsadresse fuer Dateidescriptor!
	ld	r11, #%80	!Laenge des Dateidescriptors!
GET_DESCR1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
GET_DESCR2:			!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Stand r9/r11 fuer den Fall einer!
	ld	r10, r11	!Wiederholung eines Blockes retten!
	sc	#SC_TYRD_UDOS_INT	!1. Zeichen eines Blockes!
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, GET_DES1	!nein!
	sc	#SC_TYRD_UDOS_INT	!noch 1 Zeichen im Puffer!
	jr	GET_END		!EOT empfangen --> ENDE!
GET_DES1:
	cpb	rl3, #CAN
	jr	z, GET_ERROR	!Abbruch von 8-Bit-Teil!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, GET_DESCR4	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD_UDOS_INT
	ldb	rh2, rl3
	sc	#SC_TYRD_UDOS_INT
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
GET_DESCR3:			!Scheife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD_UDOS_INT	!Daten lesen!
	test	r11
	jr	z,GET_DES2	!Descriptor bereits vollst. eingelsen -->
				 ggf. restl. Daten des Blockes ignorieren!
	ldb	@r9, rl3	!Datenbyte in Puffer abspeichern!
	inc	r9, #1
	dec	r11, #1		!Zaehler decr!
GET_DES2:
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, GET_DESCR3	!naechstes Datenbyte einlesen!
GET_DESCR4:			!Schleife: Warten auf ETX!
	sc	#SC_TYRD_UDOS_INT
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, GET_DESCR4
 
	sc	#SC_TYRD_UDOS_INT	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, GET_DES3	!Fehler!
 
	test	r11
	jr	z, GET_DESCR5	!Descriptor vollstaendig eingelesen!
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR_UDOS_INT
	jr	GET_DESCR1	!naechsten Block einlesen!
GET_DES3:
	decb	rh1, #1
	jr	z, GET_ERROR	!Abbruch!
 
	ld	r9, r8		!r9/r11 auf Wert vor der Blockuebertragung!
	ld	r11, r10	!setzen!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR_UDOS_INT
	jr	GET_DESCR2	!Wiederholung der Blockuebertragung!

GET_DESCR5:			!Test auf zulaessige Anfangsadresse!
	ld	r1, #%28	!relative Adresse der Anfangsadresse im
				 Dateidescriptor!
	ld	r5, INBFF(r1)
	exb	rh5, rl5	!r5 := Anfangsadresse fuer Dateidaten!
	ld	r0, PC_SEG
	res	r0, #15
	test	r0
	jr	nz, GET_DES4	!Segmentnummer ungleich 0!
	cp	r5, #%8000	!kleinste zulaessige Dateianfangsadresse 
				 im Segment 0!
	jr	nc, GET_DES4	!zulaessig!
	ld	r2, #TXT_ILA
	jr	GET_ERROR1	!unzulaessige Anfangsadresse!
GET_DES4:
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR_UDOS_INT

!Einlesen aller Bloecke mit den Daten der Datei! 
GET_DATEI:
	ld	r9, r5		!r9 := Anfangsadresse fuer Dateidaten!
GET_DATEI1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
GET_DATEI2:			!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	 rl1, #0	!Checksumme init!
	ld	r8, r9		!Buffer Pointer retten!
	sc	#SC_TYRD_UDOS_INT
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, GET_DAT1	!nein!
	sc	#SC_TYRD_UDOS_INT	!noch 1 Zeichen im Puffer!
	jr	GET_EPNT	!file geladen!
GET_DAT1:
	cpb	rl3, #CAN
	jr	z, GET_ERROR	!Abbruch von 8-Bit-Teil!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, GET_DATEI4	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD_UDOS_INT
	ldb	rh2, rl3
	sc	#SC_TYRD_UDOS_INT
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
GET_DATEI3:			!Scheife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD_UDOS_INT	!Daten lesen!
	push	@r15, r8
	ld	r8, PC_SEG
	sc	#SC_SEGV
	ldb	@r8, rl3	!Datenbyte in Speicher schreiben (segmentiert)!
	sc	#SC_NSEGV
	pop	r8, @r15
	inc	r9, #1
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, GET_DATEI3
GET_DATEI4:			!Schleife: Warten auf ETX!
	sc	#SC_TYRD_UDOS_INT
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, GET_DATEI4
 
	sc	#SC_TYRD_UDOS_INT	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, GET_DAT3	!Fehler!
 
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR_UDOS_INT
	jr	GET_DATEI1	!und naechsten Block einlesen!
GET_DAT3:
	decb	rh1, #1
	jr	z, GET_ERROR	!Abbruch!
 
	ld	r9, r8		!Buffer Pointer auf Anfang zurueck!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR_UDOS_INT
	jr	GET_DATEI2	!Wiederholung der Blockuebertragung!

!Ausgabe des ENTRY POINT!
GET_EPNT:
	ld	r2, #TXT_EPNT
	sc	#SC_WR_MSG	!Textausgabe!
	ld	r1, #%14	!relat. Adr. des Entry Point im Descriptor!
	ld	r5, INBFF(r1)	
	exb	rh5, rl5	!r5 := ENTRY POINT!
	sc	#SC_BTOH16
	sc	#SC_WR_OUTBFF_CR	!Ausgabe Entry Point!
	jr	GET_END

!Uebertragungsfehler! 
GET_ERROR:
	ld	r2, #TXT_ABO
GET_ERROR1:
	sc	#SC_WR_MSG	!Textausgabe!
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT

!Ende!
GET_END:
	pop	PC_SEG, @r15
	ret
    end GET

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SAVE
SAVE-Routine "8-Bit-Teil" (Monitorkommando)
Abspeichern eines Speicherinhaltes als UDOS-Procedure-Datei (8-Bit-Teil)
(ueber 8-Bit-/16-Bit-Koppelschnittstelle)

Kommandoeingabe: S filename [<segnr>]offset1 offset2 [E=Entry_point]
							[RL=Record_length]
		   (Dateiname Anfangsadresse Endadresse Eintrittspunkt
							     Recordlaenge)
		   (Segmentnummer implizit PC_SEG,
		    Eintrittspunkt implizit Anfangsadresse,
		    Recordlaenge implizit %100)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    SAVE procedure
      entry
	cpb	PTYPORT, #%02	!8-Bit-Teil aktiv und UDOS verfuegbar?!
	jp	nz, ERROR	!nein --> Kommando nicht ausfuehrbar!

	clr	COUNT_UDOS
	clr	INPTR_UDOS
	clr	OUTPTR_UDOS

!Auswertung der eingegebenen Parameter!
! PC_SEG := Anfangsadresse (Segmentnummer)
  r9     := Anfangsadresse (Offset)
  r11    := Bytezahl
  r10    := Entry Point
  r13    := Record Length!
	call	GET_SIGN	!INPTR zeigt auf 2. Zeichen des Dateinamens!
	jp 	z, ERROR	!kein Dateinamen angegeben!
	ld	r2, INPTR
	dec	r2, #1		!r2 zeigt auf 1. Zeichen des Dateinamens!
	lda	r2, INBFF(r2)	!r2 := Adresse des Dateinamens!
	ld	FILENAME_PTR, r2 !Adresse des Dateinamens merken!

	call	GET_SIGN	!rl0 := 1. Zeichen der Anfangsadresse!
	jp	z, ERROR	!keine Anfangsadresse angegeben!
	call	GET_ADR		!rr2 := angegebene Anfangsadresse / 
				 rl0 := 1. Zeichen der Endadresse!
	ldl	rr8, rr2	!rr8 := angegebene Anfangsadresse!
	jp	z, ERROR	!keine Offsetadresse oder kein weiterer
				 Parameter angegeben!

	call	GET_ADR		!r3 := angegebene Endadresse /
			   	 rl0 := 1. Zeichen des naechsten Parameters!
	ldctlb	rh0, FLAGS	!Stand Z-Flag retten!
	sub	r3,r9
	jp	c, ERROR	!Endadresse < Anfangsadresse!
	inc	r3	
	ld	r11, r3		!r11:=Bytezahl!
	ld	r10, r9		!impliziter Wert fuer Entry Point = Anf.-Adr.!
	ld	r13, #%100	!impliziter Wert fuer Record length!
	bitb	rh0, #6		!Z-Flag testen!
	jr	nz, SAVE_PAR2	!kein weiterer Parameter angegeben!

	cpb	rl0, #'E'
	jr	nz, SAVE_PAR1	!keine Entry Point-Angabe!
	call	GET_CHR
	cpb	rl0, #'='
	jp	nz, ERROR	!falsche Entry Point-Angabe!
	call	GET_SIGNW	!rl0:=1. Zeichen der Entry Point-Adresse!
	jp	z, ERROR	!keine Entry Point-Adresse angegeben (CR)!
	call	GET_HEXZ	!r3 := angegebener Entry Point,
				 rl0 := 1. Zeichen der Record Length-Angabe!
	ld	r10, r3         !r10 := angegebener Entry Point!
	jr	z, SAVE_PAR2	!keine Recordlaenge angegeben!
SAVE_PAR1:
	cpb	rl0, #'R'
	jp	nz, ERROR	!keine Record Length-Angabe!
	call	GET_CHR
	cpb	rl0, #'L'
	jp	nz, ERROR	!falsche Record Length-Angabe!
	call	GET_CHR
	cpb	rl0, #'='
	jp	nz, ERROR	!falsche Record Length-Angabe!
	call	GET_SIGNW	!rl0:=1. Zeichen der Recordlaengenzahl!
	jp	z, ERROR	!keine Recordlaengenzahl angegeben (CR)!
	call	GET_HEXZ	!r3 := angegebener Recordlaenge!
	ld	r13, r3         !r13 := angegebene Recordlaenge!
SAVE_PAR2:
	push	@r15, PC_SEG
	ld	PC_SEG, r8	!Segmentnummer merken!

!Startmeldung an 8-Bit-Teil senden (Datenbyte: "L")!
	ldb	rl0, #'L'	!Datenbyte: 8-Bit-Teil soll Datei empfangen!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Filetyp an 8-Bit-Teil! 
	ldb	rl0, #FILETYP
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
!Ausgabe Dateinamen an 8-Bit-Teil!
	ldb	rh1, #10	!retries!
SAVE_OPEN1:			!Schleife: Ausgabe des Blockes mit dem
					   Dateinamen!
	ldb	rl1, #0		!Checksumme init!
	ld	r12, FILENAME_PTR !Anfangsadr. des Dateinamens in INBFF!
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR_UDOS_INT
SAVE_OPEN2:			!Schleife: Ausgabe der Zeichen des Dateinamens!
	ldb	rl0 ,@r12	!Zeichen von filename!
	inc	r12, #1
	cpb	rl0, #' '	!ende filename ?!
	jr	z, SAVE_OP1	!ja!
	cpb	rl0, #CR	!ende filename ?!
	jr	z, SAVE_OP1	!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR_UDOS_INT
	jr	SAVE_OPEN2
SAVE_OP1:			
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR_UDOS_INT
	exb	rl0, rl1
	sc	#SC_TYWR_UDOS_INT	!Checksumme ausgeben!
	sc	#SC_TYRD_UDOS_INT	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, SAVE_DESCR	!filename richtig uebertragen!
	cpb	rl3, #CAN	!Abbrechen ?!
	jp	z, SAVE_ERROR	!ja!
	dbjnz	rh1, SAVE_OPEN1	!naechster Versuch!
	jp	SAVE_ERROR	!Abbruch!

!UDOS-Dateidescriptor in OUTBFF aufbauen!
SAVE_DESCR:
	ld	r4, #OUTBFF	!OUTBFF loeschen!
	ld	r2, #OUTBFF+2
	ld	r1, #%3F
	ld	@r4, #0
	ldir	@r2, @r4, r1
	ld	r1, #0		!Magic-Nr. im Descriptor abspeichern
				 (wird von remote-Programm benoetigt)!
	ldb	OUTBFF(r1), #%D6
	inc	r1, #1
	ldb	OUTBFF(r1), #%07
	ld	r1, #%0C	!Dateityp im Descriptor abspeichern!
	ldb	OUTBFF(r1), #%80
	ld	r1, #%0F	!Recordlaenge im Descriptor abspeichern!
	ld	r5, r13
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%14	!Entry Point im Descriptor abspeichern!
	ld	r5, r10
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%28	!Anfangsadresse im Descriptor abspeichern!
	ld	r5, r9
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%2A	!Laenge im Descriptor abspeichern!
	ld	r5, r11
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7A	!LOW_ADDRESS im Descriptor abspeichern!
	ld	r5, r9
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7C	!HIGH_ADDRESS im Descriptor abspeichern!
	ld	r5, r9
	add	r5, r11
	dec	r5, #1
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7E	!STACK_SIZE im Descriptor abspeichern!
	ldb	OUTBFF(r1), #%80
	inc	r1, #1
	ldb	OUTBFF(r1), #%00

!UDOS-Dateidescriptor (Laenge %80 Byte) aus OUTBFF senden!
	ld	r12, #OUTBFF	!Speicheranfangsadresse fuer Dateidescriptor!
	ld	r13, #%80	!Laenge des Dateidescriptors!
SAVE_DESCR1:			!Schleife: Schreiben eines Blockes!
	ldb	rh1, #10	!retries!
SAVE_DESCR2:			!Schleife: Wdhl. des Schreibens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r12		!Stand r12/r13 fuer den Fall einer!
	ld	r10, r13	!Wiederholung eines Blockes retten!
	ldb	rl0, #STX	!STX senden!
	sc	#SC_TYWR_UDOS_INT	!Start of text!
	ld	r2, #BLOCKLEN	!Blocklaenge berechnen!
	sub	r13, r2		!r13 := restliche Datenanzahl!
	jr	nc, SAVE_DES1	!Datenanzahl >= BLOCKLEN
				 --> Blocklaenge (r2) := BLOCKLEN!
	ld	r2, r10		!Datenanzahl < BLOCKLEN
				 --> Blocklaenge (r2) := Datenanzahl!
	ld	r13, #0		!restl. Datenanzahl := 0!
SAVE_DES1:
	ldb	rl0, rh2	!Blocklaenge senden!
	sc	#SC_TYWR_UDOS_INT
	ldb	rl0, rl2
	sc	#SC_TYWR_UDOS_INT
SAVE_DESCR3:			!Scheife: Ausgabe aller Daten eines Blockes!
	ldb	rl0, @r12
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR_UDOS_INT	!Datenbyte senden!
	inc	r12, #1
	djnz	r2, SAVE_DESCR3	!naechstes Datenbyte senden!
	ldb	rl0, #ETX	!ETX senden!
	sc	#SC_TYWR_UDOS_INT
	ldb	rl0, rl1	!Checksumme senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
	cpb	rl3, #ACK
	jr	nz, SAVE_DES3
	test	r13
	jr	z, SAVE_DATEI	!Descriptor vollst. ausgegeben!
	jr	SAVE_DESCR1	!naechsten Block senden!
SAVE_DES3:
	cpb	rl3, #CAN
	jr	z, SAVE_ERROR	!Abbruch!
	decb	rh1, #1
	jr	z, SAVE_ERROR	!Wiederholungszaehler=0-->Abbruch!
	ld	r12, r8		!r12/r13 auf Wert vor der Blockuebertragung!
	ld	r13, r10	!setzen!
	jr	SAVE_DESCR2	!Wiederholung der Blockuebertragung!

!alle Bloecke mit den Daten der Datei senden!
SAVE_DATEI:
	!r9 enthaelt Speicheranfangsadresse fuer Dateidaten!
	!r11 enthaelt Bytezahl!
SAVE_DATEI1:			!Schleife: Schreiben eines Blockes!
	ldb	rh1, #10	!retries!
SAVE_DATEI2:			!Schleife: Wdhl. des Schreibens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Stand r9/r11 fuer den Fall einer!
	ld	r10, r11	!Wiederholung eines Blockes retten!
	ldb	rl0, #STX	!STX senden!
	sc	#SC_TYWR_UDOS_INT	!Start of text!
	ld	r2, #BLOCKLEN	!Blocklaenge berechnen!
	sub	r11, r2		!r11 := restliche Datenanzahl!
	jr	nc, SAVE_DAT1	!Datenanzahl >= BLOCKLEN
				 --> Blocklaenge (r2) := BLOCKLEN!
	ld	r2, r10		!Datenanzahl < BLOCKLEN
				 --> Blocklaenge (r2) := Datenanzahl!
	ld	r11, #0		!restl. Datenanzahl := 0!
SAVE_DAT1:
	ldb	rl0, rh2	!Blocklaenge senden!
	sc	#SC_TYWR_UDOS_INT
	ldb	rl0, rl2
	sc	#SC_TYWR_UDOS_INT
SAVE_DATEI3:			!Scheife: Ausgabe aller Daten eines Blockes!
	push	@r15, r8
	ld	r8, PC_SEG
	sc	#SC_SEGV
	ldb	rl0, @r8	!Datenbyte aus Speicher lesen (segmentiert)!
	sc	#SC_NSEGV
	pop	r8, @r15
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR_UDOS_INT	!Datenbyte senden!
	inc	r9, #1
	djnz	r2, SAVE_DATEI3	!naechstes Datenbyte senden!
	ldb	rl0, #ETX	!ETX senden!
	sc	#SC_TYWR_UDOS_INT
	ldb	rl0, rl1	!Checksumme senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
	cpb	rl3, #ACK
	jr	nz, SAVE_DAT3
	test	r11
	jr	z, SAVE_END	!Datei vollst. ausgegeben!
	jr	SAVE_DATEI1	!naechsten Block senden!
SAVE_DAT3:
	cpb	rl3, #CAN
	jr	z, SAVE_ERROR	!Abbruch!
	decb	rh1, #1
	jr	z, SAVE_ERROR	!Wiederholungszaehler=0-->Abbruch!
	ld	r9, r8		!r9/r11 auf Wert vor der Blockuebertragung!
	ld	r11, r10	!setzen!
	jr	SAVE_DATEI2	!Wiederholung der Blockuebertragung!

!Uebertragungsfehler! 
SAVE_ERROR:
	ld	r2, #TXT_ABO
	sc	#SC_WR_MSG	!Textausgabe!

!Ende!
SAVE_END:
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR_UDOS_INT
	sc	#SC_TYRD_UDOS_INT
	pop	PC_SEG, @r15
	ret
    end SAVE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE QUIT8
QUIT-Routine "8-Bit-Teil" (Monitorkommando)
Rueckkehr in 8-Bit-Teil, wenn U8000-Softwaremonitor von dort gestartet wurde -
U8000-Softwaremonitor bleibt aktiv (local Mode) (kein RESET des 16-Bit-Teils)
(Rueckkehr in das System, von dem aus der U8000-Softwaremonitor gestartet wurde
(Betriebssystem UDOS bzw. U880-Softwaremonitor))

Kommandoeingabe: Q
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    QUIT8 procedure
      entry
	testb	PTYPORT		!8-Bit-Teil aktiv?!
	jp	z, ERROR	!nein, d.h. Kommando nicht ausfuehrbar!

	ldb	rl0, #'Q'	!Datenbyte: QUIT ohne RESET U8000!
	ldb	rh0, #STAT8_QUIT !Status: Rueckkehr in 8-Bit-Teil!
	call	TYWR_KOPPEL_INT	!Datenbyte+Status an Koppelschnittstelle
				 ausgeben!
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
	ret
    end QUIT8

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE QUIT8_RESET
QUIT-Routine "8-Bit-Teil" (Monitorkommando)
Rueckkehr in 8-Bit-Teil, wenn U8000-Softwaremonitor von dort gestartet wurde -
RESET des 16-Bit-Teils
(Rueckkehr in das System, von dem aus der U8000-Softwaremonitor gestartet wurde
(Betriebssystem UDOS bzw. U880-Softwaremonitor))

Kommandoeingabe: QRES
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    QUIT8_RESET procedure
      entry
	testb	PTYPORT		!8-Bit-Teil aktiv?!
	jp	z, ERROR	!nein, d.h. Kommando nicht ausfuehrbar!

	ldb	rl0, #'R'	!Datenbyte: QUIT mit RESET U8000!
	ldb	rh0, #STAT8_QUIT !Status: Rueckkehr in 8-Bit-Teil!
	call	TYWR_KOPPEL_INT	!Datenbyte+Status an Koppelschnittstelle
				 ausgeben!
	sc	#SC_TYRD_UDOS_INT	!acknowledge!
	ret			!danach erfolgt RESET U8000!
    end QUIT8_RESET

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE FLOPPY
Treiber zur Arbeit mit einer blockorientierten Diskette auf 8-Bit-Teil -
Lesen/Schreiben eines Blockes bzw. Formatieren einer blockorientierten Diskette
(ueber 8-Bit/16-Bit Koppelschnittstelle)

Input:	r7  - Requestcode (1=Read / 2=Write / 3=Format)
	r6  - Bit 0-3: Drivenummer (0-3)
	      Bit 4-7: Floppytyp (0-F)
	rr4 - Blocknummer (r4=0)
	rr8 - Pufferadresse
	r10 - Laenge in Byte (<= %200)
Output: r2  - Return Code
	      0 - kein Fehler
	      1 - Fehler (8-Bit-Teil nicht aktiv bzw.
		          8-Bit-Teil aktiv, aber UDOS nicht verfuegbar)
	      2 - Fehler (falscher Requestcode)
	      sonst - Fehler vom 8-Bit-Teil gesendet
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    FLOPPY procedure
      entry
	cpb	PTYPORT, #%02	!8-Bit-Teil aktiv und UDOS verfuegbar?!
	ld	r2, #1		!Fehler 0001: 8-Bit-Teil nicht aktiv bzw.
				 8-Bit-Teil aktiv, aber UDOS nicht verfuegbar!
	ret	nz	

	clr	COUNT_UDOS
	clr	INPTR_UDOS
	clr	OUTPTR_UDOS

!Auswertung des angegebenen Requests!
	cp	r7, #1
	jr	z, BLOCK_READ
	cp	r7, #2
	jr	z, BLOCK_WRITE
	cp	r7, #3
	jr	z, BLOCK_FORMAT
	ld	r2, #2		!Fehler 0002: falscher Requestcode!
	ret

!Formatieren einer blockorientierten Diskette!
BLOCK_FORMAT:
	!Startmeldung an 8-Bit-Teil senden (Datenbyte 'F')!
	ldb	rl0, #'F'
	sc	#SC_TYWR_UDOS_INT
	!Ausgabe Floppytyp/Drive-Nr. an 8-Bit-Teil!
	ldb	rl0, rl6
	sc	#SC_TYWR_UDOS_INT
	!Warten auf Fertigstellung!
	sc	#SC_TYRD_UDOS_INT
	cpb	rl3, #ACK
	ld	r2, #0		!Rueckkehrcode 0000: kein Fehler!
	ret	z
	ldb 	rl2, rl3	!Fehlercode vom 8-Bit-Teil!
	ret

!Lesen eines Blockes von einer blockorientierten Diskette!
BLOCK_READ:
	!Startmeldung an 8-Bit-Teil senden (Datenbyte 'R')!
	ldb	rl0, #'R'
	sc	#SC_TYWR_UDOS_INT
	!Ausgabe Floppytyp/Drive-Nr. an 8-Bit-Teil!
	ldb	rl0, rl6
	call	TYWR_UDOS_POL
	!Ausgabe Blocknummer an 8-Bit-Teil!
	ldb	rl0, rh5	!High-Byte!
	call	TYWR_UDOS_POL
	ldb	rl0, rl5	!Low-Byte!
	call	TYWR_UDOS_POL
	!Ausgabe der Datenlaenge in Byte (max. %200)!
	ld	r0, r10
	ldb	rl0, rh0	!High-Byte!
	call	TYWR_UDOS_POL
	ld	r0, r10		!Low-Byte!
	call	TYWR_UDOS_POL
	!Warten auf Bereitmeldung von 8-Bit-Teil
	 (erfolgt, wenn Block von Diskette gelesen wurde)!
	sc	#SC_TYRD_UDOS_INT
	cpb	rl3, #ACK
	ldb	rl2, rl3	!Fehlercode vom 8-Bit-Teil!
	ldb	rh2, #0
	ret	nz
	!Block einlesen!
BLK_RD:
	call	TYRD_UDOS_POL	!rl0:=eingelesenens Datenbyte!
	sc	#SC_SEGV
	ldb	@r8, rl0	!Datenbyte im Puffer abspeichern!
	sc	#SC_NSEGV
	inc	r9, #1		!Pufferadresse inc!
	djnz	r10, BLK_RD	!Byteanzahl dec!
	!Fertigstellungsmeldung senden!
	ldb	rl0, #ACK
	call	TYWR_UDOS_POL
	ld	r2, #0		!Rueckkehrcode 0000: kein Fehler!
	ret

!Schreiben eines Blockes auf eine blockorientierte Diskette!
BLOCK_WRITE:
	!Startmeldung an 8-Bit-Teil senden (Datenbyte 'W')!
	ldb	rl0, #'W'
	sc	#SC_TYWR_UDOS_INT
	!Ausgabe Floppytyp/Drive-Nr. an 8-Bit-Teil!
	ldb	rl0, rl6
	call	TYWR_UDOS_POL
	!Ausgabe Blocknummer an 8-Bit-Teil!
	ldb	rl0, rh5	!High-Byte!
	call	TYWR_UDOS_POL
	ldb	rl0, rl5	!Low-Byte!
	call	TYWR_UDOS_POL
	!Ausgabe der Datenlaenge in Byte (max. %200)!
	ld	r0, r10
	ldb	rl0, rh0	!High-Byte!
	call	TYWR_UDOS_POL
	ld	r0, r10		!Low-Byte!
	call	TYWR_UDOS_POL
	!Block ausgeben!
BLK_WR:
	sc	#SC_SEGV
	ldb	rl0, @r8	!Datenbyte aus Puffer lesen!
	sc	#SC_NSEGV
	call	TYWR_UDOS_POL	!Datenbyte ausgeben!
	inc	r9, #1		!Pufferadr. inc!
	djnz	r10, BLK_WR	!Bytezahl dec!
	!Warten auf Fertigstellungsmeldung von 8-Bit-Teil
     	 (erfolgt, wenn Block auf Diskette geschrieben wurde)!
	sc	#SC_TYRD_UDOS_INT
	cpb	rl3, #ACK
	ld	r2, #0		!Rueckkehrcode 0000: kein Fehler!
	ret	z
	ldb 	rl2, rl3	!Fehlercode vom 8-Bit-Teil!
	ret
    end FLOPPY

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RM_GET
GET-Routine "Lokales System" (Monitorkommando)
Laden einer UDOS-Procedure-Datei (von lokalem System) in RAM
(ueber "remote"-Programm des lokalen Systems)

Kommandoeingabe: RGE filename [<segnr>]
		     (Dateiname Segmentnummer(implizit PC_SEG))
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RM_GET procedure
      entry
	setb	FLAG0, #0	!Abschalten von xoff!

!Auswertung der eingegebenen Parameter!
	call	GET_SIGN	!INPTR zeigt auf 2. Zeichen des Dateinamens!
	jp 	z, ERROR	!kein Dateinamen angegeben!
	ld	r2, INPTR
	dec	r2, #1		!r2 zeigt auf 1. Zeichen des Dateinamens!
	lda	r2, INBFF(r2)	!r2 := Adresse des Dateinamens!
	ld	FILENAME_PTR, r2 !Adresse des Dateinamens merken!

	push	@r15, PC_SEG
	call	GET_SIGN	!rl0 := 1. Zeichen der Segmentangabe ('<')!
	call	GET_SGNR	!r2 := Segmentnummer (implizit PC_SEG)!
	ld	PC_SEG, r2	!Segmentnummer merken!

!Startmeldung (SOH, ESC, 'S') an local senden!
	ldb	rl0, #SOH
	sc	#SC_TYWR
	ldb	rl0, #ESC
	sc	#SC_TYWR
	ldb	rl0, #'S'
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
!Ausgabe Filetyp an local!
	ldb	rl0, #FILETYP
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
!Ausgabe Dateinamen an local!
	ldb	rh1, #10	!retries!
RGET_OPEN1:			!Schleife: Ausgabe des Blockes mit dem
					   Dateinamen!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, FILENAME_PTR !Anfangsadr. des Dateinamens in INBFF!
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR
RGET_OPEN2:			!Schleife: Ausgabe der Zeichen des Dateinamens!
	ldb	rl0 ,@r8	!Zeichen von filename!
	inc	r8, #1
	cpb	rl0, #' '	!ende filename ?!
	jr	z, RGET_OP1	!ja!
	cpb	rl0, #CR	!ende filename ?!
	jr	z, RGET_OP1	!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR
	jr	RGET_OPEN2
RGET_OP1:			
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR
	exb	rl0, rl1
	sc	#SC_TYWR	!Checksumme ausgeben!
	sc	#SC_TYRD	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, RGET_DESCR	!filename richtig uebertragen
			 	 file vorhanden!
	cpb	rl3, #CAN	!Abbrechen ?!
	jp	z, RGET_ERROR	!ja!
	dbjnz	rh1, RGET_OPEN1	!naechster Versuch!
	jp	RGET_ERROR	!Abbruch!

!UDOS-Dateidescriptor (Laenge %80 Byte) empfangen und in INBFF abspeichern!
RGET_DESCR:
	ld	r9, #INBFF	!Speicheranfangsadresse fuer Dateidescriptor!
	ld	r11, #%80	!Laenge des Dateidescriptors!
RGET_DESCR1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
RGET_DESCR2:			!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Stand r9/r11 fuer den Fall einer!
	ld	r10, r11	!Wiederholung eines Blockes retten!
	sc	#SC_TYRD	!1. Zeichen eines Blockes!
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, RGET_DES1	!nein!
	sc	#SC_TYRD	!noch 1 Zeichen im Puffer!
	jr	RGET_END		!EOT empfangen --> ENDE!
RGET_DES1:
	cpb	rl3, #CAN
	jr	z, RGET_ERROR	!Abbruch von 8-Bit-Teil!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, RGET_DESCR4	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD
	ldb	rh2, rl3
	sc	#SC_TYRD
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
RGET_DESCR3:			!Scheife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD	!Daten lesen!
	test	r11
	jr	z,RGET_DES2	!Descriptor bereits vollst. eingelsen -->
				 ggf. restl. Daten des Blockes ignorieren!
	ldb	@r9, rl3	!Datenbyte in Puffer abspeichern!
	inc	r9, #1
	dec	r11, #1		!Zaehler decr!
RGET_DES2:
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, RGET_DESCR3	!naechstes Datenbyte einlesen!
RGET_DESCR4:			!Schleife: Warten auf ETX!
	sc	#SC_TYRD
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, RGET_DESCR4
 
	sc	#SC_TYRD	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, RGET_DES3	!Fehler!
 
	test	r11
	jr	z, RGET_DESCR5	!Descriptor vollstaendig eingelesen!
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR
	jr	RGET_DESCR1	!naechsten Block einlesen!
RGET_DES3:
	decb	rh1, #1
	jr	z, RGET_ERROR	!Abbruch!
 
	ld	r9, r8		!r9/r11 auf Wert vor der Blockuebertragung!
	ld	r11, r10	!setzen!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR
	jr	RGET_DESCR2	!Wiederholung der Blockuebertragung!

RGET_DESCR5:			!Test auf zulaessige Anfangsadresse!
	ld	r1, #%28	!relative Adresse der Anfangsadresse im
				 Dateidescriptor!
	ld	r5, INBFF(r1)
	exb	rh5, rl5	!r5 := Anfangsadresse fuer Dateidaten!
	ld	r0, PC_SEG
	res	r0, #15
	test	r0
	jr	nz, RGET_DES4	!Segmentnummer ungleich 0!
	cp	r5, #%8000	!kleinste zulaessige Dateianfangsadresse 
				 im Segment 0!
	jr	nc, RGET_DES4	!zulaessig!
	ld	r2, #TXT_ILA
	jr	RGET_ERROR1	!unzulaessige Anfangsadresse!
RGET_DES4:
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR

!Einlesen aller Bloecke mit den Daten der Datei! 
RGET_DATEI:
	ld	r9, r5		!r9 := Anfangsadresse fuer Dateidaten!
RGET_DATEI1:			!Schleife: Lesen eines Blockes!
	ldb	rh1, #10	!retries!
RGET_DATEI2:			!Schleife: Wdhl. des Lesens eines Blockes!
	ldb	 rl1, #0	!Checksumme init!
	ld	r8, r9		!Buffer Pointer retten!
	sc	#SC_TYRD
	resb	rl3, #7
	cpb	rl3, #EOT	!Ende der Uebertragung?!
	jr	nz, RGET_DAT1	!nein!
	sc	#SC_TYRD	!noch 1 Zeichen im Puffer!
	jr	RGET_EPNT	!file geladen!
RGET_DAT1:
	cpb	rl3, #CAN
	jr	z, RGET_ERROR	!Abbruch von 8-Bit-Teil!
	cpb	rl3, #STX	!Start of text muss 1.Zeichen sein!
	jr	nz, RGET_DATEI4	!warten auf ETX und Uebertragung wiederholen!
	sc	#SC_TYRD
	ldb	rh2, rl3
	sc	#SC_TYRD
	ldb	rl2, rl3	!r2 := Laenge des Blockes!
RGET_DATEI3:			!Scheife: Eingabe aller Daten eines Blockes!
	sc	#SC_TYRD	!Daten lesen!
	push	@r15, r8
	ld	r8, PC_SEG
	sc	#SC_SEGV
	ldb	@r8, rl3	!Datenbyte in Speicher schreiben (segmentiert)!
	sc	#SC_NSEGV
	pop	r8, @r15
	inc	r9, #1
	xorb	rl1, rl3	!Checksumme berechnen!
	djnz	r2, RGET_DATEI3
RGET_DATEI4:			!Schleife: Warten auf ETX!
	sc	#SC_TYRD
	resb	rl3, #7
	cpb	rl3, #ETX
	jr	nz, RGET_DATEI4
 
	sc	#SC_TYRD	!Checksumme!
	cpb	rl3, rl1	!uebertragene = berechnete ?!
	jr	nz, RGET_DAT3	!Fehler!
 
	ldb	rl0, #ACK	!richtig uebertragen Qittung ok geben!
	sc	#SC_TYWR
	jr	RGET_DATEI1	!und naechsten Block einlesen!
RGET_DAT3:
	decb	rh1, #1
	jr	z, RGET_ERROR	!Abbruch!
 
	ld	r9, r8		!Buffer Pointer auf Anfang zurueck!
	ldb	rl0, #NAK	!n mal wiederholen!
	sc	#SC_TYWR
	jr	RGET_DATEI2	!Wiederholung der Blockuebertragung!

!Ausgabe des ENTRY POINT!
RGET_EPNT:
	ld	r2, #TXT_EPNT
	sc	#SC_WR_MSG	!Textausgabe!
	ld	r1, #%14	!relat. Adr. des Entry Point im Descriptor!
	ld	r5, INBFF(r1)	
	exb	rh5, rl5	!r5 := ENTRY POINT!
	sc	#SC_BTOH16
	sc	#SC_WR_OUTBFF_CR	!Ausgabe Entry Point!
	jr	RGET_END

!Uebertragungsfehler! 
RGET_ERROR:
	ld	r2, #TXT_ABO
RGET_ERROR1:
	sc	#SC_WR_MSG	!Textausgabe!
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR
	sc	#SC_TYRD

!Ende!
RGET_END:
	resb	FLAG0, #0
	pop	PC_SEG, @r15
	ret
    end RM_GET

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RM_SAVE
SAVE-Routine "Lokales System" (Monitorkommando)
Abspeichern eines Speicherinhaltes als UDOS-Procedure-Datei (lokales System)
(ueber "remote"-Programm des lokalen Systems)

Kommandoeingabe: RS filename [<segnr>]offset1 offset2 [E=Entry_point]
							[RL=Record_length]
		    (Dateiname Anfangsadresse Endadresse Eintrittspunkt
							     Recordlaenge)
		    (Segmentnummer implizit PC_SEG,
		     Eintrittspunkt implizit Anfangsadresse,
		     Recordlaenge implizit %100)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RM_SAVE procedure
      entry
	setb	FLAG0, #0	!Abschalten von xoff!

!Auswertung der eingegebenen Parameter!
! PC_SEG := Anfangsadresse (Segmentnummer)
  r9     := Anfangsadresse (Offset)
  r11    := Bytezahl
  r10    := Entry Point
  r13    := Record Length!
	call	GET_SIGN	!INPTR zeigt auf 2. Zeichen des Dateinamens!
	jp 	z, ERROR	!kein Dateinamen angegeben!
	ld	r2, INPTR
	dec	r2, #1		!r2 zeigt auf 1. Zeichen des Dateinamens!
	lda	r2, INBFF(r2)	!r2 := Adresse des Dateinamens!
	ld	FILENAME_PTR, r2 !Adresse des Dateinamens merken!

	call	GET_SIGN	!rl0 := 1. Zeichen der Anfangsadresse!
	jp	z, ERROR	!keine Anfangsadresse angegeben!
	call	GET_ADR		!rr2 := angegebene Anfangsadresse / 
				 rl0 := 1. Zeichen der Endadresse!
	ldl	rr8, rr2	!rr8 := angegebene Anfangsadresse!
	jp	z, ERROR	!keine Offsetadresse oder kein weiterer
				 Parameter angegeben!

	call	GET_ADR		!r3 := angegebene Endadresse /
			   	 rl0 := 1. Zeichen des naechsten Parameters!
	ldctlb	rh0, FLAGS	!Stand Z-Flag retten!
	sub	r3,r9
	jp	c, ERROR	!Endadresse < Anfangsadresse!
	inc	r3	
	ld	r11, r3		!r11:=Bytezahl!
	ld	r10, r9		!impliziter Wert fuer Entry Point = Anf.-Adr.!
	ld	r13, #%100	!impliziter Wert fuer Record length!
	bitb	rh0, #6		!Z-Flag testen!
	jr	nz, RSAVE_PAR2	!kein weiterer Parameter angegeben!

	cpb	rl0, #'E'
	jr	nz, RSAVE_PAR1	!keine Entry Point-Angabe!
	call	GET_CHR
	cpb	rl0, #'='
	jp	nz, ERROR	!falsche Entry Point-Angabe!
	call	GET_SIGNW	!rl0:=1. Zeichen der Entry Point-Adresse!
	jp	z, ERROR	!keine Entry Point-Adresse angegeben (CR)!
	call	GET_HEXZ	!r3 := angegebener Entry Point,
				 rl0 := 1. Zeichen der Record Length-Angabe!
	ld	r10, r3         !r10 := angegebener Entry Point!
	jr	z, RSAVE_PAR2	!keine Recordlaenge angegeben!
RSAVE_PAR1:
	cpb	rl0, #'R'
	jp	nz, ERROR	!keine Record Length-Angabe!
	call	GET_CHR
	cpb	rl0, #'L'
	jp	nz, ERROR	!falsche Record Length-Angabe!
	call	GET_CHR
	cpb	rl0, #'='
	jp	nz, ERROR	!falsche Record Length-Angabe!
	call	GET_SIGNW	!rl0:=1. Zeichen der Recordlaengenzahl!
	jp	z, ERROR	!keine Recordlaengenzahl angegeben (CR)!
	call	GET_HEXZ	!r3 := angegebener Recordlaenge!
	ld	r13, r3         !r13 := angegebene Recordlaenge!
RSAVE_PAR2:
	push	@r15, PC_SEG
	ld	PC_SEG, r8	!Segmentnummer merken!

!Startmeldung (SOH, ESC, 'L') an local senden!
	ldb	rl0, #SOH
	sc	#SC_TYWR
	ldb	rl0, #ESC
	sc	#SC_TYWR
	ldb	rl0, #'L'
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
!Ausgabe Filetyp an local! 
	ldb	rl0, #FILETYP
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
!Ausgabe Dateinamen an local!
	ldb	rh1, #10	!retries!
RSAVE_OPEN1:			!Schleife: Ausgabe des Blockes mit dem
					   Dateinamen!
	ldb	rl1, #0		!Checksumme init!
	ld	r12, FILENAME_PTR !Anfangsadr. des Dateinamens in INBFF!
	ldb	rl0, #STX	!Start of Text!
	sc	#SC_TYWR
RSAVE_OPEN2:			!Schleife: Ausgabe der Zeichen des Dateinamens!
	ldb	rl0 ,@r12	!Zeichen von filename!
	inc	r12, #1
	cpb	rl0, #' '	!ende filename ?!
	jr	z, RSAVE_OP1	!ja!
	cpb	rl0, #CR	!ende filename ?!
	jr	z, RSAVE_OP1	!ja!
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR
	jr	RSAVE_OPEN2
RSAVE_OP1:			
	ldb	rl0, #ETX	!end of text!
	sc	#SC_TYWR
	exb	rl0, rl1
	sc	#SC_TYWR	!Checksumme ausgeben!
	sc	#SC_TYRD	!Warten auf Quittung!
	resb	rl3, #7
	cpb	rl3, #ACK
	jr	z, RSAVE_DESCR	!filename richtig uebertragen!
	cpb	rl3, #CAN	!Abbrechen ?!
	jp	z, RSAVE_ERROR	!ja!
	dbjnz	rh1, RSAVE_OPEN1	!naechster Versuch!
	jp	RSAVE_ERROR	!Abbruch!

!UDOS-Dateidescriptor in OUTBFF aufbauen!
RSAVE_DESCR:
	ld	r4, #OUTBFF	!OUTBFF loeschen!
	ld	r2, #OUTBFF+2
	ld	r1, #%3F
	ld	@r4, #0
	ldir	@r2, @r4, r1
	ld	r1, #0		!Magic-Nr. im Descriptor abspeichern
				 (wird von remote-Programm benoetigt)!
	ldb	OUTBFF(r1), #%D6
	inc	r1, #1
	ldb	OUTBFF(r1), #%07
	ld	r1, #%0C	!Dateityp im Descriptor abspeichern!
	ldb	OUTBFF(r1), #%80
	ld	r1, #%0F	!Recordlaenge im Descriptor abspeichern!
	ld	r5, r13
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%14	!Entry Point im Descriptor abspeichern!
	ld	r5, r10
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%28	!Anfangsadresse im Descriptor abspeichern!
	ld	r5, r9
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%2A	!Laenge im Descriptor abspeichern!
	ld	r5, r11
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7A	!LOW_ADDRESS im Descriptor abspeichern!
	ld	r5, r9
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7C	!HIGH_ADDRESS im Descriptor abspeichern!
	ld	r5, r9
	add	r5, r11
	dec	r5, #1
	ldb	OUTBFF(r1), rl5
	inc	r1, #1
	ldb	OUTBFF(r1), rh5
	ld	r1, #%7E	!STACK_SIZE im Descriptor abspeichern!
	ldb	OUTBFF(r1), #%80
	inc	r1, #1
	ldb	OUTBFF(r1), #%00

!UDOS-Dateidescriptor (Laenge %80 Byte) aus OUTBFF senden!
	ld	r12, #OUTBFF	!Speicheranfangsadresse fuer Dateidescriptor!
	ld	r13, #%80	!Laenge des Dateidescriptors!
RSAVE_DESCR1:			!Schleife: Schreiben eines Blockes!
	ldb	rh1, #10	!retries!
RSAVE_DESCR2:			!Schleife: Wdhl. des Schreibens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r12		!Stand r12/r13 fuer den Fall einer!
	ld	r10, r13	!Wiederholung eines Blockes retten!
	ldb	rl0, #STX	!STX senden!
	sc	#SC_TYWR	!Start of text!
	ld	r2, #BLOCKLEN	!Blocklaenge berechnen!
	sub	r13, r2		!r13 := restliche Datenanzahl!
	jr	nc, RSAVE_DES1	!Datenanzahl >= BLOCKLEN
				 --> Blocklaenge (r2) := BLOCKLEN!
	ld	r2, r10		!Datenanzahl < BLOCKLEN
				 --> Blocklaenge (r2) := Datenanzahl!
	ld	r13, #0		!restl. Datenanzahl := 0!
RSAVE_DES1:
	ldb	rl0, rh2	!Blocklaenge senden!
	sc	#SC_TYWR
	ldb	rl0, rl2
	sc	#SC_TYWR
RSAVE_DESCR3:			!Scheife: Ausgabe aller Daten eines Blockes!
	ldb	rl0, @r12
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR	!Datenbyte senden!
	inc	r12, #1
	djnz	r2, RSAVE_DESCR3	!naechstes Datenbyte senden!
	ldb	rl0, #ETX	!ETX senden!
	sc	#SC_TYWR
	ldb	rl0, rl1	!Checksumme senden!
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
	cpb	rl3, #ACK
	jr	nz, RSAVE_DES3
	test	r13
	jr	z, RSAVE_DATEI	!Descriptor vollst. ausgegeben!
	jr	RSAVE_DESCR1	!naechsten Block senden!
RSAVE_DES3:
	cpb	rl3, #CAN
	jr	z, RSAVE_ERROR	!Abbruch!
	decb	rh1, #1
	jr	z, RSAVE_ERROR	!Wiederholungszaehler=0-->Abbruch!
	ld	r12, r8		!r12/r13 auf Wert vor der Blockuebertragung!
	ld	r13, r10	!setzen!
	jr	RSAVE_DESCR2	!Wiederholung der Blockuebertragung!

!alle Bloecke mit den Daten der Datei senden!
RSAVE_DATEI:
	!r9 enthaelt Speicheranfangsadresse fuer Dateidaten!
	!r11 enthaelt Bytezahl!
RSAVE_DATEI1:			!Schleife: Schreiben eines Blockes!
	ldb	rh1, #10	!retries!
RSAVE_DATEI2:			!Schleife: Wdhl. des Schreibens eines Blockes!
	ldb	rl1, #0		!Checksumme init!
	ld	r8, r9		!Stand r9/r11 fuer den Fall einer!
	ld	r10, r11	!Wiederholung eines Blockes retten!
	ldb	rl0, #STX	!STX senden!
	sc	#SC_TYWR	!Start of text!
	ld	r2, #BLOCKLEN	!Blocklaenge berechnen!
	sub	r11, r2		!r11 := restliche Datenanzahl!
	jr	nc, RSAVE_DAT1	!Datenanzahl >= BLOCKLEN
				 --> Blocklaenge (r2) := BLOCKLEN!
	ld	r2, r10		!Datenanzahl < BLOCKLEN
				 --> Blocklaenge (r2) := Datenanzahl!
	ld	r11, #0		!restl. Datenanzahl := 0!
RSAVE_DAT1:
	ldb	rl0, rh2	!Blocklaenge senden!
	sc	#SC_TYWR
	ldb	rl0, rl2
	sc	#SC_TYWR
RSAVE_DATEI3:			!Scheife: Ausgabe aller Daten eines Blockes!
	push	@r15, r8
	ld	r8, PC_SEG
	sc	#SC_SEGV
	ldb	rl0, @r8	!Datenbyte aus Speicher lesen (segmentiert)!
	sc	#SC_NSEGV
	pop	r8, @r15
	xorb	rl1, rl0	!Checksumme berechnen!
	sc	#SC_TYWR	!Datenbyte senden!
	inc	r9, #1
	djnz	r2, RSAVE_DATEI3	!naechstes Datenbyte senden!
	ldb	rl0, #ETX	!ETX senden!
	sc	#SC_TYWR
	ldb	rl0, rl1	!Checksumme senden!
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
	cpb	rl3, #ACK
	jr	nz, RSAVE_DAT3
	test	r11
	jr	z, RSAVE_END	!Datei vollst. ausgegeben!
	jr	RSAVE_DATEI1	!naechsten Block senden!
RSAVE_DAT3:
	cpb	rl3, #CAN
	jr	z, RSAVE_ERROR	!Abbruch!
	decb	rh1, #1
	jr	z, RSAVE_ERROR	!Wiederholungszaehler=0-->Abbruch!
	ld	r9, r8		!r9/r11 auf Wert vor der Blockuebertragung!
	ld	r11, r10	!setzen!
	jr	RSAVE_DATEI2	!Wiederholung der Blockuebertragung!

!Uebertragungsfehler! 
RSAVE_ERROR:
	ld	r2, #TXT_ABO
	sc	#SC_WR_MSG	!Textausgabe!

!Ende!
RSAVE_END:
	ldb	rl0, #CAN	!Abbruch senden!
	sc	#SC_TYWR
	sc	#SC_TYRD
	resb	FLAG0, #0
	pop	PC_SEG, @r15
	ret
    end RM_SAVE

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE RM_QUIT
QUIT-Routine "Lokales System" (Monitorkommando)
Rueckkehr in lokales System (local Mode) -
U8000-Softwaremonitor (16-Bit-Teil) bleibt aktiv

Kommandoeingabe: RQ
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    RM_QUIT procedure
      entry
!Quit-Meldung (SOH, ESC, 'Q') an local senden!
	ldb	rl0, #SOH
	sc	#SC_TYWR
	ldb	rl0, #ESC
	sc	#SC_TYWR
	ldb	rl0, #'Q'
	sc	#SC_TYWR
	sc	#SC_TYRD	!acknowledge!
	ret
    end RM_QUIT

end p_gesa

