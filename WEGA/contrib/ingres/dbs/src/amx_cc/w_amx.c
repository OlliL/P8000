# include	"amx.h"

amx()
{
	register int		c;

	fclose(FILE_tmp);
	FILE_tmp = (FILE *) 0;

	/* setup temp file name */
	*Filespec = 'c';

	if (!freopen(Filename, "r", FILE_inp))
		yyexit(13);
# ifdef SETBUF
	setbuf(FILE_inp, BUF_inp);
# endif
	/* copy user program to amx */
	while ((c = getc(FILE_inp)) > 0)
		putc(c, FILE_amx);

# ifdef AMX_SC
	if (ExpImp < 0)
		goto eof;
# endif
	/* main */
# ifndef MEM_N
	if (Xbufs)
		fprintf(FILE_amx, msg(35), (Xbufs * sizeof (ACCBUF)) / sizeof (int));
# endif
	fprintf(FILE_amx, msg(33));
# ifdef MSDOS
	fprintf(FILE_amx, msg(205));
# endif
# ifdef MEM_N
	if (Xbufs)
		fprintf(FILE_amx, msg(35), (Xbufs * sizeof (ACCBUF)) / sizeof (int));
# endif
	fprintf(FILE_amx, msg(66));
	/* initialize access method  */
	if (Xbufs)	/* add some buffers for the access methods */
		fprintf(FILE_amx, msg(36), Xbufs, Xlocks);
	else
		fprintf(FILE_amx, msg(37), Xlocks);
	fprintf(FILE_amx, msg(38));

# ifdef AMX_SC
eof:
# endif
	end_file();
}
