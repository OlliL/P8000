# include	<stdio.h>

/*
**  CAT -- "cat" a file
**
**	This function is essentially identical to the cat(1).
**
**	Parameters:
**		file -- the name of the file to be cat'ed
**
**	Returns:
**		zero -- success
**		else -- failure (could not open file)
*/
AAcat(file)
register char	*file;
{
	register int	i;
	register int	fd;
	char		buf[BUFSIZ];

	if ((fd = open(file, 0)) < 0)
		return (1);

	while ((i = read(fd, buf, BUFSIZ)) > 0)
		write(1, buf, i);

	close(fd);
	return (0);
}
