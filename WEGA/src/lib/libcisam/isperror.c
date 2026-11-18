char	*is_errlist[] = {
	"Duplicate Record",
	"File not open",
	"Illegal Argument",
	"Illegal key descriptor",
	"Too many C-ISAM files open",
	"Bad C-ISAM file format",
	"Non-exclusive access",
	"Record locked by other process",
	"Key already exists",
	"Is prime key",
	"End/begin of file",
	"No record found",
	"No current record",
	"File locked"
};
int	is_nerr = 14;
int	is_ferror = 100;

int	iserrno;
int	sys_nerr;
char	*sys_errlist[];

isperror(s)
char	*s;
{
	register char	*c;
	register int	n;
	register int	k;
	int	m;

	c = "Unknown error";
	if (iserrno < sys_nerr)
		c = sys_errlist[iserrno];
	else if (iserrno >= is_ferror && (k = iserrno - is_ferror) < is_nerr)
		c = is_errlist[k];
	if (n = strlen(s)) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	m = strlen(c);
	write(2, c, m);
	write(2, "\n", 1);
}
