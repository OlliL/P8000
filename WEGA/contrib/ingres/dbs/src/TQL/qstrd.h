/* IDENT: 102159 - qstrd */
/* Design: van Treek */

/*
** Definition der Symbole fuer ausgelassene Funktionsargumente
*/

# ifndef	G_NILA
# define	G_NILA	(char *)(-1)	/* ausgelassener Adress-Parameter */
# endif

# ifndef	G_NILI
# define	G_NILI	(-1)		/* ausgelassener INT-Parameter */
# endif

# ifndef	G_NILL
# define	G_NILL	(-1l)		/* ausgelassener LONG-Parameter */
# endif

# ifndef	G_NILC
# define	G_NILC	(char)0xFF	/* ausgelassener CHAR-Parameter */
# endif

# ifndef	G_NILF
# define	G_NILF	(FILE *)(-1)	/* ausgelassener FILE-Pointer	*/
# endif

/*
** Definition des Macros DEBUG zum Ausschalten des Funktionsrufes
*/

# ifndef	gdebug

# define	gdebug(x,y)		/* DEBUG ausschalten */     \
{     \
char	*parm1, *parm2;     \
	parm1 = (char *)x;	/* Zuweisung wird durch Optimierung entfernt*/ \
	parm2 = (char *)y;	/* Zuweisung wird durch Optimierung entfernt*/ \
}

# endif

/****************************************************************************
*
* Definition der Makros zur Arbeit mit Interface
*
****************************************************************************/

# ifdef SPSU
# define fopen(a,b) \
		gu_fopen_eser(a,b)
# endif

/****************************************************************************
*
* Definition der Maximalen Feldgroessen bzw. Stackgroessen
*
****************************************************************************/

# ifndef NULL
# include <stdio.h> /* nur einziehen wenn nicht gleichzeitig rostrd.h */
# endif

/*********************** 32 BIT VARIANTE ANFANG ***************************/

# ifdef  SSVP

# ifndef QC_MAXPGR
# define QC_MAXPGR    32		/* geschacht. Programmdatein */
# endif

/*  HORN 13.1.89 Diese Definition wird auch in rostrd.h gebraucht, da sie
    in einer Strukturdefinition verwendet wird. */

# define QC_MAXLOOP   32		/* geschacht. DO WHILE-Schleifen */
# define QC_MAXIF     32		/* geschacht. IF-Anweisungen */
# define QC_MAXCASE   32		/* geschacht. DO CASE-Anweisungen */
# define QC_MAXFILSPEC	100	/* max.Laenge einer File-Angabe */
# define QC_MAXBEREICH	256	/* Eingabepufferlaenge */
# define	QC_MAXPROC	101	/* max.Anzahl an Prozeduren */
# define	QC_MAXDO	33	/* max.Anzahl an Do-Dateien */
# define	QC_MAXPARAMETER 17	/* max.Anzahl an DO-Parametern */

# else

/*********************** 16 BIT VARIANTE ANFANG ***************************/


# ifndef QC_MAXPGR
# define QC_MAXPGR    5 		/* geschacht. Programmdatein */
# endif

/*  HORN 13.1.89 Diese Definition wird auch in rostrd.h gebraucht, da sie
    in einer Strukturdefinition verwendet wird. */

# define QC_MAXLOOP   5 		/* geschacht. DO WHILE-Schleifen */
# define QC_MAXIF     5 		/* geschacht. IF-Anweisungen */
# define QC_MAXCASE   5 		/* geschacht. DO CASE-Anweisungen */
# define QC_MAXFILSPEC	40	/* max.Laenge einer File-Angabe */
# define QC_MAXBEREICH	256	/* Eingabepufferlaenge */
# define	QC_MAXPROC	10	/* max.Anzahl an Prozeduren */
# define	QC_MAXDO	5	/* max.Anzahl an Do-Dateien */
# define	QC_MAXPARAMETER 5	/* max.Anzahl an DO-Parametern */

# endif

/****************************************************************************
*
* Stackdefinitionen: Programmstack und Schleifenstack.
* Jedes Programm (DO NAME) hat MAXLOOP moegliche Schleifen.
* Es gibt MAXPGR Programmstacks.
*
****************************************************************************/

struct	 doparm
{
	int	do_flag;		/* Kenner: 0->keine Variable 1->Var.*/
	char	do_variable[40];	/* Variablen-Name */
};

struct	 loopstack			/* Schleifenstack */
{
	int	lcount; 		/* Stacktiefe */
	char	*position[QC_MAXLOOP];	 /* Satzposition */
	int	aktifcount[QC_MAXLOOP];  /* If-Tiefe */
	int	aktcasecount[QC_MAXLOOP];	/* Case-Tiefe */
};

struct	 pgrstack			/* Programmstack */
{
	char  pgrname[QC_MAXFILSPEC];	/* Programm-Name */
	struct loopstack *loop; 	/* Schleifenstack-Pointer */
	char   *last;			/* letzte Satzposition im Programm */
	struct	doparm	withparm[QC_MAXPARAMETER];	/* WITH */
	struct	doparm	paraparm[QC_MAXPARAMETER];	/* PARAMETERS */
};

struct	descr
{
	int	len;			/* Laenge der Zeichenkette */
	char	*string;		/* -> Zeichenkette */
};

/****************************************************************************
*
* Struktur des Parameterblocks, der an das RO-Interface uebergeben
* wird
*
****************************************************************************/


struct qi_parameter
{
	long	bereich;		/* ALL | NEXT */
	char	select; 		/* 'P' | 'S' */
	int	q_opsave;		/* FUER CONTINUE */
	int	fl_all; 		/* ALL-Flag */
	int	fl_record;		/* RECORD-Flag */
	int	fl_next;		/* NEXT-Flag */
	int	fl_parameter;		/* Parameter-Flag 0->ohne Parameter */
	char	*forp;			/* Bedingung */
	char	*to;			/* Speicher-Name */
	char	*file;			/* TOPAS-Datei */
	char	*input; 		/* Eigabedatei */
	char	*outfile;		/* Ausgabedatei */
	char	*field; 		/* Spaltenliste */
	char	*on;			/* Spaltenname */
	char	*mdm;			/* Entity-Name */
	char	*from;			/* Append-From */
	char	*delimit;		/* Begrenzungszeichen */
	char	*skey;			/* Schluesselbaumname */
	char	*pkey;
	char	*find;			/* Zeichenkette */
	char	*aus1;			/* Ausdruck 1 */
	char	*aus2;			/* Ausdruck 2 */
	char	*add;			/* Spaltenliste */
	char	*rep;			/*   -=-	*/

	int	sdf;			/* SDF */
	int	blank;			/* BLANK */
	int	extended;		/* EXTENDED */
	int	mdmfl;			/* MDM */
	int	structure;		/* STRUCTURE */
	int	off;			/* OFF*/
	int	rotop;			/* TOP */
	int	memory; 		/* MEMORY */
	int	qindex; 		/* INDEX */
	int	record; 		/* RECORD */
	int	setfkt;
	char	*form;			/* FORM */
	int	report; 		/* REPORT */
	char	*qheading;		/* HEADING */
	char	*with;			/* WITH bei JOIN */
	int	whileflag;		/* WHILE spezif. */
	int	read;			/* READFLAG F. USE */
	char	q_findsave[79]; 	/* FELD F.FIND UND LOCATE: CONTINUE */
	char	qi_frei[1];		/* Reserve */

};

/****************************************************************************
*
* Struktur des TQL-Steuerblocks q_tql
*
****************************************************************************/

struct	q_tql
{
	struct	ro_zvt	zvt;			/* zentrale Verarb.-Tabelle */
	int	q_terminal;			/* Terminalflag: 1 = TT: */
	int	q_error;			/* Fehlerwort:	 0 = okay */
	int	q_stopflag;			/* Stopwort: 1 = Abbruch */
	int	q_topkdo;			/* Kommando-Code */
	FILE	*q_kdofile;			/* Kommando-Eingabefile */
	char	*q_actsatz;			/* aktueller Eingabesatz */
	char	*q_kdoptr;			/* Kdo-Pointer */
	char	*q_kdosave;			/*    =-=      */
	int	q_rorc; 			/* RO-Rerurn-Code */
	struct	pgrstack q_pgrstack[QC_MAXPGR]; /* Programm-Stack */
	struct	loopstack q_loopstack[QC_MAXPGR];/* Schleifen-Stack */
	int	q_ifstack[QC_MAXIF];		/* IF-Stack */
	int	q_ifcount;			/* IF-Zaehler */
	int	q_casestack[QC_MAXCASE];	/* CASE-Stack */
	int	q_casecount;			/* CASE-Zaehler */
	struct	qi_parameter qi_roparm; 	/* Parameterblock Interface */
	char	q_parmbereich[QC_MAXBEREICH];	/* Parameterbereich */
	char	*q_proctab;			/* Prozedur-Tabelle */
	char	*q_dotab;			/* kdo-Datei-Tabelle */
	char	*q_procspace;			/* Speicher f.Procedure-Datei */
	char	*q_aliastab;			/* Aliasnamen-Tabelle */
	int	q_assist;			/* 0:aus 1:ein */
	char	*q_lastparm;			/* Letzter gelexter Parameter */
	int	q_fullscr;			/* 0:aus 1:ein */
	int	q_stmflag;			/* Debugger:TQL-Kdo -Flag */
	int	q_frei[7];			/* Reserve */
};

/*****************************************************************
*
* Struktur des Prozedurstacks
*
* Der Stack beinhaltet die Namen und die Adressen der sich im HS
* befindlichen Prozeduren.
*
*****************************************************************/

struct	qc_proctab
{
	char	procname[QC_MAXFILSPEC];	/* Prozedurname */
	char	*procptr;			/* Prozeduradresse */
};
