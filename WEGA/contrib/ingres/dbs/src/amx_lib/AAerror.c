# include	"AA_liba.h"

static char	*AAERR[] =
{
	"OPEN",					/*   0 */
	"CLOSE",				/*   1 */
	"APPEND",				/*   2 */
	"DELETE",				/*   3 */
	"DELETE.AAgetequal",			/*   4 */
	"REPLACE",				/*   5 */
	"REPLACE.AAgetequal",			/*   6 */
	"REPLACE.delete",			/*   7 */
	"FIND.isam",				/*   8 */
	"FIND.lisam",				/*   9 */
	"FIND.hisam",				/*  10 */
	"GET.getup",				/*  11 */
	"GET.getid",				/*  12 */
	"SETKEY.dom",				/*  13 */
	"SETKEY.cond",				/*  14 */
	"request",				/*  15 */
	"release",				/*  16 */
	"lock",					/*  17 */
	"unlock",				/*  18 */
	"get: not locked",			/*  19 */
	"append: not locked",			/*  20 */
	"delete: not locked",			/*  21 */
	"replace: not locked",			/*  22 */
	"release: not locked",			/*  23 */
	"refresh",				/*  24 */
	"GET.get",				/*  25 */
};

static char	*AAERR100[] =
{
	"not available",			/* 100 */
	"already open",				/* 101 */
	"not enough memory",			/* 102 */
	"has integrities",			/* 103 */
	"has protections",			/* 104 */
	"view",					/* 105 */
	"not open",				/* 106 */
	"concurrency confusion",		/* 107 */
	"relation outside in use",		/* 108 */
	"TA",					/* 109 */
	"permission denied",			/* 110 */
	"database lost",			/* 111 */
};

AAerror(err, msg)
register int	err;
register char	*msg;
{
	extern int	errno;
	extern int	AAccerror;

	printf("\n\007AMXERR:\t* * * * * * *\t%d\n", err);
	printf("\tBS\tpid\t%d\n", getpid());
	if (errno)
		printf("\tBS\terror\t%d\n", errno);
	if (AAccerror)
		printf("\tACCESS\terror\t%d\n", AAccerror);

	if (err < 100)
	{
		printf("\tAMX-RC\t\t%d\n", AA);
# ifdef MSDOS
		printf("bad %s: relation `%.10s'\n",
# else
		printf("bad %s: relation `%.14s'\n",
# endif
			AAERR[err], msg);
	}
	else
		printf("`%s': %s\n", msg, AAERR100[err -100]);

	fflush(stdout);
	exit(1);
}
