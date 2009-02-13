/* This file defines certain low level operations and characteristics that
 * are likely to be machine dependent.
 */

#ifdef        LITTLE_ENDIAN
int32 ntohl();
int16 ntohs();
#else        /* Degenerate case for Big Endian machines */
#define        ntohl(x)        (x)
#define ntohs(x)        (x)
#endif
/*
#define        min(x,y)        ((x)<(y)?(x):(y))
#define        max(x,y)        ((x)>(y)?(x):(y))
*/
/* Asynch controller control block */
struct asy {
	int speed;		/* Line speed */
	char tty[32];
};
extern struct asy asy[];
extern unsigned nasy;		/* Actual number of asynch lines */

#define        ASY_MAX        5                /* Five asynch ports allowed on the PC */
extern int _clktick;


