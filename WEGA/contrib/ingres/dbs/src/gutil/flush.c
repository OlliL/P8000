# include	<stdio.h>

/*
**  FLUSH -- flush standard output and error
*/
flush()
{
	fflush(stdout);
	fflush(stderr);
}
