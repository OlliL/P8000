/* chkwhat: print the @[$] identification strings of files (WEGA, no System III source; reconstructed from /bin/chkwhat) */
#include <stdio.h>

static char sccsid[] = "@[$]chkwhat.c\t1.4  02/10/83 16:53:20 - 87wega3.2";

char pattern[] = "@[$]";
int wflag = 0;
char usage[] = "Usage: %s [-w] file ...\n";

int errors;
int nfiles;
char *progname;

main(argc, argv)
int argc;
char **argv;
{
	argc--;
	progname = *argv++;
	if (**argv == '-') {
		wflag++;
		argc--;
		argv++;
	}
	if (argc == 0) {
		fprintf(stderr, usage, progname);
		exit(1);
	}
	nfiles = argc;
	while (argc--)
		dowhat(*argv++);
	exit(errors);
}

dowhat(name)
char *name;
{
	FILE *fp;
	int c;
	char *p;

	if ((fp = fopen(name, "r")) == NULL) {
		fprintf(stderr, "%s: cannot open %s\n", progname, name);
		errors++;
		return;
	}
	p = pattern;
	while ((c = getc(fp)) != EOF) {
		if (*p == c) {
			if (*++p)
				continue;
			if (nfiles > 1)
				printf("%s: ", name);
			if (wflag)
				printf("%s ", pattern);
			for (c = getc(fp); c && c != '\n' && c != '"' && c != '\''; c = getc(fp))
				putchar(c);
			putchar('\n');
		}
		p = pattern;
	}
	fclose(fp);
}
