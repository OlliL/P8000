# include	<stdio.h>
# include	"gen.h"

# define	CUS_BUF		51

char	*AAcustomer()
{
	register int	fd;
	register int	i;
	static char	cbuf[CUS_BUF];
	extern char	*AApath;
	extern char	*AAztack();

	if (!(*cbuf))
	{
		if ((fd = open(AAztack(AApath, "/sys/etc/customer"), 0)) < 0)
			return ((char *) 0);
		if ((i = read(fd, cbuf, CUS_BUF)) < 2)
			return ((char *) 0);
		close(fd);
		cbuf[--i] = 0;
	}
	return (cbuf);
}
