/*
 * init2.c - gopt: the options and the files of the optimizer pass. Written from /lib/gopt (no source known); names guessed.
 */
static char sccsid[] __attribute__((unused)) = "@[$]init2.c\t1.3  02/07/83 15:53:16 Copr. 1981 87wega3.2.";
#include "mfile2.h"

char ftitle[100] = {0};
int zflag = 0, nlabrm = 0, njmprm = 0, ndeadrm = 0, hascall = 0, qflag = 0, aflag = 0, bdebug = 0, ddebug = 0, ldebug = 0, pdebug = 0, fdebug = 0;
int dflag = 0, tflag = 0, eflag = 0, rdebug = 0, sdebug = 0, maxtmp = 0, unusedflag = 0;	/* unusedflag: no code uses it */
long baseoff = 0;	/* host: long, as reader2.c stores it (the original: an int, the long also covers unusedflag) */

extern long stpos;

/* the options of the pass: -1 .. -5 the level, -a -b -d -e -f -l -p -q -r -s -t -y -Z the debug flags, -E base the files base.S, base.I, base.D */
void initopts(int argc, char **argv)
{
	register int i;
	register char *s;
	char buf[50];

	for (i = 1; i < argc; i++) {
		s = argv[i];
		if (*s == '-')
			while (*++s)
				switch (*s) {
				case 'y':
					dflag++;
					break;
				case 'q':
					qflag++;
					break;
				case 'd':
					ddebug++;
					break;
				case 'l':
					ldebug++;
					break;
				case 'p':
					pdebug++;
					break;
				case 'f':
					fdebug++;
					break;
				case 'e':
					eflag++;
					break;
				case 't':
					tflag++;
					break;
				case 'a':
					aflag++;
					break;
				case 'b':
					bdebug++;
					break;
				case 'r':
					rdebug++;
					break;
				case 's':
					sdebug++;
					break;
				case 'Z':
					zflag++;
					break;
				case 'E':
					stfile = fopen(strcat(strcpy(buf, argv[++i]), ".S"), "r");
					if (stfile == 0)
						printf("error in opening symbol table file %s\n", argv[i - 1]);
					stg = rd32(stfile);
					stpos = ftell(stfile);
					infile = fopen(strcat(strcpy(buf, argv[i]), ".I"), "r");
					if (infile == 0)
						printf("error in opening intermediate file %s\n", argv[i - 1]);
					dfile = fopenrw(strcat(strcpy(buf, argv[i]), ".D"));
					if (dfile == 0)
						printf("error in opening data file %s\n", argv[i - 1]);
					break;
				case '5':
					olevel = 5;
					break;
				case '4':
					olevel = 4;
					break;
				case '3':
					olevel = 3;
					break;
				case '2':
					olevel = 2;
					break;
				case '1':
					olevel = 1;
					break;
				default:
					uerror("bad option: %c", *s);
				}
	}
	mkdope();
	mkqnames();
}
