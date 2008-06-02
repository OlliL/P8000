/* lockblk.h */

struct lockblk 
{
	long	lklen;		/* length of region to lock */
	long	lkoff;		/* offset of 1st byte, only w/ flag=2,3,6,7 */
	int 	lkwhnce;	/* whence flag ala lseek, only w/ flag=2,3,6,7*/
};

long lkdata(), unlk();

# define LKRONLY 	0   /*  Request is for a read-only lock               */
# define LKEXCLUSIVE 	1   /*  Request is for an exclusive lock              */
# define LKUSP	    	0   /*  Use current seek pointer                      */
# define LKEXP	    	2   /*  Use explicit offset                           */
# define LKBLOCKING  	0   /*  Block if request cant be immediately honored  */
# define LKNOBLOCK   	4   /*  Do not block, but return an error if request  */
			    /*  cannot immediately be honored                 */
