/*
** ---  RC_REP.C -------- REPORT OUTPUT  --------------------------------------
**
**	Version: @(#)rc_rep.c		4.0	02/05/89
**
*/

# include	"rc.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif

# undef FILEMODE
# define	FILEMODE		0644


rc_report(file)
char	*file;
{
	register int	fd;
	register int	len;
	register char	*name;

# ifndef RC_TRACE
	if (S_error || Compile == FALSE)
		return;
# endif

	S_name = name = Oflag? "r_code": file;

	if ((fd = creat(name, FILEMODE)) < 0)
	{
		rc_error(4, name);
		return;
	}
# ifdef MSDOS
	setmode(fd, O_BINARY);
# endif
	len = R_rc - R_code;
# ifdef RC_TRACE
	if (TR_OUTPUT)
# ifdef MSDOS
		printf("REPORT `%s': write from %Fp to %Fp, len %d\n",
# else
		printf("REPORT `%s': write from %08x to %08x, len %d\n",
# endif
			name, R_code, R_rc, len);
# endif
	len =    write(fd, R_res, sizeof R_res - 1) != sizeof R_res - 1
	      || write(fd, (char *) &R_cols, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_lines, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_all_lines, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_for_all, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_first_page, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_last_page, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_from_tuple, sizeof (int)) != sizeof (int)
	      || write(fd, (char *) &R_to_tuple, sizeof (int)) != sizeof (int)
	      || write(fd, R_code, len) != len;
	close(fd);
	if (len)
		rc_error(5, name);

	if (Doms)
	{
		if (!(name = S_desc))
			return;
		if ((fd = creat(name, FILEMODE)) < 0)
		{
			rc_error(4, name);
			return;
		}
# ifdef MSDOS
		setmode(fd, O_BINARY);
# endif
		len = Doms * (2 * sizeof (char) + sizeof (short));
# ifdef RC_TRACE
		if (TR_OUTPUT)
			printf("REPORT `%s': write %d doms, len %d\n", name, Doms, len);
# endif
		len = write(fd, Desc, len) != len;
		close(fd);
		if (len)
			rc_error(5, name);
	}
}
