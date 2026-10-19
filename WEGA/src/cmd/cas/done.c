/*
 * done - the end of the program
 *
 * (no SCCS id in the binary: this is a module of its own after main.c)
 */
#include <stdio.h>
#include <signal.h>
#include "cas.h"

extern int (*oldint)(), (*oldquit)(), (*oldterm)();

sigdone()
{
	done(-1);
}

done(code)
int code;
{
	if (vflag && code != -1)
		printf("Assembly Complete\n");
	if (code == -1)
		fprintf(stderr, "Assembler Abort\n");
	signal(SIGINT, oldint);
	signal(SIGQUIT, oldquit);
	signal(SIGTERM, oldterm);
	exit(code);
}
