static char sccsid[] = "@[$]echo.c\t2.2  08/12/82 13:47:35 - 87wega3.2";

/* echo of System III with the -n of the Berkeley echo (no newline at the end); without stdio.h: putchar is the function
   of libc */
main(argc, argv)
char **argv;
{
	register char	*cp;
	register int	i, wd;
	int	j;
	int	nflg = 0;

	if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
		nflg++;
		argc--;
		argv++;
	}
	if(--argc == 0 && !nflg) {
		putchar('\n');
		exit(0);
	}
	for(i = 1; i <= argc; i++) {
		for(cp = argv[i]; *cp; cp++) {
			if(*cp == '\\')
			switch(*++cp) {
				case 'b':
					putchar('\b');
					continue;

				case 'c':
					exit(0);

				case 'f':
					putchar('\f');
					continue;

				case 'n':
					putchar('\n');
					continue;

				case 'r':
					putchar('\r');
					continue;

				case 't':
					putchar('\t');
					continue;

				case '\\':
					putchar('\\');
					continue;
				case '0':
					j = wd = 0;
					while ((*++cp >= '0' && *cp <= '7') && j++ < 3) {
						wd <<= 3;
						wd |= (*cp - '0');
					}
					putchar(wd);
					--cp;
					continue;

				default:
					cp--;
			}
			putchar(*cp);
		}
		if(i == argc) {
			if(!nflg)
				putchar('\n');
		} else
			putchar(' ');
	}
	exit(0);
}
