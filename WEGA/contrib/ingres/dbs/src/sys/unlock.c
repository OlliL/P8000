# include	"../conf/gen.h"
# include	"../h/lock.h"

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		pid;

	if (AAsequal(argv[1], "dbserver"))
		pid = 0;
	else if ((pid = atoi(argv[1])) < 2)
	{
		printf("UNLOCK: no process specified\n");
		exit(1);
	}
	AALOCKREQ.l_db = pid;
	AAlockdes = open(AA_DBS_LOCK, 1);
	AAsyslock(A_ABT, M_DB, 0L, 0L);
	sync();
}

AApageflush()
{
}
