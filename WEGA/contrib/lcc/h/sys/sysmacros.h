/*
 * Some macros for units conversion
 */

	/* Core clicks (256 bytes) to PDP-11 segments and vice versa */
# define ctos(x)	(((x)+31)>>5)
# define stoc(x) 	((x)<<5)

	/* Core clicks (256 bytes) to disk blocks */
# define ctod(x)	(((x)+1)>>1)

	/* inumber to disk address */
# define itod(x)	(daddr_t)((((unsigned)(x)+15)>>3))

	/* inumber to disk offset */
# define itoo(x)	(int)(((x)+15)&07)

	/* clicks to bytes */
# define ctob(x)	((x)<<8)

	/* bytes to clicks */
# define btoc(x) 	((unsigned)(((long)((unsigned)x) + 255L)>>8))

	/* long bytes to clicks */
# define lbtoc(x)	((unsigned)(((x)+255L)>>8))

	/* major part of a device */
# define major(x)	(int)(((unsigned)(x)>>8))

	/* minor part of a device */
# define minor(x)	(int)((x)&0377)

	/* make a device number */
# define makedev(x,y)	(dev_t)((x)<<8 | (y))
