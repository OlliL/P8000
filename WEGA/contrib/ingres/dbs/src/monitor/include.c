# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  INCLUDE FILE
**
**	A file name, which must follow the \i, is read and inserted
**	into the text stream at this location.  It may include all of
**	the standard control functions.  Includes may be nested.
**
**	If the parameter is 0, the file name is taken from the input;
**	otherwise it is taken directly from the parameter.  In this
**	mode, errors are not printed.
**
**	Prompts are turned off during the include.
*/
include(filename)
char	*filename;
{
	register char		*f;
	register FILE		*b;
	register int		savendf;
	register FILE		*saveinp;
# ifdef SETBUF
	char			inpbuf[BUFSIZ];
# endif
	extern char		*getfilename();

	if (!(f = filename))
		f = getfilename();
	if (AAsequal(f, "-"))	/* read keyboard */
		b = stdin;
	else if (!*f)
	{
		/* back up one level (EOF on next read) */
		Peekch = -1;
		return;
	}
	else	/* read file */
	{
		if (!(b = fopen(f, "r")))
		{
			if (!filename)
				printf("%sopen INCLUDE file '%s'\n", ERROR, f);
			return;
		}
# ifdef SETBUF
		setbuf(b, inpbuf);
# endif
	}

	/* check for too deep */
	if (Idepth >= INC_DEPTH)
	{
		printf("%sMehr als %d INCLUDE's\n", ERROR, INC_DEPTH);
		if (b && b != stdin)
			fclose(b);
		return;
	}
	Idepth++;

	/* get input from alternate file */
	savendf = Nodayfile;
	if (b == stdin)
	{
		Nodayfile = Userdflag;
		prompt(INPUT);
	}
	else
		Nodayfile = -1;
	saveinp = Input;
	Input = b;
	command();

	/* done -- restore old file */
	Input = saveinp;
	Nodayfile = savendf;
	Idepth--;
}
