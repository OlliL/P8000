/* structure that the routines use to allocate space			*/

struct nodbuffer
{
	short		nleft;		/* bytes left			*/
	short		err_num;	/* error code on overflow	*/
	int		(*err_func)();	/* error function on overflow	*/
	char		*xfree;		/* next free byte		*/
	char		buffer[1];	/* beginning of buffer area	*/
};
