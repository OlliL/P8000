/******************************************************************************
*******************************************************************************

	W E G A - Quelle       (C) ZFT/KEAW Abt. Basissoftware - 1986
	KERN 3.2	Modul: wpar.c

	Date:		9.8.88
	Version:	1.3

*******************************************************************************

	This is the file used by SYSGEN to build a customer configured
	WEGA Kernel.  The structures and variables defined here were
	extracted from various WEGA Kernel source files as noted.

*******************************************************************************
******************************************************************************/

# define   	UTS_LENGTH     9

# include	"../h/sysparm.h"
# include	"../h/mdsize.h"
# include	"../h/ver.h"

#define  O_SWAP	USR
#define  O_ROOT	USR+SWAP
#define  O_TMP	USR+SWAP+ROOT
#define  O_Z	USR+SWAP+ROOT+TMP

#if MD5
#define  O_MD5	O_Z+Z
#else
#define  O_MD5	0
#endif

#define  O_MD6	O_MD5+MD5
#define  O_MD7	O_MD6+MD6
#define  O_MD8	O_MD7+MD7
#define  O_MD9	O_MD8+MD8

#define  O_MD11	MD10
#define  O_MD12	O_MD11+MD11
#define  O_MD13	O_MD12+MD12
#define  O_MD14	O_MD13+MD13
#define  O_MD15	O_MD14+MD14
#define  O_MD16	O_MD15+MD15
#define  O_MD17	O_MD16+MD16
#define  O_MD18	O_MD17+MD17
#define  O_MD19	O_MD18+MD18

/* interrupt status flags for various devices */

int    	md_flag  	= 0,
	fd_flag		= 0,
	ud_flag		= 0,
	mt_flag		= 0,
	cpu2_flag	= 0,
    	user1_flag  	= 0,
    	user2_flag  	= 0,
    	user3_flag  	= 0,
    	user4_flag  	= 0,
    	user5_flag  	= 0,
    	user6_flag  	= 0;

int	last_int_serv 	= 0;	/* type of last interrupt serviced */

int	Canbsiz		= CANBSIZ;
int	Dstflag		= DSTFLAG;
int	Maxmem 		= MAXMEM ;
int	Maxuprc		= MAXUPRC;

int	Nbuf1  		= NBUF1;
int	Nbuf2  		= NBUF2;
int	Nbuf3  		= NBUF3;
int	Nbuf4  		= NBUF4;
int	Nbuf5  		= NBUF5;
int	Nhbuf  		= NHBUF;

int	Nclist 		= NCLIST;
int	Nfile  		= NFILE ;
int	Ninode 		= NINODE;
int	Nmount 		= NMOUNT;
int	Nproc  		= NPROC ;
int	Ntext  		= NTEXT ;
int	Timezone	= TIMEZONE; 
int	Nflock 		= NFLOCK;

struct	inode
	{
	char	coop[82];
	}	inode[NINODE];

struct	mount 
	{		/**  from mount.h  **/
	char	poop[12];
	}	mount[NMOUNT];

char buffer1 [NBUF1][512];
char buffer2 [NBUF2][512];
char buffer3 [NBUF3][512];
char buffer4 [NBUF4][512];
char buffer5 [NBUF5][512];

char canonb  [CANBSIZ];

struct	buf 
	{               /**  from conf.c  **/
	char	loop[38];
	}	buf1[NBUF1],
		buf2[NBUF2],  
		buf3[NBUF3],  
		buf4[NBUF4],  
		buf5[NBUF5];  

struct 	hbuf
	{
	char	soup[10];
	}	hbuf[NHBUF];

struct	file 
	{
	char	boop[10];
	}	file[NFILE];

struct	proc 
	{
	char	doop[36];
	}	proc[NPROC];

struct	text 
	{
	char	hoop[14];
	}	text[NTEXT];

struct	cblock 
	{               /*  from prim.c  **/
	char	dummy[30];
	}	cfree[NCLIST];

struct	vd_size 
	{
	int		vd_unit; 
	long    vd_blkoff;
	long    vd_nblocks;
	};

struct	locklist 
	{
	char	lkdumy[18];
	}	locklist[NFLOCK];

typedef	long		time_t;
typedef	int		dev_t;

# define	makedev(x,y)	(dev_t)((x)<<8 | (y))

struct	map
	{
	short		m_size;
	unsigned short 	m_addr;
	};

int ndrives = 2;

#define NMD 20
int nmd = NMD;

struct vd_size md_sizes[NMD] = {
0,  0,  USR,
0,  O_SWAP,  SWAP,
0,  O_ROOT,  ROOT,
0,  O_TMP,  TMP,
0,  O_Z,  Z,
0,  O_MD5,  MD5,
0,  O_MD6,  MD6,
0,  O_MD7,  MD7,
0,  O_MD8,  MD8,
0,  O_MD9,  MD9,

1,  0,  MD10,
1,  O_MD11,  MD11,
1,  O_MD12,  MD12,
1,  O_MD13,  MD13,
1,  O_MD14,  MD14,
1,  O_MD15,  MD15,
1,  O_MD16,  MD16,
1,  O_MD17,  MD17,
1,  O_MD18,  MD18,
1,  O_MD19,  MD19,
};

int nswap = SWAP-1;

int  linecnt = 1; 

int  rootdev = makedev(0,2);
int  swapdev = makedev(0,1);
int  pipedev = makedev(0,2);

char utsysname[UTS_LENGTH] = SYSNAME;
char utsnodename[UTS_LENGTH] = NODENAME;
char utsversion[UTS_LENGTH] = VERSION;
