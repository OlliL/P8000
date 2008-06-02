/* segio.h */



/*  segiocalls.h - header for runtime	 */
/*	environment modules		 */

/* file should be moved to /usr/include  */
/* before compiling runtime env. modules */



						/* open file flag values */
# define	F_READ  001			/* open for reading      */
# define	F_WRITE 002			/* open for writing      */


 						/* I/O devices           */
						/* in Prom monitor	 */
# define	TTYRD	0x0ad8			/* read line from tty    */
# define 	TTYWR 	0x0b76			/* write char to tty     */
# define	PDPRD	0x169e			/* read char from pdp    */
# define	PDPWR	0x0f3c			/* write a char to pdp   */
# define	STOPS	8			/* separation of tabs    */
# define	NULL	-1			/* null device           */



						/* codes for calls to 11 */
# define	READ	"0"			/* note they are strings */
# define	WRITE	"1"			/* not chars             */
# define	OPEN	"2"			/* this is more general  */
# define	CREAT	"3" 			/* and you can hav
