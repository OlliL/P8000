/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN 3.2      Modul: conf.c

	Bearbeiter:	U. Oefler, P. Hoge
	Datum:		29.7.88
	Version:	1.2

*******************************************************************************
******************************************************************************/
 

#include "sys/param.h"
#include "sys/inode.h"
#include "sys/ino.h"
#include "sys/acct.h"
#include "sys/conf.h"
#include "sys/filsys.h"
#include "sys/buf.h"
#include "sys/tty.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"

 
daddr_t swplo = 1;
 
int nodev(), nulldev();
 
int mdopen(), mdstrategy();
int fdopen(), fdclose(), fdstrategy();
int mtopen(), mtclose(), mtstrategy();
int rdopen(), rdclose(), rdstrategy();
int cpu2open(), cpu2close(), cpu2strategy();
int usr1open(), usr1close(), usr1strategy();
int usr2open(), usr2close(), usr2strategy();
int usr3open(), usr3close(), usr3strategy();
int usr4open(), usr4close(), usr4strategy();
int usr5open(), usr5close(), usr5strategy();
int usr6open(), usr6close(), usr6strategy();

struct bdevsw bdevsw[]=
{
	mdopen,		nulldev,	mdstrategy,	/* md */
	fdopen,		fdclose,	fdstrategy,	/* fd */
	nodev,		nodev,		nodev,
	mtopen,		mtclose,	mtstrategy,	/* mt */
	cpu2open,	cpu2close,	cpu2strategy,	/* MP 600 */
	rdopen,		rdclose,	rdstrategy,	/* rd */
	nodev,		nodev,		nodev,
	nodev,		nodev,		nodev,
	nodev,		nodev,		nodev,
	nodev,		nodev,		nodev,
	usr1open,	usr1close,	usr1strategy,	/* usr1	*/
	usr2open,	usr2close,	usr2strategy,	/* usr2 */
	usr3open,	usr3close,	usr3strategy,	/* usr3 */
	usr4open,	usr4close,	usr4strategy,	/* usr4 */
	usr5open,	usr5close,	usr5strategy,	/* usr5 */
	usr6open,	usr6close,	usr6strategy,	/* usr6 */
	0,		0,		0,
};
 
int mdread(), mdwrite();
int fdread(), fdwrite(), fdioctl();
int udopen(), udclose(), udread(), udwrite();
int mtread(), mtwrite(), mtioctl();
int cpu2read(), cpu2write();
int rdread(), rdwrite();
int mmread(), mmwrite();
int syopen(), syread(), sywrite(), sysioctl();
int sioopen(), sioclose(), sioread(), siowrite(), sioioctl();
int lpopen(), lpclose(), lpwrite(), lpioctl();
int u1open(), u1close(), u1read(), u1write(), u1ioctl();
int u2open(), u2close(), u2read(), u2write(), u2ioctl();
int u3open(), u3close(), u3read(), u3write(), u3ioctl();
int u4open(), u4close(), u4read(), u4write(), u4ioctl();
int u5open(), u5close(), u5read(), u5write(), u5ioctl();
int u6open(), u6close(), u6read(), u6write(), u6ioctl();
 
struct cdevsw cdevsw[]=
{
nulldev,  nulldev,   mdread,   mdwrite,   nodev,    nulldev, 0,
fdopen,   fdclose,   fdread,   fdwrite,   fdioctl,  nulldev, 0,
udopen,   udclose,   udread,   udwrite,   nulldev,  nulldev, 0,
mtopen,	  mtclose,   mtread,   mtwrite,   mtioctl,  nulldev, 0,	
cpu2open, cpu2close, cpu2read, cpu2write, nulldev,  nulldev, 0,
rdopen,   rdclose,   rdread,   rdwrite,   nulldev,  nulldev, 0,
nulldev,  nulldev,   mmread,   mmwrite,   nodev,    nulldev, 0,
syopen,   nulldev,   syread,   sywrite,   sysioctl, nulldev, 0,
sioopen,  sioclose,  sioread,  siowrite,  sioioctl, nulldev, 0,
lpopen,   lpclose,   nodev,    lpwrite,   lpioctl,  nulldev, 0,
u1open,   u1close,   u1read,   u1write,   u1ioctl,  nulldev, 0,
u2open,   u2close,   u2read,   u2write,   u2ioctl,  nulldev, 0,
u3open,   u3close,   u3read,   u3write,   u3ioctl,  nulldev, 0,
u4open,   u4close,   u4read,   u4write,   u4ioctl,  nulldev, 0,
u5open,   u5close,   u5read,   u5write,   u5ioctl,  nulldev, 0,
u6open,   u6close,   u6read,   u6write,   u6ioctl,  nulldev, 0,
0,	  0,	     0,	       0,	  0,	    0,	     0,
};
 
int ttopen(), ttclose(), ttread(), ttwrite(), ttin(), ttout();
 
struct linesw linesw[]=
{
ttopen, ttclose, ttread, ttwrite, nulldev, ttin, ttout, nulldev, nulldev,
0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct buf	bfreelist;
struct acct	acctbuf;
struct inode	*acctp;		/* inode of accounting file */
