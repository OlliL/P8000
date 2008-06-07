/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988

	KERN 3.2	Modul: sys1.c


	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/

char sys1wstr[] = "@[$]sys1.c		Rev : 4.1	09/28/83 00:28:52";

#include <sys/param.h>
#include <sys/sysparm.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/state.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/map.h>
#include <sys/tty.h>
#include <sys/acct.h>

extern Nproc, Maxmem, Maxuprc;

/*
 * exec system call, with and without environments.
 */

struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

exec()
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

#define	NCABLK	(NCARGS+BSIZE-1)/BSIZE
exece()
{
	register int nc, nc1;
	register char *cp;
	register struct buf *bp;
	register struct execa *uap;
	int ne;
	register int bno;
	unsigned ap;
	char b;
	saddr_t c, stkv;
	struct inode *ip;
	extern struct inode *gethead();

	if ((ip = namei(uchar, 0)) == NULL)
		return;
	bno = 0;
	bp = 0;
	if (access(ip, IEXEC))
		goto bad1;
	if ((ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0){
bad:		u.u_error = EACCES;
		goto bad1;
	}
	nc1 = 0;
	ne = 0;
	nc = 0;
	uap = (struct execa *)u.u_ap;
	if ((bno = malloc(swapmap, 10)) == 0)
		panic("Out of swap");
	if (((!u.u_segmented) && ((long)uap->argp & 0x0ffffL) != 0L) ||
	    (u.u_segmented && uap->argp))
		for (;;){
			c.l = 0;
			if (((!u.u_segmented) && (long)uap->argp & 0x0ffffL) ||
			    (u.u_segmented && uap->argp)){
				if (u.u_segmented){
					if (fuword((caddr_t)uap->argp, &c.left) ||	/*FIXME: uses different temp regs here */
					    (fuword((long)uap->argp+2, &c.right)))
						goto bad;
					(long)uap->argp += 4;
				} else { if (fuword(uap->argp,&c.right))
						goto bad;
					c.left = nsseg(c.right);
					(long)uap->argp += 2;
				}
			}
			if ((!u.u_segmented && c.right  == NULL && ((long)uap->envp & 0xffffL) != NULL) ||
			    ( u.u_segmented && !c.l && uap->envp )){		/*FIXME: uses different temp regs here */
				uap->argp = NULL;					/*FIXME: uses different temp regs here */
				if (u.u_segmented){
					if ((fuword(uap->envp, &c.left)) ||
					   (fuword((long)uap->envp+2, &c.right)))
						goto bad;
					if (!c.l)
						break;
					(long)uap->envp += 2;
				} else  {
					if (fuword(uap->envp, &c.right))
						goto bad;
					if (c.right == 0L)
						break;
					c.left = nsseg(c.right);
			}
			(long)uap->envp += 2;
			ne++;
			}
			if ((u.u_segmented || c.right != 0L) &&
			    (!u.u_segmented || c.l != 0)){
				nc1++;
				do {
					if (nc >= NCARGS-1)
						u.u_error = E2BIG;
					if (fubyte (c.l++, &b))
						u.u_error = EFAULT;
					if (u.u_error)
						goto bad1;
					if ((nc & 0x1ff) == 0){
						if (bp)
							bawrite(bp);
						bp = getblk(swapdev, (daddr_t)(swplo+bno+(nc>>BSHIFT)));
						cp = bp->b_un.b_addr;
					}
					nc++;
					*cp++ = b;
				} while (b > 0);
			} else
				break;
		}
	if (bp)
		bawrite(bp);
	bp = 0;
	nc = (nc + NBPW -1) & ~(NBPW -1);
	if (u.u_error || getxfile(ip, nc) || u.u_error){
		u.u_segmented = (u.u_state->s_ps & SEGFCW)?1:0;
		setscr();
		goto bad1;
	}
	ap = -nc - NBPW;
	c.left = u.u_stakseg << 8;
	if (u.u_segmented)
		c.right = ap - nc1*2*NBPW - 3*2*NBPW;
	else
		c.right = ap - nc1*NBPW - 3*NBPW;
	u.u_state->s_sp = c.right;
	if (suword(c.l, nc1 - ne))
		goto bad;
	nc = 0;
	while (1){
		if (u.u_segmented){
			c.right += 2;
			if (nc1 == ne){
				if (suword(c.l, 0) || suword(c.l+2, 0))
					goto bad;
				c.right += 4;
			}
			if (--nc1 >= 0){
				if (suword(c.l, u.u_stakseg<<8) || suword(c.l+2, ap))
					goto bad;
				c.right += 2;
			} else
				break;
		} else {
			c.right += 2;
			if (nc1 == ne){
				if (suword(c.l, 0))
					goto bad;
				c.right += 2;
			}
			if (--nc1 >= 0){
				if (suword(c.l, ap))
					goto bad;
			} else
				break;
		}
		while (1){
			if ((nc & 0x1ff) == 0){
				if (bp)
					brelse(bp);
				bp = bread(swapdev, (daddr_t)(swplo+bno+(nc>>BSHIFT)));
				cp = bp->b_un.b_addr;
			}
			stkv.left = u.u_stakseg<<8;
			stkv.right = ap++;
			if (subyte(stkv.l, (b = *cp++)))
				goto bad;
			nc++;
			if (b & 0xff)
				continue;
			else
				break;
		}
	}
	if (suword(c.l, 0))
		goto bad;
	if (u.u_segmented)
		if (suword(c.l+2, 0))
			goto bad;
	stkv.left = u.u_stakseg<<8;
	stkv.right = ap;
	if (suword(stkv.l, 0))
		goto bad;
	setregs();
bad1:	if (bp)
		brelse(bp);
	if (bno)
		mfree(swapmap, 10, bno);
	iput(ip);
}

/*
 * Read in and set up memory for executed file.
 */

getxfile(ip, nargc)
register struct inode *ip;
register unsigned nargc;
{
	register i;
	register magic, udsize;
	unsigned int utsize, ussize, head_size;
	int sepid;
	int ovly;
	unsigned long curdsiz;
	unsigned long code_size, data_size;
	unsigned long bss_size;
	int scn, nseg, cseg;
	int segment;
	int stackno,  offs;
	char segno[NUSEGS];
	int j;
	unsigned int segmts[NUSEGS];
	unsigned int stext;
	unsigned int sdata;
	unsigned int sbss;

	u.u_base.l = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(struct s_head);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi();
	u.u_segflg = 0;
	if (u.u_error)
		return ovly;
	if (u.u_count){
bad0:		u.u_error = ENOEXEC;
		return ovly;
	}
	sepid = 0;
	ovly = 0;
	if (((magic = u.u_exdata.s_exc.s_magic) & 0x100) == 0){
		bss_size = 0;
		data_size = 0;
		code_size = 0;
		cseg = 0;
		nseg = u.u_exdata.s_exc.s_segt/sizeof(struct segt);
		if (u.u_exdata.s_exc.s_flag & SF_7FSTK)
			stackno = STAK;
		else
			stackno = OLDSEGSTAK;
		if (nseg > NUSEGS){
			u.u_error = ENOEXEC;
			return ovly;
		}
		switch (magic){
			case S_MAGIC3:
				sepid++;
			case S_MAGIC1:
				scn = -1;
				for (i=0; i < (nseg+1)/2; i++){
					for(j = 0;; j++){
						if ( j <= 1){
							segment = u.u_exdata.segtable[j].sg_segno;
							stext = u.u_exdata.segtable[j].sg_code;
							sdata = u.u_exdata.segtable[j].sg_data;
							sbss = u.u_exdata.segtable[j].sg_bss;
							if (++cseg > nseg)
								break;
						} else
							break;
						segno[++scn] = segment;
						segmts[scn] = 0;
						if (u.u_exdata.segtable[j].sg_atr & SG_CODE){
							segmts[scn] = stext;
							if (sepid){
								segno[scn] |= 0x80;
								code_size += (unsigned long)stext;
								if (sestabur(lbtoc((long)stext), segno[scn], scn, 0))
									goto bad0;
								else
									continue;
							} else {
								u.u_tseg = segno[scn];
								u.u_toff = lbtoc((long)stext);
								data_size += stext;
								if (sdata == 0 && sbss == 0)
									if (sestabur(lbtoc((long)stext), segno[scn], scn, 1))
										goto bad0;
							}
						}
						if (u.u_exdata.segtable[j].sg_atr & SG_DATA){
							segmts[scn] += sdata;
							curdsiz = (unsigned long)segmts[scn] + (unsigned long)sbss;
							if ((unsigned)curdsiz != curdsiz){
								u.u_error = ENOMEM;
								return ovly;
							}
							data_size += sdata;
							bss_size += sbss;
							if (sestabur(lbtoc((long)(segmts[scn]+sbss)), segno[scn], scn, 1))
								goto bad0;
						} else {
							if (u.u_exdata.segtable[j].sg_atr & SG_BSS){
								bss_size += sbss;
								if (sestabur(lbtoc((long)(sbss + stext)), segno[scn], scn, 1))
									goto bad0;
							}
						}
					}
					u.u_base.l = (caddr_t)u.u_exdata.segtable;
					u.u_count = 32;
					u.u_segflg = 1;
					readi(ip);
					u.u_segflg = 0;
				}
				if (code_size && (ip->i_flag & ITEXT) == 0 && ip->i_count != 1){
					u.u_error = ETXTBSY;
					return ovly;
				}
				ussize = lbtoc((long)nargc) + SSIZE;
				u.u_stakseg = stackno;
				if (sestabur(ussize, stackno, 0, 0))
					goto bad0;
				utsize = lbtoc((long)code_size);
				udsize = lbtoc((long)(data_size + bss_size));
				if ((utsize+USIZE+udsize+ussize) > umemory){
					u.u_error = ENOMEM;
					return ovly;
				}
				u.u_segmented = 1;
				setscr();
				u.u_exdsz = u.u_exdata.s_exc.s_segt + 24;
				u.u_nsegs = nseg;
				for (i = 0; i < nseg; i++)
					u.u_segno[i] = segno[i];
				u.u_sep = (unsigned char)sepid;
				u.u_prof[0].pr_scale = 0;
				u.u_nprof = 0;
				xfree();
				i = udsize + USIZE + ussize;
				expand(i);
				while (--i >= USIZE)
					clearseg(u.u_procp->p_addr + i);
				xalloc(ip, code_size, &segmts[0]);
				if (code_size == 0){
					invsdrs();
					segureg();
				}
				u.u_offset = u.u_exdsz;
				for (scn = 0;scn < nseg; scn++){
					if (u.u_segno[scn] & 0x80){
						u.u_offset += segmts[scn];
						sestabur(u.u_segmts[scn].sg_limit+1, u.u_segno[scn], scn, 1);
					} else {
						u.u_base.left = ctob(u.u_segno[scn] & 0x7f);
						u.u_base.right = 0;
						u.u_count = segmts[scn];
						readi(ip);
					}
				}
				if ((u.u_procp->p_flag & STRC) == 0){
					if ((ip->i_mode & ISUID) && (u.u_uid)){
						u.u_uid = ip->i_uid;
						u.u_procp->p_uid = ip->i_uid;
					}
					if (ip->i_mode & ISGID)
						u.u_gid = ip->i_gid;
				} else
					psignal(u.u_procp, SIGTRAP);
				u.u_dsize = u.u_tsize = utsize;
				u.u_ssize = ussize;
				segureg();
				return ovly;
			case S_MAGIC4:
			default:
				u.u_error = ENOEXEC;
				return ovly;
		}
	} else
		code_size = u.u_exdata.segtable[0].sg_code;
	if ((head_size = u.u_exdata.s_exc.s_segt) == sizeof(struct segt) || (magic == N_MAGIC1))
		data_size = u.u_exdata.segtable[0].sg_data;
	else
		data_size = u.u_exdata.segtable[1].sg_data;
	bss_size = (unsigned)u.u_exdata.s_exc.s_bss;
	u.u_exdsz = sizeof(struct s_head);
	if (head_size == sizeof(struct segt))
		u.u_exdsz -= head_size;
	switch(magic){
		case N_MAGIC1:
			data_size = curdsiz = data_size + code_size;
			u.u_exdata.segtable[0].sg_data = curdsiz;
			if ((unsigned)curdsiz != curdsiz){
				u.u_error = ENOMEM;
				return ovly;
			}
			code_size=0;
			u.u_exdata.segtable[0].sg_code=0;
			break;
		case N_MAGIC3:
			sepid++;
			break;
		case N_MAGIC4:
			ovly++;
			break;
		default:
			u.u_error = ENOEXEC;
			return ovly;
	}
	if ((code_size) && (ip->i_flag & ITEXT) == 0 &&
	    ip->i_count!=(cnt_t)1){
		u.u_error = ETXTBSY;
		return ovly;
	}
	utsize = lbtoc((long)code_size);
	curdsiz = data_size + bss_size;
	if ((unsigned)curdsiz != curdsiz){
		u.u_error = ENOMEM;
		return ovly;
	}
	udsize = lbtoc((long)curdsiz);
	ussize = lbtoc((long)nargc) + SSIZE;
	u.u_segmented = 0;
	setscr();
	if (ovly){
		if (u.u_sep == 0)
			if (ctos(utsize) != ctos(u.u_tsize)){
				u.u_error = ENOMEM;
				return ovly;
			}
		if (nargc){
			u.u_error = ENOMEM;
			return ovly;
		}
		udsize = u.u_dsize;
		ussize = u.u_ssize;
		sepid = u.u_sep;
		xfree();
		xalloc(ip, code_size);
		u.u_state->s_pc = u.u_exdata.s_exc.s_entry & -2L;
	} else {
		u.u_prof[0].pr_scale = 0;
		xfree();
		i = udsize + USIZE + ussize;
		expand(i);
		while (--i >= USIZE)
			clearseg(u.u_procp->p_addr + i);
		xalloc(ip, code_size);
		/* read in data segment */
		estabur((unsigned)0, udsize, (unsigned)0, 1);
		u.u_base.left = USEGW;
		u.u_base.right = 0;
		u.u_offset = (unsigned long)(u.u_exdsz+(unsigned)code_size);
		u.u_count = data_size;
		readi(ip);
		/*
		 * set SUID/SGID protections, if no tracing
		 */
		if ((u.u_procp->p_flag & STRC)==0){
			if (ip->i_mode & ISUID)
				if (u.u_uid){
					u.u_uid = ip->i_uid;
					u.u_procp->p_uid = ip->i_uid;
				}
			if (ip->i_mode & ISGID)
				u.u_gid = ip->i_gid;
		} else
			psignal(u.u_procp, SIGTRAP);
	}
	u.u_tsize = utsize;
	u.u_dsize = udsize;
	u.u_ssize = ussize;
	u.u_sep = (unsigned char)sepid;
	u.u_stakseg = STAK;
	estabur(utsize, udsize, ussize, 1);
	return ovly;
}

/*
 * Clear registers on exec
 */
setregs()
{
	register long *rp;
	register i;
	register struct file *fp;

	for (rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if ((*rp & 1) == 0)
			*rp = 0;
	for (i = 0; i < 15; i++)
		u.u_state->s_reg[i] = 0;
	if (u.u_segmented)
		u.u_state->s_reg[14] = u.u_stakseg<<8;
	for (i = 0; i < 8; i++){
		u.u_fpstate.uf_reg[i][0] = 0x3fff;
		u.u_fpstate.uf_reg[i][1] = 0x8000;
		u.u_fpstate.uf_reg[i][2] = 0;
		u.u_fpstate.uf_reg[i][3] = 0;
		u.u_fpstate.uf_reg[i][4] = 0;
	}
	fsetregs();
	u.u_state->s_pc = u.u_exdata.s_exc.s_entry & -2;
	if (!u.u_segmented)
		u.u_state->s_pcseg = USEGW;
	else
		u.u_state->s_pcseg = u.u_exdata.s_exc.s_entry >> 16;
	for (i=0; i<NOFILE; i++) {
		if (u.u_pofile[i]&EXCLOSE){
			fp = u.u_ofile[i];
			u.u_ofile[i] = NULL;
			closef(fp);
			u.u_pofile[i] &= ~EXCLOSE;
		}
	}
	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_comm, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
rexit()
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0xff) << 8);
}

/*
 * Release resources.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
exit(rv)
{
	register int i;
	register struct proc *p, *q;
	struct file *fp;

	p = u.u_procp;
	p->p_flag &= ~(SULOCK | STRC);
	p->p_clktim = 0;
	for (i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	if ((p->p_pid == p->p_pgrp)
	 && ((u.u_ttyp != NULL)
	 && (u.u_ttyp->t_pgrp == p->p_pgrp))) {
		u.u_ttyp->t_pgrp = 0;
		signal(p->p_pgrp, SIGHUP);
	}
	p->p_pgrp = 0;
	for (i=0; i<NOFILE; i++) {
		fp = u.u_ofile[i];
		u.u_ofile[i] = NULL;
		closef(fp);
	}
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();
	acct();
	mfree(coremap, p->p_size, p->p_addr);
	p->p_stat = SZOMB;
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
	for (q = &proc[1]; q < &proc[Nproc]; q++) {
		if (p->p_pid == q->p_ppid) {
			q->p_ppid = 1;
			if (q->p_stat == SZOMB)
				psignal(&proc[1], SIGCLD);
			if (q->p_stat == SSTOP)
				setrun(q);
		} else  if (p->p_ppid==q->p_pid)
			psignal(q, SIGCLD);
		if (p->p_pid == q->p_pgrp)
			q->p_pgrp = 0;
	}
	swtch();
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
wait()
{
	int foo,foo2;
	register f;
	register struct proc *p;

	f = 0;
loop:
	for (p = &proc[0]; p < &proc[Nproc]; p++)
	if (p->p_ppid == u.u_procp->p_pid) {
		f++;
		if (p->p_stat == SZOMB) {
			freeproc(p, 1);
			return;
		}
		if (p->p_stat == SSTOP) {
			if ((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_r.r_val1 = p->p_pid;
				u.u_r.r_val2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if (f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * Remove zombie children from the process table.
 */
freeproc(p, flag)
register struct proc *p;
{
	if (flag) {
		u.u_r.r_val1 = p->p_pid;
		u.u_r.r_val2 = ((struct xproc *)p)->xp_xstat;
	}
	u.u_cutime += ((struct xproc *)p)->xp_utime;
	u.u_cstime += ((struct xproc *)p)->xp_stime;
	p->p_stat = NULL;
	p->p_pid = 0;
	p->p_ppid = 0;
	p->p_sig = 0L;
	p->p_flag = 0;
	p->p_wchan = 0;
}

/*
 * fork system call.
 */
fork()
{
	int foo,foo2;
	register struct proc *p1, *p2;
	register a;
	register n;

	/*
	 * Make sure there's enough swap space for max
	 * core image, thus reducing chances of running out
	 */
	if (u.u_segmented){
		n = ctod(u.u_tsize + USIZE + u.u_dsize + u.u_ssize);
		if ((a = malloc(swapmap, n)) == 0){
			u.u_error = ENOMEM;
			goto out;
		} else
			mfree(swapmap, n, a);
	} else {
		if ((a = malloc(swapmap, ctod(Maxmem))) == 0) {
			u.u_error = ENOMEM;
			goto out;
		} else
			mfree(swapmap, ctod(Maxmem), a);
	}
	a = 0;
	p2 = NULL;
	for (p1 = &proc[0]; p1 < &proc[Nproc]; p1++){
		if (p1->p_stat==NULL && p2==NULL)
			p2 = p1;
		else {
			if (p1->p_uid==u.u_uid && p1->p_stat!=NULL)
				a++;
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[Nproc-1] || a>Maxuprc))) {
		u.u_error = EAGAIN;
		goto out;
	}
	p1 = u.u_procp;
	if (newproc()) {
		u.u_r.r_val1 = p1->p_pid;
		u.u_start = time;
		u.u_mem = p2->p_size;
		u.u_ior = u.u_iow = u.u_ioch = 0;
		u.u_cstime = 0;
		u.u_stime = 0;
		u.u_cutime = 0;
		u.u_utime = 0;
		u.u_acflag = AFORK;
		return;
	}
	u.u_r.r_val1 = p2->p_pid;

out:
	u.u_state->s_pc += NBPW;
}
