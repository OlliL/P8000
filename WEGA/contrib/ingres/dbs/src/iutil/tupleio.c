# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

		/*
		** the buffer, PGSIZE
		** place for one more tuple, MAXTUP - 1
		** and the HASH-key, sizeof (long), see dbu/dbu___s.c
		*/
char	Tup_buf[TUP_BUF_SIZ];
char	*Tup_ptr = Tup_buf;	/* the actual buffer position */
short	Tup_size = 0;		/* the length of written tuple's */
int	Tup_fd = -1;		/* the write descriptor of the tuple file */


/*
** tup_read -- read one tuple
**
**	tup_fd is the file descriptor from which to read
**	tup_ptr points to the actual tuple
**	tup_end points to the first invalid byte
**	tup_len is the size of tup_buf without the space
**		for the additional tuple
**	tup_no is untouched
*/
char	*tup_read(hdr)
register struct tup_hdr		*hdr;
{
	register int		i;
	register char		*end;
	extern char		*AAbmove();

	hdr->tup_ptr += Tup_size;

	if ((i = (end = hdr->tup_end) - hdr->tup_ptr) < Tup_size)
	{
		end = (i > 0)? AAbmove(hdr->tup_ptr, hdr->tup_buf, i): hdr->tup_buf;
		hdr->tup_ptr = hdr->tup_buf;

		if ((i = read(hdr->tup_fd, end, hdr->tup_len)) < 0)
			AAsyserr(15042);

		end += i;
		hdr->tup_end = end;

		if (!(i = end - hdr->tup_ptr))
			return ((char *) 0);	/* EOF */

		if (i < Tup_size)
			AAsyserr(15043, i);
	}

	return (hdr->tup_ptr);
}


/*
** tup_write -- write one tuple
**
**	there is always space for at least one tuple in the buffer
*/
tup_write(tup)
register char	*tup;
{
	register int		i;
	register char		*ptr;
	extern char		*AAbmove();

	ptr = AAbmove(tup, Tup_ptr, Tup_size);
	if ((i = ptr - &Tup_buf[PGSIZE]) >= 0)
	{
		if (write(Tup_fd, ptr = Tup_buf, PGSIZE) != PGSIZE)
			AAsyserr(15044);

		if (i)
			ptr = AAbmove(&Tup_buf[PGSIZE], ptr, i);
	}
	Tup_ptr = ptr;
}


/*
** tup_close -- close the write descriptor of the tuple file
**
**	flush's the buffer
*/
tup_close()
{
	register int		i;

	if ((i = Tup_ptr - Tup_buf) && write(Tup_fd, Tup_ptr = Tup_buf, i) != i)
		AAsyserr(15045);

	close(Tup_fd);
	Tup_fd = -1;
	Tup_ptr = Tup_buf;
}
