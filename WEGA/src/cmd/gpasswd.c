/*
 * gpasswd - change the password of a group (WEGA; no source, written from /bin/gpasswd 1.4, 09/83, after the pattern of
 * System III passwd.c). The members of the group get a mail. The names are guesses.
 */

static char sccsid[] = "@[$]gpasswd.c\t1.4  09/12/83 22:05:09 - 87wega3.2";

#include <stdio.h>
#include <signal.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/lockblk.h>

char	mailcmd[512] = "mail ";
char	msg[] = "has changed the password for ";
char	grfile[] = "/etc/group";
char	temp[] = "/etc/gtmpXXXXXX";

char	buf[512];		/* the argument of notify, not used otherwise (the name is a guess) */
char	*lname;
char	gname[32];
char	*tname;
struct	lockblk lk;
int	unused;			/* a word of the bss of the original between lk and gr */
struct	group *gr, *grp;
char	*pw;
char	pwbuf[10];
char	line[512];		/* 512 bytes of the bss of the original behind pwbuf, not used */

struct	group *getgrent(), *getgrgid();
char	*crypt(), *getpass(), *getlogin(), *mktemp();
FILE	*popen();

catlist(s, list)
char *s;
char **list;
{
	int unused;	/* a word of the frame of the original, not used */

	while (*list) {
		strcat(s, *list++);
		strcat(s, " ");
	}
}

main(argc, argv)
char **argv;
{
	char *p;
	int i;
	char saltc[2];
	long salt;
	int u;
	int g;
	int x, y;	/* three words of the frame of the original, not used (also z) */
	int insist;
	long lkret;
	int mode;
	int ok, flags;
	int c;
	int pwlen;
	FILE *tf;
	FILE *gf;
	int z;

	lk.lkwhnce = 0;
	lk.lklen = 0L;
	mode = LKEXCLUSIVE|LKEXP;
	lk.lkoff = 0L;
	insist = 0;
	if (argc < 2) {
		if ((grp = getgrgid(getgid())) == NULL) {
			printf("usage: gpasswd group-name\n");
			goto bex;
		}
		printf("Changing password for %s group\n", grp->gr_name);
		strcpy(gname, grp->gr_name);
	} else
		strcpy(gname, argv[1]);
	lname = getlogin();
	while ((gr = getgrent()) != NULL)
		if (strcmp(gr->gr_name, gname) == 0)
			break;
	g = getgid();
	u = getuid();
	if (gr == NULL || (u != 0 && g != gr->gr_gid)) {
		printf("Permission denied.\n");
		goto bex;
	}
	endgrent();
	if (*gr->gr_passwd && u != 0) {
		strcpy(pwbuf, getpass("Old password:"));
		pw = crypt(pwbuf, gr->gr_passwd);
		if (strcmp(pw, gr->gr_passwd) != 0) {
			printf("Sorry.\n");
			goto bex;
		}
	}
tryagn:
	strcpy(pwbuf, getpass("New password:"));
	pwlen = strlen(pwbuf);
	if (pwlen == 0) {
		printf("Password unchanged.\n");
		goto bex;
	}
	ok = 0;
	flags = 0;
	p = pwbuf;
	while (c = *p++) {
		if (c >= 'a' && c <= 'z')
			flags |= 2;
		else if (c >= 'A' && c <= 'Z')
			flags |= 4;
		else if (c >= '0' && c <= '9')
			flags |= 1;
		else
			flags |= 8;
	}
	if (flags >= 7 && pwlen >= 4)
		ok = 1;
	if ((flags == 2 || flags == 4) && pwlen >= 6)
		ok = 1;
	if ((flags == 3 || flags == 5 || flags == 6) && pwlen >= 5)
		ok = 1;
	if (!ok && insist < 2) {
		if (flags == 1)
			printf("Please use at least one non-numeric character.\n");
		else
			printf("Please use a longer password.\n");
		insist++;
		goto tryagn;
	}
	if (strcmp(pwbuf, getpass("Retype new password:")) != 0) {
		printf("Mismatch - password unchanged.\n");
		goto bex;
	}
	time(&salt);
	salt += getpid();
	saltc[0] = salt & 077;
	saltc[1] = (salt >> 6) & 077;
	for (i = 0; i < 2; i++) {
		c = saltc[i] + '.';
		if (c > '9')
			c += 7;
		if (c > 'Z')
			c += 6;
		saltc[i] = c;
	}
	pw = crypt(pwbuf, saltc);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	tname = mktemp(temp);
	close(creat(temp, 0600));
	if ((tf = fopen(temp, "w")) == NULL) {
		printf("Cannot create temporary file\n");
		goto bex;
	}
	if ((gf = fopen(grfile, "r")) == NULL) {
		printf("Unable to open group file\n");
		goto bex;
	}
	if ((lkret = lkdata(fileno(gf), mode, &lk)) == -1) {
		fprintf(stderr, "Error locking group file--no change made\n");
		goto bex;
	}
	while ((gr = getgrent()) != NULL) {
		if (strcmp(gr->gr_name, gname) == 0) {
			g = getgid();
			u = getuid();
			if (u != 0 && g != gr->gr_gid) {
				printf("Permission denied.\n");
				goto out;
			}
			catlist(mailcmd, gr->gr_mem);
			gr->gr_passwd = pw;
		}
		fprintf(tf, "%s:%s:%d:", gr->gr_name, gr->gr_passwd, gr->gr_gid);
		if (*gr->gr_mem == NULL)
			fprintf(tf, "\n");
		else
			while (*gr->gr_mem) {
				fprintf(tf, "%s", *gr->gr_mem++);
				fprintf(tf, "%c", *gr->gr_mem == NULL ? '\n' : ',');
			}
	}
	endgrent();
	fclose(tf);
	if (unlink(grfile)) {
		fprintf(stderr, "Change failed--unable to make new group file\n");
		goto bex;
	}
	if (link(temp, grfile) && unlink(grfile)) {
		fprintf(stderr, "Group file destroyed.\nUpdated file in %s\n", grfile);
		goto bex;
	}
	unlk(fileno(gf), mode, &lk);
	fclose(gf);
	chmod(grfile, 0644);
	notify(buf);
out:
	unlink(temp);
bex:
	exit(1);	/* sic: also after a change */
}

notify(s)
char *s;
{
	register FILE *fp;

	if ((fp = popen(mailcmd, "w")) == NULL) {
		fprintf(stderr, "Unable to notify group members of password change\n");
		return(1);
	}
	setbuf(fp, NULL);
	fprintf(fp, "%s %s%s  group\n", lname, msg, gname);
	pclose(fp);
	return(0);
}
