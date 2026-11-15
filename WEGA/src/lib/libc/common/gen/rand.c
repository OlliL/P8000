/* @[$] rand.c	2.1  09/12/83 11:29:35 - 87wega3.2 */

static	long	randx = 1;

srand(x)
unsigned x;
{
	randx = x;
}

rand()
{
	return(((randx = randx * 1103515245L + 12345)>>16) & 0x7fff);
}
