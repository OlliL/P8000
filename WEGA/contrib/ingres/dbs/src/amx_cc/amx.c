# include	"amx.h"

# ifdef MSDOS
/*	ALL INDES catalog names					  */
char		AA_ADMIN[]	= "admin";
char		AA_ATT[]	= "domain";
char		AA_INDEX[]	= "indexes";
char		AA_REL[]	= "relation";
char		*AAdatabase;
# else
# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif
# endif

/* ACCESS PATH globals */
int		AP_level;	/* access path level		  */
int		AP_stmt;	/* statement count		  */
int		AP_label[MAX_LAB];	/* labels for retrieve	  */
int		AP_index;	/* index path descriptor	  */
int		AP_low;		/* 1st tuple width for key values */
int		AP_high;	/* 2nd tuple width for key values */
int		AP_ilow;	/* == 0: if primary access path   */
				/*  < 0: if indexed access path   */
int		AP_ihigh;	/* == 0: if primary access path   */
				/*  < 0: if indexed access path   */
int		AP_mode;	/* mode of access		  */
AMX_KEY		AP_key[MAXDOM + 1];

/* LOCK globals */
int		Xlocks;
int		No_locks;
int		Lock_mode;
# ifdef PAGE_LOCKING
int		Dead_lock;
int		Lock_level;
int		Read_lock;
int		Max_locks;
int		Time_out;
# endif

/* uniq relation name == AA_'Relcount' */
int		Relcount;

/* pointer to symbol table of relation names */
char		*Reltab;
AMX_REL		*Temp_rel;

/* pointer to current active relation */
AMX_REL		*Relation;

/* pointer to actual scanned relation */
AMX_REL		*Scan_rel;

/* curent DML-statement: RETRIEVE, APPEND, DELETE or APPEND	  */
int		Qry_mode;

/* catalog relation descriptors
DESC		Reldes == AAdmin.adreld;
DESC		Attdes == AAdmin.adattd;			  */
DESC		Indes;

/* count of additional buffers to the access methods */
int		Xbufs;

/* temp file for globals and main() */
int		FILE_msg = -1;
FILE		*FILE_inp;
FILE		*FILE_amx;
FILE		*FILE_scn;
FILE		*FILE_tmp;
int		FILE_rel = -1;

# ifdef SETBUF
# ifdef INP_STACK
char		*BUF_inp;
# else
char		BUF_inp[BUFSIZ];
# endif
# ifdef AMX_STACK
char		*BUF_amx;
# else
char		BUF_amx[BUFSIZ];
# endif
# ifdef SCN_STACK
char		*BUF_scn;
# else
char		BUF_scn[BUFSIZ];
# endif
# ifdef TMP_STACK
char		*BUF_tmp;
# else
char		BUF_tmp[BUFSIZ];
# endif
# endif

# ifdef AMX_SC
/* seperate compiling */
int		ExpImp;		/* Export/Import flag		  */
FILE		*FILE_exp;	/* file pointer stack		  */
int		Line_exp;	/* line counter stack		  */
# endif

/* name of files */
# ifdef MSDOS
char		Filename[] = "###pid##";
# else
char		Filename[] = "/tmp/amx#.#pid#";
# endif
char		*Filespec  = &Filename[Filepos];
char		*File_src;	/* name of file.x		  */

/* global db-variables */
char		*AAusercode;
char		*AApath;
# ifndef MSDOS
char		*AAdbpath;
# else
char		*AAdbpath[DBP_MAX];
# endif

/* assignment stack */
struct amx_format	Destination;
struct amx_format	Source;

/* DOMAIN handling */
int		Dom;		/* actual domain		  */
int		Domains;	/*  # of domains		  */
i_1		Dom_set[MAXDOM];

# ifdef TRACE_ALLOC
char	*talloc(size)
register int	size;
{
	register char		*ptr;
	extern char		*malloc();

	ptr = malloc(size);
	printf("ALLOC: %06o (%4d)\n", ptr, size);
	return (ptr);
}


tfree(ptr)
register char	*ptr;
{
	printf("FREE:  %06o\n", ptr);
	return (free(ptr));
}
# endif
