# include	"amx.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif

# define	OUT_FLAG	"-o"

extern char	*AMX;
extern char	AMXF;
extern char	*A_out;
extern int	Argc;
extern char	**Argv;
extern int	Xargc;
extern char	**Xargv;
extern int	Xerrors;

# ifndef MSDOS
/*
**  GETPATH -- initialize the AApath variable
**
**	Sets AApath to the home directory of the USERDBS
**	[bs.h] user.
*/
getpath()
{
	char			line[MAXLINE + 1];
	static char		path[41];
	static int		reenter;
	register int		i;
	register char		*lp;
	register FILE		*iop;
	char			*field[UF_NFIELDS];

	if (reenter)
		return;
	else
		reenter++;

	if (!(iop = fopen(AA_PASSWD, "r")))
	{
		printf("Passwordfile %s nicht lesbar\n", AA_PASSWD);
		exit(1);
	}

	AApath = path;
	do
	{
		/* get a line from the AA_PASSWD */
		i = 0;
		for (lp = line; (*lp = getc(iop)) != '\n'; lp++)
		{
			if (*lp == (char) -1)
			{
				printf("Nutzer %s im Passwordfile %s nicht verzeichnet\n", USERDBS, AA_PASSWD);
				exit(1);
			}
			if (++i > sizeof line - 1)
			{
				*lp = '\0';
				printf("Zeilenpufferueberlauf bei \"%s\"\n", line);
				exit(1);
			}
		}
		*lp = '\0';
		for (i = 0, lp = line; *lp; lp++)
		{
			if (*lp == ':')
			{
				*lp = '\0';
				field[++i] = lp + 1;
			}
		}
	} while (!AAsequal(line, USERDBS));
	fclose(iop);

	/* check for enough fields for valid entry */
	if (i < PF_HOME)
	{
		printf("Zu wenig Felder in dem Eintrag \"%s\"\n", line);
		exit(1);
	}

	/* check that pathname won't overflow static buffer */
	if ((i = AAlength(field[PF_HOME]) + 1) > sizeof path)
	{
		printf("DBS-Pfadname \"%s\" ist zu lang\n", field[PF_HOME]);
		exit(1);
	}

	/* move pathname into buffer */
	AAbmove(field[PF_HOME], AApath, i);
}
# endif


get_next_file()
{
	register char		*file;
	register int		extension;
	register int		i;
	extern int		end_amx();
	extern char		*AAcustomer();
	extern char		**talloc();

	while (file = Argv[++Argc])
	{
		if (AAsequal(file, OUT_FLAG))
		{
			A_out = Argv[++Argc];
			continue;
		}

		i = AAlength(file);
		extension = --i;
		if (--i < 0 || (!(AAsequal(file + i, ".x"))
# ifdef MSDOS
			 && !AAsequal(file + i, ".X")
# endif
		))
			continue;

# ifdef MSDOS
		printf("%s:\n", file);
# else
		printf("%s -- %s/%c %s:\n",
			AAcustomer(end_amx), AMX, AMXF, file);
# endif
		if (!(FILE_inp = fopen(file, "r")))
		{
			amxerror(188, file);
			continue;
		}
# ifdef SETBUF
		setbuf(FILE_inp, BUF_inp);
# endif
		/* save source file name */
		File_src = (char *) talloc(extension + 2);
		AAbmove(file, File_src, extension + 2);

		/* create amx file */
		file[extension] = 'c';
		if (!(FILE_amx = fopen(file, "w")))
			yyexit(185, file);
# ifdef SETBUF
		setbuf(FILE_amx, BUF_amx);
# endif
		Xargv[Xargc++] = file;

		/* create desc stream */
		*Filespec = 'r';
		close(creat(Filename, 0600));
		if ((FILE_rel = open(Filename, 2)) < 0)
			yyexit(1);

		/* create tmp stream */
		*Filespec = 'c';
		if (!(FILE_tmp = fopen(Filename, "w")))
			yyexit(1);
# ifdef SETBUF
		setbuf(FILE_tmp, BUF_tmp);
# endif
		return (1);
	}
	return (0);
}


end_file()
{
	extern int		AAcc_init;

	/* close files and unlink temps */
	if (FILE_inp)
	{
		fclose(FILE_inp);
		FILE_inp = (FILE *) 0;
	}
	if (FILE_amx)
	{
		fclose(FILE_amx);
		FILE_amx = (FILE *) 0;
	}
	if (FILE_tmp)
	{
		fclose(FILE_tmp);
		FILE_tmp = (FILE *) 0;
	}
	if (FILE_scn)
	{
		fclose(FILE_scn);
		FILE_scn = (FILE *) 0;
	}
	if (FILE_rel)
	{
		close(FILE_rel);
		FILE_rel = -1;
	}
	*Filespec = 'r';
	unlink(Filename);
	*Filespec = 'c';
	unlink(Filename);
	*Filespec = 's';
	unlink(Filename);
	if (AAcc_init)
	{
		AAinpclose(&Indes);
		AAunlall();
		AAam_exit();
	}
# ifdef MSDOS
	fcloseall();
# endif
}


end_amx(stat)
register int	stat;
{
# ifndef MSDOS
	register char		**argv;
	register char		*file;

	argv = Xargv;
	argv[Xargc] = (char *) 0;
	while (file = *argv++)
		unlink(file);
	close(FILE_msg);
	fflush(stdout);
# else
	fcloseall();
# endif
	exit(stat? stat: Xerrors);
}


AAndxsearch(d)
register DESC	*d;
{
	yyexit(207, d->reldum.relid);
}
