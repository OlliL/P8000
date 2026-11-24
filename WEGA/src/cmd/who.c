/*
 * who
 */

static char sccsid[] = "@[$]who.c\t2.1  11/18/83 14:53:44 - 87wega3.2";

#include <stdio.h>
#include <utmp.h>
#include <pwd.h>
#include <grp.h>
struct utmp utmp;
struct passwd *pw;
struct passwd *getpwuid();

char *ttyname(), *strrchr(), *ctime(), *strcpy();
struct group *getgrgid();
main(argc, argv)
char **argv;
{
	register char *tp, *s;
	register FILE *fi;
	int i;
	int id;
	char *name;
	static char stdbuf[BUFSIZ];

	setbuf(stdout,stdbuf);
	s = "/etc/utmp";
	if(argc == 2) {
		if(argv[1][0] == '-') {
			if(argv[1][1] == 'g') {
				id = getgid();
				name = getgrgid(id)->gr_name;
				printf("group: %s\n", name);
				exit(0);
			} else {
				fprintf(stderr, "who: illegal argument\n");
				exit(2);
			}
		} else
			s = argv[1];
	}
	if (argc == 3) {
		for(i=0;i<=2;i++)
			if((tp=ttyname(i)) != NULL) break;
		if (tp)
			tp = strrchr(tp, '/') + 1;
		else {	/* no tty - use best guess from passwd file */
			id = getuid();
			pw = getpwuid(id);
			strcpy(utmp.ut_name, pw?pw->pw_name: "?");
			strcpy(utmp.ut_line, "tty??");
			time(&utmp.ut_time);
			putline();
			exit(0);
		}
	}
	if ((fi = fopen(s, "r")) == NULL) {
		fprintf(stderr, "who: cannot open %s\n", s);
		exit(2);
	}
	while (fread((char *)&utmp, sizeof(utmp), 1, fi) == 1) {
		if(argc == 3) {
			if (strcmp(utmp.ut_line, tp))
				continue;
			putline();
			exit(0);
		}
		if(utmp.ut_name[0] == '\0' && argc==1)
			continue;
		putline();
	}
}

putline()
{
	register char *cbuf;

	printf("%-8.8s %-8.8s", utmp.ut_name, utmp.ut_line);
	cbuf = ctime(&utmp.ut_time);
	printf("%.12s\n", cbuf+4);
}
