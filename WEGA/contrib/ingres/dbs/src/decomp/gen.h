# include	"../conf/gen.h"

/*							*/
/*	extra buffers for AM				*/
/*		for decomp/ovqp = 5			*/
/*		for decomp      =  2			*/
/*							*/

# define	XTRA

# ifdef	XTRA
# ifdef		OD_SEPERATE
# define		XTRABUFS	2
# else
# define		XTRABUFS	5
# endif
# endif
