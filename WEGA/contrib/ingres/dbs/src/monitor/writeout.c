# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  WRITE OUT QUERY BUFFER TO BS FILE
**
**	The logical buffer is written to a BS file, the name of which
**	must follow the \w command.
*/
writeout()
{
	register int		i;
	register char		*file;
	register int		source;
	register int		dest;
	char			buf[BUFSIZ];
	extern char		*getfilename();

	file = getfilename();
	if (!*file || *file == '-')
	{
		printf("%sUnakzeptabler file name `%s'\n", ERROR, file);
		return;
	}

	if ((dest = creat(file, 0644)) < 0)
	{
		printf("%screate WRITE file `%s'\n", ERROR, file);
		return;
	}

	if (!Nautoclear)
		Autoclear = 1;

	if ((source = open(Qbname, 0)) < 0)
		AAsyserr(16025, Qbname);

	fflush(Qryiop);

	while ((i = read(source, buf, sizeof buf)) > 0)
		write(dest, buf, i);

	close(source);
	close(dest);
}
