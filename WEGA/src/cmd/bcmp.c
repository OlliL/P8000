/* bcmp: compare two files byte by byte (WEGA, no System III source; reconstructed from /bin/bcmp) */
#include <stdio.h>

FILE *fp1, *fp2;

main(argc, argv)
int argc;
char **argv;
{
	register int eof1;
	register long n;
	register int eof2;
	char c1, c2;
	char diff;

	if (argc == 2)
		fp1 = stdin;
	else {
		if ((fp1 = fopen(*++argv, "r")) == NULL) {
			printf("can't open file %s\n", *argv);
			exit(1);
		}
		if ((fp2 = fopen(*++argv, "r")) == NULL) {
			printf("can't open file %s\n", *argv);
			exit(1);
		}
	}
	n = 0;
	diff = 0;
	c1 = getc(fp1);
	c2 = getc(fp2);
	while (!(eof1 = feof(fp1)) && !(eof2 = feof(fp2))) {
		if ((c1 &= 0377) != (c2 &= 0377)) {
			diff = 1;
			printf("%6X :  ", n);
			printf("%02x ", c1 & 0xff);
			if (c1 <= 0x7f && c1 >= ' ')
				printf("'%c'", c1);
			else
				printf("   ");
			printf("\t%02x ", c2 & 0xff);
			if (c2 <= 0x7f && c2 >= ' ')
				printf("'%c'\n", c2);
			else
				putchar('\n');
		}
		c1 = getc(fp1);
		c2 = getc(fp2);
		n++;
	}
	if (diff) {
		printf("file ");
		if (eof1)
			putchar('1');
		if (feof(fp2)) {
			if (eof1)
				printf(" and ");
			putchar('2');
		}
		printf(" EOF\n");
	}
}
