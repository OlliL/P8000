/* IDENT: 010487 - SOREXD.H */
/*
** Autor: Buchert
*/

/* Die Sortierfeldbeschreibungen sind in einem zusammenhaengenden Bereich
** in der Reihenfolge ihrer Anwendung abzulegen.
**
** Globale Symbole, die vom Nutzer der Sortierroutinen sorta() und sorto()
** vor dem ersten Aufruf von sorta() veraendert werden koennen.
** Andernfalls werden die im Kommentar angegebenen Werte verwendet.
*/

extern	int	uksort();	/* Compare Key by Type			*/

extern	int	sort_l;		/* max. Laenge der zu sortierenden Saetze,
				** 256 Bytes */
extern	int	sort_v;		/* Eingabesatzformat, 0 - Zeichenketten */
extern	int	(*sort_c)();	/* Adr. der Vergleichsfunktion, &strcmp() */
extern	char	*sort_f;	/* Name der Sortierarbeitsdatei, "sort.tmp" */
extern	int	sort_r;		/* Sortierrichtung, aufsteigend - 0 */
extern	int	sort_n;		/* Anzahl der uebergebenen Sortierfeldbeschrei-
				** bungen, 0 */
extern	int	*f_p;		/* Adr. der 1. Sortierfeldbeschreibung, NULL */


/* Festlegungen fuer die einzelnen Feld-Typen */

/* #define	D_Z   0	  	 unspecified */
/* #define D_B   6		 byte integer; 8-bit signed 2's-complement integer */
/* #define D_BU  2		 byte logical; 8-bit unsigned quantity */
#define D_W   7		/* word integer; 16-bit signed 2's-complement integer */
/* #define D_WU  3		 word logical; 16-bit unsigned quantity */
#define D_L   8		/* longword integer */
			/*32-bit signed 2's-complement integer */
/* #define D_LU  4		 longword logical; 32-bit unsigned quantity */
/* #define D_Q   9		 quadword integer */
			/* 64-bit signed 2's-complement integer */
/* #define D_QU  5		/* quadword logical; 64-bit unsigned quantity */
/* #define D_O   26	 octaword integer */
			/* 128-bit signed 2's-complement integer */
/* #define D_OU  25	 octaword logical; 128-bit unsigned quantity */
#define D_F   10	/* F_floating */
			/* 32-bit single-precision floating point */
#define D_D   11	/* D_floating */
			/* 64-bit double-precision floating point */
/* #define D_G   27	 G_floating */
			/* 64-bit double-precision floating point */
/* #define D_H   28	 H_floating */
			/* 128-bit quadruple-precision floating point */
#define D_T   14	/* a single character or a string */
/* #define D_NU  15	 numeric string, unsigned */
/* #define D_NL  16	 numeric string, left separate sign */
/* #define D_NLO 17	 numeric string, left overpunched sign */
/* #define D_NR  18	 numeric string, right separate sign */
/* #define D_NRO 19	 numeric string, right overpunched sign */
/* #define D_NZ  20	 numeric string, zoned sign */
#define D_P   21	/* packed decimal string */

#define	RECORD	0	/* RECORD-Sortierprozess */
#define	TAG	1	/* TAG-Sortierprozess */
