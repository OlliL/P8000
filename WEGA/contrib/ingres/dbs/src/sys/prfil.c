# include	<stdio.h>
# include	"../conf/gen.h"


main()
{
	register int	c;
# ifdef SETBUF
	char		ibuf[BUFSIZ];
	char		obuf[BUFSIZ];

	setbuf(stdin, ibuf);
	setbuf(stdout, obuf);
# endif

	while ((c = getchar()) >= 0)
		AAxputchar(c);
	putchar('\n');
}
