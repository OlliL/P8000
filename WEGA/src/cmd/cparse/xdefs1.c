# include "mfile1"

static char sccsid[] = "@[$]xdefs1.c\t2.3  08/24/83 15:25:15 Copr. 1981 87wega3.2.";

/*	communication between lexical routines	*/

int	ncharsig = 125;	/* significant characters of a name (-L: 8) */
int	regflag = 0;	/* -O: register variables also for ... (see oalloc) */
int	Fflag = 0;	/* -F: write the names of the source and of each function into a file */
int	reached = 1;	/* true if statement can be reached... */
char	ftitle[100] = "";   	/* title of the file */
int	lineno = 0;		/* line number of the input file */
struct strlist *strhead = 0;	/* the collected string constants */
struct strlist *strcur = 0;
int	symcount = 1;	/* the number of the next symbol of the .S file */
int	ftnno = 0;  /* "current" function number */
int	ntabindx = 0;	/* the next free byte for the names of locals in ntab */
int	ntab_globals = NTABSZ;	/* the first byte of the names of globals in ntab */
int	tmpno = 0;	/* the number of the next TEMP node */
int	tmpoff = 0;	/* the offset of the next TEMP in the frame */
int	retlab = NOLAB;
int	curloc = DATA;	/* the current location counter */
int	crslab = 10;	/* the next label */

/* save array for break, continue labels, and flostat */

int asavbc[BCSZ];
int *psavbc = asavbc ;

CONSZ lastcon;  /* the last constant read by the lexical analyzer */
double dcon;   /* the last double read by the lexical analyzer */

/*	symbol table maintainence */

struct symtab *curftn;  /* "current" function */

int	curclass,	  /* current storage class */
	instruct,	/* "in structure" flag */
	stwart,		/* for accessing names which are structure members or names */
	blevel,		/* block level: 0 for extern, 1 for ftn args, >=2 inside function */
	curdim;		/* current offset into the dimension table */

OFFSZ	dimtab[ DIMTABSZ ];

int	paramstk[ PARAMSZ ];  /* used in the definition of function parameters */
int	paramno;	  /* the number of parameters */
OFFSZ	autooff,	/* the next unused automatic offset */
	strucoff;	/*  the next structure offset position */
int	regvar;		/* the next free register for register variables */
int	minrvar;	/* the smallest that regvar gets witing a function */
int	fregvar, fminrvar;	/* the same for the float registers */
int	lregvar;	/* the next register pair for a long register variable */
OFFSZ	inoff;		/* offset of external element being initialized */

struct sw swtab[SWITSZ];  /* table for cases within a switch */
struct sw *swp;  /* pointer to next free entry in swtab */
int swx;  /* index of beginning of cases for current switch */

int strflg;  /* if on, strings are to be treated as lists */

struct symtab *idname;	/* tunnel to buildtree for name id's */

NODE node[TREESZ];

int brklab;
int contlab;
int flostat;
int retstat;

static char *
ccnames[] = { /* names of storage classes */
	"SNULL",
	"AUTO",
	"EXTERN",
	"STATIC",
	"REGISTER",
	"EXTDEF",
	"LABEL",
	"ULABEL",
	"MOS",
	"PARAM",
	"STNAME",
	"MOU",
	"UNAME",
	"TYPEDEF",
	"notused",
	"ENAME",
	"MOE",
	"notused",
	"USTATIC",
	};

char * scnames( c ) register c; {
	/* return the name for storage class c */
	static char buf[12] = "";
	if( c&FIELD ){
		if( sprintf( buf, "FIELD[%d]", c&FLDSIZ ) < 0 ) syserr( "sprintf in scnames" );
		return( buf );
		}
	return( ccnames[c] );
	}
