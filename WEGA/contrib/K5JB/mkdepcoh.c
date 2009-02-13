/* mkdepcoh.c, based on mkdep from the NET/NOS suite, with file lookup from 
filelu.c.  Quickly hacked for Coherent because the Unix script didn't
work.  Note that there is also a mkdep.c for MS-DOS and it is different 
from this one.  It is found in netmsdos.lzh.  With mkdepcoh you have to
edit out *.h references that were #ifdefed out in the source file. 
If you are using Unix instead of Coherent, use mkdep, the shell script. 
K5JB */

#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>		/* K5JB */
#include <dirent.h>
#include <string.h>

#define COH	/* which uses direct, Unix uses dirent */

char include[] = "#include";

#define TRUE		1
#define FALSE		0


static int
Star(s, p)
    register char	*s;
    register char	*p;
{
    while (wildmat(s, p) == FALSE)
	if (*++s == '\0')
	    return(FALSE);
    return(TRUE);
}


int
wildmat(s, p)
    register char	*s;
    register char	*p;
{
    register int 	 last;
    register int 	 matched;
    register int 	 reverse;

    for ( ; *p; s++, p++)
	switch (*p) {
	    case '\\':
		/* Literal match with following character; fall through. */
		p++;
	    default:
		if (*s != *p)
		    return(FALSE);
		continue;
	    case '?':
		/* Match anything. */
		if (*s == '\0')
		    return(FALSE);
		continue;
	    case '*':
		/* Trailing star matches everything. */
		return(*++p ? Star(s, p) : TRUE);
	    case '[':
		/* [^....] means inverse character class. */
		if (reverse = p[1] == '^')
		    p++;
		for (last = 0400, matched = FALSE; *++p && *p != ']'; last = *p)
		    /* This next line requires a good C compiler. */
		    if (*p == '-' ? *s <= *++p && *s >= last : *s == *p)
			matched = TRUE;
		if (matched == reverse)
		    return(FALSE);
		continue;
	}

    /* For "tar" use, matches that end at a slash also work. --hoptoad!gnu */
    return(*s == '\0' || *s == '/');
}

/* wildcard filename lookup */
filedir(name, times, ret_str)
char	*name;
int	times;
char	*ret_str;
{
	static char	dname[128], fname[128];
	static DIR *dirp = NULL;
#ifdef COH
	struct direct *dp;
#else
	struct dirent *dp;
#endif
	struct stat sbuf;
	char	*cp, temp[128];

	/*
	 * Make sure that the NULL is there in case we don't find anything
	 */
	ret_str[0] = '\0';

	if (times == 0) {
		/* default a null name to *.* */
		if (name == NULL || *name == '\0')
			name = "*.*";
		/* split path into directory and filename */
		if ((cp = strrchr(name, '/')) == NULL) {
			strcpy(dname, ".");
			strcpy(fname, name);
		} else {
			strcpy(dname, name);
			dname[cp - name] = '\0';
			strcpy(fname, cp + 1);
			/* root directory */
			if (dname[0] == '\0')
				strcpy(dname, "/");
			/* trailing '/' */
			if (fname[0] == '\0')
				strcpy(fname, "*.*");
		}
		/* close directory left over from another call */
		if (dirp != NULL)
			closedir(dirp);
		/* open directory */
		if ((dirp = opendir(dname)) == NULL) {
			printf("Could not open DIR (%s)\n", dname);
			return;
		}
	} else {
		/* for people who don't check return values */
		if (dirp == NULL)
			return;
	}

	/* scan directory */
	while ((dp = readdir(dirp)) != NULL) {
		/* test for name match */
		if (wildmat(dp->d_name, fname)) {
			/* test for regular file */
			sprintf(temp, "%s/%s", dname, dp->d_name);
			if (stat(temp, &sbuf) < 0)
				continue;
			if ((sbuf.st_mode & S_IFMT) != S_IFREG)
				continue;
			strcpy(ret_str, dp->d_name);
			break;
		}
	}

	/* close directory if we hit the end */
	if (dp == NULL) {
		closedir(dirp);
		dirp = NULL;
	}
}


main(argc, argv)
int argc;
char *argv[];
{
char buf[512];
static int times = 0;
	FILE *fp;
	char buf1[512],*cp,*cp1;


	if(argc == 1){
		fprintf(stderr,"No argument; I'll make dependency for *.c\n");
		argv[1] = "*.c";   /* or whatever we want for default */
	}
	for(;;){
		filedir(argv[1],times++,buf);  /* see dirlu for a different way of */
												 /* doing this */
		if(buf[0] == '\0')
			break;
		strcpy(buf1,buf);
		if((cp = strchr(buf1,'.')) == NULL)
			continue;
		*cp = '\0';
		fprintf(stdout,"%s.o: %s",buf1,buf);
		if((fp = fopen(buf,"r")) == NULL){
			fprintf(stderr,"Trouble opening %s\n",buf);
			exit(1);
		}
		while(fgets(buf,512,fp) != NULL){
			if(strncmp(buf,include,sizeof(include)-1) != 0)
				continue;
			if((cp = strchr(buf,'\"')) == NULL)
				continue;
			cp++;
			if((cp1 = strchr(cp,'\"')) == NULL)
				continue;
			putchar(' ');
			while(cp != cp1)
				putchar(*cp++);
		}
		putchar('\n');
		fclose(fp);
	}
	return 0;
}
