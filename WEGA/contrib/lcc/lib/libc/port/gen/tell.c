/*
 * return offset in file.
 */

extern long lseek();

long
tell(f)
int	f;
{
	return(lseek(f, 0L, 1));
}
