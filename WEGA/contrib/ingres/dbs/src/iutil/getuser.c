# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

/*
**  GET LINE FROM USER FILE
**
**	Given a user code (a two byte string), this routine returns
**	the line from AA_USERS into 'buf'.  The users
**	file is automatically opened, and it is closed if getuser
**	is called with 'code' == 0.
**
**	If 'code' == -1 then getuser will reinitialize itself.
**	This will guarantee that getuser will reopen the file
**	if (for example) an interrupt occured during the previous
**	call.
*/
getuser(code, buf)
register char	*code;
char		buf[];
{
	register int	c;
	register char	*bp;
	static FILE	*userf;
# ifdef SETBUF
	char		userb[BUFSIZ];
# endif
	extern char	*fgetline();

	if (!code)
	{
		if (userf)
			fclose(userf);
		userf = (FILE *) 0;
		return (0);
	}
	if (code == (char *) -1)
	{
		userf = (FILE *) 0;
		return (0);
	}
	if (!userf)
	{
		if (!(userf = fopen(AAztack(AApath, AA_USERS), "r")))
			AAsyserr(15002);
# ifdef SETBUF
		setbuf(userf, userb);
# endif
	}
	else
	{
		rewind(userf);
# ifdef SETBUF
		setbuf(userf, userb);
# endif
	}

	for ( ; ; )
	{
		if (!fgetline(bp = buf, MAXLINE, userf))
			goto err;
		while ((c = *bp++) != ':')
			if (!c)
			{
err:
# ifdef SETBUF
				setbuf(userf, (char *) 0);
# endif
				return (1);
			}
		if (AAbequal(bp, code, 2))
		{
# ifdef SETBUF
			setbuf(userf, (char *) 0);
# endif
			return (0);
		}
	}
}
