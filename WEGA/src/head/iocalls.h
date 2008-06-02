/* iocalls.h */


/*	iocalls.h - header for runtime	 */
/*	environment modules		 */

/* file should be moved to /usr/include  */
/* before compiling runtime env. modules */



						/* open file flag values */
# define	F_READ  001			/* open for reading      */
# define	F_WRITE 002			/* open for writing      */


 						/* I/O devices           */
						/* in Prom monitor	 */
# define	TTYRD	0x0FDC			/* read line from tty    */
# define 	TTYWR 	0x0FC8			/* write char to tty     */
# define	PDPRD	0x0D9C			/* read char from pdp    */
# define	PDPWR	0x078E			/* write a char to pdp   */
# define	STOPS	8			/* separation of tabs    */
# define	NULL	-1			/* null device           */



						/* codes for calls to 11 */
# define	READ	"0"			/* note they are strings */
# define	WRITE	"1"			/* not chars             */
# define	OPEN	"2"			/* this is more general  */
# define	CREAT	"3" 			/* and you can have more */
# define	CLOSE	"4"			/* than 10 types of call */
# define	LSEEK	"5"




# define	BUFFSIZ	80			/* size of ttyline buffer*/
# define	MAXOPEN 11			/* max # of open files   */
# define	MAXARGS  6			/* max # of args to call */





struct device {					/* template for devices  */

  int   indev  ;				/* input function        */
  int   outdev ;				/* output function       */
  int	flags  ;				/* status bits           */

} ;


