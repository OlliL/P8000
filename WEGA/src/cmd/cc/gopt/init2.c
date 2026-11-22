/*
 * init2.c - gopt: the options and the files of the optimizer pass. Written from /lib/gopt (no source known); names guessed.
 */
static char sccsid[] = "@[$]init2.c\t1.3  02/07/83 15:53:16 Copr. 1981 87wega3.2.";
#include "gopt.h"

char ifname[100] = {0};
int s4d72 = 0, s4d74 = 0, s4d76 = 0, s4d78 = 0, s4d7a = 0, qflag = 0, aflag = 0, s4d80 = 0, s4d82 = 0, s4d84 = 0, s4d86 = 0, s4d88 = 0;
int dflag = 0, tflag = 0, eflag = 0, s4d90 = 0, s4d92 = 0, s4d94 = 0, s4d96 = 0, s4d98 = 0;	/* s4d98: no code uses it */

extern int aflag;
extern int s4d72, qflag, s4d80, s4d82, s4d84, s4d86, s4d88, dflag, tflag, eflag, s4d90, s4d92, s0b5e;
extern FILE *stfile, *infile, *dfile;
extern long stg, stpos;
extern char *strcpy(), *strcat();
extern int uerror();
extern void mkdope(), mkqnames();

/* the options of the pass: -1 .. -5 the level, -a -b -d -e -f -l -p -q -r -s -t -y -Z the debug flags, -E base the files base.S, base.I, base.D */
void
initopts(argc, argv)
	int argc;
	char **argv;
{
	register int i;
	register char *s;
	register int j;
	register char *t;
	char *unused;	/* the frame of the original has four more bytes here */
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
					s4d82++;
					break;
				case 'l':
					s4d84++;
					break;
				case 'p':
					s4d86++;
					break;
				case 'f':
					s4d88++;
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
					s4d80++;
					break;
				case 'r':
					s4d90++;
					break;
				case 's':
					s4d92++;
					break;
				case 'Z':
					s4d72++;
					break;
				case 'E':
					stfile = fopen(strcat(strcpy(buf, argv[++i]), ".S"), "r");
					if (stfile == 0)
						printf("error in opening symbol table file %s\n", argv[i - 1]);
					fread(&stg, 4, 1, stfile);
					stpos = ftell(stfile);
					infile = fopen(strcat(strcpy(buf, argv[i]), ".I"), "r");
					if (infile == 0)
						printf("error in opening intermediate file %s\n", argv[i - 1]);
					dfile = fopen(strcat(strcpy(buf, argv[i]), ".D"), "rw");
					if (dfile == 0)
						printf("error in opening data file %s\n", argv[i - 1]);
					break;
				case '5':
					s0b5e = 5;
					break;
				case '4':
					s0b5e = 4;
					break;
				case '3':
					s0b5e = 3;
					break;
				case '2':
					s0b5e = 2;
					break;
				case '1':
					s0b5e = 1;
					break;
				default:
					uerror("bad option: %c", *s);
				}
	}
	mkdope();
	mkqnames();
}
