/*
** ---  RP_DESC.C ------- DESCRITION PRINTER  ---------------------------------
**
**	Version: @(#)rp_desc.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"r_code.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif


main(argc, argv)
int	argc;
char	**argv;
{
	register int		f;
	register int		c;
	register char		*p;
	struct desc_t		d;
# ifdef SETBUF
	char			out_buf[BUFSIZ];

	setbuf(stdout, out_buf);
# endif
	PQR_system = "description printer";
	PQR();

	if (argc == 1)
		p = "desc";
	else
		p = argv[1];

# ifdef MSDOS
	if ((f = open(p, 0 | O_BINARY)) < 0)
# else
	if ((f = open(p, 0)) < 0)
# endif
	{
		printf("Cannot open *** %s ***\n", p);
		fflush(stdout);
		exit(-1);
	}

	c = 1;
	printf("| field | type  | len   | off   |\n");
	printf("+-------+-------+-------+-------+\n");
	while (read(f, &d, sizeof (struct desc_t)) == sizeof (struct desc_t))
	{
		printf("! %2d\t", c++);
		switch (d.domfrmt)
		{
		  case INT:
			printf("| int\t", d.domfrmt);
			break;

		  case FLOAT:
			printf("| real\t", d.domfrmt);
			break;

		  case CHAR:
			printf("| char\t", d.domfrmt);
			break;

		  default:
			printf("| %3d\t", ctou(d.domfrmt));
		}
		printf("| %3d\t", ctou(d.domfrml));
		printf("| %3d\t|\n", d.domoff);
		fflush(stdout);
	}
	printf("+-------+-------+-------+-------+\n");
	fflush(stdout);
}
