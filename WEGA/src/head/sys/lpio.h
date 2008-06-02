/* lpio.h */

/*
 *	This is the structure of the arguments to the lpr ioctl program
 *	there is an identical structure in /usr/include/sgtty.h
 */
struct lparms
{
	int	lines;		/* 	number of lines per page	*/
	int	cols;		/*	number of columns per page	*/
	int	indent;		/*	default indentation for         */
				/*      line printer	                */
};
