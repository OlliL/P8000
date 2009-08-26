/* @[$] abs.c	2.1  09/12/83 11:19:20 - 87wega3.2 */

int
abs(arg)
int	arg;
{
	if(arg < 0)
		arg = -arg;
	return(arg);
}
