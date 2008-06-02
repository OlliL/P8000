/* ioctl.h */

/*
 * Ioctl include file
 *	These are ALL of the ioctl calls used for ALL devices.
 */


/*
 * tty ioctls
 */

# define IOCTYPE	0xFF00

# define TIOC		('T'<<8)
# define TCGETA		(TIOC|1)	/* get all parameters		*/
# define TCSETA		(TIOC|2)	/* set all parameters		*/
# define TCSETAW	(TIOC|3)	/* wait for output to drain,	*/
					/* set all parameters		*/
# define TCSETAF	(TIOC|4)	/* wait for output to drain,	*/
					/* flush input q, set all params*/
# define TCSBRK		(TIOC|5)	/* wait for output to drain, 	*/
					/* if arg is 0, send a break	*/
# define TCXONC		(TIOC|6)	/* start/stop control, if arg 	*/
					/* 0, suspend output; if 1, re-	*/
					/* start suspended output	*/
# define TCFLSH		(TIOC|7)	/* if arg = 0, flush input q	*/
					/* if arg = 1, flush ouput q	*/
					/* if arg = 2, flush both qs	*/
# define TCDSET		(TIOC|32)

# define LDIOC		('D'<<8)
# define LDOPEN		(LDIOC|0)
# define LDCLOSE	(LDIOC|1)
# define LDCHG		(LDIOC|2)

# define tIOC		('t'<<8)
# define TIOCHPCL	(tIOC|2)
# define TIOCGETP	(tIOC|8)
# define TIOCSETP	(tIOC|9)
# define TIOCSETN	(tIOC|10)
# define TCXCLUSE	(TIOC|13)
# define TCNXCLUSE	(TIOC|14)
# define TIOCFLUSH	(tIOC|16)
# define TIOCSETC	(tIOC|17)
# define TIOCGETC	(tIOC|18)

# define LIOC		('l'<<8)
# define LIOCGETP	(LIOC|1)
# define LIOCSETP	(LIOC|2)
# define LIOCGETS	(LIOC|5)
# define LIOCSETS	(LIOC|6)

# define DIOC		('d'<<8)
# define DIOCGETC	(DIOC|1)
# define DIOCGETB	(DIOC|2)
# define DIOCSETE	(DIOC|3)

/*
 * some new ioctl values for the modem control
 * system call
 */
# define TIOCMCG	(('t'<<8)|19)	/* get tty/modem configuation	*/
# define TIOCMCS	(('t'<<8)|20)	/* set tty/modem configuration	*/


/*
 * ct ioctl commands
 * Only CTIOFF, CTIOFR, CTIOBF, CTIOBR are implemented.
 */
# define CTIOFF		(('c'<<8)|0)	/* file forward			*/
# define CTIOFR		(('c'<<8)|1)	/* file reverse			*/
# define CTIOBF		(('c'<<8)|2)	/* block forward		*/
# define CTIOBR		(('c'<<8)|3)	/* block reverse		*/
# define CTIORWF	(('c'<<8)|4)	/* rewind file			*/
# define CTIOEOF	(('c'<<8)|5)	/* seek to end-of-file		*/
# define CTIORWT	(('c'<<8)|6)	/* rewind tape			*/

# define CTIOFM		(('c'<<8)|10)	/* write filemark		*/
# define CTIOGAP	(('c'<<8)|11)	/* write gap			*/


/*
 *    mag tape ioctl commands
 */ 
# define MTIOFF		(('c'<<8)|0)	/* skip file forward		*/
# define MTIOFR		(('c'<<8)|1)	/* skip file reverse		*/
# define MTIOBF		(('c'<<8)|2)	/* skip block forward		*/
# define MTIOBR		(('c'<<8)|3)	/* skip block reverse		*/
# define MTIOSE		(('c'<<8)|4)	/* security erase		*/
# define MTIOWFM	(('c'<<8)|5)	/* write file mark		*/
# define MTIORW		(('c'<<8)|6)	/* rewind			*/
# define MTIORWUL 	(('c'<<8)|7)	/* rewind and unload		*/
# define MTIOOL		(('c'<<8)|8)	/* on line, ( CDC only)		*/

/*
 * line printer ioctl calls
 */
# define SHOWLPR 	(('l'<<8)|0)	/* get lpr params 		*/
# define SETLPR		(('l'<<8)|1)	/* set lpr params 		*/

/*
 * Floppy ioctl calls
 */
# define SHOWFD		(('f'<<8)|0)	/* get number of blocks		*/
