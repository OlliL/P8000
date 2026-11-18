/* internal declarations of C-ISAM (written from the objects) */

struct buf {
	int	bf_flags;		/* BFMODF, BFVALID, BFBUSY */
	struct buf	*bf_forw;
	struct buf	*bf_back;
	long	bf_fileid;
	int	bf_fd;
	long	bf_blkno;
	char	*bf_buf;
	int	bf_used;
	char	bf_type;
	char	bf_level;
};
#define	BFMODF	01
#define	BFVALID	02
#define	BFBUSY	04

struct tree {
	long	t_blk;			/* node of this level */
	int	t_off;			/* offset of the current item */
	int	t_next;			/* offset of the item behind it */
	int	t_used;			/* bytes used in the node */
};

struct item {
	int	it_flags;		/* 1: duplicate of the item before, 2: the next item is a duplicate */
	int	it_totlen;		/* length of the packed item */
	int	it_keylen;		/* length of the stored key part */
	unsigned	it_dupnum;
	long	it_ptr;
	int	it_leadcnt;
	int	it_trailcnt;
	char	it_key[120];
};

struct file {
	int	f_flags;
	int	f_refcnt;
	long	f_fileid;
	long	f_chgcnt;		/* change count, as in the dictionary node */
	struct buf	*f_dictbp;
	int	f_datfd;
	int	f_idxfd;
	int	f_audfd;
	int	f_recsize;
	int	f_nodsiz;
};

struct open {
	int	o_flags;
	int	o_pid;
	int	o_uid;
	int	o_mode;
	struct file	*o_filep;
	struct keydesc	o_key;
	struct item	o_item;
};

extern int	errno;
extern struct file	*filep;
extern struct open	*openp;
extern char	*dictp;
extern int	iserrno;
extern int	iserrio;
extern struct buf	bufs[];
extern char	buffers[];
extern struct buf	avbufs;
extern int	nbufs;

long	lseek();

#define	DI_AUDIT	49	/* in the dictionary node: the audit trail node, 0200 of the first byte: running */

long	ldlong(), allocrec(), time();
struct buf	*bfread(), *bfgetclr();

extern struct tree	trees[];
extern int	treelev, treeleaf;
extern char	tempbuf[];
struct buf	*bfgetblk();
extern struct item	treeitem;
extern int	treeflags;
extern struct open	opens[];
extern struct file	files[];
extern int	nopens, nfiles;
extern char	temprecord[];
long	longmult(), getfileid();
extern int	treeisfd;
extern char	isstat1, isstat2;
struct file	*allocfile();
extern struct keydesc	tempkey;
extern struct item	tempitem;
