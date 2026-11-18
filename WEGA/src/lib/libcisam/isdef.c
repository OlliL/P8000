#include	<isam.h>
#include	"isint.h"

char	*isversnumber = "C-ISAM version 1.03";
char	*iscopyright = "xxxxxxxxx (C) 1982 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
long	isserial = 1;
int	nopens = 6;
int	nfiles = 6;
int	nbufs = 8;

int	errno;
struct tree	trees[10];
struct item	treeitem;
int	treelev;
int	treeleaf;
int	treeflags;
int	treeisfd;
struct file	files[6];
struct file	*filep;
struct open	opens[6];
struct open	*openp;
char	*dictp;
int	iserrno;
int	iserrio;
char	isstat1;
char	isstat2;
char	temprecord[2052];
struct item	tempitem;
struct keydesc	tempkey;
struct buf	bufs[8];
char	buffers[8 * 512];
struct buf	avbufs;
char	tempbuf[640];
