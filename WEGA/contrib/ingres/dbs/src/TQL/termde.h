/*$$+ TERMDE.h +$$*/
/* IDENT: 010387 - TERMDE.h */
/* Design: Buchert */

/******************************************************************************
*
* TERMDE.h
*
* include-File zur Beschreibung des verwendeten Terminals
*
* #define <terminaltyp>  1   ==>  dieser Terminaltyp wird verwendet,
*                                 sonst wird '0' zugeordnet
*
*****************************************************************************/

/* Die #define-Anweisung fuer das entsprechende Terminal ist zu aktivieren */

#define VT_200_V 1		/*   SMG-Routinen anwendbar */
/* #define VT_100 0 */		/* Terminals der VT-Familie */
/* #define T_7920 1 */		/* ESER-Terminal 7920       */


#define TERMCOLS  80		/* Anzahl der Spalten */
#define TERMROWS  24		/* Anzahl der Zeilen  */
#define FIRSTROW  1		/* Nummer der 1. Zeile */
#define FIRSTCOL  1		/* Nummer der 1. Spalte */

/* Definitionen der moeglichen Eingabe-Terminatoren ***/

#define TRM_CR  1		/* zum naechsten Eingabe-Feld */
#define TRM_DOWN_CUR  2		/* zum naechsten Eingabe-Feld */
#define TRM_UP_CUR  3		/* zum vorhergehenden Eingabe-Feld */
#define TRM_ENTER  4		/* fuer EDIT: zum naechsten Satz */
#define TRM_CTRLE  7		/* Ende aller Eingaben */
#define TRM_PRESC  9		/* vorhergehender Bildschirminhalt */
#define TRM_NEXSC  10		/* Uebergang zum naechsten Bildschirm */

/* weitere Eingabe-Terminatoren */
#define TRM_ACTRL  5
#define TRM_DCTRL  6
#define TRM_ICTRL  8
#define TRM_UCTRL  11		/* Loeschen des aktuellen Eingabe-Feldes */
#define TRM_WCTRL  12		/* Refresh Display */
#define TRM_KCTRL  13		/* Eingabefeld verlassen ohne Pruefung */

/* Video-Attribute ************************************/

#define VIDEO_REVERSE 2

/* Fehlercodes ****************************************/

#define FMERRSC  63		/* Fehler bei Setzen des Cursors */
#define FMERRIN  64		/* Fehler bei Eingabe */
#define FMERROU  65		/* Fehler bei Ausgabe */
#define FMERRCL  66		/* Fehler bei CLEAR */
#define FMERRCR  67		/* Fehler bei Initialisierung (Create) */
#define FMNOINI  68		/* Terminal nicht fuer Full-SCR initialisiert */
#define NOGETS   103		/* Keine GET-Variablen im READ-Zugriff */


/* UEBERTRAG AUS ROERR.H (UM NICHT NOCH #include  "ROERR.H"  zu machen */

#define TERMBUF 6       /* Terminalpuffer ist zu klein */
#define NOMEMORY 14	/* malloc kann keinen weiteren Speicherplatz zuordnen */
#define OFPOLNOT 15	/* Bereich fuer Interndarstellung von Ausdruecken ist voll */
#define NOCOND 17	/* ungueltige Bedingung */
#define NOINIT 22	/* kein INIT-Operator abgearbeitet oder falsche ZVT-Adresse angegeben */
#define EFOPEN 28	/* Fehler beim Eroeffnen eines seriellen Files */
#define GETINDEX 33	/* kein Ausgabe-Index gefunden */
#define EKYUPDT 42	/* Fehler bei Aktualisierung Schluesselbaum */
#define NOMEMNAME 45	/* Speichername nicht gefunden */
