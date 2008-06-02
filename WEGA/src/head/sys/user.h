/* user.h */

/*
 * The user structure. One allocated per process. Contains all per process data
 * that doesn't need to be referenced while the process is swapped. The user 
 * block is USIZE*256 bytes long; resides at virtual kernel loc 0xFC00; contains
 * the system stack per user; is cross referenced with the proc structure for 
 * the same process.
 */

# define EXCLOSE	01

struct	user
{
	label_t		u_rsav;		/* save info when exchanging stacks   */
	char		u_segflg;	/* IO flag: 0:user D;1:system;2:user I*/
	char		u_error;	/* return error code 		      */
	char		u_deverr;	/* device-specific error code 	      */
	short		u_uid;		/* effective user id 		      */
	short		u_gid;		/* effective group id 		      */
	short		u_ruid;		/* real user id 		      */
	short		u_rgid;		/* real group id 		      */
	struct proc 	*u_procp;	/* pointer to proc structure 	      */
	int		*u_ap;		/* pointer to arglist 		      */
	union 				/* syscall return values 	      */
	{
	    struct
	    {
		int		r_val1;
		int		r_val2;
	    }			r_val;
	    off_t		r_off;
	    time_t		r_time;
	} 		u_r;
	saddr_t		u_base;		/* base address for byte transfers    */
	unsigned int 	u_count;	/* bytes remaining for IO 	      */
	off_t		u_offset;	/* offset in file for IO 	      */
	short		u_fmode;	/* file mode for IO		      */
	struct inode 	*u_cdir;	/* pointer to inode of current dir    */
	struct inode 	*u_rdir;	/* root dir of current process 	      */
	char		u_dbuf[DIRSIZ];	/* current pathname component 	      */
	saddr_t		u_dirp;		/* pathname pointer 		      */
	struct direct 	u_dent;		/* current directory entry 	      */
	struct inode 	*u_pdir;	/* inode of parent dir of dirp 	      */
	unsigned int 	u_break;	/* user break value 		      */
	struct file 	*u_ofile[NOFILE];/* pointers to file structs 	      */
	char		u_pofile[NOFILE];/* flags of open files 	      */
	int		u_arg[6];	/* arguments to current system call   */
	int  		u_tsize;	/* size of text area 		      */
	int  		u_dsize;	/* size of data area 		      */
	int  		u_ssize;	/* size of stack area 		      */
	label_t		u_qsav;		/* label variable for quits and ints  */
	label_t		u_ssav;		/* label variable for swapping 	      */
	long		u_signal[NSIG];	/* disposition of signals 	      */
	time_t		u_utime;	/* this process user time 	      */
	time_t		u_stime;	/* this process system time 	      */
	time_t		u_cutime;	/* sum of childs' utimes 	      */
	time_t		u_cstime;	/* sum of childs' stimes 	      */
	time_t		u_syst;		/* temp for system call time 	      */
	struct state 	*u_state;	/* address of user's saved state      */

	struct 				/* profiling 			      */
	{
	    long		pr_base;/* buffer base 			      */
	    unsigned 		pr_size;/* buffer size 			      */
	    unsigned 		pr_off;	/* pc offset 			      */
	    unsigned 		pr_scale;/* pc scaling 			      */
	} 		u_prof[10];
	char    	u_pr_segno[10];	/* segment number 		      */
	char    	u_nprof;       	/* number of segments being profiled  */
	char		u_intflg;	/* catch intr from sys 		      */
	struct tty 	*u_ttyp;	/* controlling tty pointer 	      */
	dev_t		u_ttyd;		/* controlling tty dev 		      */

		/*  structures needed by getxfile when exec'ing 	      */
	char		u_sep;		/* flag for I and D separation 	      */
	int		u_exdsz;	/* size of executable file s.out head */
	struct 
	{
	    struct s_exec	s_exc;	/* s.out header 		      */
	    struct segt		segtable[2];/* seg table holder 	      */
	} 		u_exdata;
	char		u_segmented;	/* is the user segmented? 	      */
	struct	segd	u_nsucode;	/* non-segmented code segment descr   */
	struct  segd    u_segmts[NUSEGS];/* code and data seg descriptors     */
	char		u_segno[NUSEGS];/* segment numbers used 	      */
	char		u_nsegs;	/* # of segments used by seg user     */

	char 	 	u_tseg;		/* segment # and offset of highest    */
	unsigned 	u_toff;		/* code addr in seg, combined case    */
	char		u_comm[DIRSIZ];
	char		u_acflag;
	short		u_fpflag;	/* unused now, will be later 	      */
	time_t		u_start;	/* III 				      */
	time_t		u_ticks;	/* III 				      */
	long		u_mem;		/* III 				      */
	long		u_ior;		/* III 				      */
	long		u_iow;		/* III 				      */
	long		u_iosw;		/* III 				      */
	long		u_ioch;		/* III 				      */
	short		u_cmask;	/* mask for file creation 	      */
	daddr_t		u_limit;	/* Maximum write address   III        */

	struct		fpstate		/* floating-pt state area 	      */
	{
	    int		uf_reg[8][5];	/* fp registers, C is last fastest    */
	    int		uf_creg[32];
	} 		u_fpstate;
	int		u_stakseg;
	int		u_stack[1]; 	/* kernel stack per user
				 	 * extends from u + USIZE*256
					 * backward not to reach here
					 */
};

extern struct user 	u;

/* 
 * u_error codes
 */
# define EPERM		1
# define ENOENT		2
# define ESRCH		3
# define EINTR		4
# define EIO		5
# define ENXIO		6
# define E2BIG		7
# define ENOEXEC	8
# define EBADF		9
# define ECHILD		10
# define EAGAIN		11
# define ENOMEM		12
# define EACCES		13
# define EFAULT		14
# define ENOTBLK	15
# define EBUSY		16
# define EEXIST		17
# define EXDEV		18
# define ENODEV		19
# define ENOTDIR	20
# define EISDIR		21
# define EINVAL		22
# define ENFILE		23
# define EMFILE		24
# define ENOTTY		25
# define ETXTBSY	26
# define EFBIG		27
# define ENOSPC		28
# define ESPIPE		29
# define EROFS		30
# define EMLINK		31
# define EPIPE		32
# define EDOM		33
# define ERANGE		34
# define EDEADLOCK	35
# define ENOSEG		36
# define ENOPROF	37

# define DGEN		0	/* General device-dependent error	*/
# define DNUNIT		1	/* No such unit				*/
# define DIO		2	/* I/O (data transfer) error		*/
# define DBUSY		3	/* Device busy				*/
# define DPROT		4	/* Device protected			*/
# define DNMEDIA	5	/* No media				*/
# define DEMEDIA	6	/* End of media				*/
# define DEDATA		7	/* End of data				*/
