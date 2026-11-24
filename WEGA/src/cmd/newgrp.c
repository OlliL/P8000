/*
 * newgrp - the newgrp of V7 as WEGA changed it: without an argument the group of the passwd entry, root needs no
 * membership or password, the messages to stderr, done() starts the shell of the passwd entry with the name "-name".
 */

static char sccsid[] = "@[$]newgrp.c\t2.1  09/22/83 11:07:33 - 87wega3.2";

#include <stdio.h>
#include <signal.h>
#include <grp.h>
#include <pwd.h>

struct	group	*getgrnam(), *getgrgid(), *grp;
struct	passwd	*getpwuid(), *pwd;
char	*getpass(), *crypt();

main(argc,argv)
int	argc;
char	**argv;
{
	register i;

	if((pwd=getpwuid(getuid())) == NULL) {
		fprintf(stderr, "You do not exist!\n");
		done("/bin/sh");
	}
	endpwent();
	if(argc > 2) {
		fprintf(stderr, "usage: newgrp [groupname]\n");
		done(pwd->pw_shell);
	}
	if(argc == 1) {
		grp = getgrgid(pwd->pw_gid);
		endgrent();
		if(grp == NULL) {
			fprintf(stderr, "Unknown group\n");
			done(pwd->pw_shell);
		}
	} else {
		grp = getgrnam(argv[1]);
		endgrent();
		if(grp == NULL) {
			fprintf(stderr, "%s: no such group\n", argv[1]);
			done(pwd->pw_shell);
		}
		if(pwd->pw_uid != 0) {
			for(i=0;grp->gr_mem[i];i++)
				if(strcmp(grp->gr_mem[i], pwd->pw_name) == 0)
					break;
			if(grp->gr_mem[i] == 0 && strcmp(grp->gr_name,"other")) {
				fprintf(stderr, "Sorry\n");
				done(pwd->pw_shell);
			}
			if(grp->gr_passwd[0] != '\0' && pwd->pw_passwd[0] == '\0') {
				if(strcmp(grp->gr_passwd, crypt(getpass("Password:"),grp->gr_passwd)) != 0) {
					fprintf(stderr, "Sorry\n");
					done(pwd->pw_shell);
				}
			}
		}
	}
	if(setgid(grp->gr_gid) < 0)
		perror("setgid");
	done(pwd->pw_shell);
}

done(s)
char *s;
{
	register i;
	register char *p, *q;
	static char name[14] = "";

	q = &name[12];
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setuid(getuid());
	for (i=3; i<15; i++)
		close(i);
	for (p = s; *p; p++)
		;
	while (p != s) {
		if (*p == '/')
			break;
		*q-- = *p--;
	}
	*q = *p;
	if (*q == '/')
		*q = '-';
	execl(s, q, 0);
	fprintf(stderr, "No shell!\n");
	exit(1);
}
