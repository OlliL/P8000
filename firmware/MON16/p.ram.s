!******************************************************************************
*******************************************************************************

  P8000-Firmware		       (C) ZFT/KEAW Abt. Basissoftware - 1989
  U8000-Softwaremonitor (MON16)        Modul: p_ram

  Bearbeiter:	J. Zabel
  Datum:	14.04.1989
  Version:	3.1

*******************************************************************************
******************************************************************************!


p_ram module

$SECTION RAM

!******************************************************************************
Arbeitsspeicher
******************************************************************************!

GLOBAL

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

RAM_ANF

		ARRAY [%10 WORD]
NSP_OFF		WORD
	
		ARRAY [%3F WORD]	!Stackbereich!
STACK		WORD

		ARRAY [4 WORD]

_numterm	WORD 			!Anzahl der Terminals!
PTYPORT		BYTE			!Terminalkanal:
					 00 - SIO0_B (16-Bit-Teil)
					 01 - SIO0_B (8-Bit-Teil) ueber 
					      Koppelschnittstelle (UDOS 
					      auf 8-Bit-Teil nicht verfuegbar)
					 02 - SIO0_B (8-Bit-Teil) ueber 
					      Koppelschnittstelle (UDOS 
					      auf 8-Bit-Teil verfuegbar)!
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

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Flags
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

FLAG0	BYTE

!
7 6 5 4 3 2 1 0
              0-> 0: empf. Zeichen vom Terminalkanal ist ASCII-Zeichen 
		     (Reset Bit 7 des Zeichens, Test auf ^S, ^Q, ESC)
                  1: empf. Zeichen vom Terminalkanal ist beliebiges Datenbyte
		     (keine Sonderbehandlung)
                  P: ENTRY(0), PTY_INT(T), KOPPEL_INT(T), QUIT(1), 
		     NMI_INT(T/0), MCZ_INT(T), RM_BOOT(0)
            1---> 0:
                  1:
                  P: ENTRY(1)
          2-----> 0: Ausgabe an Terminal nicht gesperrt (^Q)
                  1: Ausgabe an Terminal gesperrt (^S)
                  P: ENTRY(1), TYWR(T), PTY_INT(0,1), KOPPEL_INT(0,1)
        3-------> 0: und gleichzeitig Bit4/FLAG0=0: ESC wirkt nicht als
                     Abbruchtaste, sondern ist normales ASCII-Zeichen
                  1: ESC wirkt als Abbruchtaste
                     (Bit5/FLAG0 wird bei Eingabe von ESC gesetzt)
                  P: ENTRY(0), PTY_INT(T)
      4---------> 0: und gleichzeitig Bit3/FLAG0=0: ESC wirkt nicht als
                     Abbruchtaste, sondern ist normales ASCII-Zeichen
                  1: ESC wirkt als Abbruchtaste
                     (Bit5/FLAG0 wird bei Eingabe von ESC gesetzt)
                  P: ENTRY(0), PTY_INT(T), KOPPEL_INT(T), LOAD(1)
    5-----------> 0: Abbruchtaste nicht gedrueckt
                  1: Abbruchtaste gedrueckt (ESC)
                  P: ENTRY(0), PTY_INT(1), KOPPEL_INT(1), LOAD(T), SAW_MCZ(T)
  6-------------> 0:
                  1:
                  P: ENTRY(0)
7---------------> 0:
                  1:
                  P: ENTRY(0)
!


FLAG1	BYTE

!
7 6 5 4 3 2 1 0
              0-> 0: keine Segmentnummer angegeben (???)
                  1: Segmentnummer angegeben (???)
                  P: ENTRY(0), GET_SGNR(0,1)
            1---> 0: noch Parameter einlesen
                  1: alle Parameter eingelesen
                  P: ENTRY(0), DISPLAY (0,1,T)
          2-----> 0: kein NEXT-Betrieb
                  1: NEXT-Betrieb
                  P: ENTRY(0,T/0), NEXT(1)
        3-------> 0: nur Grossbuchstaben erlaubt
                     (Kleinbuchstaben werden in Grossbuchstaben umgewandelt)
                  1: Klein- und Grossbuchstaben erlaubt
                  P: ENTRY(1), RD_LINE_BFF(T)
      4---------> 0: Ausgabe bei CR abbrechen
                  1: Ausgabe nicht bei CR abbrechen
                  P: ENTRY(0), WR_OUTBFF(T)
    5-----------> 0:
                  1:
                  P: ENTRY(0)
  6-------------> 0:
                  1:
                  P: ENTRY(0)
7---------------> 0:
                  1:
                  P: ENTRY(0)
!

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Variablen (initialisiert bei Systemstart)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

CHRDEL	BYTE		!Zeichen loeschen (Backspace %08)!
LINDEL	BYTE		!Zeile loeschen (Line delete %7F)!
XONCHR	BYTE		!Terminalausgabe fortsetzen (^Q %11)!
XOFCHR	BYTE		!Terminalausgabe anhalten (^S %13)!
NULLCT	WORD		!Anzahl der Nullen nach CR bei Terminalausgabe (%0000)!
B_CODE	WORD		!Unterbrechungscode Z8000 (7F00)!
STKCTR	WORD		!Zaehler der Stackoperationen fuer NEXT (%0004)!
NXTCTR	WORD		!SCHRITTZAEHLER FUER NEXT (%0001)!
PROMT	WORD		!Promt-Zeichen (%2000)!


!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Program Status Area (initialisiert bei Systemstart)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

PSAREA
!		                    ID    FCW   PCSEG PCOFF!
		ARRAY [4 WORD] !:= [%0000 %0000 %0000 %0000]
		               RESERVED!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 UINSTR_ERR] 
		               UNIMLEMENTED INSTRUCTIONS!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 PINSTR_ERR] 
		               PRIVILEGED INSTRUCTIONS!
PSA_SC		ARRAY [4 WORD] !:= [%0000 %C000 %8000 SC_ENTRY]
		               SYSTEM_CALL INSTRUCTIONS!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 MMU_ERR] 
		               SEGMENT TRAP!
PSA_NMI		ARRAY [4 WORD] !:= [%0000 %4000 %8000 AUTOBOOT bzw. NMI_INT] 
		               NONMASKABLE INTERRUPT
		               (AUTOBOOT vor erster Terminaleingabe;
		                NMI_INT nach erster Terminaleingabe)!
		ARRAY [4 WORD] !:= [%0000 %4000 %8000 NVI_ERR] 
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
 

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Arbeitsspeicher fuer Kommando "T" (Hardwareeigentest)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

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

		ARRAY [%34 WORD] !unbenutzt!


!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Arbeitsspeicher fuer Kommandos "GE"/"S"/"O U" und Floppytreiber 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

FILENAME_PTR	WORD			!Anfangsadresse Dateiname!

COUNT_UDOS	WORD			!akt. Anzahl der Zeichen in UDOSBFF!
INPTR_UDOS	WORD			!Eingabezeiger fuer UDOSBFF 
					 (naechster freier Platz)!
OUTPTR_UDOS	WORD			!Ausgabezeiger fuer UDOSBFF
			 		 naechstes auszulesende Zeichen)!
UDOSBFF		ARRAY [%100 BYTE]	!Eingabepuffer fuer interrupt-
					 gesteuerte Eingabe bei Dateitransfer
					 von/zu UDOS bzw. Blocktransfer!
end p_ram
