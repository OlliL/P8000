# include	<stdio.h>
# ifdef MSDOS
# include	"../h/gen.h"
# include	"../h/ingres.h"
# else
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# endif
# include	"../h/access.h"
# ifdef MSDOS
# include	"../h/aux#.h"
# else
# include	"../h/aux.h"
# endif
# include	"../h/catalog.h"
# include	"../h/lock.h"
# include	"../h/bs.h"
# include	"parser.h"

# ifndef bitset
# define	bitset(b, s)	((b) & (s))
# define	clrbit(b, s)	((s) &= ~(b))
# define	setbit(b, s)	((s) |= (b))
# endif

# ifdef MSDOS
extern char	AA_ADMIN[];
extern char	AA_ATT[];
extern char	AA_INDEX[];
extern char	AA_REL[];
# define	AA_atof		v6_atof
# define	AA_atol		v6_atol
# define	AA_iocv		iocv
# define	AA_itoa		v6_itoa
# define	AA_locv		locv
# define	AAam_clearkeys	clearkeys
# define	AAam_error	acc_err
# define	AAam_exit	acc_close
# define	AAam_find	find
# define	AAam_flush_rel	flush_rel
# define	AAam_get	get
# define	AAam_setkey	setkey
# define	AAam_start	acc_init
# define	AAbequal	bequal
# define	AAbmove(s,d,l)	memcpy(d,s,l)
# define	AAcc_init	Acc_init
# define	AAdbname	Dbname
# define	AAdbpath	Dbpath
# define	AAdmin		Admin
# define	AAexitfn	ExitFn
# define	AAgetequal	getequal
# define	AAinpclose	inpclose
# define	AAkcompare	kcompare
# define	AAlength	length
# define	AApath		Pathname
# define	AArelclose	closer
# define	AAscompare	scompare
# define	AAsmove		smove
# define	AAsyserr	syserr
# define	AAtTrace	tTrace
# define	AAusercode	Usercode
# define	AAztack		ztack
# define	c_12		c_8
# define	attribute	domain
typedef struct accbuf		ACCBUF;
extern char	*AAdatabase;
extern char	*Parmvect[];
# define	RELROPEN(fp)	((fp + 1) *  5)
# define	RELWOPEN(fp)	((fp + 1) * -5)
# endif

# ifdef AMX_QRYMOD
struct amx_qrymod
{
	struct amx_qrymod	*qm_next;
	short			qm_opset;
	char			qm_term[PROTERM];
	short			qm_todbgn;
	short			qm_todend;
	char			qm_dowbgn;
	char			qm_dowend;
	short			qm_domset[8];
};
typedef struct amx_qrymod	AMX_QM;
# endif

struct amx_attribute
{
	c_12			att_name[MAXNAME + 1];
	i_1			att_id;
	i_1			att_key;
	i_1			att_type;
	i_1			att_len;
	i_2			att_off;
};
typedef struct amx_attribute	AMX_ATT;

struct amx_relation
{
# ifdef AMX_QRYMOD
	struct amx_qrymod	*rel_qm;
	c_2			rel_owner[2];
# endif
	c_12			rel_id[MAXNAME + 1];
	i_1			rel_uniq;
	i_1			rel_index;
	i_1			rel_spec;
	i_2			rel_status;
	i_2			rel_wid;
	i_2			ind_wid;
	i_1			ind_dom[MAXKEYS + 1];
	i_1			rel_atts;
	struct amx_attribute	rel_dom[MAXDOM];
};
typedef struct amx_relation	AMX_REL;

/* struct amx_relation.rel_status bits */
# define	AMX_RETRIEVE	000001	/* retrieve perm == PRO_RETR	    */
# define	AMX_REPLACE	000002	/* replace perm  == PRO_REPL	    */
# define	AMX_DELETE	000004	/* delete perm   == PRO_DEL	    */
# define	AMX_APPEND	000010	/* append perm   == PRO_APP	    */
# define	AMX_CATALOG	000020	/* catalog relation		    */
# define	AMX_PRIMARY	000040	/* relation has indexes		    */
# define	AMX_INDEX	000100	/* index relation		    */
# define	AMX_INTERNAL	010000	/* catalog or index relation	    */

struct amx_format
{
	i_1			type;
	i_1			len;
	i_2			off;
	char			*c_var;	/* off == -1			    */
};
typedef struct amx_format	AMX_FRM;

struct amx_key
{
	char			key_dom;
	char			key_cond;
};
typedef struct amx_key		AMX_KEY;

/* ACCESS PATH globals */
	/* ACCESS MODES */
# define	AP_NOKEY	000
# define	AP_LOWKEY	001
# define	AP_HIGHKEY	002
# define	AP_RANGEKEY	004
# define	AP_FULLKEY	010

	/* ACCESS CONDITIONS */
# define	AP_EQ		001
# define	AP_LT		002
# define	AP_LE		004
# define	AP_NE		010
# define	AP_GT		020
# define	AP_GE		040

extern int			AP_level;	/* access path level	    */
extern int			AP_stmt;	/* statement count	    */
# define	MAX_LAB		20
extern int			AP_label[MAX_LAB];	/* retrieve labels  */
extern int			AP_index;	/* index path descriptor    */
extern int			AP_low;		/* 1st tuple width for keys */
extern int			AP_high;	/* 2nd tuple width for keys */
extern int			AP_ilow;	/* == 0: if primary access  */
						/*  > 0: if indexed access  */
extern int			AP_ihigh;	/* == 0: if primary access  */
						/*  > 0: if indexed access  */
extern int			AP_mode;	/* mode of access	    */

/* LOCK globals */
# define	DB_SHARE	010

extern int			Xlocks;
extern int			No_locks;
extern int			Lock_mode;
# ifdef PAGE_LOCKING
extern int			Dead_lock;
extern int			Lock_level;
extern int			Read_lock;
extern int			Max_locks;
extern int			Time_out;
# endif

/* uniq relation name == AA_'Relcount' */
extern int			Relcount;

/* pointer to symbol table of relation names */
# define	MAX_RELS	30
extern char			*Reltab;
extern struct amx_relation	*Temp_rel;

/* pointer to current active relation */
extern struct amx_relation	*Relation;

/* pointer to actual scanned relation */
extern struct amx_relation	*Scan_rel;

/* cuurent DML-statement: RETRIEVE, APPEND, DELETE or APPEND */
extern int			Qry_mode;

/* catalog relation descriptors
extern DESC			Reldes == AAdmin.adreld;
extern DESC			Attdes == AAdmin.adattd;		    */
extern DESC			Indes;

/* count of additional buffers to the access methods */
extern int			Xbufs;

/* temp file for globals and main() */
extern char			*msg();
extern int			FILE_msg;
extern FILE			*FILE_inp;
extern FILE			*FILE_amx;
extern FILE			*FILE_scn;
extern FILE			*FILE_tmp;
extern int			FILE_rel;

# ifdef SETBUF
# ifdef MEM_N
# define INP_STACK
# define AMX_STACK
# define SCN_STACK
# define TMP_STACK
# endif

# ifdef INP_STACK
extern char			*BUF_inp;
# else
extern char			BUF_inp[BUFSIZ];
# endif

# ifdef AMX_STACK
extern char			*BUF_amx;
# else
extern char			BUF_amx[BUFSIZ];
# endif

# ifdef SCN_STACK
extern char			*BUF_scn;
# else
extern char			BUF_scn[BUFSIZ];
# endif

# ifdef TMP_STACK
extern char			*BUF_tmp;
# else
extern char			BUF_tmp[BUFSIZ];
# endif
# endif

# ifdef AMX_SC
/* seperate compiling */
extern int			ExpImp;		/* Export/Import flag	    */
						/* +1: Export  module	    */
						/*  0: Program module	    */
						/* -1: Import  module	    */
extern FILE			*FILE_exp;	/* file pointer stack	    */
extern int			Line_exp;	/* line counter stack	    */
# endif

/* name of temp files */
# ifdef MSDOS
# define	Filepos		0
# else
# define	Filepos		8
# endif
extern char			Filename[];
extern char			*Filespec;
extern char			*File_src;	/* name of file.x	    */

/* global db-variables */
extern char			*AAusercode;
/*     char			*AApath;		in aux.h	    */
# ifndef MSDOS
extern char			*AAdbpath;
# else
extern char			*AAdbpath[];
# endif

/* assignment stack */
extern struct amx_format	Destination;
extern struct amx_format	Source;

/* DOMAIN handling */
extern int			Dom;		/* actual domain	    */
extern int			Domains;	/*  # of domains	    */
extern i_1			Dom_set[MAXDOM];

/* character type mapping */
# define	ASCII_MASK	0177
# define	BYTE_MASK	0377
extern char			Cmap[];
# ifdef EBCDIC
# ifdef ESER_PSU
extern char			_etoa_[];
# define	ETOA(c)		_etoa_[(c) & BYTE_MASK]
# endif
# define	CMAP(c)		Cmap[(ETOA(c)) & ASCII_MASK]
# else
# define	CMAP(c)		Cmap[(c) & ASCII_MASK]
# endif

/* # define	TRACE_ALLOC */
# ifndef TRACE_ALLOC
# define	talloc		malloc
# define	tfree		free
# endif
