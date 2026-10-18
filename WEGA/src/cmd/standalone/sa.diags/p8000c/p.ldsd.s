!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1989
  U8000-Softwaremonitor (MON16)        Modul: p_ldsd

  Bearbeiter:	J. Zabel
  Datum:	04.05.1989
  Version:	3.1

*******************************************************************************
******************************************************************************!

!==============================================================================
Inhaltsuebersicht Modul p_ldsd
------------------------------
Dieser Modul umfasst die Monitorroutinen fuer die Arbeit mit dem
LOAD/SEND-System (Kopplung mit MCZ bzw. anderen Systemen, auf dem LOAD/SEND
zur Verfuegung steht (z.B. P8000-UDOS oder P8000-WEGA)) - (LOAD,SEND,QUIT).

Als serieller Kanal (SI) wird der SIO0_PortA benutzt, d.h. dieser Kanal ist
mit dem Terminalein-/-ausgang des MCZ zu verbinden. Auf der MCZ-Systemdiskette
muessen die Kommandos "LOAD" und "SEND" verfuegbar sein oder es muss auf dem
MCZ ein entsprechendes Programm laufen, das LOAD und SEND realisiert.

I----------I             I-------------I                 I-------------I
I          I             I             I                 I MCZ/RIO     I
I Terminal I -------- (T)I P8000/MON16 I(SI) -------- (T)I P8000/UDOS  I
I          I             I             I                 I P8000/WEGA  I
I----------I             I-------------I                 I-------------I

(T) - Terminalein-/-ausgang

Prozeduren
----------
QUIT

LOAD
MCZ_INT
MCZ_ERR
INCPTR_MCZ
TYWR_MCZ
WR_OUTBFF_CR_MCZ
WR_OUTBFF_MCZ
IN_POI_IB
EKBL
EKTB_PST
EKBY_PSB
LDB_MEM
EKLA
SAW_MCZ
WOEOI_MCZ
WOEOS_MCZ
WOCF_MCZ
WOCF_MCZ_OI
F_IB_MCZ
O_FIB_MCZ
S_RPB
S_LAB
S_SNB

SEND
MIOB_CON
==============================================================================!


p_ldsd module

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
Prozeduren
******************************************************************************!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE QUIT
QUIT-Routine "MCZ" (Monitorkommando)
Uebergang vom Monitor-Mode in Transparent-Mode, d.h. in das Betriebssystem des
MCZ (P8000-Terminal arbeitet dann als Terminal fuer dss MCZ)

Kommandoeingabe: QUIT
- im Eingabepuffer PTYBFF (Terminal) eingehende Zeichen werden an das MCZ
  gesendet
- vom MCZ eingehende Zeichen werden auf dem Terminal ausgegeben
Verlassen der Routine ueber NMI-Taste
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    QUIT procedure
      entry
	clr	INPTR_MCZ	!Loeschen Eingabezeiger MCZBFF!
	clr	OUTPTR_MCZ	!Loeschen Ausgabezeiger MCZBFF!
	setb	FLAG0, #%00	!kein Ruecksetzen Bit 7 bei den vom Terminal
				 bzw. vom MCZ eingehenden Zeichen (PTYINT bzw.
				 MCZINT) - (keine ASCII-Zeichen)!
Q1:
	testb	COUNT_PTY	!Zeichen vom Terminal eingegangen?!
	jr	z, Q2		!nein!

	ld	r0, OUTPTR_PTY
	call	INCPTR
	ld	OUTPTR_PTY, r0
	ldb	rl0, PTYBFF(r2)
	decb	COUNT_PTY, #%01
	calr	TYWR_MCZ	!Ausgabe Zeichen an MCZ!
	jr	nz, Q1		!kein CR ausgegeben!
	
Q2:
	ld	r0, OUTPTR_MCZ
	cp	r0, INPTR_MCZ	!Zeichen vom MCZ eingegangen?!
	JR	Z, Q1		!nein!

	calr	INCPTR_MCZ
	ld	OUTPTR_MCZ, r0	!Ausgabezeiger incr., d.h. Zeichen abgeholt!
	ldb	rl0, MCZBFF(r2)
	call	TYWR		!Ausgabe Zeichen an Terminal!
	jr	Q2
    end QUIT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE LOAD
LOAD-Routine "MCZ" (Monitorkommando)
Laden eines U8000-Maschinenprogramms vom MCZ in RAM

Kommandoeingabe: LOAD filename [<segnr>]
	              (Dateiname Segmentnummer)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    LOAD procedure
      entry
	calr	IN_POI_IB	!filename uebergehen!
	push	@r15, PC_SEG
	call	GET_SIGN	!rl0:=1.Zeichen ungleich Space nach naechstem
				 Space im Eingaepuffer INBFF
				 (1. Zeichen der segnr)!
	call	GET_SGNR	!r2:=angegebene Segmentnummer!
	ld	PC_SEG, r2
	setb	FLAG0, #%04	!ESC wirkt als Abbruchtaste fuer LOAD!

!'<segnr>' aus INBFF entfernen (wird von WEGA-Shell als
 Dateiumlenkung interpretiert!
	ld	r2, #INBFF	
	ld	r1, #%80	
	ldb	rl0, #'<'
	cpirb	rl0, @r2, r1, eq
	jr	nz, XL2		! '<' nicht gefunden !
	dec	r2, #1		!r2 zeigt auf Anfang der Segmentangabe!
	inc	r1, #1
XL1:	ldb	rl0, @r2
	cpb	rl0, #CR
	jr	z, XL2
	ldb	@r2, #' '
	cpb	rl0, #'>'
	jr	z, XL2     	!Ende der Segmentangabe erreicht!
	inc	r2, #1
	djnz	r1,XL1
XL2:

	calr	SAW_MCZ		!eingegebene Kommandozeile an MCZ senden und
				 Antwort abwarten!
	jr	z, LD5		!Abbruch; Fehlerausschrift vom MCZ wurde auf
				 Terminal ausgegeben!

!empfangen eines Datenblockes vom MCZ (in INBFF)!
LD1:
	calr	F_IB_MCZ	!INBFF ab empfangenen Zeichen '/' (aus-
                                 schliesslich) von MCZ fuellen!
	ld	r2, #INBFF
	ldb	rl0, @r2
	cpb	rl0, #'/'
	jr	nz, LD2		!erstes Zeichen in INBFF ungleich '/',
				 d.h. keine Fehlermeldung!

!Fehlernachricht von MCZ empfangen ('// error_massage CR')!
	ld	r4, #OUTBFF
	ld	r1, #%007F
	ldir	@r4, @r2, r1	!OUTBFF mit Inhalt von INBFF (Fehlernachricht)
				 fuellen!
	ld	OUTPTR, #%007F
	call	WR_OUTBFF	!Ausgabe OUTBFF auf Terminal / Loeschen OUTBFF!
	jr	LD5		!Abbruch!

LD2:
	bitb	FLAG0, #%05
	jr	nz, LD4		!Abbruchtaste gedrueckt (ESC)!

	calr	EKBL
	jr	nc, LD2A
	calr	S_RPB
	jr	LD1

LD2A:
	ldb	rl0, #'.'
	call	TYWR
	testb	rh3
	jr	nz, LD3
	calr	S_SNB
	call	WR_CRLF
	ldl	rr0, INBFF
	ldl	OUTBFF + 12, rr0
	ld	r4, #T_EPNT
	ld	r2, #OUTBFF
	ld	r1, #%0006
	ldir	@r2, @r4, r1
	ld	OUTPTR, #%0010
	call	WR_OUTBFF_CR
	jr	LD5

LD3:
	calr	EKLA
	PUSH	@R15, R4
	LD	R4, PC_SEG
	RES	R4, #15
	TEST	R4
	POP	R4, @R15
	JR	NZ, LD3A
 
	cp	r4, #%8000	!kleinste LOAD-Adresse in Segment 0
				 (in Vs. 1.7: %4000)!
	jr	nc,  LD3A
	ld	r2, #T_ILA
	jr	LD4A

LD3A:
	calr	S_SNB
	calr	LDB_MEM
	jr	LD1

LD4:
	ld	r2, #T_ABO
LD4A:
	call	WR_MSG		!AUSGABE : '/ABORT'!
	calr	S_LAB
LD5:	POP	PC_SEG, @R15
	ret	
    end LOAD

  INTERNAL
T_ILA:	WORD := %0019
	ARRAY [* BYTE] := '/INCORRECT LOAD ADDRESS %0D%00'
T_ABO:	WORD := %0007
	ARRAY [* BYTE] := '/ABORT%0D%00'
T_EPNT:	ARRAY [* BYTE] :='ENTRY POINT '

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MCZ_INT
VI-Routine fuer MCZ-Eingabe
Abspeicherung des Zeichens im MCZ-Eingabepuffer MCZBFF;
Inkrementieren des Eingabezeigers INPTR_MCZ zur Kennzeichnung, dass ein Zeichen
eingegengen ist
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    MCZ_INT procedure
      entry
	push	@r15, r0
	push	@r15, r1
	push	@r15, r2
	INB	RL1, SIO0D_A	!rl1:=eingelesenes Zeichen!
	bitb	FLAG0, #%00
	jr	nz, MCZ1	!keine ASCII-Zeichen einlesen!

	resb	rl1, #%07	!Bit 7 des ASCII-Zeichens zuruecksetzen!

MCZ1:
	ld	r0, INPTR_MCZ
	calr	INCPTR_MCZ
	ld	INPTR_MCZ, r0	!MCZ-Eingabezeiger incr.!
	ldb	MCZBFF(r2), rl1	!Zeichen im MCZBFF abspeichern!

	ldb	rl1, #%38	!RETI AN SIO SENDEN!
	OUTB	SIO0C_A, RL1

	pop	r2, @r15
	pop	r1, @r15
	pop	r0, @r15
	sc	#SC_SEGV
	iret	
    end MCZ_INT

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MCZ_ERR
VI-Routine fuer MCZ-Empfangsfehler
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    MCZ_ERR procedure
      entry
	push	@r15, r1
	ldb	rl1, #%30
	OUTB	SIO0C_A, RL1	!SIO_ERROR_RESET!
	ldb	rl1, #%38	!RETI AN SIO SENDEN!
	OUTB	SIO0C_A, RL1	!SIO_RETI_SENDEN!
	pop	r1, @r15
	sc	#SC_SEGV
	iret	
    end MCZ_ERR

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE INCPTR_MCZ
Zeichenkettenzeiger inkrementieren und auf Ueberlauf testen
Input:	r0 - Zeichenkettenzeiger
Output:	r2 := alter Stand Zeichenkettenzeiger (Input-Wert)
	r0 := r0+1, wenn C=1 (d.h. r0+1 < %E8)
	r0 := 0,    wenn C=0 (d.h. r0+1 >= %E8)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    INCPTR_MCZ procedure
      entry
	ld	r2, r0
	inc	r0, #%01
	cp	r0, #%00E8
	ret	c
	clr	r0
	ret	
    end INCPTR_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE TYWR_MCZ
Ausgabe eines Zeichens an das MCZ
Input:	rl0 - auszugebendes Zeichen
Output:	Z=1, wenn Zeichen = CR
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    TYWR_MCZ procedure
      entry
	INB	RH0, SIO0C_A
	bitb	rh0, #%02	!TRANSMIT BUFFER EMPTY ? !
	jr	z, TYWR_MCZ	!NO, WAIT!
	OUTB	SIO0D_A, RL0	!AUSGABE DES ZEICHENS!
	cpb	rl0, #CR
	ret	
    end TYWR_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_OUTBFF_CR_MCZ
Anhaengen eines CR an Zeichenkette im Ausgabepuffer OUTBFF;
Ausgabe des Inhaltes des Ausgabepuffers OUTBFF an das MCZ
(bis CR, maximal bis OUTPTR);
Loeschen des Ausgabepuffers OUTBFF
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_OUTBFF_CR_MCZ procedure
      entry
	ld	r2, OUTPTR
	ldb	OUTBFF(r2), #CR
	inc	OUTPTR, #%01
    end WR_OUTBFF_CR_MCZ	!Weiterlauf bei WR_OUTBFF_MCZ!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WR_OUTBFF_MCZ
Ausgabe des Inhaltes des Ausgabepuffers OUTBFF an das MCZ
(bis CR, maximal bis OUTPTR);
Loeschen des Ausgabepuffers OUTBFF
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WR_OUTBFF_MCZ procedure
      entry
	clr	r1
STRO1:
	ldb	rl0, OUTBFF(r1)
	inc	r1, #%01
	calr	TYWR_MCZ	!Ausgabe rl0 an MCZ!
	jr	z, STRO2	!CR ausgegeben --> Ende!

	cp	r1, OUTPTR
	jr	c, STRO1	!noch nicht alle Zeichen ausgegeben!

STRO2:
	clr	OUTPTR		!Ausgabepuffer loeschen!
	ld	r1, #%003F
	ld	OUTBFF, #'  '
	ld	r4, #OUTBFF
	ld	r2, #OUTBFF + 2
	ldir	@r2, @r4, r1
	ret	
    end WR_OUTBFF_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE IN_POI_IB
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    IN_POI_IB procedure
      entry
	clr	INPTR_MCZ
	clr	OUTPTR_MCZ
	call	GET_SIGN
    end IN_POI_IB		!Weiterlauf bei EKBL!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE EKBL
Eingabekonvertierung eines LOAD-Blockes
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    EKBL procedure
      entry
	clr	r2
	ldb	rh3, #%03
	calr	EKTB_PST
	ret	c
	ldb	KONV, rl3
	testb	rh3
	ret	z
	push	@r15, r3
	calr	EKTB_PST
	pop	r3, @r15
	ret	
    end EKBL

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE EKTB_PST
Eingabekonvertierung eines LOAD-Teilblockes mit Pruefsummentest
Input:	r2 - Zeiger auf aktuelles Zeichen in INBFF
	rh3 - Anzahl der einzulesenden Bytes aus INBFF (jedes Byte ist in INBFF
	      durch 2 ASCII-Zeichen kodiert (z.B. 'A','F' fuer %AF)
	(nach der angegebenen Bytezahl muss die Pruefsumme (1 Byte = 2 ASCII-
	Zeichen) in INBFF stehen
Output: rl3 - Pruefsumme 
	rh3 - letztes Byte der angeg. Anzahl (Byte vor Pruefsumme)
	C=1, wenn keine Hexaziffer in INBFF bzw. falsche Pruefsumme
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    EKTB_PST procedure
      entry
	clrb	rl3		!Pruefsumme loeschen!
EKTB1:
	calr	EKBY_PSB	!rl0:=eingelesenes Byte aus INBFF!
				!rl3:=aktuelle Pruefsumme!
	ret	c		!Fehler: keine Hexaziffer!
	dbjnz	rh3, EKTB1	!naechstes Byte einlesen!

	ldb	rh3, rl0	!rh3:=letztes Byte der angeg. Anzahl!
	push	@r15, r3
	calr	EKBY_PSB	!rl0:=aus INBFF eingelesene Pruefsumme!
	pop	r3, @r15	!r3:=berechnete Pruefsumme!
	ret	c		!Fehler: keine Hexaziffer!

	cpb	rl0, rl3	!Pruefsummenvergleich!
	ret	z		!i.O.!

	setflg	c		!C=1, da Pruefsummenfehler!
	ret	
    end EKTB_PST

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE EKBY_PSB
Eingabekonvertierung eines Bytes aus INBFF mit Pruefsummenberechnung
(in INBFF stehen die Datenbytes ASCII-kodiert, d.h. 1 Byte beansprucht
2 ASCII-Zeichen (z.B. 'A','F' fuer 'AF'))
Input:	r2 - Zeiger auf aktuelles Zeichen in INBFF
	rl3 - aktueller Stand der Pruefsumme
Output:	rl0 - eingelesenes Byte (binaer, =2 ASCII-Zeichen aus INBFF)
        rl3 - neue Pruefsumme
	r2  - neuer Stand des Zeigers auf aktuelles Zeichen in INBFF
	C=1, wenn keine Hexaziffer
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    EKBY_PSB procedure
      entry
	ldb	rl0, INBFF(r2)	!rl0:=1. Hexaziffer (ASCII)!
	inc	r2, #%01
	call	HTOB		!Konv. rl0 (ASCII) --> rl0 (binaer)!
	ret	c		!keine Hexaziffer (Fehler)!

	addb	rl3, rl0	!rl3:=neue Pruefsumme!
	sla	r0, #%0C
	ldb	rl0, INBFF(r2)	!rl0:=2. Hexaziffer (ASCII)!
	inc	r2, #%01
	call	HTOB		!Konv. rl0 (ASCII) --> rl0 (binaer)!
	ret	c		!keine Hexaziffer (Fehler)!

	addb	rl3, rl0	!rl3:=neuer Pruefsumme!
	orb	rl0, rh0	!rl0 := eingelesenes Byte!
	resflg	c
	ret	
    end EKBY_PSB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE LDB_MEM
Laden des LOAD-Datenblockes in den Speicher
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    LDB_MEM procedure
      entry
	ldb	rl1, rh3
	calr	EKLA
	ld	r2, #%0008
LDB_M_NB:
	calr	EKBY_PSB
 
	PUSH	@R15, R4
	LD	R5, R4
	LD	R4, PC_SEG
	SC	#SC_SEGV
	LDB	@R4, RL0
	SC	#SC_NSEGV
	POP	R4, @R15
 
	inc	r4, #%01
	dbjnz	rl1, LDB_M_NB	!LADE NAECHSTES BYTE!
	ret	
    end LDB_MEM

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE EKLA
Eingabekonvertierung der Ladeadresse fuer den LOAD-Datenblock
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    EKLA procedure
      entry
	clr	r2
	calr	EKBY_PSB
	ldb	rh4, rl0
	calr	EKBY_PSB
	ldb	rl4, rl0
	ret	
    end EKLA

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SAW_MCZ
Senden einer Zeichenkette (Inhalt von INBFF) zum MCZ und warten auf Antwort
Output:	Z=0, wenn Zeichen '9' oder '0' oder '7' vom MCZ empfangen wurde
	Z=1, wenn Fehler MCZ bzw. Abbruchtaste (ESC) gedrueckt
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    SAW_MCZ procedure
      entry
	bitb	FLAG0, #%05
	jr	z, SAW0		!Abbruchtaste (ESC) nicht gedrueckt!

	setflg	z		!Z=1, Abbruchtaste gedrueckt!
	ret	

SAW0:
	ld	r4, #OUTBFF
	ld	r2, #INBFF
	ld	r1, #%0040
	ldir	@r4, @r2, r1	!Umspeicherung INBFF-->OUTBFF!

	ld	OUTPTR, #%0080	!Zeilenlaenge OUTBFF = %80!
	calr	WR_OUTBFF_MCZ	!Inhalt OUTBFF an MCZ senden!

	calr	WOEOI_MCZ	!Warten auf Antwort vom MCZ (bis CR) in MCZBFF!
	calr	WOCF_MCZ_OI	!rl1:=naechstes Zeichen vom MCZ
 				(kein incr. des MCZ-Ausgabezeigers OUTPTR_MCZ)!
	cpb	rl1, #'L'
	jr	z, SAW1		!ECHO VOM MCZ: 'LOAD ...' EMPFANGEN!
	jr	SAW2

SAW1:
	calr	WOEOI_MCZ	!ganze Zeile bis CR von MCZ einlesen!
	calr	WOCF_MCZ_OI	!rl1:=naechstes Zeichen!
SAW2:
	cpb	rl1, #'9'
	jr	z, SAW4
	cpb	rl1, #'0'
	jr	z, SAW4
	cpb	rl1, #'7'
	jr	z, SAW4

SAW3:
	calr	WOCF_MCZ	!rl0:=Zeichen vom MCZ!
	call	TYWR		!Zeichen auf Terminal ausgeben!
	cpb	rl0, #LF
	ret	z		!Zeichen=LF --> Ende (Z=1)!
	jr	SAW3

SAW4:
	resflg	z
	ret	
    end SAW_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WOEOI_MCZ
Warten auf das Ende der Information vom MCZ (CR empfangen);
weiter siehe WEOS_MCZ
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WOEOI_MCZ procedure
      entry
	calr	WOCF_MCZ	!rl0:=Zeichen vom MCZ!
	cpb	rl0, #CR
	jr	nz, WOEOI_MCZ	!Warten auf CR!
    end WOEOI_MCZ		!Weiterlauf bei WOEOS_MCZ!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WOEOS_MCZ
Warten auf das Ende des Sendens vom MCZ
(Warten auf ein Zeichen >='Space' oder bis Wartezeit abgelaufen)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WOEOS_MCZ procedure
      entry
	ld	r1, #%0200	!Festlegung der Wartezeit bis zur Bereit-
				stellung eines Zeichens in MCZBFF!
WOEOS1:
	ld	r0, OUTPTR_MCZ	!bestimmte Zeit auf ein Zeichen vom MCZ warten!
	cp	r0, INPTR_MCZ
	jr	nz, WOEOS2	!Zeichen steht in MCZBFF bereit!

	dec	r1, #%01	!Zeitzaehler decr.!
	jr	nz, WOEOS1
	ret	 		!Wartezeit abgelaufen!

WOEOS2:
	calr	WOCF_MCZ_OI	!rl1:=Zeichen (OUTPTR_MCZ nicht incr.)!
	cpb	rl1, #' '
	ret	pl		!Zeichen >='Space'!

	calr	WOCF_MCZ	!Zeichen uebergehen!
	jr	WOEOS_MCZ	!Warten auf naechstes Zeichen!
    end WOEOS_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WOCF_MCZ
Warten, bis ein Zeichen im MCZ-Puffer MCZBFF bereitsteht
(MCZBFF wird durch empfangene Zeichen vom MCZ in MCZ_INT gefuellt)
Output:	rl0 - Zeichen 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    WOCF_MCZ procedure
      entry
	ld	r0, OUTPTR_MCZ
	cp	r0, INPTR_MCZ
	jr	z, WOCF_MCZ	!kein Zeichen in MCZBFF eingegangen!

!Zeichen empfangen!
	calr	INCPTR_MCZ
	ld	OUTPTR_MCZ, r0	!Ausgabezeiger aktualisieren (Zeichen wurde
				abgeholt!
	ldb	rl0, MCZBFF(r2)	!rl0:=Zeichen!
	ret	
    end WOCF_MCZ

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE WOCF_MCZ_OI
Warten, bis ein Zeichen im MCZ-Puffer MCZBFF bereitsteht
(ohne Aktualisierung von OUTPTR_MCZ, d.h. Zeichen gilt nach Aufruf von
WOCF_MCZ_OI als noch nicht abgeholt);
(MCZBFF wird durch empfangene Zeichen vom MCZ in MCZ_INT gefuellt)
Output:	rl1 - Zeichen 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    WOCF_MCZ_OI procedure
      entry
	ld	r0, OUTPTR_MCZ
	cp	r0, INPTR_MCZ
	jr	z, WOCF_MCZ_OI	!kein Zeichen in MCZBFF eingegangen!

	calr	INCPTR_MCZ	!r2:=OUTPTR_MCZ!
	ldb	rl1, MCZBFF(r2)	!rl1:=Zeichen!
	ret	
    end WOCF_MCZ_OI

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE F_IB_MCZ
Fuellen von INBFF von MCZ nach empfangenen Zeichen '/' bis CR 
(max. %50 Zeichen)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    F_IB_MCZ procedure
      entry
	calr	WOCF_MCZ	!rl0:=Zeichen von MCZ!
	cpb	rl0, #'/'
	jr	nz, F_IB_MCZ	!Warten bis Zeichen = '/' eingegangen!
    end F_IB_MCZ		!Weiterlauf bei O_FIB_MCZ!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE O_FIB_MCZ
Fuellen von INBFF von MCZ bis CR (max. %50 Zeichen)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    O_FIB_MCZ procedure
      entry
	CLR	R4		!Loeschen Zeiger Eingabepuffer!
	ldb	rl3, #%50	!max. %50 Zeichen einlesen!
FIB1:
	calr	WOCF_MCZ	!rl0:=Zeichen vom MCZ!
	ldb	INBFF(r4), rl0	!Zeichen in INBFF abspeichern!
	cpb	rl0, #CR
	jr	nz, FIB2	!Zeichen ungleich CR!

	calr	WOEOS_MCZ	!Zeichen war CR --> warten auf Sendeende!
	ret	

FIB2:
	cpb	rl0, #%20
	jr	c, FIB1		!Zeichen < Space ignorieren!
	
	inc	r4, #%01
	dbjnz	rl3, FIB1	!naechstes Zeichen von MCZ einlesen!

!%50 Zeichen vom MCZ eingelesen / restliche Zeichen bis CR ignorieren!
FIB3:
	calr	WOCF_MCZ	!rl0:=Zeichen von MCZ!
	cpb	rl0, #CR
	jr	nz, FIB3	!Warten bis CR gelesen!

	ldb	INBFF + %50, rl0	!CR als %51-stes Zeichen in INBFF
					abspeichern!
	ret	
    end O_FIB_MCZ
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE S_RPB
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    S_RPB procedure
      entry
	ldb	rl0, #'7'	!SEND TO MCZ: PLEASE REPEAT SENDET INFORMATION!
	jr	SAN_MCZ
    end S_RPB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE S_LAB
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    S_LAB procedure
      entry
	ldb	rl0, #'9'	!SEND TO MCZ: ABORT THE LOAD-ROUTINE!
	jr	SAN_MCZ
    end S_LAB

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE S_SNB
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    S_SNB procedure
      entry
	ldb	rl0, #'0'	!SEND TO MCZ: RECIVED INF. IS OK (SEND NEXT)!
SAN_MCZ:
	ldb	OUTBFF, rl0
	ld	OUTPTR, #1
	calr	WR_OUTBFF_CR_MCZ
	jr	WOEOI_MCZ
    end S_SNB
 
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE SEND
SEND-Routine "MCZ" (Monitorkommando)
Abspeichern eines Speicherinhaltes als Datei auf dem MCZ

Kommandoeingabe: SEND filename [<segnr>]offset1 offset2 [E=Entry_point]
        	      (Dateiname Anfangsadresse Endadresse Entrypoint)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  GLOBAL
    SEND procedure
      entry
	PUSH	@R15, PC_SEG
	SUB	R1, R1		!INITIALISIERUNG!
	LD	R10, R1
	LD	R5, R1
	LD	R6, R1
	LD	R11, R1
 
	LD	INPTR_MCZ, R1
	LD	OUTPTR_MCZ, R1
 
	call	GET_SIGN	!INPTR0 --> 1.ZEICHEN VON FILE_NAME!
	call	GET_SIGN	!INPTR0 --> '<' oder 1.ZEICHEN DER ANF.ADR.!
	call	GET_SGNR	!R2 := SEGMENTNR., WENN VORHANDEN!
	LD	PC_SEG, R2
 
	jr	z, ZS_ERROR	!FEHLT: ANF.ADR., END-ADR. u. Entry-POINT!
	calr	EK2B		!EINGABEKONVERTIERUNG: ANF.ADR.!
	jr	z, ZS_ERROR	!FEHLT: END-ADR. u. ENTRY-POINT!
	LD	R10, R3
	calr	EK2B		!EINGABEKONV.: END-ADR.!
	sub	r3, r10  	!endadr-anfadr!
	jr	mi, ZS_ERROR	!endadr<anfadr!
	LD	R5, R3
	inc	r5		!r5:=bytezahl!
	CPB	RL0, #%0D
	JR	Z, KEPA		!KEINE ENTRY-POINT-ANGABE!
	RESB	RL0, #5		!KLEINBUCHSTABEN --> GROSSBUCHSTABEN!
	CPB	RL0, #'E'	!RL0 = 'E' oder 'e' ?!
	jr	nz, ZS_ERROR
	call	GET_CHR		!NAECHSTES ZEICHEN TESTEN!
	CPB	RL0, #'='
	jr	nz, ZS_ERROR
	call	GET_CHR
	calr	EK2B		!EINGABEKONV.: ENTRY-POINT!
	LD	R11, R3
 
KEPA:	
!'<segnr>' aus INBFF entfernen (wird von WEGA-Shell als
 Dateiumlenkung interpretiert!
	ld	r2, #INBFF	
	ld	r1, #%80	
	ldb	rl0, #'<'
	cpirb	rl0, @r2, r1, eq
	jr	nz, XS2		! '<' nicht gefunden !
	dec	r2, #1		!r2 zeigt auf Anfang der Segmentangabe!
	inc	r1, #1
XS1:	ldb	rl0, @r2
	cpb	rl0, #CR
	jr	z, XS2
	ldb	@r2, #' '
	cpb	rl0, #'>'
	jr	z, XS2     	!Ende der Segmentangabe erreicht!
	inc	r2, #1
	djnz	r1,XS1
XS2:

	LD	R4, #OUTBFF
	LD	R2, #INBFF
	LD	R1, #%40
	LDIR	@R4, @R2, R1
	LD	OUTPTR, #%80
	calr	WR_OUTBFF_MCZ	!SENDE BEFEHL: 'SEND ...' ANS MCZ!
 
SINTE:	calr	O_FIB_MCZ	!ZEICHENWEISES ECHO VOM MCZ UEBERSPRINGEN!
	calr	O_FIB_MCZ	!FUELLEN DES INBFF MIT DEN VOM MCZ GESENDETEN 
				 ZEICHEN!
	LD	R2, #INBFF
	LDB	RL0, @R2
	CPB	RL0, #'0'
	JR	Z, SOK		!SEND-BLOCK IST OK!
	CPB	RL0, #'7'
	JR	Z, SREP		!SEND-BLOCK WIEDERHOLT ANS MCZ SENDEN!
	CPB	RL0, #'9'
	JR	Z, SABO		!SEND-ROUTINE WURDE VOM MCZ ABGEBROCHEN!
	calr	MIOB_CON	!MOVE INBFF IN OUTBFF AND SEND TO CONSOLE!
SABO:	LD	R2, #TSAB	!DISPLAY: SEND ABORT!
	call	WR_MSG
GO_MON:	POP	PC_SEG, @R15
	RET			!RUECKSPRUNG IN DEN MONITOR!

ZS_ERROR:
	jp	ERROR 
 
SENDE:	call	WR_CRLF		!SEND FEHLERFREI BEENDET!
	JR	GO_MON
 
SOK:	LD	R2, R5
	TEST	R2
	JR	Z,SENDE
 
	LDB	RL0, #'.'
	call	TYWR		!'.' AN CONSOLE AUSGEBEN UND BLOCK SENDEN!
 
	SUB	R2, R6
	LD	R5, R2
 
	LD	R2, R10
	ADD	R2, R6
	LD	R10, R2
 
SREP:	LD	R7, R5
	TEST	R7
	JR	Z, SEB		!SENDE ENDE-BLOCK ZUM MCZ!
	SUB	R7, #%1E
	JR	NC, SBML	!SENDE BLOCK MIT MAX. LAENGE (=%1E DATENBYTE)!
	ADD	R7, #%1E
	JR	SADB		!SENDE ADRESS-BLOCK!
SBML:	LD	R7, #%1E
SADB:	LD	R6, R7
	calr	S_MCZ_AB	!SEND ADDRESS-BLOCK TO MCZ!
 
	LD	R8, PC_SEG	!INITIALISIERUNG DER REGISTER ZUM INDIREKTEN LADEN!
	LD	R9, R10
	LDB	RL4, #0
 
S_N_B:	SC	#SC_SEGV	!SEND NEXT DATEN-BYTE!
	LDB	RL3, @R8	!LADE INDIREKT VON RR8!
	SC	#SC_NSEGV
 
	calr	AK_S_MCZ	!SEND DATEN-BYTE!
 
	INC	R9, #1
	DBJNZ	RL7, S_N_B
 
	LDB	RL3, RL4
	calr	AK_S_MCZ	!SEND PRUEFSUMME!
 
S_CR:	LDB	RL0, #%0D
	calr	TYWR_MCZ
	JR	SINTE
 
SEB:	LD	R2, R11		!SEND END-BLOCK!
	LD	R10, R2
	LD	R6, #0
	calr	S_MCZ_AB
	JR	S_CR
 
 
!	UNTERPROGRAMME       !
 
!EINGABEKONVERTIERUNG BIS 4 ASCII-HEXA-ZIFFERN IN 2 BYTE-INTEGER (R3)!
EK2B:	LDB	RL4, #4		!COUNTER FUER 4 ASCII-ZEICHEN!
	SUB	R3, R3		!R3 := 0!
EK2BL:	call	HTOB
	JR	C, KAHZ		!KEINE ASCII-HEXADEZIMAL-ZIFFER!
	RLDB	RL0, RL3
	RLDB	RL0, RH3
	DECB	RL4, #1
	BITB	RL4, #0
	JR	NZ, RL4UG	!RL4 = UNGERADE!
	CPB	RL4, #2
	JP	NZ, GET_SIGN	!4 ZIFFERN KONV.,INPTR AUF NAECHSTES ZEICHEN
				 UNGLEICH SPACE SETZEN UND NACH RL0 LADEN, ENDE!!!
RL4UG:	call	GET_CHR
	RET	Z
	JR	EK2BL
 
KAHZ:	CPB	RL0, #' '
	jr	nz, ZS_ERROR
	jp	GET_SIGNW
 
!SENDE ANFANGSBLOCK ZUM MCZ!
S_MCZ_AB:
	LDB	RL4, #0		!PRUEFSUMME := 0!
 
	LDB	RL0, #'/'
	calr	TYWR_MCZ
 
	LD	R2, R10
	LDB	RL3, RH2	!H(BLOCK-ADRESSE) SENDEN!
	calr	AK_S_MCZ
	LDB	RL3, RL2	!L(BLOCK-ADRESSE) SENDEN!
	calr	AK_S_MCZ
 
	LD	R2, R6		!BLOCK-LAENGE SENDEN!
	LDB	RL3, RL2
	calr	AK_S_MCZ
 
	LDB	RL3, RL4	!PRUEFSUMME SENDEN!
		
				!Weiterlauf bei AK_S_MCZ! 

!SENDEN EINES IN 2 ASCII-HEXA-ZIFFERN UMKODIERTEN BYTES ZUM MCZ!
AK_S_MCZ:
	LDB	RH4, #2		!SCHLEIFE FUER 2 ASCII-HEXADEZIMAL-ZIFFERN!
AKSML:	LDB	RL0, #0
	RLDB	RL0, RL3
 
	ADDB	RL4, RL0	!PRUEFSUMMEN-BERECHNUNG!
	ADDB	RL0, #'0'	!AUSGABE-KONVERTIERUNG!
	CPB	RL0, #%3A
	JR	C, BEZ		!BYTE EQU ZIFFER!
	ADDB	RL0, #%7
BEZ:	calr	TYWR_MCZ	!SEND CHAR TO MCZ!
	DBJNZ	RH4, AKSML
	RET
    end SEND
 
  INTERNAL
TSAB:	WORD := 11
	ARRAY [* BYTE] := 'SEND ABORT%0D%00'

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PROCEDURE MIOB_CON
Kopieren einer Zeichenkette von INBFF nach OUTBFF und Senden zur Console
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

  INTERNAL
    MIOB_CON procedure
      entry
	LD	R4, #OUTBFF
	LD	R2, #INBFF
	LD	R1, #%7F
	LDIR	@R4, @R2, R1
	LD	OUTPTR, #%07F
	jp	WR_OUTBFF
 end MIOB_CON
 
end p_ldsd
