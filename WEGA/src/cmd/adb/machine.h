#
/*
 *	UNIX debugger: WEGA (s.out)
 */

/* unix parameters */
#define DBNAME "adb\n"

TYPE	long	SYMV;

/* symbol table in s.out file */
struct symtab {
	SYMV	symv;
	char	symf;
	char	symseg;
	char	symc[8];
};

#define SYMTABSIZ (sizeof (struct symtab))

#define SYMCHK 0377
#define SYMTYPE(symflag) (( (int)symflag>=041 || ((int)symflag>=02 && (int)symflag<=04))\
			?  (((int)symflag&017)>=3 ? DSYM : ((int)symflag&017))\
			: NSYM\
			)
