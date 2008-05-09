/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988

	KERN 3.2	Modul: slp.c


	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/

char slpwstr[] = "@[$]slp.c		Rev : 4.2	10/02/83 05:55:35";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/state.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/text.h>
#include <sys/map.h>
#include <sys/inode.h>
#include <sys/file.h>
#include <sys/buf.h>
#include <sys/user.h>

#define SQSIZE 0100
struct proc *slpque[SQSIZE+1];

#define HASH(x)	(( (int) x >> 5) & (SQSIZE-1))

extern unsigned Nproc;

sleep(chan, pri)
caddr_t chan;
int pri;
{
	register struct proc *rp;
	register s, h;
	register sig;

	rp = u.u_procp;
	s = dvi();
	if (chan==0)
		panic("zero wchan");
	rp->p_stat = SSLEEP;
	rp->p_wchan = chan;
/*	if (chan==0)
		panic("Sleeping on wchan 0");
	rp->p_pri = pri;
	h = HASH(chan);
*/
	rp->p_pri = pri;
	if (chan == &runout)
		h = 0x40;
	else
		h = HASH(chan);
	rp->p_link = slpque[h];
	slpque[h] = rp;
	if(pri > PZERO) {
		if(rp->p_sig != 0 && (sig=issig()) != 0)
	           if((sig & ~(SIGHUP|SIGILL)) || !(rp->p_flag&STRC)) {
			rp->p_wchan = 0;
			rp->p_stat = SRUN;
			slpque[h] = rp->p_link;
			evi();
			goto psig;
		}
		evi();
		if(runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
		swtch();
		if(rp->p_sig  !=0 && (sig=issig()) != 0)
		   if((sig & ~(SIGHUP|SIGILL)) || !(rp->p_flag & STRC))
			goto psig;
	} else {
		evi();
		swtch();
	}
	rvi(s);
	return;

psig:
	resume(u.u_procp->p_addr, u.u_qsav);
}

wakeup(chan)
register caddr_t chan;
{
	register struct proc *p, *q;
 	register i;
	register int s;
	struct proc *sp;

	s = dvi();
	if (chan == &runout)
		i = 0x40;
	else
		i = HASH(chan);
restrt:
	p = slpque[i];
	q = NULL;
	while(p != NULL) {
		if(p->p_wchan==chan && p->p_stat!=SZOMB) {
			if (q == NULL)
				sp = slpque[i] = p->p_link;
			else
				sp = q->p_link = p->p_link;
			p->p_wchan = 0;
			setrun(p);
			p = sp;
			continue;
		}
		q = p;
		p = p->p_link;
	}
	rvi(s);
}

setrq(p)
 struct proc *p;
{
	register struct proc *q;
	register s;

	s=dvi();
	for(q=runq; q!=NULL; q=q->p_link)
		if(q == p) {
			prstate(u.u_state);
			panic("proc on q\n");
			goto out;
		}
	p->p_link = runq;
	runq = p;
out:
	rvi(s);
}

setrun(p)
register struct proc *p;
{
	register caddr_t w;

	if (p->p_stat==0 || p->p_stat==SZOMB) {
		prstate(u.u_state);
		panic("Running a dead proc");
	}
	if (w = p->p_wchan) {
		wakeup(w);
		return;
	}
	p->p_stat = SRUN;
	setrq(p);
	if(p->p_pri < curpri)
		runrun++;
	if(runout != 0 && (p->p_flag&SLOAD) == 0) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
}

setpri(pp)
register struct proc *pp;
{
	register p;

	p = (pp->p_cpu & 0377)/16;
	p += PUSER + pp->p_nice - NZERO;
	if(p > 127)
		p = 127;
	if(p < curpri)
		runrun++;
	pp->p_pri = p;
	return(p);
}

sched()
{
	register struct proc *rp, *p;
	register outage, inage;
	register maxsize;


loop:
	dvi();
	outage = -20000;
	for (rp = &proc[0]; rp < &proc[Nproc]; rp++)
	if (rp->p_stat==SRUN && (rp->p_flag&SLOAD)==0 &&
	    rp->p_time - (rp->p_nice-NZERO)*8 > outage) {
		p = rp;
		outage = rp->p_time - (rp->p_nice-NZERO)*8;
	}
	if (outage == -20000) {
		runout++;
		sleep((caddr_t)&runout, PSWP);
		goto loop;
	}
	evi();

	if (swapin(p))
		goto loop;


	dvi();
	p = NULL;
	maxsize = -1;
	inage = -1;
	for (rp = &proc[0]; rp < &proc[Nproc]; rp++) {
		if (rp->p_stat==SZOMB
		 || (rp->p_flag&(SSYS|SLOCK|SULOCK|SLOAD))!=SLOAD)
			continue;
		if (rp->p_textp && rp->p_textp->x_flag&XLOCK)
			continue;
		if (rp->p_stat==SSLEEP&&rp->p_pri>=PZERO || rp->p_stat==SSTOP) {
			if (maxsize < rp->p_size) {
				p = rp;
				maxsize = rp->p_size;
			}
		} else if (maxsize<0 && (rp->p_stat==SRUN||rp->p_stat==SSLEEP)) {
			if (rp->p_time+rp->p_nice-NZERO > inage) {
				p = rp;
				inage = rp->p_time+rp->p_nice-NZERO;
			}
		}
	}
	evi();
	if (maxsize>=0 || (outage>=3 && inage>=2)) {
		p->p_flag &= ~SLOAD;
		xswap(p, 1, 0);
		goto loop;
	}
	dvi();
	runin++;
	sleep((caddr_t)&runin, PSWP);
	goto loop;
}

swapin(p)
register struct proc *p;
{
	register struct text *xp;
	register int a;
	register int x;

	if ((a = malloc(coremap, p->p_size)) == NULL)
		return(0);
	if (xp = p->p_textp) {
		xlock(xp);
		if (xp->x_ccount==0) {
			if ((x = malloc(coremap, xp->x_size)) == NULL) {
				xunlock(xp);
				mfree(coremap, p->p_size, a);
				return(0);
			}
			xp->x_caddr = x;
			if ((xp->x_flag&XLOAD)==0)
				swap(swapdev,xp->x_daddr,x,xp->x_size,B_READ);
		}
		xp->x_ccount++;
		xunlock(xp);
	}
	swap(swapdev, p->p_addr, a, p->p_size, B_READ);
	mfree(swapmap, ctod(p->p_size), p->p_addr);
	p->p_addr = a;
	p->p_flag |= SLOAD;
	p->p_time = 0;
	return(1);
}

qswtch()
{

	sysinfo.qswitch++;
	setrq(u.u_procp);
	swtch();
}

swtch()
{
	register n;
	register struct proc *p, *q;
	int undef1, undef2, undef3;
	struct proc  *pp, *pq;

	if (u.u_procp != &proc[0]) {
		if (save(u.u_rsav)) {
			sysinfo.pswitch++;
			setscr();
			if(u.u_segmented) {
				invsdrs();
				segureg();
				return;
			}
			sureg();
			return;
		}
		resume(proc[0].p_addr, u.u_qsav);
	}
	if (save(u.u_qsav)==0 && save(u.u_rsav)) {
		setscr();
		return;
	}
loop:
	dvi();
	runrun = 0;
	pp = NULL;
	q = NULL;
	n = 128;
	for(p=runq; p!=NULL; p=p->p_link) {
		if((p->p_stat==SRUN) && (p->p_flag&SLOAD)) {
			if(p->p_pri <= n) {
				pp = p;
				pq = q;
				n = p->p_pri;
			}
		}
		q = p;
	}
	p = pp;
	if(p == NULL) {
		sysinfo.idle++;
		idle();
		goto loop;
	}
	q = pq;
	if(q == NULL)
		runq = p->p_link;
	else
		q->p_link = p->p_link;
	curpri = n;
	evi();
	n = p->p_flag&SSWAP;
	p->p_flag &= ~SSWAP;
	resume(p->p_addr, n? u.u_ssav: u.u_rsav);
}

newproc()
{
	long undef;
	struct proc *rpp, *rip;
	int n;
	register a1, a2;
	register struct proc *p, *up;

	p = NULL;
retry:
	mpid++;
	if(mpid >= 30000) {
		mpid = 0;
		goto retry;
	}
	for(rpp = &proc[0]; rpp < &proc[Nproc]; rpp++) {
		if(rpp->p_stat == NULL && p==NULL)
			p = rpp;
		if (rpp->p_pid==mpid || rpp->p_pgrp==mpid)
			goto retry;
	}
	if ((rpp = p)==NULL)
		panic("no procs");

	rip = u.u_procp;
	up =rip;
	rpp->p_stat = SRUN;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD;
	rpp->p_uid = rip->p_uid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_textp = rip->p_textp;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_time = 0;
	rpp->p_cpu = 0;


	for(n=0; n<NOFILE; n++)
		if(u.u_ofile[n] != NULL)
			u.u_ofile[n]->f_count++;
	if(up->p_textp != NULL) {
		up->p_textp->x_count++;
		up->p_textp->x_ccount++;
	}
	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;
	rpp = p;
	u.u_procp = rpp;
	rip = up;
	n = rip->p_size;
	a1 = rip->p_addr;
	rpp->p_size = n;
	if (save(u.u_ssav)) {
		setscr();
		if(u.u_segmented) {
			invsdrs();
			segureg();
		} else
			sureg();
		return(1);
	}
	a2 = malloc(coremap, n);
	if(a2 == NULL) {
		rip->p_stat = SIDL;
		rpp->p_addr = a1;
		xswap(rpp, 0, 0);
		rip->p_stat = SRUN;
	} else {
		rpp->p_addr = a2;
		while(n--)
			copyseg(a1++, a2++);
	}
	u.u_procp = rip;
	setrq(rpp);
	rpp->p_flag |= SSWAP;
	return(0);
}

expand(newsize)
{
	register i, n;
	register struct proc *p;
	register a1, a2;
	int k;

	p = u.u_procp;
	n = p->p_size;
	p->p_size = newsize;
	a1 = p->p_addr;
	if(n >= newsize) {
		mfree(coremap, n-newsize, a1+newsize);
		return;
	}
	if (save(u.u_ssav)) {
		setscr();
		if(u.u_segmented) {
			invsdrs();
			segureg();
			return;
		}
		sureg();
		return;
	}
	a2 = malloc(coremap, newsize);
	if(a2 == NULL) {
		xswap(p, 1, n);
		p->p_flag |= SSWAP;
		qswtch();
		/* no return */
	}
	p->p_addr = a2;
	for(i=0; i<n; i++)
		copyseg(a1+i, a2+i);
	mfree(coremap, n, a1);
	resume(a2, u.u_ssav);
}
