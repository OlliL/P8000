# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

# define	N		MERGESIZE
# ifdef MEM_E
# define	MEM		COREBUFSIZE
char		start[MEM]		= {0};
# else
# define	MEM		((char *) COREBUFSIZE)
# endif
# define	BUCKETSIZE	sizeof (long)
# define	ENDKEY		(MAXDOM + 1)

char			SYSSORT[]	= "_SYS";

static char		File[15];
static char		DBU_buf[TUP_BUF_SIZ];
static char		*DBU_ptr;
static char		Descsort[MAXDOM + 1];
static DESC		Desc;
static char		**Lspace;
static char		*Tspace;
static char		*Infile;
static char		*Outfile;
char			*Fileset;
static char		*Filep;
char			*AApath;
static long		Ccount;
static long		Page;
static long		Tupsout;
static int		Outpipe		= -2;	/* fd to write to modify */
static unsigned int	Memory;		/* available memory buffer size */
# ifdef VPGSIZE
int			AApgsize	= PGSIZE;
int			AAmaxtup	= MAXTUP;
int			AAltsize	= 0;
# endif
static short		Trace;
static short		Bucket;
static short		Nfiles;
static short		Nlines;
static short		Reorg;
static short		AppendOutput;
static struct tup_id	Lowtid;
static struct tup_id	Hightid;

extern int		term();
extern int		cmpa();

/*
** argv[0] --	sx_dbu for sorting or
**		rx_dbu for reorganization
**			and x is the fd for outfile or '_'
**
** argv[1] --	Fileset (process id)
**
** argv[2] --	trace information
**		comes from trace flag Z23
**
**	The bits used are:
**		0001	main trace info
**		0002	secondary trace info
**		0004	terciary trace info
**		0010	don't truncate temps
**		0020	don't unlink temps
**		0040	print am page refs
**		0100	print am tuple gets
**
** argv[3] --	pathname of DBS subtree
**
** argv[4] --	page size of data base
**
** argv[5] --	name of descritor file
**
** argv[6] --	name of infile
**
** argv[7] --	name of outfile
*/
main(argc, argv)
int	argc;
char	**argv;
{
	register int			i;
	register int			j;
	register int			maxkey;
	short				rev;
# ifndef MEM_E
	register char			*start;
	register char			*stopp;
	extern char			*sbrk();
	extern char			*brk();
# endif
	extern char			*setfil();
	extern char			*AAproc_name;
	extern int			(*AAexitfn)();

	AAproc_name = "DB_SORT";
	AAexitfn = term;

	Reorg = **argv == 'r';
	if ((i = (*argv)[1]) != '_')
	{
		Outpipe = i - '0';
		Outpipe = 10 * Outpipe + (*argv)[2] - '0';
	}

	Fileset = *++argv;

	AA_atoi(*++argv, &Trace);

	AApath = *++argv;

# ifdef VPGSIZE
	AA_atoi(*++argv, &rev);
	AApgsize = rev;
	/* get the linetab size, the difference */
	AAltsize = (PGSIZE - AApgsize) / sizeof (short);
# else
	argv++;
# endif

	if (Trace & 01)
		printf("DB_SORT: %s path %s set %s pipe %d\n",
			Reorg? "Reorg": "Sort", AApath, Fileset, Outpipe);

	if ((i = open(*++argv, 0)) < 0)
		AAsyserr(11055, *argv);
	if (read(i, &Desc, sizeof Desc) < sizeof Desc)
		AAsyserr(11049);
	close(i);

	/* set up Descsort to indicate the sort order for tuple */
	/* if domain zero is given prepare to generate "hash bucket"
	** value for tuple */
	maxkey = 0;
	for (i = 0; i <= Desc.reldum.relatts; i++)
		if (j = Desc.relgiven[i])
		{
			if ((rev = j) < 0)
				j = -j;
			if (maxkey < j)
				maxkey = j;
			Descsort[--j] = rev < 0? -i: i;
		}

	Descsort[maxkey] = ENDKEY;	/* mark end of list */

	Tup_size = Desc.reldum.relwid;

	if (Bucket = !Descsort[0])
	{
		/* we will be generating hash bucket */
		Tup_size += BUCKETSIZE;
		Desc.relfrml[0] = BUCKETSIZE;
		Desc.relfrmt[0] = INT;
		Desc.reloff[0] = Desc.reldum.relwid;
	}

	if (Trace & 01)
	{
		printf("Bucket is %d,Sort is:\n", Bucket);
		for (i = 0; ctou(j = Descsort[i]) != ENDKEY; i++)
			printf("Descsort[%d]=%d\n", i, j);
	}
	if (i = (maxkey - Bucket - Desc.reldum.relatts))
		AAsyserr(11050, -i);

	Infile = *++argv;

	Outfile = (Outpipe > 0)? "PIPE": *++argv;

# ifdef MEM_E
	Memory = MEM;
# else
	/* get up to MEM-K bytes of memory for buffers. Note that mem */
	/* must end up positive so that Nlines computation is right   */
	start = sbrk((char *) 0);
	stopp = MEM;
	while (brk(stopp) == (char *) -1)
		stopp -= 512;
	Memory = stopp - start;
# endif
	Lspace = (char **) start;

	/* compute pointers and sizes into buffer memory */
	Nlines = Memory / (Tup_size + sizeof (char *));
	Tspace = (char *) (Lspace + Nlines);
	if (Trace & 01)
		printf("Tspace=0%o,Lspace=0%o,Nlines=%u,mem=0%o,start=0%o\n",
		Tspace, Lspace, Nlines, Memory, start);

	/* set up temp files */
	AAconcat(AAztack(SYSSORT, Fileset), "Sxxxx", File);
	for (Filep = File; *Filep != 'x'; Filep++)
		continue;

	/* initialize tids for full scan */
	Page = 0L;
	Lowtid.line_id = SCANTID;
	AAstuff_page(&Lowtid, &Page);
	Page = -1L;
	Hightid.line_id = SCANTID;
	AAstuff_page(&Hightid, &Page);
	Page = 1L;

	/* open relation */
	close(0);
	if ((Desc.relfp = open(Infile, 0)) < 0)
		AAsyserr(11055, Infile);
	Desc.relopn = RELROPEN(Desc.relfp);

	signal(2, term);
	signal(3, term);

	while (Desc.relopn)
	{
		/* sort stage -- create a bunch of temporaries */
		Ccount = 0;
		Tupsout = 0;
		Nfiles = 1;
		if (Trace & 01)
			printf("sorting\n");
		sort();
		if (Trace & 01)
		{
			printf("done sorting\n%ld tuples written to %d files\n", Tupsout, Nfiles - 1);
			printf("sort required %ld compares\n", Ccount);
		}

		/* merge stage -- merge up to N temps into a new temp */
		Ccount = 0;
		for (i = 1; i + N < Nfiles; i += N)
		{
			makfile(setfil(Nfiles++));
			merge(i, i + N);
		}

		/* merge last set of temps into target file */
		if (i != Nfiles)
		{
			oldfile();
			merge(i, Nfiles);
		}
		if (Trace & 01)
		{
			printf("%ld tuples in out file\n", Tupsout);
			printf("merge required %ld compares\n", Ccount);
		}
	}
	term(0);
}


sort()
{
	register char		*cp;
	register char		**lp;
	register int		i;
	register int		done;
	char			*xp;
	long			ntups;
	long			pageid;
	extern char		*setfil();
	extern long		AArhash();
	extern char		*AAbmove();

	done = 0;
	ntups = 0;

	do
	{
		cp = Tspace;
		lp = Lspace;
		while (lp < Lspace + Nlines)
		{
			if (i = AAam_get(&Desc, &Lowtid, &Hightid, cp, TRUE))
			{
				if (i < 0)
					AAsyserr(11051, i);

				close(Desc.relfp);
				Desc.relopn = 0;
				goto step_ok;
			}
			if (Reorg && Page != AAcc_head->mainpg)
			{
				Page = AAcc_head->mainpg;
				Lowtid.line_id = SCANTID;
step_ok:
				done++;
				break;
			}
			if (Bucket)
			{
				/* compute hash bucket and insert at end */
				pageid = AArhash(&Desc, cp);
				AAbmove(&pageid, cp + Desc.reldum.relwid, BUCKETSIZE);
			}
			*lp++ = cp;
			cp += Tup_size;
			ntups++;
		}

		qsort(Lspace, lp - Lspace, sizeof (char *), cmpa);

		if (!done || Nfiles != 1)
			makfile(setfil(Nfiles++));
		else
			oldfile();

		while (lp > Lspace)
		{
			cp = *--lp;
			xp = cp;
			if ((lp == Lspace) || cmpa(&xp, &lp[-1]))
			{
				tup_write(cp);
				Tupsout++;
			}
		}

		if (Tup_fd != Outpipe)
			tup_close();
	} while (!done);
	if (Trace & 01)
		printf("%ld tuples in\n", ntups);
}


merge(low, high)
int	low;
int	high;
{
	register struct tup_hdr	*merg;
	register int		i;
	register int		j;
	register char		*f;
	register unsigned int	blen;
	register unsigned int	mlen;
	register char		*mem;
	char			*yesno;
	struct tup_hdr		*merg_buf[N + 1];
	extern char		*setfil();

	if (Trace & 02)
		printf("merge %d to %d\n", low, high);

	/* compute length of merge buffers */
	j = sizeof (struct tup_hdr);
	i = ((Memory / (high - low)) - j) / PGSIZE;
	if ((blen = i * PGSIZE) > 32768L - PGSIZE)
		blen = 32768L - PGSIZE;
	mlen = ALIGNMENT(j + blen);
	blen += PGSIZE;

	if (Trace & 02)
		printf("merge mem 0%o buflen 0%o\n", Memory, blen);

	mem = (char *) Lspace;
	for (j = 0, i = low; i < high; i++)
	{
		merg_buf[j] = merg = (struct tup_hdr *) mem;

		if ((merg->tup_fd = open(f = setfil(i), 0)) < 0)
			AAsyserr(11055, f);

		merg->tup_ptr = merg->tup_buf;
		merg->tup_end = merg->tup_buf;
		merg->tup_len = blen;
		merg->tup_no = i;
		if (!next_tuple(merg))
			j++;
		mem += mlen;
	}

	i = j - 1;
	if (Trace & 04)
		printf("start merg with %d\n", i);
	while (i >= 0)
	{
		if (Trace & 04)
			printf("mintup %d\n", i);
		if (mintup(merg_buf, i, cmpa))
		{
			tup_write(merg_buf[i]->tup_ptr);
			Tupsout++;
		}
		if (next_tuple(merg = merg_buf[i]))
		{
			yesno = "not ";
			if (!(Trace & 010))
			{
				/* truncate temporary files to zero length */
				yesno = "";
				close(creat(setfil(merg->tup_no), FILEMODE));
			}
			if (Trace & 02 || Trace & 010)
				printf("dropping and %struncating %s\n", yesno, setfil(merg->tup_no));
			i--;
		}
	}

	if (Tup_fd != Outpipe)
		tup_close();
}


/*
**	Mintup puts the smallest tuple in merg_buf[cnt-1].
**	If the tuple is a duplicate of another then
**	mintup returns 0, else 1.
**
**	Cnt is the number of compares to make; i.e.
**	merg_buf[cnt] is the last element.
*/
mintup(merg_buf, cnt, cmpfunc)
struct tup_hdr	*merg_buf[];
int		cnt;
int		(*cmpfunc)();
{
	register struct tup_hdr	**next;
	register struct tup_hdr	**last;
	register int		nodup;
	register struct tup_hdr	*temp;
	register int		j;

	nodup = TRUE;
	next = merg_buf;
	last = &next[cnt];

	while (cnt--)
	{
		if (j = (*cmpfunc)(*last, *next))
		{
			/* tuples not equal. keep smallest */
			if (j < 0)
			{
				/* exchange */
				temp = *last;
				*last = *next;
				*next = temp;
				nodup = TRUE;
			}
		}
		else
			nodup = FALSE;

		next++;
	}
	return (nodup);
}


next_tuple(merg)
register struct tup_hdr		*merg;
{
	register int		eof;
	extern char		*tup_read();

	if (eof = !tup_read(merg))
		close(merg->tup_fd);

	return (eof);
}


/*
**	Convert the number i to a char
**	sequence 1, 2, ..., 10, 11, etc.
*/
char	*setfil(i)
register int	i;
{
	AA_itoa(--i, Filep);
	return (File);
}


oldfile()
{
	register int		i;
	register char		*name;
	register int		fd;
	long			off;
	extern long		lseek();
	extern char		*AAbmove();

	name = Outfile;

	if (Trace & 02)
		printf("OLDFILE: %s\n", name);

	if (Outpipe > 0)
	{
		if (AppendOutput)
		{
			if (Tup_fd == Outpipe)
				return;
			if (i = DBU_ptr - DBU_buf)
				Tup_ptr = AAbmove(DBU_buf, Tup_buf, i);
			else
				Tup_ptr = Tup_buf;
		}
		else
			Tup_ptr = Tup_buf;
		Tup_fd = Outpipe;
	}
	else if (AppendOutput)
	{
		if ((fd = open(name, 2)) < 0)
			AAsyserr(11055, name);
		if ((off = lseek(fd, 0L, 2)) < 0)
			AAsyserr(11055, name);
		if (i = off % PGSIZE)
		{
			if (lseek(fd, off - i, 0) < 0 || read(fd, Tup_buf, i) != i)
				AAsyserr(11055, name);
			lseek(fd, off - i, 0);
		}
		Tup_ptr = &Tup_buf[i];
		Tup_fd = fd;
	}
	else
		makfile(name);

	AppendOutput++;
}


makfile(name)
register char	*name;
{
	register int		i;
	extern char		*AAbmove();

	if (Trace & 02)
		printf("MAKFILE: %s\n", name);

	if (Outpipe > 0 && AppendOutput)
	{
		if (i = Tup_ptr - Tup_buf)
			DBU_ptr = AAbmove(Tup_buf, DBU_buf, i);
		else
			DBU_ptr = DBU_buf;
	}

	if ((Tup_fd = creat(name, FILEMODE)) < 0)
		AAsyserr(11055, name);

	Tup_ptr = Tup_buf;
}


term(error)
register int	error;
{
	register int		i;
	extern char		*setfil();

	if (Nfiles == 1)
		Nfiles++;

	if (Trace & 020)
		printf("temp files not removed\n");
	else
		for (i = 1; i < Nfiles; i++)
			unlink(setfil(i));

	if (Tup_fd >= 0)
		tup_close();

	if (Outpipe > 0)
		close(Outpipe);

	fflush(stdout);
	sync();
	exit(error);
}


AAunlall()
{
}


cmpa(a, b)
char	**a;
char	**b;
{
	register char		*tupa;
	register char		*tupb;
	register int		frml;
	register int		i;
	register int		dom;
	register int		off;
	char			*dp;
	int			frmt;
	char			*pa;
	char			*pb;
	char			af[sizeof (double)];
	char			bf[sizeof (double)];

	pa = *a;
	pb = *b;
	Ccount++;
	dp = Descsort;
	while (ctou(i = *dp++) != ENDKEY)
	{
		if ((dom = i) < 0)
			dom = -i;
		frml = ctou(Desc.relfrml[dom]);
		frmt = Desc.relfrmt[dom];
		off = Desc.reloff[dom];
		tupa = &pa[off];
		tupb = &pb[off];
		if (i < 0)
		{
			tupb = tupa;
			tupa = &pb[off];
		}
		if (frmt == CHAR)
		{
			if (i = AAscompare(tupb, frml, tupa, frml))
				return (i);
			continue;
		}

		/* domain is a numeric type */
		if (AAbequal(tupa, tupb, frml))
			continue;

		/* copy to even word boundary */
		AAbmove(tupa, af, frml);
		AAbmove(tupb, bf, frml);
		tupa = af;
		tupb = bf;

		switch (frmt)
		{
		  case INT:
			switch (frml)
			{
			  case sizeof (char):
				return (ctoi(((I1TYPE *) tupa)->i1type) > ctoi(((I1TYPE *) tupb)->i1type)? -1: 1);

			  case sizeof (short):
				return (((I2TYPE *) tupa)->i2type > ((I2TYPE *) tupb)->i2type? -1: 1);

			  case sizeof (long):
				return (((I4TYPE *) tupa)->i4type > ((I4TYPE *) tupb)->i4type? -1: 1);
			}

		  case FLOAT:
# ifndef NO_F4
			switch (frml)
			{
			  case sizeof (float):
				return (((F4TYPE *) tupa)->f4type > ((F4TYPE *) tupb)->f4type? -1: 1);

			  case sizeof (double):
# endif
				return (((F8TYPE *) tupa)->f8type > ((F8TYPE *) tupb)->f8type? -1: 1);
# ifndef NO_F4
			}
# endif
		}
	}
	return (0);
}


/*
**	Replacement for access method routine AAgetpage();
**	and associated globals and routines.
*/
ACCBUF			*AAcc_head;
static ACCBUF		AAcc_buf;
static long		AAccuread;
static long		AAccuwrite;

AAgetpage(d, tid)
register DESC		*d;
struct tup_id		*tid;
{
	register int			i;
	register ACCBUF			*b;
	long				pageid;
	extern long			lseek();

	if (Trace & 0100)
	{
		printf("GET_PAGE: %.14s,", d->reldum.relid);
		AAdumptid(tid);
	}
	if (!(b = AAcc_head))
	{
		/* initialize buffer */
		AAcc_head = &AAcc_buf;
		b = &AAcc_buf;
		b->thispage = -1L;
	}
	AApluck_page(tid, &pageid);
	i = 0;
	if (b->thispage != pageid)
	{
		if (Trace & 040)
			printf("GET_PAGE: rdg pg %ld\n", pageid);
		b->thispage = pageid;
		if (lseek(d->relfp, pageid * AApgsize, 0) < 0L)
			i = AMSEEK_ERR;
		else if (read(d->relfp, b, AApgsize) != AApgsize)
			i = AMREAD_ERR;
		AAccuread++;
	}
	return (i);
}


AAresetacc()
{
	return (0);
}


AAam_error(err)
register int	err;
{
	return (err);
}
