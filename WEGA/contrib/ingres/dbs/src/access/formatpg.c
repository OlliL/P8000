# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

# ifdef STACK
static ACCBUF		buf;
#  endif

formatpg(d, n)
register DESC		*d;
register long		n;
{
	register char	*p;
# ifndef STACK
	ACCBUF		buf;
#  endif
	extern long	lseek();

	if (!AAcc_head)
		AAam_start();
	if (lseek(d->relfp, 0L, 0) < 0L)
		return (-2);
	buf.rel_tupid = d->reltid;
	buf.filedesc = d->relfp;
	for (p = (char *) &buf; p <= (char *) buf.linetab; p++)
		*p = '\0';
	buf.nxtlino = 0;
	buf.linetab[(int) LINETAB(0)] = buf.firstup - (char *) &buf;
	buf.ovflopg = 0;
	for (buf.mainpg = 1; buf.mainpg < n; (buf.mainpg)++)
		if (write(buf.filedesc, &buf, AApgsize) != AApgsize)
			return (-3);
	buf.mainpg = 0;
	if (write(buf.filedesc, &buf, AApgsize) != AApgsize)
		return (-4);
	AAccuwrite += n;
	return (0);
}
