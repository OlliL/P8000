/* @[$] tell.c	2.1  09/12/83 11:34:32 - 87wega3.2 */

/*
 * return offset in file.
 */

long	lseek();

long tell(f)
{
	return(lseek(f, ((long) 0), 1));
}
