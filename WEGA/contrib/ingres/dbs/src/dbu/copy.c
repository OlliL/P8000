# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"

# define	BUFSIZE		1024
# define	MAXMAP		3 * MAXDOM
# define	DUMMY		'd'
# define	ESCAPE		'\\'

struct map
{
	char	name[MAXNAME + 1];	/* attribute name */
	char	ftype;		/* attfrmt of file domain */
	char	rtype;		/* attfrmt of relation domain */
	short	flen;		/* attfrml of file domain */
	short	rlen;		/* attfrml of relation domain */
	int	roffset;	/* attoff of relation domain */
	int	used;		/* tag for duplicate checking */
	char	*fdelim;	/* pointer to list of file param delims */
	char	*paramname;	/* pointer to original parameter name */
				/* used for supplying domain name in case of error */
};

int			Stdio;
extern int		Standalone;
extern struct out_arg	AAout_arg;	/* user defined formats for numeric output */

static struct map	Map[MAXMAP];	/* one entry for each user
				   specified domain in copy statement. */

static int	Mapcount;	/* number of Map entries   */

static DESC	 Des;		/* descriptor for copied relation     */

static FILE	*File_iop;	/* i/o file pointer */
# ifdef SETBUF
static char	Iobuf[BUFSIZ];	/* buffer for file i/o */
# endif
static char	*Filename;	/* pointer to file name */
static int	Into;		/* into is one if this is a copy into file */
static char	Inbuf[BUFSIZE];	/* input holder */
static char	Outbuf[BUFSIZE];/* output holder */
static long	Tupcount;	/* number of tuples processed */
static char	*Relname;	/* name of relation */
static long	Duptuple;	/* number of duplicate tuples */
static long	Baddoms;	/* number of domains with control chars */
static long	Truncount;	/* number of truncations on a c0 field */
static int	Piped[2];	/* pipe descriptor for copy communication */
static char	*Cpdomains[] =	/* dummy domain names for copy "into" */
{
	"nl",		"\n",
	"tab",		"\t",
	"sp",		" ",
	"nul",		"\0",
	"null",		"\0",
	"comma",	",",
	"colon",	":",
	"dash",		"-",
	"lparen",	"(",
	"rparen",	")",
	(char *) 0
};
static char	Delimitor[] =	/* default delims for c0 & d0 */
	",\n\t";

/*
**  COPY -- Performs an DBS COPY.
*/
copy(pc, pv)
int	pc;
char	**pv;
{
	register int	i;
	register int	pid;
	register char	*cp;
	register int	op;
	int		stat;
	extern char	*AAusercode;
	extern char	*AA_locv();
	extern int	copydone();
	extern int	Noupdt;
	extern int	(*signal())();

#	ifdef xZTR1
	AAtTfp(7, 1, "entered COPY\n");
#	endif
	Duptuple = 0;
	Truncount = 0;
	Tupcount = 0;
	Baddoms = 0;
	Relname = pv[0];
	Into = (pv[pc - 2][0] == 'i');
	Filename = pv[pc - 1];

	/* relation must exist and not be a system relation */
	/* in addition a copy "from" can't be done if the user */
	/* doesn't own the relation */
	/* and furthermore it can't have an index */
	i = 0;	/* assume all is well */
	if (op = AArelopen(&Des, 2, Relname))
	{
		if (op == AMOPNVIEW_ERR)
			i = 5818;
		else
		{
			if (op < 0)
				AAsyserr(11000, Relname, op);
			else
				/* non-existant relation */
				i = 5800;
		}
	}
	else
	{
		if (Into)
		{
			if ((Des.reldum.relstat & S_PROTALL)
				&& (Des.reldum.relstat & S_PROTRET)
				&& !AAbequal(AAusercode, Des.reldum.relowner, 2))
				i = 5822;
		}
		else
		{
			/* extra checking if this is a copy "from" */

			/* must be owned by the user */
			if (!AAbequal(AAusercode, Des.reldum.relowner, 2))
				i = 5814;
			else
				/* must be updateable */
				if ((Des.reldum.relstat & S_NOUPDT) && Noupdt)
					i = 5813;
				else
					/* must not be indexed */
					if (ctoi(Des.reldum.relindxd) == SECBASE)
						i = 5812;
		}
	}
	if (i)
	{
		AArelclose(&Des);
		return (error(i, Relname, (char *) 0));	/* relation doesn't exist for this user */
	}

	/* check that file name begins with a "/" */
	cp = Filename;
	while (*cp == ' ')
		cp++;
	if (Standalone)
		Stdio = *cp == '#';
	else if (*cp != '/')
	{
		AArelclose(&Des);
		return (error(5816, Filename, (char *) 0));
	}

	/* fill map structures with transfer information */
	if (i = mapfill(&pv[1]))
	{
		AArelclose(&Des);
		return (i);	/* error in user semantics */
	}

	Piped[0] = -1;
	Piped[1] = -1;
	if (Standalone)
		goto one_process;

	/* fork a child process which will run as the real user */
	/* that child will complete the copy and exit */
	if (pipe(Piped))
		AAsyserr(11001);
	fflush(stdout);
	if ((pid = fork()) < 0)
		AAsyserr(11002);
	if (pid)
	{
		/* the DBS parent */
		close(Piped[1]);
		ruboff(0);	/* interrupts off */
		stat = fullwait(pid, "copy");
		if (read(Piped[0], &Des.reladds, sizeof (long)) != sizeof (long))
			AAsyserr(11003);
		close(Piped[0]);
		AArelclose(&Des);	/* close the rel */
		rubon();
		/* if stat is != 0 then add on 5800 for error */
		if (stat)
			stat += 5800;
		return (stat);	/* done */
	}

one_process:
	/* the child. change to run as the real user */
	if (signal(2, (int (*)()) 1) != (int (*)()) 1)
	{
		signal(2, copydone);	/* clean up on rubout */
		signal(3, copydone);
	}

	setuid(getuid());
	setgid(getgid());

	if (Into)	/* from relation into file */
	{
		if (Stdio)
			File_iop = stdout;
		else if (!(File_iop = fopen(Filename, "w")))
		{
			copydone(error(5806, Filename, (char *) 0));
			return;
		}
# ifdef SETBUF
		setbuf(File_iop, Iobuf);
# endif

		/* set a shared lock on relation*/
		if (AAsetrll(A_SLP, Des.reltid, M_SHARE) < 0)
			goto tabort;
		i = rel_file();
	}
	else		/* from file into relation */
	{
		if (Stdio)
			File_iop = stdin;
		else if (!(File_iop = fopen(Filename, "r")))
		{
			copydone(error(5805, Filename, (char *) 0));
			return;
		}
# ifdef SETBUF
		setbuf(File_iop, Iobuf);
# endif

		/* set an exclusive lock on relation */
		if (AAsetrll(A_SLP, Des.reltid, M_EXCL) < 0)
		{
tabort:
			copydone(error(5000, (char *) 0));
			return;
		}
		i = file_rel();
		if (Duptuple)	/* warning only */
			error(5819, AA_locv(Duptuple), (char *) 0);
		if (Baddoms)	/* warning only */
			error(5820, AA_locv(Baddoms), (char *) 0);
	}
	copydone(i);
}


/*
**	Finish up and exit after a copy or interrupt
**
**	I is the return code. Since only a byte can be
**	returned, only the least significant 2 decimal
**	digits are returned. i is either 0 or a number like 58??
*/
copydone(i)
int	i;
{
	extern char	*AA_locv();

	/* unlock relation */
	AAunlrl(Des.reltid);
	if (Truncount)	/* warning only */
		error(5821, AA_locv(Truncount), (char *) 0);
	if (File_iop)
	{
		fflush(File_iop);
		if (!Stdio)
			fclose(File_iop);
		File_iop = (FILE *) 0;
	}
	fflush(stdout);
	if (Standalone)
		AArelclose(&Des);	/* close the rel */
	else
	{
		cleanrel(&Des);	/*  force the updates to be flushed */
		if (write(Piped[1], &Des.reladds, sizeof (long)) != sizeof (long))
			AAsyserr(11004);
		exit(i % 100);
	}
}


rel_file()
{
	register int		offset;
	register int		i;
	register struct map	*mp;
	register char		*cp;
	register int		j;
	char			save;
	struct tup_id		tid;
	struct tup_id		limtid;
	extern char		*AA_locv();
	extern int		errno;

	/* set scan limits to scan the entire relation */
	if (AAam_find(&Des, NOKEY, &tid, &limtid))
		AAsyserr(11005);

	while (!(i = AAam_get(&Des, &tid, &limtid, Inbuf, 1)))
	{
		mp = Map;
		offset = 0;
		for (i = 0; i < Mapcount; i++)
		{
			/* For cases of char to numeric conversion,
			   there must be a null byte at the end of the
			   string. The character just past the current
			   domain is saved an a null byte inserted */
			cp = &Inbuf[mp->roffset + mp->rlen];	/* compute address */
			save = *cp;	/* get the character */
			*cp = '\0';	/* insert a null */
			j = transfer(&Inbuf[mp->roffset], mp->rtype, mp->rlen,
				     mp->ftype, mp->flen, offset);
			if (j)
			{
				/* bad ascii to numeric conversion or field length too small */
				return (error(j, mp->paramname, &Inbuf[mp->roffset], AA_locv(Tupcount), Relname, Filename, (char *) 0));
			}
			*cp = save;	/* restore the saved character */
			offset += mp->flen;
			mp++;
		}
		Tupcount++;
# ifdef FWRITE
		if (fwrite(Outbuf, 1, offset, File_iop) != offset)
# else
		errno = 0;
		for (cp = Outbuf; offset > 0; offset--, cp++)
			if (putc(*cp, File_iop) < 0 && errno)
# endif
				AAsyserr(11006, Filename);
	}
	if (i < 0)
		AAsyserr(11007, i);
	return (0);
}


/*
**	file_rel is called to transfer tuples from
**	the input file and append them to the relation
**
**	Char domains are initialized to blank and numeric
**	domains are initialized to zero.
*/
file_rel()
{
	register int		i;
	register int		j;
	register struct map	*mp;
	struct tup_id		tid;
	extern char		*AA_locv();

	clr_tuple(&Des, Outbuf);

	/* copy domains until an end of file or an error */
	for ( ; ; )
	{
		mp = Map;
		for (i = 0; i < Mapcount; i++)
		{
			if ((j = bread(mp)) <= 0)
			{
				if (j < 0)
				{
					i = 1;	/* force an error */
					j = 5815;	/* unterminated string */
				}
				else
					j = 5810;	/* end of file */
				if (i)	/* error only if end of file during a tuple or unterminated string */
				{
					i = error(j, mp->paramname, AA_locv(Tupcount), Filename, Relname, (char *) 0);
				}
				return (i);
			}
			j = transfer(Inbuf, mp->ftype, mp->flen, mp->rtype, mp->rlen, mp->roffset);
			if (j)
			{
				/* bad ascii to numeric or field length too small */
				return (error(j, mp->paramname, Inbuf, AA_locv(Tupcount), Filename, Relname, (char *) 0));
			}
			mp++;
		}
		Tupcount++;
		if ((j = AAam_insert(&Des, &tid, Outbuf, 1)) < 0)
			AAsyserr(11008, j, Relname);
		if (j == 1)
			Duptuple++;
		mp++;
	}
}


/*
**	transfer copies data from "*in" to
**	Outbuf doing conversions whenever
**	necessary
*/
transfer(in, sf, sl, df, dl, doff)
char	*in;	/* pointer to input chars */
char	sf;	/* source format */
int	sl;	/* source length */
char	df;	/* destination format */
int	dl;	/* destination length */
int	doff;	/* destination offset */
{
	double		d;
# ifndef NO_F4
	float		f;
# endif
	register char	*inp;
	register char	*outp;
	register int	i;
	short		j;
	long		l;
	char		temp[MAXFIELD];	/* holds char during conversions to ascii */
	extern char	*AA_locv();


	outp = &Outbuf[doff];
	inp = in;

	if (sf == DUMMY)
		/* if source format is a dummy fields then
		   nothing else need be done */
		return (0);

	if (df == DUMMY)
	{
		/* fill field with dummy domain character */
		i = dl;	/* i equals the number of chars */
		while (i--)
			*outp++ = sf;	/* sf holds dummy char */
		return (0);
	}

	if (sf != CHAR)
	{
		if (df == CHAR)	/* numeric to char conversion */
		{
			switch (sl)
			{
			  case sizeof (char):	  /* int of size 1 */
				AA_itoa(ctoi(*inp), temp);
				break;

			  case sizeof (short):	  /* int of size 2 */
				AAbmove(inp, &j, sizeof (short));	/* copy to an integer */
				AA_itoa(j, temp);	/* convert to ascii */
				break;

			  case sizeof (long):	/*   int of size 4 **
			  case sizeof (float):	** float of size 4 */
# ifndef NO_F4
				if (sf == INT)
				{
# endif
					AAbmove(inp, &l, sizeof (long));	/* copy to a long */
					AAsmove(AA_locv(l), temp);	/* convert and copy */
# ifndef NO_F4
				}
				else
				{
					AAbmove(inp, &f, sizeof (float));
					AA_ftoa(f, temp, dl, AAout_arg.f4prec, AAout_arg.f4style);
				}
# endif
				break;

			  case sizeof (double):	  /* float of size 8 */
				AAbmove(inp, &d, sizeof (double));	/* copy to a dbl variable */
				AA_ftoa(d, temp, dl, AAout_arg.f8prec, AAout_arg.f8style);
				break;

			  /* there is no possible default */
			  default:
				AAsyserr(11009, sl);
			}

			j = AAlength(temp);
			if ((i = dl - j) < 0)
				return (5808);	/* field won't fit */

			/* blank pad from left. Number will be right justified */
			while (i--)
				*outp++ = ' ';

			AAbmove(temp, outp, j);
			return (0);
		}

		if (convert(inp, outp, sf, sl, df, dl))	/* numeric to numeric transfer */
			return (5808);	/* numeric truncation error */
		return (0);
	}

	/* character to numeric conversion */
	/* and character to character conversion */
	switch (df)
	{
	  case CHAR:
		i = sl;
		if (!i)
			i = AAlength(inp);
		if (i > dl)
			i = dl;
		if (charmove(inp, outp, i))
			Baddoms++;
		for (outp += i; i < dl; i++)
			*outp++ = ' ';
		return (0);

	  case FLOAT:
		if (AA_atof(inp, &d))
			return (5809);	/* bad conversion to numeric */
# ifndef NO_F4
		if (dl == sizeof (float))	/* f8 to f4 conversion */
		{
			f = d;
			AAbmove(&f, outp, dl);
		}
		else
# endif
			AAbmove(&d, outp, dl);
		return (0);

	  case INT:
		if (dl == sizeof (long))
		{
			if (AA_atol(inp, &l))
				return (5809);
			AAbmove(&l, outp, sizeof (long));
			return (0);
		}
		if (AA_atoi(inp, &j))
			return (5809);
		inp = (char *) &j;
		if (dl == sizeof (char))
		{
			if (j < -128 || j > 127)
				return (5809);
			((I1TYPE *) inp)->i1type = ((I2TYPE *) inp)->i2type;
		}
		AAbmove(inp, outp, dl);
		return (0);
	}
}


/*
**	moves a character string from "in"
**	to "out" removing any control characters.
**	returns true if any control characters were found
*/
charmove(in, out, length)
char	*in, *out;
int	length;
{
	register char	*ip;
	register char	*op;
	register int	l;
	int		bad;

	bad = FALSE;
	ip = in;
	op = out;
	l = length;

	while (l--)
		if ((*op++ = *ip++) < ' ')
		{
			*(op - 1) = ' ';
			bad = TRUE;
		}
	return (bad);
}


/*
**	Mapfill fills the Map structure with the list
**	of user supplied attributes. It then reads
**	the list of relation attributes and checks
**	for matching attribute names.
**
**	if an error occures then mapfill returns -1
**		else it returns 0
**
**	Mapfill performs special processing on
**	dummy domains.
**
**	If no user attributes are given, then "given"=FALSE
**	and each attribute in the relation is set up to be
**	copied in the formats and order in which they
**	exist in the relation
*/
mapfill(aptr)
char	**aptr;
{
	register char		**ap;
	register struct map	*mp;
	register int		i;
	char			*fp;
	extern DESC		 Attdes;
	struct	attribute	att;
	struct tup_id		tid;
	struct tup_id		limtid;
	int			given;
	int			cnt;
	extern char		*dumvalue();
	extern char		*zcheck();

	Mapcount = 0;
	mp = Map;
	ap = aptr;

	/* Gather list of user supplied attributes */

	while (**ap)
	{
		/* check for overflow */
		if (Mapcount == MAXMAP)
			return (error(5803, (char *) 0));	/* more than MAXMAP specifiers */

		mp->paramname = *ap;	/* save pointer to user supplied name */
		AApmove(*ap++, mp->name, MAXNAME, ' ');
		fp = *ap++;	/* fp points to format string */
		mp->used = 0;
		mp->rlen = 0;	/* zero in case this is a dummy domain */
		mp->roffset = 0;
		mp->fdelim = 0;
		/* check domain type in *fp */
		switch (*fp++)
		{
		  case 'c':
			i =  CHAR;
			if (!(mp->fdelim = zcheck(fp)))
				return (-1);	/* bad delimitor */
			break;

		  case 'f':
			i = FLOAT;
			break;

		  case 'i':
			i = INT;
			break;

		  case DUMMY:
			i = DUMMY;
			if (!(mp->fdelim = zcheck(fp)))
				return (-1);
			break;

		  default:
			return (error(5811, mp->paramname, --fp, (char *) 0));
		}
		mp->ftype = i;


		/* convert format length to binary */
		if (AA_atoi(fp, &mp->flen) || mp->flen < 0 || mp->flen > 511 ||
		    (mp->ftype == FLOAT
# ifndef NO_F4
			&& mp->flen != sizeof (float)
# endif
			&& mp->flen != sizeof (double)) ||
		    (mp->ftype == INT
			&& mp->flen != sizeof (char)
			&& mp->flen != sizeof (short)
			&& mp->flen != sizeof (long)))
		{
			/* bad length for attribute */
			return (error(5804, mp->paramname, --fp, (char *) 0));
		}

		/* process dummy domain if any */
		if (Into && mp->ftype == DUMMY && mp->flen)
		{
			if (!(fp = dumvalue(mp->paramname)))
				return (5807);	/* bad dummy name */
			mp->rtype = *fp;	/* use first char of string */
		}

		/* check for format of type "c0delim" on copy "into" */
		if (Into && !mp->flen && mp->fdelim != Delimitor)
		{
			fp = mp->fdelim;

			/* is there room for a dummy domain? */
			mp++;
			if (++Mapcount == MAXMAP)
				return (error(5803, (char *) 0));	/* no room */

			/* create a dummy entry */
			mp->ftype = DUMMY;
			mp->flen = sizeof (char);
			mp->rtype = *fp;
			mp->roffset = mp->rlen = 0;
		}

		mp++;
		Mapcount++;
	}
	/* if no atributes were given, set flag */
	if (Mapcount)
		given = TRUE;
	else
		given = FALSE;

	/* open attribute relation and prepare for scan */
	opencatalog(AA_ATT, 0);

	AAam_setkey(&Attdes, &att, Des.reldum.relid, ATTRELID);
	AAam_setkey(&Attdes, &att, Des.reldum.relowner, ATTOWNER);

	if (AAam_find(&Attdes, EXACTKEY, &tid, &limtid, &att))
		AAsyserr(11010);

	/* scan Map for each relation attribute */
	while (!(i = AAam_get(&Attdes, &tid, &limtid, &att, 1)))
	{
		if (!AAbequal(&Des, &att, MAXNAME + 2))
			continue;
		/* if no user attributes were supplied, fake an entry */
		if (!given)
		{
			Mapcount++;
			i = att.attid;
			mp = &Map[i - 1];
			mp->rtype = mp->ftype = att.attfrmt;
			mp->rlen = mp->flen = ctou(att.attfrml);
			mp->roffset = att.attoff;
			mp->used = 1;
			mp->paramname = mp->name;	/* point to name */
			AAbmove(att.attname, mp->name, MAXNAME);	/* copy name */
			continue;
		}
		mp = Map;

		/* check each user domain for match with relation domain */
		for (i = Mapcount; i--;  mp++)
		{
			if (mp->ftype == DUMMY)
				continue; /* ignore dummy */
			if (!AAbequal(mp->name, att.attname, MAXNAME))
				continue;

			mp->rtype = att.attfrmt;
			mp->rlen = ctou(att.attfrml);
			mp->roffset = att.attoff;
			mp->used++;

			/* check for special case of C0 in a copy "into" */
			if (Into && !mp->flen && mp->ftype == CHAR)
			{
				switch (mp->rtype)
				{
				  case CHAR:
					mp->flen = mp->rlen;
					break;

				  case INT:
					switch (mp->rlen)
					{
					  case sizeof (char):
						mp->flen = AAout_arg.i1width;
						break;

					  case sizeof (short):
						mp->flen = AAout_arg.i2width;
						break;

					  case sizeof (long):
						mp->flen = AAout_arg.i4width;
					}
					break;

				  case FLOAT:
# ifndef NO_F4
					if (mp->rlen == sizeof (float))
						mp->flen = AAout_arg.f4width;
					else
# endif
						mp->flen = AAout_arg.f8width;
				}
			}
			/*  if this is a copy "from" then break
			    otherwise continue. In a copy "into"
			    an attribute might be copied more than once */
			if (!Into)
				break;
		}
	}
	if (i < 0)
		AAsyserr(11011, i);

	/* check that all user domains have been identified */
	cnt = 0;
	mp = Map;
	for (i = Mapcount; i--; mp++)
	{
		cnt += mp->flen;
		if (mp->ftype == DUMMY)
			continue;
		if (!mp->used)
		{
			return (error(5801, mp->paramname, Relname, (char *) 0));	/* unrecognizable domain name */
		}
	}
	/* check that copy into doesn't exceed buffer size */
	if (Into && cnt > BUFSIZE)
		return (error(5817, (char *) 0));	/* cnt too large */
	return (0);
}


bread(mp)
struct map	*mp;
{
	register int	count;
	register int	i;
	register char	*inp;
	char		*dl;
	register int	esc;	/* escape flag */

	count = mp->flen;
	inp = Inbuf;

	if (count)
	{
		/* block mode. read characters */
# ifdef FREAD
		i = fread(inp, 1, count, File_iop);
		/* null terminate */
		*(inp + count) = '\0';
# else
		for (i = 0; i < count; i++)
		{
			if ((esc = getc(File_iop)) < 0)
				break;
			*inp++ = esc;
		}
		/* null terminate */
		*inp = '\0';
# endif

		return (i == count);	/* true -> normal, false ->eof */
	}

	/* string mode read */
	/*
	** Determine the maximum size the C0 field being read can be.
	** In the case where it is being copied into a CHAR field, then
	** the size is that of the char field (+1 for the delimitor).
	** In the case of a numeric, it is limited only by the size of the
	** buffer area.
	*/
	count = mp->rtype == CHAR? mp->rlen + 1: BUFSIZE;
	esc = FALSE;

	for ( ; ; )
	{
		if ((i = getc(File_iop)) == EOF)
			return (inp == Inbuf? 0: -1);	/* -1 -> unexpected EOF, 0 -> normal EOF */

		if (count > 0)
		{
			count--;
			*inp++ = i;
		}
		else
		{
			if (!count)
			{
				/* determine type of overflow */
				if (mp->rtype == CHAR)
				{
					Truncount++;
					count--;	/* read until delim */
				}
				else
					return (-1);
			}
		}
		if (esc)
		{
			esc = FALSE;
			continue;
		}
		if (i == ESCAPE)
		{
			esc = TRUE;
			/*
			** If esc was stored, back it up.
			*/
			if (count >= 0)
			{
				inp--;	/* remove escape char */
				count++;	/* restore counter */
			}
		}
		else
		{
			for (dl = mp->fdelim; *dl; dl++)
				if (*dl == i)
				{
					*(inp - 1) = '\0';
					return (1);
				}
		}
	}
}


/*
**	Look for the existence of a param of the
**	form "0nl" or "00comma" etc.
**
**	Returns the correct delim list or 0
**	if there was a user error
**
**	If successful, a null is inserted at the
**	rightmost '0' so the subsequent AA_atoi will work.
*/
char	*zcheck(np)
register char	*np;
{
	register char	*ret;
	extern char	*dumvalue();

	ret = Delimitor;	/* assume default delimitors */

	if (*np++ == '0')
	{
		/* we have a starting zero. trim the rest */
		while (*np == '0')
			np++;

		if (*np > '9' || (*np < '0' && *np >= ' '))
		{
			/* we have a special delim on a 0 width field */
			if (ret = dumvalue(np))
				*(--np) = '\0';	/*
						** end string before delim
						** Do not alter delimitor but
						** instead destroy last '0'.
						*/
		}
	}
	return (ret);
}


/*
**	Search list of valid dummy names looking
**	for 'name'. If 'name' is a single char
**	then use just that name else it is
**	an error if the name is not found
*/
char	*dumvalue(name)
char	*name;
{
	register char	**dp;
	register char	*np;
	register char	*ret;

	dp = Cpdomains;	/* get list of valid dummy names */
	np = name;
	ret = 0;

	/* first look for a matching key word */
	while (*dp)
	{
		if (AAsequal(np, *dp++))
		{
			ret = *dp;
			break;
		}
		dp++;
	}

	/* If single char, use that char */
	if (AAlength(np) == sizeof (char))
		ret = np;	/* use first char of name */
	if (!ret)
		error(5807, np, (char *) 0);

	return (ret);
}
