# include	"../conf/conf.h"

/*								*/
/*	for real values see gutil/gen.c				*/
/*								*/

/*	Character defining delimiter of error messages.		*/
/*	Normally:	'~'.					*/

# define	ERRDELIM		'~'


/*	Standard location of the DBS files			*/

extern char	AA_PASSWD[];
extern char	AA_USERS[];
extern char	DBTMPLTFILE[];
extern char	AA_ERRFN[];
extern char	KSORT[];
extern char	PURGE[];
extern char	SYSMOD[];
extern char	MKDIR[];
extern char	RMDIR[];
extern char	USE_CREATDB[];
extern char	USE_DBS[];


/*	ALL DBS catalog names					*/

extern char	AA_ADMIN[];
extern char	AA_REL[];
extern char	AA_ATT[];
extern char	AA_INDEX[];
extern char	AA_INTEG[];
extern char	AA_PROTECT[];
extern char	AA_TREE[];
extern char	AA_REF[];
extern char	AA_DBS_LOCK[];


/*	All SYS names and relations				*/

extern char	SYSNAME[];
extern int	s_SYSNAME;	/* sizeof SYSNAME - 1		*/
extern char	SYSBATCH[];
extern int	s_SYSBATCH;	/* sizeof SYSBATCH - 1		*/
extern char	SYSSORT[];
extern char	MODBATCH[];
extern int	s_MODBATCH;	/* sizeof MODBATCH - 1		*/
extern char	MODTEMP[];
extern char	MODPREBATCH[];
extern char	ISAM_SORTED[];
extern char	ISAM_DESC[];
extern char	ISAM_SPOOL[];
