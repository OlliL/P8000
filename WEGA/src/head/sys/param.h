/* param.h */

/*
 * tunable variables
 */

# define SSIZE		5		/* initial stack size (clicks) 	      */
# define SINCR		5		/* increment of stack (clicks) 	      */
# define NOFILE		20		/* max open files per process 	      */
# define CMAPSIZ	50		/* size of core allocation area       */
# define SMAPSIZ	50		/* size of swap allocation area       */
# define NCALL		20		/* max simultaneous time callouts     */
# define HZ		60		/* clock tics/second 		      */
# define MSGBUFS	128		/* Characters saved from error mesgs  */
# define NCARGS		5120		/* # characters in exec arglist       */
# define NUSEGS		128		/* # segments avail to segmented user */
# define MAXUID		60000		/* Maximum user id  		      */
# define MAXLINK	1000		/* Maximum # of links to a file ???   */

/*
 * priorities: probably should not be altered too much
 */

# define PSWP		0
# define PINOD		10
# define PRIBIO		20
# define PZERO		25
# define NZERO		20
# define PPIPE		26
# define PWAIT		30
# define PSLEP		40
# define PUSER		50

/*
 * signals: dont change
 */

# define NSIG		20

/*
 * No more than 32 signals (1-32) because they are stored in bits in a long.
 */
# define SIGHUP		1	/* hangup 				*/
# define SIGINT		2	/* interrupt (rubout) 			*/
# define SIGQUIT	3	/* quit (FS) 				*/
# define SIGILL		4	/* illegal instruction 			*/
# define SIGTRAP	5	/* trace or breakpoint 			*/
# define SIGIOT		6	/* iot 					*/
# define SIGEMT		7	/* emt 					*/
# define SIGFPE		8	/* floating exception 			*/
# define SIGKILL	9	/* kill, uncatchable termination 	*/
# define SIGBUS		10	/* bus error 				*/
# define SIGSEG		11	/* segmentation violation 		*/
# define SIGSYS		12	/* bad system call 			*/
# define SIGPIPE	13	/* end of pipe 				*/
# define SIGALRM	14	/* alarm clock 				*/
# define SIGTRM		15	/* catchable termination 		*/
# define SIGUSR1	16	/* user defined signal 1 		*/
# define SIGUSR2	17	/* user defined signal 2 		*/
# define SIGCLD		18	/* child death 				*/
# define SIGPWR		19	/* power fail 				*/

/*
 * system call constants
 */

# define ACCESS  	33 	
# define BRK		17	
# define CHDIR		12	
# define CHMOD		15	
# define CHOWN		16	
# define CHROOT		61	
# define CREAT    	8 	
# define EXEC		11	
# define EXECE		59	
# define FCNTL		38
# define FTIME		35 	
# define FSTAT		28	
# define GTTY		32	
# define IOCTL		54	
# define LINK 	 	9 	
# define LKDATA		49	
# define MDMCTL		62	
# define MKNOD		14 	
# define MOUNT		21 	
# define OPENS    	5 	
# define PROFIL		44	
# define PTRACE		26	
# define READS	 	3 	
# define SETPGRP	39
# define SIGNAL		48	
# define SPTRACE	58	
# define STATS		18	
# define STTY		31	
# define SYSACCT 	51	
# define TIMES		43	
# define ULIMIT		64
# define UMOUNT		22 	
# define UNLINK		10	
# define UNLK		50	
# define UTIME		30	
# define UTSSYS		57
# define WRITES   	4	

/*
 * fundamental constants of the implementation -- cannot be changed easily
 */

# define NBPW		sizeof(int)	/* number of bytes in an integer      */
# define CPAS		256		/* clicks per address space 	      */
# define WPC     	128		/* words per clicks 		      */
# define BSIZE		512		/* size of secondary block (bytes)    */
/* BSLOP can be 0 unless you have a TIU/Spider                                */
# define BSLOP		2		/* for bigger buffers 		      */
# define NINDIR		(BSIZE/sizeof(daddr_t))
# define BMASK		0777		/* BSIZE-1 			      */
# define BSHIFT		9		/* LOG2(BSIZE) 			      */
# define NMASK		0177		/* NINDIR-1 			      */
# define NSHIFT		7		/* LOG2(NINDIR) 		      */

# define USIZE		10		/* size of PPDA (user struct)(clicks) */
# define UBASE		0x3E00F600	/* kernel virtual addr of user struct */

# define CMASK		0		/* default file creation mask 	      */
# define NODEV		(dev_t)(-1)
# define ROOTINO	((ino_t)2)	/* i number of all roots 	      */
# define SUPERB		((daddr_t)1)	/* block number of the super block    */
# define DIRSIZ		14		/* max characters per directory       */
# define NICINOD	100		/* number of superblock inodes 	      */
# define NICFREE	50		/* number of superblock free blocks   */
# define INFSIZE	138		/* size of per-proc info for users    */
# define CBSIZE		14		/* number of chars in a clist block   */
# define CROUND		017		/* sizeof(int *) + CBSIZE - 1 	      */
# define INDSCID	0		/* indirect system call id value      */
# define MAXSCID	64		/* max number of system calls 	      */
# define BADSCID	63		/* bad system call id value 	      */
# define NSUSER		0x3F00		/* SEG non-seg user segment nbr       */

/*
 * System Configuration Register constants
 */

# define SCR		0xFFC1		/* System Configuration Register      */
# define SUSRON		0x0F		/* Segmented User bit on 	      */
# define SUSROFF	0x0B		/* Segmented User bit off 	      */


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

typedef	struct 
{ 
	int r[1]; 
} 			*physadr;
typedef	unsigned short	ushort;
typedef	char		cnt_t;
typedef	unsigned long	daddr_t;
typedef char 		*caddr_t;	/* pointer to kernel things */
typedef unsigned long 	uaddr_t;	/* pointer to user things */
typedef	union	
{
    caddr_t		l;
    struct
    {
	unsigned	left;
	unsigned	right;
    }			half;
}			saddr_t;	/* segmented address with parts */
typedef	unsigned long	paddr_t;	/* physical address */
typedef	unsigned int	ino_t;
typedef	long		time_t;
typedef	int		label_t[10];	/* regs 8-15, pcseg and pcoff */
typedef	int		dev_t;
typedef	long		off_t;

# define lobyte(X)	(((unsigned char *)&X)[sizeof(X) - \
			(sizeof(unsigned char)+1)])
# define hibyte(X)	(((unsigned char *)&X)[0])
# define loword(X)	(((unsigned short *)&X)[sizeof(X) - \
			(sizeof(unsigned short)+1)])
# define hiword(X)	(((unsigned short *)&X)[0])
