/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1989
	Programm: udformat.c
 
	Bearbeiter	: P. Hoge
	Datum		: 10.3.89
	Version		: 1.1
 
***************************************************************************

	Formatieren von UDOS-Disketten
	Syntax: udformat [udosdev]

***************************************************************************
**************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* 
 * USER-DISK:
 *  SPUR 16H	SEKTOREN 0,5,A,1,6,B		DIRECTORY
 *  SPUR 16H	SEKTOR   2			POINTERSEKTOR DER DIRECTORY
 *  SPUR 17H	SEKTOREN 0,1			DISK-ALLOCATION-MAP
 *  ZAHL DER FREIEN SEKTOREN: 277H
 *  ZAHL DER BELEGTEN SEKTOREN: 9 + 720H (NICHT VORHANDENE SEKTOREN) = 729H
 */

char *arg0;
int fd;
char buf[512];

char f48ss9[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	2,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	64, 0,				/* directory entries */
	360%256, 360/256,		/* number of all sectors */
	0xfc,				/* format id */
	2,0,				/* sectors per fat */
	9,0,				/* sectors per track */
	1,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf0,				/* 1. byte fat */
};
char f96ds9[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	2,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	112, 0,				/* directory entries */
	1440%256, 1440/256,		/* number of all sectors */
	0xf9,				/* format id */
	3,0,				/* sectors per fat */
	9,0,				/* sectors per track */
	2,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf9				/* 1. byte fat */
};


main(argc,argv)
char **argv;
{
 char *fdname;
 int dev = 0;
 struct stat mstat;
 char *pboot;

 arg0 = argv[0];
 switch (argc) {
 case 1:
 	fdname = "/dev/rfd196ds16";
	break;
 case 2:
	fdname = argv[1];
	break;
 default:
 error:
	fprintf(stderr,"usage: udformat [udosdev]\n");
	exit(1);
 }
 if (stat(fdname, &mstat) == 0) {
	if (mstat.st_mode & (S_IFBLK | S_IFCHR)) {
		dev++;		/* echte Floppy */
	}
 }
 pboot = f96ds16;
 if (cmp(fdname, "/dev/") {
	if (dev == 0)
		goto error;
	if (cmp(fdname, "48ss16")
		pboot = f48ss16;
 	else if (cmp(fdname, "96ds16") == 0) {
		fprintf(stderr, "%s: invalid UDOS format\n", arg0);
		exit(1);
	}
 }

 if (dev) {
	strcpy(buf, "format ");		/* physisch Formatieren */
	strcat(buf, fdname);
	if (system(buf))
		exit(1);
 }

 if ((fd = open(fdname, O_RDWR|O_CREAT|O_TRUNC, 0664)) < 0) {
	fprintf(stderr, "%s: kann %s nicht oeffnen\n", arg0, fdname);
	exit(1);
 }

 signal(SIGALRM, SIG_IGN);
 signal(SIGHUP, SIG_IGN);
 signal(SIGINT, SIG_IGN);
 signal(SIGQUIT, SIG_IGN);
 signal(SIGTERM, SIG_IGN);

 putdata(pboot);
 exit(0);
}

putsect()
{
	if (write(fd, buf, 512) != 512) {
		fprintf(stderr, "%s: Schreibfehler in %s\n", arg0, fdname);
		exit(1);
	}
}

putdata(p)
register char *p;
{
 register i, j;

 clear_buf();			/* write boot sector */
 for (i = 0; i < 30; i++)
	buf[i] = p[i];
 buf[0x2f]  = p[24];	/* sectors/track */
 buf[0x1fe] = 0x55;
 buf[0x1ff] = 0xaa;
 putsect();
				/* write fat's */
 i = p[16];			/* number of fats */
 while (i--) {
	clear_buf();
	buf[0] = p[30];		/* 1. byte fat */
	buf[1] = 0xff;
	buf[2] = 0xff;
	j = p[22];		/* number of fat sectors */
	while (j--) {
		putsect();
		clear_buf();
	}
 }
				/* write directory */
 i = ((p[17]&0x00ff) * 32)/512;	/* number of directory sectors */
 while (i--)
	putsect();
}

clear_buf()
{
	register i = sizeof(buf);
	register char *s = buf;

	while (i--)
		*s++ = 0;
}

cmp(s1, s2)
register char *s1, *s2;
{
	register char *s3;

	s3 = s2;
	while (1) {
		if (*s1 == 0)
			return(0);
		if (*s1 != *s2) {
			s1++;
			continue;
		}
	  loop:
		if (*s2 == 0)
			return(1);
		if (*s1 != *s2) {
			s2 = s3;
			continue;
		}
		s1++;
		s2++;
		goto loop;
	}
}
123:
;******************************************************************************
; KOMMANDOABARBEITUNG
;******************************************************************************

; ERZEUGEN DER BITMAP, DES DIRECTORY-DESCRIPTORS UND DES 1. DIRECTORYSEKTORS
;
	CALL	ERBIMA		;ERZEUGEN DER BITMAP
	CALL	ERDIDE		;ERZEUGEN DES DIRECTORYDESCRIPTORS U. 1. SEKTOR
;
; BITMAP SCHREIBEN (1700 / 1701)
;
	CALL	WRBIMA		;BITMAP SCHREIBEN
;
; DESCRIPTOR DER DIRECTORY SCHREIBEN (1600)
;
	LD	IY,DIDESV	;VEKTOR FUER DESCRIPTOR
	LD	HL,DIRDES	;PUFFERANFANGSADRESSE
	LD	(DIDESV+2),HL
	LD	HL,100H		;DATENLAENGE
	LD	(DIDESV+4),HL
	LD	HL,1600H
	LD	(DIDESV+11),HL
	LD	A,(DRIVAD)	;DRIVEADRESSE
	LD	(IY+11),A	;IN VEKTOR LADEN
	CALL	FLOPPY		;DIR.-DESCRIPTOR SCHREIBEN
	LD	A,(IY+10)	;COMPLETION CODE
	BIT	6,A		;FEHLER ?
	JP	NZ,FTEST	;FEHLERTEST
;
; 1. DATENSEKTOR DER DIRECTORY SCHREIBEN (1605)
;
	LD	A,(DIDESV+11)
	ADD	A,5		;SEKTORADRESSE+5
	LD	(DIDESV+11),A
	LD	HL,DIRSEC	;DATENADRESSE 1. DIR.-SEKTOR
	LD	(DIDESV+2),HL	;ADRESSE LADEN
	LD	HL,100H
	LD	(DIDESV+4),HL	;DATENLAENGE LADEN
	CALL	FLOPPY		;1.SEKTOR SCHREIBEN
	LD	A,(IY+10)	;COMPLETION CODE
	BIT	6,A		;FEHLER ?
	JP	NZ,FTEST	;FEHLERTEST
;
; RESTLICHE DATENSEKTOREN DER DIRECTORY SCHREIBEN
;
	CALL	CLEAR		;SPEICHER LOESCHEN
	LD	A,-1		;-1 AM SEKTORANFANG,DA SEKTOR	
	LD	(DIRSEC),A	;OHNE INFORMATION
;
	LD	HL,SEKTAB	;SEKTORADRESSTABELLE
	LD	B,4		;WEITERE 4 SEKTOREN DER DIRECTORY
WRLOOP
	LD	A,(DRIVAD)	;DRIVEADRESSE
	ADD	A,(HL)		;AKTUELLE SEKTORADRESSE
	LD	(DIDESV+11),A	;IN VEKTOR LADEN
	INC	HL
	LD	A,(HL)		;SPUR
	LD	(DIDESV+12),A	;IN VEKTOR LADEN
	INC	HL
	PUSH	HL		;POSITION RETTEN
	PUSH	BC		;B-REGISTER RETTEN
	LD	HL,100H
	LD	(DIDESV+4),HL	;DATENLAENGE LADEN
	CALL	FLOPPY		;SEKTOR SCHREIBEN
	POP	BC
	POP	HL
	LD	A,(DIDESV+10)	;COMPLETION CODE
	BIT	6,A		;FEHLER ?
	JP	NZ,FTEST	;FEHLERTEST
	DJNZ	WRLOOP		;NAECHSTER SEKTOR
;
; POINTERSEKTOR DER DIRECTORY SCHREIBEN (1602)
;
	CALL	CLEAR		;SPEICHER LOESCHEN
	LD	HL,POITAB	;POINTERTABELLE
	LD	DE,DIRSEC
	LD	BC,14
	LDIR			;ZEIGER DER DIRECTORY IN POINTERSEKTOR EINTR.
;
	LD	HL,10		;RELAT. ADRESSE DES LETZTEN ZEIGERS IM
				;POINTERSEKTOR BZGL. DES SEKTORANFANGES
	LD	(DIRSEC+0FAH),HL
;
	LD	HL,-1		;BACKPOINTER DES POINTERSEKTORS
	LD	(DIRSEC+0FEH),HL
;
	LD	HL,1600H	;FOREPOINTER DES POINTERSEKTORS (DESCR.-ADR.)
	LD	(DIRSEC+0FCH),HL
;
	LD	HL,100H		;DATENLAENGE
	LD	(DIDESV+4),HL
	LD	HL,1602H
	LD	A,(DRIVAD)
	ADD	A,L
	LD	L,A
	LD	(DIDESV+11),HL	;SPUR/SEKTORADR. LADEN
	CALL	FLOPPY
	LD	A,(DIDESV+10)
	BIT	6,A
	JP	NZ,FTEST	;FEHLER

;***ERZEUGEN DER BITMAP***
erbimap procedure()
{
	register i, j;

	i = 4*80+4;

	clr_buf();
	LD	BC,4*80+4	;4 * 80 SPUREN + PRUEFSUMMEN
	LDIR			;LOESCHEN

	LD	(HL),77H
	LD	BC,1BH
	LDIR
;
; Adresse fuer die Anzahl belegter und freier Sektoren erzeugen
;
	LD	A,(CMDF)	;aktuelle Diskettenkonfiguration laden
	LD	B,A
	PUSH	HL		;<HL> speichern
	LD	HL,BELTAB	;laden mit dem Anfangswert der Belegungstabelle
	LD	DE,7		;Wert fuer die Verschiebung
	JR	DECB
ABT	ADD	HL,DE
DECB	DJNZ	ABT
	POP	DE		;Adresse aus <HL> in <DE> zurueck
;
; Eintragen der Werte aus der Tabelle
;
	LD	BC,7		;alle restlichen Werte eintragen
	LDIR			;belegte Sektoren eintragen
;
	XOR	A
	LD	(DE),A
	INC	DE
	LD	(DE),A
;
	LD	HL,UNBEN
	LD	DE,UNBEN+1
	LD	BC,80H
	LD	(HL),0
	LDIR			;REST ZU 200H LAENGE UNBENUTZT
;
	LD	HL,30E6H
	LD	(SPUR16),HL	;SPUR 16H, SEKTOREN 0,1,2,5,6,A,B BELEGT SETZEN
	LD	A,0C0H
	LD	(SPUR17),A	;SPUR 17H, SEKTOREN 0,1 BELEGT SETZEN
;
; Belegte Sektoren entsprechend Diskettentyp eintragen
;
	LD	A,(CMDF)
	CP	5
	RET	Z
	CP	4
	JR	Z,BSB32
;
; Rest kannn nur noch 40-Spuren-Diskette sein.
;
	LD	HL,SPUR28	;SPUREN 28H-4CH NICHT VORHANDEN
	LD	(HL),0FFH	;--> BELEGT SETZEN
	LD	DE,SPUR28+1
	LD	BC,4*28H-1	;28H SPUREN
	LDIR
;
BSB32	LD	HL,SPUR0	;SPUR 0-4CH, SEKTOREN 10H-1FH NICHT VORHANDEN
	LD	B,80		;--> BELEGT SETZEN
ERB1:	INC	HL
	INC	HL
	LD	(HL),0FFH	;SEKTOREN 10H-17H
	INC	HL
	LD	(HL),0FFH	;SEKTOREN 18H-1FH
	INC	HL
	DJNZ	ERB1
	RET

;***ERZEUGEN DES DIRECTORYDESCRIPTORS UND DES 1. DIRECTORYSEKTORS***

ERDIDE	LD	HL,KDIDES	;DESCRIPTORKONSTANTEN
	LD	DE,DIRDES	;DATENFELD
	LD	BC,24
	LDIR			;KONSTANTEN UMLADEN
	LD	HL,DATE		;AKTUELLES DATUM
	LD	BC,8
	PUSH	HL
	PUSH	BC
	LDIR			;DATE OF CREATION
	POP	BC
	POP	HL
	LDIR			;DATE OF LAST MODIFICATION
	LD	H,D
	LD	L,E
	INC	DE
	LD	(HL),0		;REST MIT 0 FUELLEN
	LD	BC,215
	LDIR
	LD	HL,1602H
	LD	(DIRDES+80H),HL	;ADRESSE DES POINTERSEKTORS DER DIRECTORY
;
	LD	HL,KDIREC1	;KONSTANTEN FUER 1.DIR.-SEKTOR
	LD	DE,DIRSEC
	LD	C,13
	LDIR			;UMLADEN
	RET

;***DIRECTORYDESCRIPTORKONSTANTEN***

KDIDES	DEFW	0		;NICHT BENUTZT
	DEFW	0		;NICHT BENUTZT
	DEFW	0		;NICHT BENUTZT
	DEFW	1605H
	DEFW	1605H		;ERSTER SEKTOR
	DEFW	160BH		;LETZTER SEKTOR
	DEFB	40H		;FILETYP=DIRECTORY
	DEFW	5		;RECORDANZAHL=5
	DEFW	100H		;RECORDLAENGE=100H
	DEFW	100H		;BLOCKLAENGE=100H
	DEFB	0F0H		;PROPERTIES='WELS'
	DEFW	0
	DEFW	0

;***KONSTANTEN FUER 1.DIR.-SEKTOR***

KDIREC1	DEFB	89H		;BIT 7=1 DA SECRET, 9 BUCHSTABEN
	DEFM	'DIRECTORY'	;NAME
	DEFW	1600H		;ADRESSE DES DESCRIPTORS
	DEFB	-1		;LETZTE ANGEGEBENE DATEI


;***SEKTORADRESSTABELLE DER DIRECTORY***

POITAB	DEFW	1600H
	DEFW	1605H
SEKTAB	DEFW	160AH
	DEFW	1601H
	DEFW	1606H
	DEFW	160BH
	DEFW	-1		;NACHFOLGER FEHLT

; Tabelle fuer Anzahl belegter und freier Sektoren und Diskettenwerte

BELTAB
	DEFW	1929		;Typ 2, 40-Spuren-Diskette
	DEFB	0
	DEFB	16
	DEFB	40
	DEFW	631

	DEFW	9		;Typ 5, 80-Spuren-Diskette, doppelseitig
	DEFB	1
	DEFB	32
	DEFB	80
	DEFW	2551


; Tabelle der Datenlaengen fuer die Sektorinformationen

DLTAB
	DEFB	0DH
	DEFB	10H
	DEFB	10H
	DEFB	10H
	DEFB	20H
	DEFB	24H
	DEFB	28H
	DEFB	3CH
	DEFB	40H


;***SCHREIBEN DER BITMAP AUF DISKETTE***

WRBIMA	LD	IY,BITMAV	;VEKTOR FUER BITMAP SCHREIBEN
	LD	A,(DRIVAD)	;DRIVEADRESSE
	LD	(IY+11),A	;IN VEKTOR LADEN
	CALL	FLOPPY		;SCHREIBEN AUF DISKETTE
	RET

;
; ***SCHREIBEN VON SEKTOREN AUF FORMATIERTE DISKETTE***
;	INPUT:	HL - PUFFERANFANGSADRESSE
;		BC - DATENLAENGE
;		DE - SPURNUMMER / SEKTORNUMMER
;
FLPWRT:	LD	IY,RDVEKT
	LD	(IY+0),0	;UNIT 0
	LD	(IY+1),0EH	;WRITE BINARY
	LD	(RDVEKT+2),HL
	LD	(RDVEKT+4),BC
	LD	A,(DRIVAD)
	OR	E
	LD	E,A
	LD	(RDVEKT+11),DE
	CALL	FLOPPY
	LD	A,(RDVEKT+10)
	BIT	6,A
	RET	Z
	POP	DE
	JP	FTEST


; DISK ALLOCATION MAP
;
BITMAP	DEFS	18H		;DISK ID (MAX. 24 ZEICHEN)
MAPTAB
SPUR0	DEFS	54H		;SPUREN 0-14H
SPUR15	DEFS	4		;SPUR 15H
SPUR16	DEFS	4		;SPUR 16H
SPUR17	DEFS	44H		;SPUREN 17H-27H
SPUR28	DEFS	94H		;SPUREN	28H-4CH
	DEFS	4		;=0
	DEFS	0AH		;=33H
KZ1	DEFS	2		;=33F7H USER-DISK BZW. =38F8H SYSTEM-DISK
	DEFS	1BH		;=77H
BELEGT	DEFS	2		;ANZAHL DER BELEGTEN SEKTOREN
	DEFS	2		;=0
KZ2	DEFS	1		;=4BH USER-DISK BZW. =4AH SYSTEM-DISK
FREI	DEFS	2		;ANZAHL DER FREIEN SEKTOREN
	DEFS	2		;=0
UNBEN	DEFS	80H		;UNBENUTZT (NUR REST ZU RL=200)
;
DIRDES	DEFS	100H		;DIRECTORYDESCRIPTORSPEICHER
DIRSEC	DEFS	100H		;DIRECTORYSEKTORSPEICHER
;
URLAD	DEFS	100H
OSLOAD	DEFS	300H
GET	DEFS	400H
SAVE	DEFS	0E00H
PFRL	DEFS	4000H
RAMEND	defs	0
