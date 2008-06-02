/* inode.h */

/*
 * The I node is the focus of all file activity in wega. There is a unique
 * inode allocated for each active file, each current directory, each mounted-on
 * file, text file, and the root. An inode is 'named' by its dev/inumber pair. 
 * (iget/iget.c) Data, from mode on, is read in from permanent inode on volume.
 */

# define NADDR		13
# define NSADDR		(NADDR*sizeof(daddr_t)/sizeof(short))

struct	inode
{
	char		i_flag;
	cnt_t		i_count;	/* reference count 		      */
	dev_t		i_dev;		/* device where inode resides 	      */
	ino_t		i_number;	/* i number, 1to1 with device address */
	ushort		i_mode;
	short		i_nlink;	/* directory entries 		      */
	ushort		i_uid;		/* owner 			      */
	ushort		i_gid;		/* group of owner 		      */
	off_t		i_size;		/* size of file 		      */
	struct 
	{
	    union 
	    {
		daddr_t i_a[NADDR];	/* if normal file/directory 	      */
		short	i_f[NSADDR];	/* if fifio's 			      */
	    } 		i_p;
	    daddr_t	i_l;		/* last log blk read (for read-ahead) */
	}		i_blks;
	struct locklist	*i_locklist;	/* locked region list head	      */
	struct	inode 	*i_link;	/* hash link 			      */
};

extern struct inode 	inode[];	/* The inode table itself 	      */

struct locklist 
{
	struct locklist *ll_link;	/* link to next cell in list	*/
	int		ll_flag;	/* read-only and wanted flags	*/
	struct proc	*ll_proc;	/* proc owning locked region	*/
	off_t		ll_start;	/* beginning of region		*/
	off_t		ll_end;		/* end of region		*/
};
extern struct locklist 	locklist[];	/* lock table itself		*/

/* flags */
# define ILOCK		01		/* inode is locked 		*/
# define IUPD		02		/* file has been modified 	*/
# define IACC		04		/* inode access time to be updated */
# define IMOUNT		010		/* inode is mounted on 		*/
# define IWANT		020		/* some process waiting on lock */
# define ITEXT		040		/* inode is pure text prototype */
# define ICHG		0100		/* inode has been changed 	*/

/* modes */
# define IFMT		0170000		/* type of file 		*/
# define IFDIR		0040000		/* directory 			*/
# define IFCHR		0020000		/* character special 		*/
# define IFBLK		0060000		/* block special 		*/
# define IFREG		0100000		/* regular 			*/
# define IFMPC		0030000		/* multiplexed char special 	*/
# define IFMPB		0070000		/* multiplexed block special 	*/
# define IFIFO		0010000		/* fifo special 		*/
# define ISUID		04000		/* set user id on execution 	*/
# define ISGID		02000		/* set group id on execution 	*/
# define ISVTX		01000		/* save swapped text even after use */
# define IREAD		0400		/* read, write, execute permissions */
# define IWRITE		0200
# define IEXEC		0100

# define i_addr		i_blks.i_p.i_a
# define i_lastr	i_blks.i_l
# define i_rdev		i_blks.i_p.i_a[0]

# define i_faddr	i_blks.i_p.i_a
# define NFADDR		10
# define PIPSIZ		NFADDR*BSIZE
# define i_frptr	i_blks.i_p.i_f[NSADDR-5]
# define i_fwptr	i_blks.i_p.i_f[NSADDR-4]
# define i_frcnt	i_blks.i_p.i_f[NSADDR-3]
# define i_fwcnt	i_blks.i_p.i_f[NSADDR-2]
# define i_fflag	i_blks.i_p.i_f[NSADDR-1]
# define IFIR		01
# define IFIW		02

# define INOHSZ		64		/* must be multiple of two 	*/

# define INOHASH(d,i)	(((d) + (i)) & (INOHSZ - 1))
