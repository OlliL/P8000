/*
 * return offset in file.
 */

long	lseek();

long tell(f)
{
	return(lseek(f, ((long) 0), 1));
}
