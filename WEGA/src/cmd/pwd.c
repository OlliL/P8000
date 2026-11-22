/*
**	Print working (current) directory
*/

#include	<stdio.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/dir.h>

static char sccsid[] = "@[$]pwd.c\t1.3  07/24/82 02:46:34 - 87wega3.2";

struct	stat	d;
struct	direct	dir;

char	dot[]	 = ".";
char	dotdot[] = "..";
char	name[512];

int	file;
int	off = -1;
struct	stat	dd;	/* behind the others: the order of the bss of the original */

main()
{
	for(;;) {
		if(stat(dot, &d) < 0) {
			fprintf(stderr, "pwd: cannot stat .!\n");
			exit(2);
		}
		if ((file = open(dotdot,0)) < 0) {
			fprintf(stderr,"pwd: cannot open ..\n");
			exit(2);
		}
		if(fstat(file, &dd) < 0) {
			fprintf(stderr, "pwd: cannot stat ..!\n");
			exit(2);
		}
		if(chdir(dotdot) < 0) {
			fprintf(stderr, "pwd: cannot chdir to ..\n");
			exit(2);
		}
		if(d.st_dev == dd.st_dev) {
			if(d.st_ino == dd.st_ino)
				prname();
			do
				if (read(file, (char *)&dir, sizeof(dir)) < sizeof(dir)) {
					fprintf(stderr, "pwd: read error in ..\n");
					exit(2);
				}
			while (dir.d_ino != d.st_ino);
		}
		else do {
				if(read(file, (char *)&dir, sizeof(dir)) < sizeof(dir)) {
					fprintf(stderr, "pwd: read error in ..\n");
					exit(2);
				}
				stat(dir.d_name, &dd);
		} while(dd.st_ino != d.st_ino || dd.st_dev != d.st_dev);
		close(file);
		cat();
	}
}

prname()
{
	write(1, "/", 1);
	if (off<0)
		off = 0;
	name[off] = '\n';
	write(1, name, off+1);
	exit(0);
}

cat()
{
	register i, j;

	i = -1;
	while ((dir.d_name[++i] != 0) && (i < 14));
	if ((off+i+2) > 511)
		prname();
	for(j=off+1; j>=0; --j)
		name[j+i+1] = name[j];
	off=i+off+1;
	name[i] = '/';
	for(--i; i>=0; --i)
		name[i] = dir.d_name[i];
}
