/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS -- interface definitions
*/

# include	<stdio.h>
# ifdef MSDOS
# include	"../h/gen.h"
# else
# include	"../conf/gen.h"
# endif
# if (REDABAS == 2)
# include	"db2.h"
# endif
# if (REDABAS == 3)
# include	"db3.h"
# endif
# ifdef MSDOS
# include	"../h/ingres.h"
# include	"../h/unix.h"
# include	"../h/symbol.h"
# else
# include	"../h/dbs.h"
# include	"../h/bs.h"
# endif
# include	"../h/access.h"
# include	"../h/lock.h"

# define	DBNAME		10	/* max. field name length	*/
# define	DB_FLDLAST	13	/* last field mark		*/
# define	DB_RECLAST	26	/* last record mark		*/
# define	DB_RECVAL	' '	/* valid record mark 		*/
# define	DB_RECDEL	'*'	/* deleted record mark 		*/

# define	FLD_RECNO	'#'	/* dummy field: record number	*/
# define	FLD_DELMARK	'*'	/* dummy field: erase mark	*/

# define	TYPE_CHAR	'C'	/*				*/
# define	TYPE_DATE	'D'	/*	REDABAS			*/
# define	TYPE_LOG	'L'	/*		DATA		*/
# define	TYPE_NUM	'N'	/*		TYPES		*/
# define	TYPE_INT	'I'	/*				*/
# if (REDABAS == 3)
# define	TYPE_MEMO	'M'	/*				*/
# endif

# define	CMD_COMMENT	':'	/*				*/
# define	CMD_READ	'R'	/*				*/
# define	CMD_WRITE	'W'	/*	DBI			*/
# define	CMD_DB		'I'	/*		command		*/
# define	CMD_REL		'T'	/*		line		*/
# define	CMD_FIELD	'F'	/*		identification	*/
# define	CMD_DOMAIN	'D'	/*				*/
# define	CMD_REF		'@'	/*				*/
# if (REDABAS == 2)
# define	CMD_DBD		'2'
# endif
# if (REDABAS == 3)
# define	CMD_DBD		'3'
# endif

# define	PTR_NAME	0
# define	PTR_NO		1
# define	PTR_TYPE	2
# define	PTR_DEC		3
# define	TABS		5	/* max. useable relations	*/

struct db_hdr
{
	word		db_records;
	short		db_day;
	short		db_month;
	short		db_year;
	short		db_reclen;
	short		db_hdrlen;
	short		db_fields;
};

struct db_fld
{
	char		*db_f_adr;
	char		db_f_name[DBNAME + 1];
	char		db_f_type;
	char		db_f_len;
	char		db_f_dec;
};

extern struct db_hdr	Db_hdr;		/* REDABAS header		*/
extern struct db_fld	Db_fld[];	/* REDABAS fields		*/
extern long		Recno;		/* REDABAS record count		*/
extern char		Record[];	/* REDABAS record		*/
extern char		RW_flag;	/* read/write flag		*/
extern DESC		*Desc;		/* actual relation descriptor	*/
extern DESC		Desc_s[];	/* descriptor cache		*/
extern short		Refs[];		/* DB/REDABAS-references	*/
extern short		Flds[];		/* REDABAS/DB-references	*/
extern char		List_hdr;	/* flag to list header		*/
extern char		List_fld;	/* flag to list fields		*/
extern char		List_rec;	/* flag to list records		*/
# if (REDABAS == 3)
extern char		III_plus;	/* REDABAS 3+ flag		*/
# endif

# define	BYTE_LOG2	8
# define	WORD_LOG2	16
# define	BYTE_MASK	255
# define	WORD_MASK	65535L

# ifdef MSDOS
# define	AA_atof 	v6_atof
# define	AA_atol 	v6_atol
# define	AA_ftoa 	ftoa
# define	AA_iocv 	iocv
# define	AA_itoa 	v6_itoa
# define	AA_locv 	locv
# define	AAam_addbuf	acc_addbuf
# define	AAam_clearkeys	clearkeys
# define	AAam_error	acc_err
# define	AAam_exit	acc_close
# define	AAam_find	find
# define	AAam_flush_rel	flush_rel
# define	AAam_get	get
# define	AAam_insert	insert
# define	AAam_setkey	setkey
# define	AAam_start	acc_init
# define	AAbequal	bequal
# define	AAbmove 	bmove
# define	AAcc_init	Acc_init
# define	AAdatabase	Database
# define	AAdbname	Dbname
# define	AAdbpath	Dbpath
# define	AAdmin		Admin
# define	AAgetequal	getequal
# define	AAkcompare	kcompare
# define	AAlength	length
# define	AApath		Pathname
# define	AApmove 	pmove
# define	AAproc_name	Proc_name
# define	AArelclose	closer
# define	AArelopen	openr
# define	AAscompare	scompare
# define	AAsequal	sequal
# define	AAsetrll	setrll
# define	AAsmove 	smove
# define	AAtTrace	tTrace
# define	AAunlrl 	unlrl
# define	AAusercode	Usercode
# define	AAztack 	ztack
# define	USERDBS		USERING_DB
# define	attribute	domain
# define	c_12		c_8
# define	setbuf(fp, bp)
# define	typeunconv(t)	(t)
extern char	*AAdatabase;
extern char	*Parmvect[];
extern char	AA_ADMIN[];
extern char	AA_ATT[];
extern char	AA_INDEX[];
extern char	AA_REL[];
typedef struct accbuf		ACCBUF;
typedef struct {  char i1type;}	I1TYPE;
typedef struct {  long i4type;}	I4TYPE;
typedef struct { float f4type;}	F4TYPE;
typedef struct { short i2type;}	I2TYPE;
typedef struct {double f8type;}	F8TYPE;
# endif
