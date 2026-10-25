/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Header-Datei: dos.h
 
	Bearbeiter	: P. Hoge
	Datum		: 21.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

/* Umwandlung der Clusternummer in log. Sektoradresse 
 * Offset zu Datenbereichsbeginn
*/
#define DOSADR(x,y)   (((long)x-2)*y)

   
/* DCP-Disk Vereinbarungen */

/* Absolutadressen und Parameter der DCP-Disk */
struct dosdisc {
	int fd;			/* Filedeskriptor */
	unsigned long fat1;	/* Startadresse 1. FAT */
	int fatnum;		/* Anzahl der FAT's */
	unsigned long maindir;	/* Startadresse Stammverzeichnis */
	int dirnum;		/* Anzahl der Verzeichniseintraege */
	unsigned long data;	/* Startadresse Datenbereich */
	unsigned clustsiz;	/* Anzahl Byte / Cluster */
	unsigned nclust;	/* Anzahl der Cluster */
	int fatlen;		/* Anzahl Bit / FAT-Eintrag */
	unsigned vol;		/* Anzahl Sektoren gesamt */
};


/* Dateiattribute */
struct dostat { 
	unsigned char name[12];	/* Dateiname */
	char attr;
	int mode;		/* Dateimodus */
	int flag;		/* Zugriffsart */
	struct _date {		/* Datum der letzten Modifikation */
		int year;
		int month;
		int day;
	} date;
	struct _time {		/* Zeit der letzten Modifikation */
		int hour;
		int min;
		int sec;
	} time;
	unsigned first;		/* Startcluster-Nummer */
	long dim;		/* Dateigroesse */
}; 

#define _FILE 1102	/* Filetyp */
#define _SDIR 1103	/* Subdirectory */
#define _SYSDAT 1105	/* Systemdatei */
#define _BLOCKED 1106	/* verborgene Datei */

#define READ_ONLY 0x01
#define HIDDEN_FILE 0x02
#define SYSTEM_FILE 0x04
#define VOLUME_LABEL 0x08
#define SUBDIR 0x10
#define ARCHIVE 0x20

#define RO 01		/* nur Lesen */
#define RW 02		/* Lesen und Schreiben */

#define LMAX 50		/* Laenge Pfad-Name */

long	lseek();
