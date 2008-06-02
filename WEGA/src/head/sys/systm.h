/* systm.h */

/*
 * Random set of variables used by more than one routine.
 */

char		buffer1[][512];
char		buffer2[][512];
char		buffer3[][512];
char		buffer4[][512];
char		buffer5[][512];
char		canonb[];		/* buffer for erase and kill (#@)     */

struct inode 	*rootdir;		/* pointer to inode of root directory */
struct proc 	*runq;			/* head of list of running processes  */
int		lbolt;			/* time of day in 60th not in time    */
time_t		lifetime;		/* clock ticks since boot 	      */
time_t		time;			/* time in sec from 1970 	      */

/*
 * Nblkdev is the number of entries (rows) in the block switch. It is set in 
 * binit/bio.c by making a pass over the switch. Used in bounds checking on 
 * major device numbers.
 */
int		nblkdev;

/*
 * Number of character switch entries.  Set by cinit/tty.c
 */
int		nchrdev;

int		mpid;			/* generic for unique process id's    */
char		runin;			/* scheduling flag 		      */
char		runout;			/* scheduling flag 		      */
char		runrun;			/* scheduling flag 		      */
char		curpri;			/* more scheduling 		      */
int		maxmem;			/* actual max memory per process      */
int		umemory;		/* user memory available (clicks)     */
daddr_t		swplo;			/* block number of swap space 	      */
int		nswap;			/* size of swap space 		      */
int		updlock;		/* lock for sync 		      */
daddr_t		rablock;		/* block to be read ahead 	      */
char		msgbuf[MSGBUFS];	/* saved "printf" characters 	      */
dev_t		rootdev;		/* device of the root 		      */
dev_t		swapdev;		/* swapping device 		      */
dev_t		pipedev;		/* pipe device 			      */
long		serial;			/* machine's serial number (not imp)  */
int		numterm;		/* max number of allowable terminals  */
int		manboot;		/* manual/auto boot flag 	      */
int		blkacty;		/* III: active block devices	      */
int		cputype;		/* III: 0: U8001,                     */
extern char	icode[];		/* user init code 		      */
extern int	szicode;		/* its size 			      */
extern int	bfoff;			/* offset to mode flag (single/multi) */
struct sysinfo	sysinfo;		/* III:what kernel is doing 	      */
struct syswait 	syswait;		/* III:what system is waiting for     */

dev_t 		getmdev();
daddr_t		bmap();
struct inode 	*ialloc();
struct inode 	*iget();
struct inode 	*owner();
struct inode 	*maknode();
struct inode 	*namei();
struct buf 	*alloc();
struct buf 	*getblk();
struct buf 	*geteblk();
struct buf 	*bread();
struct buf 	*breada();
struct filsys 	*getfs();
struct file 	*getf();
struct file 	*falloc();
paddr_t		physaddr();
int		uchar();

/*
 * Instrumentation
 */
int	dk_busy;
long	dk_time[32];
long	dk_numb[3];
long	dk_wds[3];
long	tk_nin;
long	tk_nout;

/*
 * Structure of the system-entry table
 */
extern struct sysent 
{
	char	sy_nreg;		/* total number of arguments          */
	int	(*sy_call)();		/* system call handler 		      */
} 		sysent[];

/* 
 * values for "last interrupt" flag  
 */
# define CLOCKINT	1
# define STEPINT	2
# define PROFINT	3
# define UNEXINT	4
# define SIOXINT	5
# define SIOEINT	6
# define SIORINT	7
# define SIOSINT	8
# define LPRINT		9
# define FDINT		10
# define MDINT		11
# define UDINT		12
# define KPINT		13	/* Koppel-Port */
# define MTINT		14
# define USR1INT	15
# define USR2INT	16
# define USR3INT	17
# define USR4INT	18
# define USR5INT	19
# define USR6INT	20
