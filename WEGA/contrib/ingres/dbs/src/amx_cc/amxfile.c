# include	<stdio.h>
# ifdef MSDOS
# include	"../h/gen.h"
# include	<fcntl.h>

# define	AA_atoi		v6_atoi
# else
# include	"../conf/gen.h"
# endif

# define	LINES		32
# define	CNTR		1000

static char	cntr[CNTR];

main(argc, argv)
int	argc;
char	**argv;
{
	register char		*p;
	register int		i;
	register int		cnt;

	amxfile(argv[1]);
	if (argc == 2)
		return (0);
	cnt = 0;
	p = cntr;
	for (i = CNTR; i; --i)
	{
		if (*p)
		{
			cnt = i;
			*p++ = '*';
		}
		else
			*p++ = ' ';
	}
	cnt = CNTR - cnt;
	cnt = ((cnt + 9) / 10) * 10;
	p = cntr;
	for (i = 0; i < cnt; i += 10, p += 10)
	{
		if (!(i % 100))
		{
			printf("------------------\n");
			printf("    | 0123456789 |\n");
			printf("------------------\n");
		}
		printf("%3d | %.10s |\n", i / 10, p);
	}
	return (0);
}


amxfile(output)
char	*output;
{
	register char		*p;
	register int		c;
	register int		i;
	register int		f;
	int			iff;
	short			ix;
	long			off;
	char			buf[2 * LINES];
# ifdef SETBUF
	char			inbuf[BUFSIZ];
# endif
	extern long		lseek();

# ifdef SETBUF
	setbuf(stdin, inbuf);
# endif

	if ((f = creat(output, 0644)) < 0)
	{
		printf("cannot open output '%s'\n", output);
		exit(1);
	}
# ifdef MSDOS
	setmode(f, O_BINARY);
# endif

	/* read in the code and check for correct */
	for ( ; ; )
	{
		p = buf;
		while ((c = getchar()) != '\t')
		{
			if (c <= 0)
				return;
			*p++ = c;
		}
		*p = 0;
		if (AA_atoi(buf, &ix))
		{
			printf("bad msg file '%s'\n", buf);
			return;
		}
		iff = ix;

		off = LINES * iff;
		lseek(f, off, 0);
		for (p = buf; p < &buf[2 * LINES]; p++)
			*p = 0;
		p = buf;
		i = 0;

		/* get the correct line */
		for ( ; ; )
		{
			c = getchar();
			i++;
			if (c <= 0 || c == ERRDELIM)
			{
				if (iff > CNTR)
				{
					printf("msg no to '%d' too big\n", iff);
					return;
				}
				if (cntr[iff]++)
				{
					printf("dup msg %d\n", iff);
					return;
				}
				if (i > 2 * LINES)
				{
					printf("too long msg '%s'\n", buf);
					return;
				}
				if (i > LINES)
				{
					iff++;
					if (cntr[iff]++)
					{
						printf("dup msg %d\n", iff);
						return;
					}
					i = 2 * LINES;
				}
				else
					i = LINES;
				write(f, buf, i);
				if (c <= 0)
					return;
				if ((c = getchar()) != '\n')
				{
					printf("format error\n");
					return;
				}
				break;
			}
			*p++ = c;
		}
	}
}
