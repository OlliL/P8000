/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle

	KERN 3.2	Modul: sig.c
 
 
	Bearbeiter:	O. Lehmann
	Datum:		22.05.08
	Version:	1.0
 
*******************************************************************************
******************************************************************************/

char sigwstr[] = "@[$]sig.c		Rev: 4.2 	09/29/83 09:47:41";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/text.h>
#include <sys/state.h>

/*
 * Priority for tracing
 */
#define	IPCPRI	PZERO

extern int Nproc;
extern int Maxmem;
/*
 * Tracing variables.
 * Used to pass trace command from
 * parent to child being traced.
 * This data base cannot be
 * shared and is locked
 * per user.
 */
struct
{
	int	ip_lock;
	int	ip_req;
	saddr_t	ip_addr;
	int	ip_data;
} ipc;

/*
 * Send the specified signal to
 * all processes with 'pgrp' as
 * process group.
 * Called by tty.c for quits and
 * interrupts.
 */
signal(pgrp, sig)
register pgrp;
{
	register struct proc *p;

	if(pgrp == 0)
		return;
	for(p = &proc[0]; p < &proc[Nproc]; p++)
		if(p->p_pgrp == pgrp)
			psignal(p, sig);
}

/*
 * Send the specified signal to
 * the specified process.
 */
psignal(p, sig)
register struct proc *p;
register sig;
{

	sig--;
	if (sig < 0 || sig >= NSIG)
		return;
	p->p_sig |= 1L<<sig;
	if(p->p_stat == SSLEEP && p->p_pri > PZERO) {
		if(p->p_pri > PUSER)
			p->p_pri = PUSER;
		setrun(p);
	}
}

/*
 * Returns true if the current
 * process has a signal to process.
 * This is asked at least once
 * each time a process enters the
 * system.
 * A signal does not do anything
 * directly to a process; it sets
 * a flag that asks the process to
 * do something to itself.
 */
issig()
{
	register n;
	register struct proc *p, *q;
	int foo1;

	p = u.u_procp;
	while(p->p_sig) {
		n = fsig(p);
		if (n == SIGCLD) {
			if (u.u_signal[SIGCLD-1]&01) {
				for (q = &proc[1];
					q < &proc[Nproc]; q++) {
			   			 if (p->p_pid == q->p_ppid &&
							q->p_stat == SZOMB)
							freeproc(q, 0) ;
				}
			} else
				if ((int)u.u_signal[SIGCLD-1])
					return(n) ;
		} else
			if((u.u_signal[n-1]&1) == 0 || (p->p_flag&STRC))
				return(n);
		p->p_sig &= ~(1L<<(n-1));
	}
	return(0);
}

/*
 * Enter the tracing STOP state.
 * In this state, the parent is
 * informed and the process is able to
 * receive commands from the parent.
 */
stop()
{
	register struct proc *pp, *cp;

loop:
	cp = u.u_procp;
	if(cp->p_ppid != 1)
	for (pp = &proc[0]; pp < &proc[Nproc]; pp++)
		if (pp->p_pid == cp->p_ppid) {
			wakeup((caddr_t)pp);
			cp->p_stat = SSTOP;
			swtch();
			if ((cp->p_flag&STRC)==0 || procxmt())
				return;
			goto loop;
		}
	exit(fsig(u.u_procp));
}

/*
 * Perform the action specified by
 * the current signal.
 * The usual sequence is:
 *	if(issig())
 *		psig();
 */
psig()
{
	register n;
	long p;
	register struct proc *rp;

	rp = u.u_procp;
	if (rp->p_flag&STRC)
		stop();
	n = fsig(rp);
	if (n==0)
		return;
	rp->p_sig &= ~(1L<<(n-1));
	if((p=u.u_signal[n-1]) != 0) {
		u.u_error = 0;
		if(n != SIGILL && n != SIGTRAP)
			u.u_signal[n-1] = 0;
		sendsig(p, n);
		return;
	}
	switch(n) {

	case SIGQUIT:
	case SIGILL:
	case SIGTRAP:
	case SIGIOT:
	case SIGEMT:
	case SIGFPE:
	case SIGBUS:
	case SIGSEG:
	case SIGSYS:
		if(core())
			n += 0200;
	}
	exit(n);
}

/*
 * find the signal in bit-position
 * representation in p_sig.
 */
fsig(p)
struct proc *p;
{
	register i;
	long n;

	n = p->p_sig;
	for(i=1; i<NSIG; i++) {
		if(n & 1L)
			return(i);
		n >>= 1;
	}
	return(0);
}

/*
 * Create a core image on the file "core"
 *
 * It writes USIZE block of the
 * user.h area followed by the entire
 * data+stack segments.
 */
int
core()
{
	register struct inode *ip;
	register i;
	char s;
	extern schar();

	if (u.u_uid != u.u_ruid)
		return(0);
	u.u_error = 0;
	u.u_dirp.l = "core";
	ip = namei(schar, 1);
	if(ip == NULL) {
		if(u.u_error)
			return(0);
		ip = maknode(0666);
		if (ip==NULL)
			return(0);
	}
	if(!access(ip, IWRITE) && (ip->i_mode&IFMT) == IFREG) {
		itrunc(ip);
		u.u_offset = 0;
		u.u_base.l = (caddr_t)&u;
		u.u_count = ctob(USIZE);
		u.u_segflg = 1;
		u.u_limit = (daddr_t)ctod(umemory);
		writei(ip);
		u.u_segflg = 0;
		if(!u.u_segmented) {
			i = u.u_procp->p_size-USIZE;
			estabur(0, i, 0, 1 );
			u.u_base.left = USEGW;
			u.u_base.right = 0;
			u.u_count = ctob(i);
			writei(ip);
		} else {
			for(s=0;s<u.u_nsegs;s++) {
				if((u.u_segno[s]&0x80))
					continue;
				u.u_base.left = ctob(u.u_segno[s] & STAK);
				u.u_base.right = 0;
				u.u_count = ctob((u.u_segmts[s].sg_limit)+1);
				writei(ip);
			}
			u.u_base.left = ctob(u.u_stakseg);
			u.u_base.right = ctob(u.u_segmts[NUSEGS-1].sg_limit);
			u.u_count = ctob(CPAS - u.u_segmts[NUSEGS-1].sg_limit);
			writei(ip);
		}
	} else
		u.u_error = EACCES;
	iput(ip);
	return((char)(u.u_error==0));
}

/*
 * grow the stack to include the SP
 * true return if successful.
 */

grow(sp)
unsigned sp;
{
	register j, si, i;
	register struct proc *p;
	register int a;

	if(sp >= ctob(256-(u.u_ssize-1)))
		return(0);

	j=6;
	while(1) {
		if(!(j = j - 1))
			return(0);
		si=(256-btoc(sp))-u.u_ssize+j;
		if(si<=0)
			continue;
		if(!u.u_segmented){
			if((u.u_ssize + USIZE + si + u.u_dsize) >= 0x100)
				continue;
			if(estabur(u.u_tsize,u.u_dsize,u.u_ssize+si,1))
				continue;

		} else {
			if(u.u_ssize + si > 0x100)
				continue;
			u.u_segmts[NUSEGS-1].sg_limit = 256 - (u.u_ssize+si);
		}
		p = u.u_procp;
		expand(p->p_size+si);
		a=p->p_addr+p->p_size;
		for(i=u.u_ssize; i; i--) {
			a--;
			copyseg(a-si, a);
		}
		for(i=si; i; i--)
			clearseg(--a);
		u.u_ssize += si;
		return(1);
	}
}

/*
 * sys-trace system call.
 */
ptrace()
{
	register struct proc *p;
	register struct a {
		int	req;
		int	pid;
		saddr_t	addr;
		int	data;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (uap->req <= 0) {
		u.u_procp->p_flag |= STRC;
		return;
	}
	for (p=proc; p < &proc[Nproc]; p++) 
		if (p->p_stat==SSTOP
		 && p->p_pid==uap->pid
		 && p->p_ppid==u.u_procp->p_pid)
			goto found;
	u.u_error = ESRCH;
	return;

    found:
	while (ipc.ip_lock)
		sleep((caddr_t)&ipc, IPCPRI);
	ipc.ip_lock = p->p_pid;
	ipc.ip_data = uap->data;
	ipc.ip_addr.l = (caddr_t)((long)uap->addr.l & 0x7F00FFFF); /* FIXME: not compatible */
	ipc.ip_req = uap->req;
	p->p_flag &= ~SWTED;
	setrun(p);
	while (ipc.ip_req > 0)
		sleep((caddr_t)&ipc, IPCPRI);
	u.u_r.r_val1 = ipc.ip_data;
	if (ipc.ip_req < 0)
		u.u_error = EIO;
	ipc.ip_lock = 0;
	wakeup((caddr_t)&ipc);
}

/*
 * Code that the child process
 * executes to implement the command
 * of the parent process in tracing.
 */
procxmt()
{
	register int i;
	register *p;
	register struct text *xp;
	int foo1, foo2;

	if (ipc.ip_lock != u.u_procp->p_pid)
		return(0);
	i = ipc.ip_req;
	ipc.ip_req = 0;
	wakeup((caddr_t)&ipc);
	if(!u.u_segmented)
		ipc.ip_addr.left = nsseg(ipc.ip_addr.right);
	switch (i) {

	/* read user I */
	case 1:
		if(!u.u_segmented) {
			if(fuiword(ipc.ip_addr.right,&ipc.ip_data))
				goto error;
			break;
		}
	/* read user D */
	case 2:
		if(fuword(ipc.ip_addr.l,&ipc.ip_data))
				goto error;
		break;

	/* read u */
	case 3:
		i = (int)ipc.ip_addr.right;
		if (i<0 || (unsigned)i >= ctob(USIZE))
			goto error;
		ipc.ip_data = (long)((physadr)&u)->r[i>>1];
		break;

	/* write user I */
	/* Must set up to allow writing */
	case 4:
		/*
		 * If text, must assure exclusive use
		 */
		if (xp = u.u_procp->p_textp) {
			if (xp->x_count!=1 || xp->x_iptr->i_mode&ISVTX)
				goto error;
			xp->x_iptr->i_flag &= ~ITEXT;
		}
		if(u.u_segmented) {
			if (u.u_sep)
				ldsdr(hibyte(ipc.ip_addr.l),-1,-1,0);
			if(suword(ipc.ip_addr.l,ipc.ip_data))
				goto error;
			if (u.u_sep)
				ldsdr(hibyte(ipc.ip_addr.l),-1,-1,1);
		} else {
			estabur(u.u_tsize,u.u_dsize,u.u_ssize,0);
			if(suiword(ipc.ip_addr.right,ipc.ip_data))
				goto error;
			estabur(u.u_tsize,u.u_dsize,u.u_ssize,1);
		}
		if (xp)
			xp->x_flag |= XWRIT;
		break;

	/* write user D */
	case 5:
		if (suword((caddr_t)ipc.ip_addr.l, ipc.ip_data))
			goto error;
		break;

	/* write u */
	case 6:
		i = (int)ipc.ip_addr.right;
		p = (int *)&((physadr)&u)->r[i>>1];
		for (i=0; i<15; i++)
			if (p == &u.u_state->s_reg[i])
				goto ok;
		if (p == &u.u_state->s_sp)
			goto ok;
		if (p == &u.u_state->s_pc)
			goto ok;
		if (p == &u.u_state->s_ps) {
			ipc.ip_data = (ipc.ip_data&0x00ff)|(u.u_state->s_ps&0xff00);
			goto ok;
		}
		goto error;

	ok:
		*p = ipc.ip_data;
		break;

	/* set signal and continue */
	/* one version causes a trace-trap */
	case 9:
		u.u_state->s_ps |= TBIT/*PS_T*/;
	case 7:
		if ((int)ipc.ip_addr.right != 1) {
			if(u.u_segmented)
				u.u_state->s_pcseg = (int)ipc.ip_addr.left;					
			u.u_state->s_pc = (int)ipc.ip_addr.right;
		}
		u.u_procp->p_sig = 0L;
		if (ipc.ip_data)
			psignal(u.u_procp, ipc.ip_data);
		return(1);

	/* force exit */
	case 8:
		exit(fsig(u.u_procp));

	default:
	error:
		ipc.ip_req = -1;
	}
	return(0);
}
