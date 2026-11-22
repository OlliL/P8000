/*
 *	env [ - ] [ name=value ]... [command arg...]
 *	set environment, then execute command (or print environment)
 *	- says start fresh, otherwise merge with inherited environment
 */
#include <stdio.h>

static char sccsid[] = "@[$]env.c\t2.1  07/28/82 15:50:51 - 87wega3.2";

#define NENV	100
char	*newenv[NENV];
char	*nullp = NULL;

extern	char **environ;
extern	errno;
extern	char *sys_errlist[];
char	*nvmatch(), *strchr();

main(argc, argv, envp)
register char **argv, **envp;
{

	argc--;
	argv++;
	if (argc && strcmp(*argv, "-") == 0) {
		envp = &nullp;
		argc--;
		argv++;
	}

	for (; *envp != NULL; envp++)
		if (strchr(*envp, '=') != NULL)
			addname(*envp);
	while (*argv != NULL && strchr(*argv, '=') != NULL)
		addname(*argv++);

	if (*argv == NULL) {
		print();
#ifdef SHIPPED
		exit(0);	/* main ends without exit: the status is the value of the last printf, 0 from the old libc (quirk) */
#endif
	}
	else {
		environ = newenv;
		execvp(*argv, argv);
		fprintf(stderr, "%s: %s\n", sys_errlist[errno], *argv);
		exit(1);
	}
}

addname(arg)
register char *arg;
{
	register char **p;

	for (p = newenv; *p != NULL && p < &newenv[NENV-1]; p++)
		if (nvmatch(arg, *p) != NULL) {
			*p = arg;
			return;
		}
	if (p >= &newenv[NENV-1]) {
		fprintf(stderr, "too many values in environment\n");
		print();
		exit(1);
	}
	*p = arg;
	return;
}

print()
{
	register char **p = newenv;

	while (*p != NULL)
		printf("%s\n", *p++);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 */

static char *
nvmatch(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);
	return(NULL);
}
