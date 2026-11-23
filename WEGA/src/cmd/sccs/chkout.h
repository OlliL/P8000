/* chkout.h: what chkout.c and common.c share (WEGA qcv(5); reconstructed from /bin/chkout, the names are guessed) */

struct ver {			/* one version of the chkfile */
	long	start;		/* offset of its first diff line */
	long	pos;		/* offset of the next diff line to read */
	long	end;		/* offset of its &D line */
	int	curline;	/* line number of the text it edits */
	int	n1;		/* the lines of the current diff command */
	int	n2;
	int	rel;		/* its number rel.lev */
	int	lev;
	struct ver *next;
	char	cmd;		/* the current diff command: a c d, '*' at the end, 0: read the next one */
};

extern char lockname[];
extern int eflag;
extern int hflag;
extern int pflag;
extern char *progname;
extern int nfiles;
extern int nlines;
extern int errors;
extern int kwflag;
extern char chkname[];
extern char outname[];
extern char *dir;

struct ver *readchk();
char *setnames();
