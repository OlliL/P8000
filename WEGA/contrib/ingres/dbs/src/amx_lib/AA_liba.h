# include	<stdio.h>
/* Flag MSDOS is predefined in MSDOS */
# ifdef MSDOS
# include	"../h/gen.h"
# else
# include	"../conf/gen.h"
# endif
# ifdef MSDOS
# include	"../h/ingres.h"
# include	"../h/aux#.h"
# else
# include	"../h/dbs.h"
# include	"../h/aux.h"
# endif
# include	"../h/access.h"
# include	"../h/symbol.h"
# include	"../h/catalog.h"
# include	"../h/lock.h"

# define	AMX_TRACE
# define	AM_TRACE

# ifdef MSDOS
# define	AAam_addbuf	acc_addbuf
# define	AAam_clearkeys	clearkeys
# define	AAam_delete	delete
# define	AAam_exit	acc_close
# define	AAam_flush_rel	flush_rel
# define	AAam_get	get
# define	AAam_insert	insert
# define	AAam_replace	replace
# define	AAam_setkey	setkey
# define	AAam_start	acc_init
# define	AAbequal	bequal
# define	AAbmove(s,d,l)	memcpy(d,s,l)
# define	AAcc_head	Acc_head
# define	AAccerror	Accerror
# define	AAdatabase	Database
# define	AAdbname	Dbname
# define	AAdbpath	Dbpath
# define	AAdmin		Admin
# define	AAdparam	paramd
# define	AAdumptid	dumptid
# define	AAerrfn		errfilen
# define	AAget_tuple	get_tuple
# define	AAgetequal	getequal
# define	AAgetpage	get_page
# define	AAicompare	icompare
# define	AAndxsearch	ndxsearch
# define	AApageflush	pageflush
# define	AApath		Pathname
# define	AApratt		printatt
# define	AAprkey		prkey
# define	AAprtup		printup
# define	AAput_tuple	put_tuple
# define	AArelclose	closer
# define	AArelopen	openr
# define	AAresetacc	resetacc
# define	AArhash		rhash
# define	AAsetcsl	setcsl
# define	AAsetdbl	setdbl
# define	AAsetpgl	setpgl
# define	AAsetrll	setrll
# define	AAstuff_page	stuff_page
# define	AAtTfp		tTfp
# define	AAunlall	unlall
# define	AAunlcs		unlcs
# define	AAunldb		unldb
# define	AAunlpg		unlpg
# define	AAunlrl		unlrl
# define	AAusercode	Usercode
# define	RELROPEN(fp)	((fp + 1) *  5)
# define	RELWOPEN(fp)	((fp + 1) * -5)
# define	attribute	domain
extern char			*AAdatabase;
extern char			*memcpy();
typedef struct accbuf		ACCBUF;
# else
extern char			*AAbmove();
# endif

# ifndef bitset
# define	bitset(b, s)	((b) & (s))
# define	clrbit(b, s)	((s) &= ~(b))
# endif

# define	DB_SHARE	0010

# define	AP_EQ		0001
# define	AP_LT		0002
# define	AP_LE		0004
# define	AP_NE		0010
# define	AP_GT		0020
# define	AP_GE		0040
# define	AP_P1		0100
# define	AP_P2		0200

# ifdef EBCDIC
# define	LAST_CHAR	0377
# else
# define	LAST_CHAR	0177
# endif

# define	AA_rel		AA_0
# define	AA_att		AA_1
# define	AA_ind		AA_2

struct descxx
{
	DESC			xx_desc;
	struct descxx		*xx_next;
	struct descxx		*xx_index;
	char			*xx_indom;
	struct descxx		*xx_ta;
	char			xx_refresh;
	char			xx_excl;
	short			xx_locks;
};
typedef struct descxx		DESCXX;

extern struct descxx		*AA_DESC;
extern struct descxx		*AA_TA;	/*   begin of transaction */
extern struct descxx		*AA_TE;	/*     end of transaction */
extern int			AA_NOLOCKS;
extern int			AA_LOCKS;
extern int			AA_PAT;
extern DESC			*AA_0;		/* relation  */
extern DESC			*AA_1;		/* attribute */
extern DESC			*AA_2;		/* indexes   */
extern int			AA;
extern long			AA_LOW;
extern long			AA_HIGH;
extern long			AA_TID;
extern char			*AA_STUP;
extern char			*AAdbpath;
extern int			errno;

/* UPDATE globals */
extern struct descxx		*AAp_desc;	/* global primary description	*/
extern char			*AAold_tuple;	/* global primary tuple pointer */
extern char			AA_SET_ALL_KEYS[];
extern long			AAdaemon;
