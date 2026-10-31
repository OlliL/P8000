static char sccsid[] = "@[$]rm.c\t1.8  05/19/83 11:33:28 - 87wega3.2";

/*
**	rm [-fir] file ...
**	rm -i		(each entry of the current directory, asked one by one)
**
**	System III rm with the WEGA changes: without a terminal on the standard input -f is set,
**	messages on the standard output, perror for what unlink and close report, rmdir from
**	/bin or /usr/bin, a question only below the named directory (-ri), and rm -i alone.
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/dir.h>

char	*myname = "rm: cannot remove";	/* name guessed */
int	errcode;
char	errbuf[100];		/* the message for perror (size guessed) */

main(argc, argv)
char *argv[];
{
	register char *arg;
	int	fflg, iflg, rflg;

	fflg = 0;
	if(!isatty(0))
		fflg++;
	iflg = 0;
	rflg = 0;
	if(argc>1 && argv[1][0]=='-') {
		arg = *++argv;
		argc--;
		while(*++arg != '\0')
			switch(*arg) {
			case 'f':
				fflg++;
				break;
			case 'i':
				iflg++;
				break;
			case 'r':
				rflg++;
				break;
			default:
				printf("usage: rm [-fir] file ...\nOR     rm -i\n");
				exit(1);
			}
	}
	if(argc-1 == 0)
		if(iflg)
			interact();
		else
			printf("usage: rm [-fir] file ...\nOR     rm -i\n");
	while(--argc > 0) {
		if(!strcmp(*++argv, "..")) {
			fprintf(stderr, "rm: cannot remove `..'\n");
			continue;
		}
		rm(*argv, fflg, rflg, iflg, 0);
	}

	exit(errcode);
}

rm(arg, fflg, rflg, iflg, level)
char arg[];
{
	struct	stat	buf;
	struct	direct	direct;
	char	name[100];
	int	d;

	if(stat(arg, &buf)) {
		if (fflg==0) {
			printf("rm: %s nonexistent\n", arg);
			++errcode;
		}
		return;
	}
	if ((buf.st_mode&S_IFMT) == S_IFDIR) {
		if(rflg) {
			if(access(arg, 02) < 0) {
				if (fflg==0)
					printf("%s not changed\n", arg);
				++errcode;
				return;
			}
			if(iflg && level) {
				printf("directory %s: ", arg);
				if(!yes())
					return;
			}
			if((d=open(arg, 0)) < 0) {
				printf("rm: %s: cannot read\n", arg);
				exit(1);
			}
			while(read(d, &direct, sizeof(direct)) == sizeof(direct)) {
				if(direct.d_ino != 0 && !dotname(direct.d_name)) {
					sprintf(name, "%s/%.14s", arg, direct.d_name);
					rm(name, fflg, rflg, iflg, level+1);
				}
			}
			if(close(d) < 0)
				perror(arg);
			errcode += rmdir(arg, iflg);
			return;
		}
		printf("rm: %s directory\n", arg);
		++errcode;
		return;
	}

	if(iflg) {
		printf("%s: ", arg);
		if(!yes())
			return;
	}
	else if(!fflg) {
		if(access(arg, 02) < 0) {
			printf("rm: %s %o mode ", arg, buf.st_mode&0777);
			if(!yes())
				return;
		}
	}
	if(unlink(arg) && (fflg==0 || iflg)) {
		sprintf(errbuf, "%s %s", myname, arg);
		perror(errbuf);
		++errcode;
	}
}

dotname(s)
char *s;
{
	if(s[0] == '.')
		if(s[1] == '.')
			if(s[2] == '\0')
				return(1);
			else
				return(0);
		else if(s[1] == '\0')
			return(1);
	return(0);
}

rmdir(f, iflg)
char *f;
{
	int	status, i;

	if(dotname(f))
		return(0);
	if(iflg) {
		printf("%s: ", f);
		if(!yes())
			return(0);
	}
	while((i=fork()) == -1)
		sleep(3);
	if(i) {
		wait(&status);
		return(status);
	}
	execl("/bin/rmdir", "rmdir", f, 0);
	execl("/usr/bin/rmdir", "rmdir", f, 0);
	printf("rm: can't find rmdir\n");
	exit(1);
}

yes()
{
	int	i, b;

	i = getchar();
	while(i == '\t' || i == ' ')
		i = getchar();
	if(i == '\n')
		return(0);
	b = getchar();
	while(b != '\n' && b != EOF)
		b = getchar();
	return(i == 'y' || i == 'Y');
}

/* rm -i: every entry of the current directory, its name printable, asked one by one (name guessed) */
interact()
{
	struct	direct	dir;
	struct	stat	st;
	char	cmd[16];
	register int fd;
	register int i, j;
	char	ans[80];
	char	name[22];

	if((fd = open(".", 0)) < 0) {
		perror(".");
		exit(1);
	}
	while(read(fd, &dir, sizeof(dir)) == sizeof(dir)) {
		strncpy(name, "", 15);
		if(dir.d_ino == 0)
			continue;
		if(!strcmp(dir.d_name, "."))
			continue;
		if(!strcmp(dir.d_name, ".."))
			continue;
		for(i = j = 0; dir.d_name[i]; ) {
			if(dir.d_name[i] > 0x1f && dir.d_name[i] < 0x7f) {
				name[j] = dir.d_name[i];
				i++;
				j++;
			} else if(dir.d_name[i] >= 0 && dir.d_name[i] < ' ') {
				name[j++] = '^';
				name[j] = dir.d_name[i] + 0x40;
				i++;
				j++;
			} else {
				name[j++] = '?';
				i++;
			}
		}
		if(stat(dir.d_name, &st) < 0)
			perror(name);
		if(st.st_mode & S_IFDIR) {
			printf("%s--directory: ");
			if(gets(ans) == NULL)
				continue;
			if(ans[0] != 'y')
				continue;
			sprintf(cmd, "rm -rf %s", dir.d_name);
			if(system(cmd))
				perror(cmd);
		} else {
			printf("%s: ", name);
			if(gets(ans) == NULL)
				continue;
			if(ans[0] == 'y') {
				if(unlink(dir.d_name) < 0)
					perror(name);
				else
					printf(" [removed]\n");
			} else
				printf(" [untouched]\n");
		}
	}
	if(close(fd) < 0)
		perror(".");
	exit(0);
}
