# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

/*
**  GETNUSER -- get line from user file based on name
**
**	Given a user name as a string, this routine returns the
**	corresponding line from AA_USERS into a buffer.
**
**	Parameters:
**		name -- the name of the user
**		buf -- a buf to dump the line in (declare as
**			char buf[MAXLINE + 1]
**
**	Returns:
**		zero -- success
**		one -- failure (user not present)
*/
getnuser(name, buf)
char	*name;
char	buf[];
{
	register int	c;
	register char	*bp;
	register FILE 	*userf;
# ifdef SETBUF
	char		userbuf[BUFSIZ];
# endif

	if (!(userf = fopen(AAztack(AApath, AA_USERS), "r")))
		AAsyserr(15001);
# ifdef SETBUF
	setbuf(userf, userbuf);
# endif

	for ( ; ; )
	{
		bp = buf;
		while ((c = getc(userf)) != '\n')
		{
			if (c == EOF)
			{
				fclose(userf);
				return (1);
			}
			*bp++ = c;
		}
		*bp++ = '\0';
		bp = buf;
		while ((c = *bp++) != ':')
			if (!c)
			{
				fclose(userf);
				return (1);
			}
		*--bp = 0;
		if (AAsequal(buf, name))
		{
			fclose(userf);
			*bp = ':';
			return (0);
		}
	}
}
