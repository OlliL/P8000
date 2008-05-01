/******************************************************************************
*******************************************************************************

       W E G A - Quelle      (C) ZFT/KEAW Abt. Basissoftware - 1986

       KERN 3.2  Modul:	main.c

       Bearbeiter:	J. Zabel / P.Hoge
       Datum:		10.5.88
       Version:		1.2

*******************************************************************************
******************************************************************************/


#include "sys/param.h"
#include "sys/state.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/filsys.h"
#include "sys/file.h"
#include "sys/mount.h"
#include "sys/map.h"
#include "sys/proc.h"
#include "sys/inode.h"
#include "sys/mmu.h"
#include "sys/buf.h"
#include "sys/s.out.h"
#include "sys/tty.h"
#include "sys/ioctl.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
#include "sys/user.h"
#include "sys/utsname.h"


/*--------------------------------------- Vereinbarungen --------------------*/

extern int	Nbuf1, Nbuf2, Nbuf3, Nbuf4, Nbuf5, Nhbuf;
extern int	Nclist;
extern int	Maxmem;
extern char	utsysname[], utsnodename[], utsversion[];
extern int	rootdev;

extern char	version[], copyrit[], utsrelease[];

extern int	kclicks;
extern int	mmut;

struct chead cfreelist;

extern struct vd_size md_sizes[];
 
/* Konstanten */
#define SEGM_3C		0x3C
#define SEGM_3D		0x3D
#define SEGM_3F		0x3F000000


/*----------------------------------------- Funktionen ----------------------*/


/*---------------------------------------------------------------------------*/
/*
 * main
 */
/*---------------------------------------------------------------------------*/

main ()
{
static int	RST_FLAG	=0;

	register unsigned	i;
	register unsigned	j;
	register int		k;
	register long		pa;
	char 			bt;
	struct   segd		sdr;
	
	if (RST_FLAG++ != 0)
		panic("restart");

	printf ("--------------------------------------------------\
-----------------------------\n");

	printf ("%s%s", &version[4], &copyrit[0]);

	k=0;
	do {
		utsname.sysname[k] = utsysname[k];
		utsname.nodename[k] = utsnodename[k];
		utsname.version[k] = utsversion[k];
		utsname.release[k] = utsrelease[k];
		k++;
	} while (k<UTS_LENGTH);
	
	printf ("\nSystem: %s  \tNode: %s  \tRelease: %s  \tVersion: %s\n\n",
		&utsname.sysname[0], &utsname.nodename[0],
		&utsname.release[0], &utsname.version[0]);
	
	printf ("number of users         = %d \n", numterm);
	printf ("size of user struct     = (%d/0x%x) bytes\n", 
						sizeof (u), sizeof (u));
	printf ("address of user struct  = 0x%X\n", &u);
	printf ("kernel memory size      = (%D/0x%X) bytes\n", 
					   ctob((long)kclicks),
					   ctob((long)kclicks));
	invsdrs ();

	sdr.sg_limit = 0xFF;
	sdr.sg_attr  = 0x02;
	loadsd (mmut, SEGM_3C, &sdr);
	loadsd (mmut, SEGM_3D, &sdr);	

	for (i=kclicks,j=0; ; i++){
		pa = ctob ((long)i);
		bt = 0;
		spbyte (pa, 0x19);
		fpbyte (pa, &bt);
		if (bt != 0x19) break;
		clearseg (i);
		j++;
		mfree (coremap, 1, i);
	}

	printf ("user memory size        = (%D/0x%X) bytes\n",
					 ctob ((long)j), ctob ((long)j));
/*
 * Ausgabe der Filesystemgenerierung
 */
printf ("file system /usr        = offset %D, %D blocks\n",
md_sizes[0].vd_blkoff, md_sizes[0].vd_nblocks);
printf ("swap space              = offset %D, %D blocks\n",
md_sizes[1].vd_blkoff, md_sizes[1].vd_nblocks);
printf ("file system /           = offset %D, %D blocks\n",
md_sizes[2].vd_blkoff, md_sizes[2].vd_nblocks);
printf ("file system /tmp        = offset %D, %D blocks\n",
md_sizes[3].vd_blkoff, md_sizes[3].vd_nblocks);
printf ("file system /z          = offset %D, %D blocks\n",
md_sizes[4].vd_blkoff, md_sizes[4].vd_nblocks);
	printf ("--------------------------------------------------\
-----------------------------\n\n");

	maxmem = j;
	umemory = j;
	if (Maxmem < maxmem)
		maxmem = Maxmem;

	mfree (swapmap, nswap, 1);
	swplo--;

	proc[0].p_addr = kclicks - 10;
	proc[0].p_size = 10;
	proc[0].p_stat = SRUN;
	proc[0].p_flag |= SLOAD | SSYS;
	proc[0].p_nice = NZERO;

	u.u_procp = &proc[0];
	u.u_cmask = CMASK;
	u.u_segmented =0;

	clkstart();
	ihinit();
	cinit();
	binit();
	iinit();

	(rootdir = iget (rootdev, (ino_t)ROOTINO))->i_flag &= ~ILOCK;
	(u.u_cdir = iget (rootdev, (ino_t)ROOTINO))->i_flag &= ~ILOCK;

	u.u_rdir = NULL;
	u.u_stakseg = 0x007f;

	if (newproc() != 0) {
		expand (USIZE + btoc (szicode));
		estabur (0, btoc (szicode), 0, 1);
		icode[bfoff] = manboot ? 's' : 'm';
		copyout (icode, SEGM_3F, szicode);
		return;
	}
	sched ();
}


/*---------------------------------------------------------------------------*/
/*
 * iinit
 * Eroeffnen und mounten des root-file-Systems
 */
/*---------------------------------------------------------------------------*/

iinit ()
{
	register struct buf	*cp, *bp;
	register struct filsys 	*fp;
	extern long gettime();

	(*bdevsw[major(rootdev)].d_open)(rootdev,1);
	bp = bread(rootdev, SUPERB);

	cp = geteblk();
	if (u.u_error != 0)
		panic ("iinit");

	bcopy (bp->b_un.b_addr, cp->b_un.b_addr, sizeof(struct filsys));
	brelse(bp);

	mount[0].m_bufp = cp;
	mount[0].m_dev = rootdev;
	mount[0].m_flags = MINUSE;

	fp = cp->b_un.b_filsys;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
	if ((time = gettime()) == 0)
		time = fp->s_time;
}



/*---------------------------------------------------------------------------*/
/*
 * binit
 * Initialisierung des Device-Puffer-Bereichs
 */
/*---------------------------------------------------------------------------*/

binit ()
{
	register struct buf *dp;
	register int i;
	register struct bdevsw *bdp;

	bfreelist.av_back = &bfreelist;
	bfreelist.av_forw = &bfreelist;
	bfreelist.b_back  = &bfreelist;
	bfreelist.b_forw  = &bfreelist;

	binit1 (buf1, buffer1, Nbuf1);
	binit1 (buf2, buffer2, Nbuf2);
	binit1 (buf3, buffer3, Nbuf3);
	binit1 (buf4, buffer4, Nbuf4);
	binit1 (buf5, buffer5, Nbuf5);

	nblkdev = 0;

	for (bdp = bdevsw; bdp->d_open != 0; bdp++)
		nblkdev++;

	for (i = 0; i < Nhbuf; ++i) 
		hbuf[i].b_forw = hbuf[i].b_back = (struct buf *)&hbuf[i];
}

binit1(buf, buffer, nbuf)
struct buf *buf;
register char *buffer;
register nbuf;
{
	register i;
	register struct buf *bp;

	for (i = 0; i < nbuf; ++i) {
		bp = &buf[i];
		bp->b_dev = NODEV;
		bp->b_paddr = physaddr(bp->b_un.b_addr = &buffer[512*i]);
		bp->b_back = &bfreelist;
		bp->b_forw = bfreelist.b_forw;
		bfreelist.b_forw->b_back = bp;
		bfreelist.b_forw = bp;
		bp->b_flags = B_BUSY;
		brelse (bp);
	}
}

/*---------------------------------------------------------------------------*/
/*
 * cinit
 */
/*---------------------------------------------------------------------------*/

cinit ()
{
	register int		ccp;
	register struct cblock	*cp;
	register struct cdevsw	*cdp;

	for (ccp=0,cp=cfree; ccp<Nclist; ccp++,cp++){
		cp->c_next = cfreelist.c_next;
		cfreelist.c_next = cp;
	}

	cfreelist.c_size = CLSIZE;

	nchrdev = 0;
	for (cdp=cdevsw; cdp->d_open!=0; cdp++) nchrdev++;
}
