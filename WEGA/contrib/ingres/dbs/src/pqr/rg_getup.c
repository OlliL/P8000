/*
** ---  RG_GETUP.C ------ TUPEL FILE HANDLING  --------------------------------
**
**	Version: @(#)rg_getup.c		4.0	02/05/89
**
*/

# include	"rg_file.h"


rg_desc()
{
	register FILETYPE	*f;
	register DESC_T		*d;
	register int		i;
	long			len;
	register char		*mem;
	extern int		errno;
	extern long		lseek();
	extern char		*xalloc();

	f = &Rg_file[F_DESC - 1];

# ifdef RG_TRACE
	if (TR_GETUP)
		printf("DESC\tread file `%s' on fd %d\n",
			f->rf_name, f->rf_fd);
# endif

	errno =	0;
	if ((len = lseek(f->rf_fd, 0L, 2)) <= 0L)
	{
		rg_error("NO tuple description");
		goto abo_desc;
	}
	if ((Doms = len / sizeof (DESC_T)) > MAX_DOM)
	{
		rg_error("TOO many domains `%d' > %d", Doms, MAX_DOM);
		goto abo_desc;
	}
	if ((i = len) % sizeof (DESC_T))
	{
		rg_error("WRONG tuple description FORMAT");
		goto abo_desc;
	}

	/* allocate descriptor space */
	mem = xalloc(i);
# ifdef RG_TRACE
	if (TR_MEM)
# ifdef MSDOS
		printf("ALLOC(Desc):\t%Fp\t(%4d)\n", mem, i);
# else
		printf("ALLOC(Desc):\t%08x (%4d)\n", mem, i);
# endif
# endif
	if (!(Desc = (DESC_T *) mem))
		goto no_mem;
	lseek(f->rf_fd, 0L, 0);
	read(f->rf_fd, (char *) Desc, i);
	rg_close(f);

	d = &Desc[Doms - 1];
	if ((Tup_len = d->domoff + ctou(d->domfrml)) > MAX_TUP)
	{
		rg_error("TUPLE length %d > MAX %d", Tup_len, MAX_TUP);
abo_desc:
		rg_exit(EXIT_DESC);
	}

# ifdef RG_TRACE
	if (TR_GETUP)
		printf("DESC\tTuple_length %d Domains %d\n", Tup_len, Doms);
# endif

	/* allocate tuple space */
	Last = xalloc(i = Tup_len);
	Value = xalloc(i);
	Next = xalloc(i);
# ifdef RG_TRACE
	if (TR_MEM)
	{
# ifdef MSDOS
		printf("ALLOC(Last):\t%Fp\t(%4d)\n", Last, i);
# else
		printf("ALLOC(Last):\t%08x (%4d)\n", Last, i);
# endif
# ifdef MSDOS
		printf("ALLOC(Value):\t%Fp\t(%4d)\n", Value, i);
# else
		printf("ALLOC(Value):\t%08x (%4d)\n", Value, i);
# endif
# ifdef MSDOS
		printf("ALLOC(Next):\t%Fp\t(%4d)\n", Next, i);
# else
		printf("ALLOC(Next):\t%08x (%4d)\n", Next, i);
# endif
	}
# endif
	if (!Next)
	{
no_mem:
		rg_error("NOT ENOUGH MEMORY");
		rg_exit(EXIT_MEM);
	}
	rg_getup();	/* get the first tuple */
}


rg_getup()
{
	register FILETYPE	*f;
	register char		*buf;
	register int		i;
	static int		tuple_count;
	extern int		errno;
	extern char		*cmove();

	if (Eof)
		return;

	SYS_tuple = tuple_count;
	/* roll	the buffer queue */
	buf = Last; Last = Value; Value	= Next;	Next = buf;

	f = Rg_file;
read_tup:
	if ((i = read(f->rf_fd, buf, Tup_len)) < 0)
	{
		rg_error("READ ERROR on tuple file");
		goto abo_read;
	}
	if (!i)
		goto close_tup;
	else if (i != Tup_len)
	{
		rg_error("WRONG tuple file FORMAT");
abo_read:
		rg_exit(EXIT_READ);
	}

	tuple_count++;
	if (tuple_count	< R_from_tuple)
		goto read_tup;
	else if (tuple_count > R_to_tuple)
	{
close_tup:
		rg_close(f);
		Eof++;
		cmove(buf, Tup_len, '\0');	/* clear tuple buffer	*/
	}
}
