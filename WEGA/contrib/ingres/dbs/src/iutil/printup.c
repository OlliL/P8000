# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/symbol.h"

# define	PAGELGTH	56

static int		AAprlength;
extern struct out_arg	AAout_arg;

AAprtup(d, tuple)
DESC			*d;
char			*tuple;
{
	register char		*ftype;
	register char		*flen;
	register short		*foff;
	register int		i;
	register int		type;
	register int		off;

	ftype = &d->relfrmt[1];
	flen = &d->relfrml[1];
	foff = &d->reloff[1];
	i = d->reldum.relatts;

	/* If relation is S_BINARY then print char fields escaped */
	if (d->reldum.relstat & S_BINARY)
		while (i--)
		{
			off = *foff++;
			AApratt((type = *ftype++) == CHAR? BINARY: type, *flen++, &tuple[off]);
		}
	else
		while (i--)
			AApratt(*ftype++, *flen++, &tuple[off = *foff++]);
	AApreol();
}


AAprkey(d, tuple)
DESC			*d;
char			*tuple;
{
	register char		*fgiven;
	register char		*ftype;
	register char		*flen;
	register short		*foff;
	register int		i;
	register int		off;

	fgiven = &d->relgiven[1];
	ftype = &d->relfrmt[1];
	flen = &d->relfrml[1];
	foff = &d->reloff[1];
	for (i = d->reldum.relatts; i; --i, ftype++, flen++, foff++)
		if (*fgiven++)
		{
			off = *foff;
			AApratt(*ftype, *flen, &tuple[off]);
		}
	AApreol();
}


AApratt(type, length, value1)
int		type;
register int	length;
register char	*value1;
{
	register char		*value;
	register int		i;
	char			valbuf[sizeof (double)];
	char			buf[MAXFIELD];
	extern char		*AA_iocv();
	extern char		*AA_locv();

	putchar(AAout_arg.coldelim);
	switch (type)
	{
	  case INT:
		value = valbuf;
		AAbmove(value1, value, length);
		switch (length)
		{
		  case sizeof (char):
			AAprfatt(AAout_arg.i1width, AA_iocv(ctoi(((I1TYPE *) value)->i1type)));
			break;

		  case sizeof (short):
			AAprfatt(AAout_arg.i2width, AA_iocv(((I2TYPE *) value)->i2type));
			break;

		  case sizeof (long):
			AAprfatt(AAout_arg.i4width, AA_locv(((I4TYPE *) value)->i4type));
			break;

		  default:
			AAsyserr(15019, length);
		}
		return (0);

	  case FLOAT:
		value = valbuf;
		AAbmove(value1, value, length);
		switch (length)
		{
# ifndef NO_F4
		  case sizeof (float):
			AA_ftoa(((F4TYPE *) value)->f4type, buf, AAout_arg.f4width, AAout_arg.f4prec, AAout_arg.f4style);
			AAprfatt(AAout_arg.f4width, buf);
			break;
# endif

		  case sizeof (double):
			AA_ftoa(((F8TYPE *) value)->f8type, buf, AAout_arg.f8width, AAout_arg.f8prec, AAout_arg.f8style);
			AAprfatt(AAout_arg.f8width, buf);
			break;

		  default:
			AAsyserr(15020, length);
		}
		return (0);

	  case CHAR:
		length = ctou(length);
# ifdef FWRITE
		fwrite(value1, 1, length, stdout);
# else
		for (i = length; i > 0; i--, value1++)
			putchar(*value1);
# endif
		if ((length = AAout_arg.c0width - length) > 0)
			while (length--)
				putchar(' ');
		return (0);

	  case BINARY:
		length = ctou(length);
		value = value1;
		while (length--)
			AAxputchar(*value++);
		return (0);

	  default:
		AAsyserr(15021, type);
	}
	/*NOTREACHED*/
}


/*
**  FORMATTED ATTRIBUTE PRINT
**
**	Attribute 'value' is printed.  It is type 'type' and has a
**	field width of 'width'.
*/
AAprfatt(w, p)
register int	w;
register char	*p;
{
	register int	v;

	if ((v = AAlength(p)) > w)
	{
		/* field overflow */
		while (w--)
			putchar('*');
		return;
	}

	/* output the field */
	for (w -= v; w > 0; w--)
		putchar(' ');
# ifdef FWRITE
	fwrite(p, 1, v, stdout);
# else
	for ( ; v > 0; v--, p++)
		putchar(*p);
# endif
}


AApreol()
{
	putchar(AAout_arg.coldelim);
	putchar('\n');
	fflush(stdout);
}


AAendhdr()
{
	register int		i;
	register int		delim;

	putchar(AAout_arg.coldelim);
	delim = AAout_arg.linedelim;
	for (i = 1; i < AAprlength; i++)
		putchar(delim);
	AApreol();
}


AAprhdr(type, length, value)
int		type;
register int	length;
register char	*value;
{
	register int		i;
	register int		c;

	length = ctou(length);

	switch (type)
	{
	  case INT:
		switch (length)
		{
		  case sizeof (char):
			length = AAout_arg.i1width;
			break;

		  case sizeof (short):
			length = AAout_arg.i2width;
			break;

		  case sizeof (long):
			length = AAout_arg.i4width;
			break;

		  default:
			AAsyserr(15022, length);
		}
		break;

	  case FLOAT:
		switch (length)
		{
# ifndef NO_F4
		  case sizeof (float):
			length = AAout_arg.f4width;
			break;
# endif

		  case sizeof (double):
			length = AAout_arg.f8width;
			break;

		  default:
			AAsyserr(15023, length);
		}
		break;

	  case CHAR:
		if (length < AAout_arg.c0width)
			length = AAout_arg.c0width;
		break;

	  default:
		AAsyserr(15024, type);
	}

	putchar(AAout_arg.coldelim);
	for (i = 0; i < length && i < MAXNAME; i++)
		if (c = *value++)
			putchar(c);
		else
			break;

	for ( ; i < length; i++)
		putchar(' ');

	AAprlength += length + 1;
}


AAbgnhdr()
{
	AAprlength = 0;
	putchar('\n');
}
