/* bd: hexadecimal byte dump (WEGA, no System III source; reconstructed from /bin/bd) */
#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
	FILE *fp;
	int i, j, addr;
	int buf[200];
	int c;

	if (argc == 1)
		fp = stdin;
	else if ((fp = fopen(*++argv, "r")) == NULL) {
		printf("can't open file %s\n", *argv);
		goto out;
	}
	i = 0;
	addr = 0;
	printf("\n%4x  ", addr);
	while ((c = getc(fp)) != EOF) {
		buf[i] = c;
		printf(" %02x", c);
		if (++i < 16)
			continue;
		printf("\t*");
		for (j = 0; j < 16; j++)
			if (buf[j] >= ' ' && buf[j] < 0177)
				printf("%c", buf[j]);
			else
				printf(".");
		i = 0;
		addr += 16;
		printf("*\n%4x  ", addr);
	}
	j = i;
	while (j++ < 16)
		printf("   ");
	printf("\t*");
	for (addr = 0; addr < i; addr++)
		if (buf[addr] >= ' ')
			printf("%c", buf[addr]);
		else
			printf(".");
	printf("*\n");
	if (fp != stdin)
		fclose(fp);
out:
	printf("\n");
}
