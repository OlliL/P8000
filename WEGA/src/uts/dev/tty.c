/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : tty.c
 
	Bearbeiter	: O. Lehmann
	Datum		: 01.05.08
	Version		: 1.0
 
***************************************************************************
**************************************************************************/

#include <sys/param.h>
#include <sys/state.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/ttold.h>
#include <sys/sysinfo.h>
#include <sys/proc.h>
#include <sys/inode.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>


extern int Canbsiz;
char canonb[256];
extern int numterm;
char ttywstr[] = {
	"@[$]tty.c		Rev : 4.2+ 	02/07/84 13:50:59"};
int tthiwat[] = {	
	0,
	60,	60,	60,	60,	60,	60,

	120,	120,	180,	180,	240,	240,
	240,	100,	100
};
int ttlowat[] = {	
	0,
	20,	20,	20,	20,	20,	20,
	40,	40,	60,	60,	80,	80,
	80,	50,	50
};
char ttcchar[NCC] = { 
	CINTR, CQUIT, CERASE, CKILL, CEOF };
static int noterm;
char maptab[]	={
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,    '|',	0,	0,	0,	0,	0,    '`',
	'{',    '}',	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,    '~',	0,
	0,    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',      0,	0,	0,	0,	0,
};



ttopen(tp)
register struct tty *tp;
{
	register struct proc *pp;

	pp = u.u_procp;
	if (tp->t_state & XCLUSE) {
		u.u_error = EACCES;
		return;
	}
	if ((pp->p_pid == pp->p_pgrp) && !(u.u_ttyp) && !(tp->t_pgrp)){
		u.u_ttyp = tp;
		tp->t_pgrp = pp->p_pgrp;
	}
	tp->t_state &=~WOPEN;
	tp->t_state |= ISOPEN;
}



ttclose(tp)
register struct tty *tp;
{
	dvi();
	(*tp->t_proc)(tp, T_RESUME);
	evi();
	ttyflush(tp, T_TIME);
	ttywait(tp);
	tp->t_state & = ~(WOPEN | ISOPEN);
}


#define CDEF	(CLOCAL | PARODD | PARENB | CREAD | CSTOPB)
#define CDEF1	(CLOCAL | PARENB | CREAD | CSTOPB)

ttiocom(tp, com, addr, flag)

register struct tty *tp;
caddr_t addr;
int com;
{
	register ushort cflg;
	register int tmod;
	register int ldmode;
	ushort iflg;
	struct termio t;
	struct sgttyb told;
	struct tchars toc;

	switch (com){
	case TCGETA:
		u.u_r.r_val1 = TIOC;
		break;
	case TIOCFLUSH:
	case TIOCSETC:
		ttywait(tp);
		if (com == TCSETAF)
			ttyflush(tp, T_RESUME);
	case TIOCGETC:
		if (copyin( addr.l, (caddr_t)&t, sizeof(t)) == 0){
		if (tp->t_line != t.c_line){
			if ((t.c_line < 0) || (t.c_line >= linecnt)){
				u.u_error = EINVAL;
				break;
			}
			(*linesw[tp->t_line].l_ioctl)(tp, LDCLOSE,  (caddr_t) 0, flag);
		}
		ldmode = tp->t_lflag;
		tmod = 0;
		if ((tp->t_cflag & CBAUD) != (t.c_cflag & CBAUD))
			tmod++;
		if ((tp->t_cflag & CSIZE) != (t.c_cflag & CSIZE))
			tmod++;
		if ((tp->t_cflag ^ t.c_cflag ) & CDEF)
			tmod++;
		if ((tp->t_iflag ^ t.c_iflag ) & IGNBRK)
			tmod++;
		tp->t_iflag = t.c_iflag;
		tp->t_oflag = t.c_oflag;
		tp->t_cflag = t.c_cflag;
		tp->t_lflag = t.c_lflag;
		bcopy(t.c_tcc, tp->t_cc, sizeof(tp->t_cc));
		if (tp->t_line != t.c_line) {
			tp->t_line = t.c_line;
			(*linesw[tp->t_line].l_ioctl)(tp, LDIOC, (caddr_t) 0, flag);
		}
		else
			if (tp->t_lflag != ldmode)
				(*linesw[tp->t_line].l_ioctl)(tp, LDCHG, (long) ldmode, flag);
		if (tmod) return(1);
break;
}
		break;
	case IOCTYPE:
		t.c_iflag = tp->t_iflag;
		t.c_oflag = tp->t_oflag;
		t.c_cflag = tp->t_cflag;
		t.c_lflag = tp->t_lflag;
		t.c_line = tp->t_line;
		bcopy (tp->t_cc, t.c_tcc, sizeof(tp->t_cc));
		if (copyout((caddr_t) &t, addr.l, sizeof(t)))
			u.u_error = EFAULT;
		break;
	case TIOCSETN:
		ttywait(tp);
		if ((((long)addr.l) & 0x0000ffffL) == 0)
			(*tp->t_proc)(tp, T_BREAK);
		break;
	case TIOCSETP:
		switch (addr.half.right){
		case 000:
			(*tp->t_proc)(tp, T_SUSPEND);
			break;
		case 001:
			(*tp->t_proc)(tp, T_RESUME);
			break;
		case 002:
			(*tp->t_proc)(tp, T_BLOCK);
			break;
		case 003:
			(*tp->t_proc)(tp, T_UNBLOCK);
			break;
		default:
			u.u_error = EINVAL;
		}
		break;
	case TCSETAF:
		ttyflush(tp, T_RESUME);
		break;
	case TIOCGETP:
		switch (addr.half.right){
		case 000:
		case 001:
		case 002:
			ttyflush(tp, (addr.half.right+1)&3);
			break;
		default:
			u.u_error  = EINVAL;
		}
		break;
	case TCXCLUSE:
		tp->t_cflag |= HUPCL;
		break;
	case TCXONC:
		ttywait(tp);
		ttyflush(tp, T_RESUME);
	case TCSBRK:
		if (copyin( addr.l, (caddr_t) &told, sizeof(told))){
			cflg = tp->t_cflag;
			iflg = tp->t_iflag;
			tp->t_iflag = 0;
			tp->t_oflag = 0;
			tp->t_lflag = 0;
			tp->t_cflag = (told.sg_ispeed & 0xf) | CREAD;
			tp->t_cflag |= CLOCAL | CSTOPB;
			tp->t_cc[VERASE] = told.sg_erase;
			tp->t_cc[VKILL] = told.sg_kill;
			ldmode = told.sg_flags;
			if (ldmode & O_TBDELAY){
				if (ldmode & O_TAB1)
					tp->t_oflag |= TAB1;
				if (ldmode & O_TAB2)
					tp->t_oflag |= TAB2;
			}
			if (ldmode & O_LCASE){
				tp->t_iflag |= IUCLC;
				tp->t_oflag |= OLCUC;
				tp->t_lflag |= XCASE;
			}
			if (ldmode & O_ECHO)
				tp->t_lflag |= ECHO;
			if (ldmode & O_CRMOD){
				tp->t_iflag |= ICRNL;
				tp->t_oflag |= ONLCR;
			}
			if (ldmode & O_CR1)
				tp->t_oflag |= CR1;
			if (ldmode & O_CR2)
				tp->t_oflag |= CR2;
			if (ldmode & O_NL1)
				tp->t_oflag |= NLDLY;
			if (ldmode & O_NL2)
				tp->t_oflag |= NLDLY;
			if (ldmode & O_RAW){
				tp->t_cc[VTIME] = 6;
				tp->t_cc[VMIN] = 1;
				tp->t_iflag &= ~(ICRNL | IUCLC);
				tp->t_cflag |= CS8;
			}
			else
				if (ldmode & O_CBREAK){
					tp->t_cc[VTIME] = 6;
					tp->t_cc[VMIN] = 1;
					tp->t_iflag |= IXON | ISTRIP | IGNBRK;
					tp->t_oflag |= OPOST;
					tp->t_cflag |= HUPCL | CS8;
					tp->t_lflag |= ECHONL | ECHOK | ECHOE | ISIG;
				}
				else {
					tp->t_cc[VMIN] = 4;
					tp->t_cc[VTIME] = 0;
					tp->t_iflag |= IXON | ISTRIP | IGNBRK;
					tp->t_oflag |= OPOST;
					tp->t_cflag |= HUPCL | CS8;
					tp->t_lflag |= ECHONL | ECHOK | ECHOE | ISIG | ICANON;
				}
			tp->t_iflag |= INPCK;
			tp->t_cflag |= PARENB;
			if (ldmode & O_ODDP)
				if (ldmode &O_EVENP){
					tp->t_iflag &= ~INPCK;
					tp->t_cflag &= ~PARENB;
				}
				else
					tp->t_cflag |= PARODD;
			if (ldmode &O_VTDELAY)
				tp->t_oflag |= FF1;
			if (ldmode & O_BSDELAY)
				tp->t_oflag |= BS1;
			if (ldmode & O_TANDEM)
				tp->t_iflag |= IXOFF;
			if (iflg & ISPCI)
				tp->t_iflag |= ISPCI;
			tmod = 0;
			if ((tp->t_cflag & CBAUD) != (cflg & CBAUD))
				tmod++;
			if ((tp->t_cflag & CSIZE) != (cflg & CSIZE))
				tmod++;
			if ((tp->t_cflag ^ cflg) & CDEF1)
				tmod++;
			if ((tp->t_iflag ^ iflg) & IGNBRK)
				tmod++;
			if (tmod)
				return(1);
		}
		else
			u.u_error = EFAULT;
		break;
	case TCFLSH:
		told.sg_ospeed = told.sg_ispeed = tp->t_cflag & CBAUD;
		told.sg_erase = tp->t_cc[VERASE];
		told.sg_kill = (long)tp->t_cc[VKILL];
		ldmode = 0;
		if ((tp->t_lflag & ICANON) || (tp->t_lflag & ISIG) )
			ldmode |= O_RAW;
		if ((tp->t_lflag & ICANON) || (tp->t_lflag & ISIG))
			ldmode |= O_CBREAK;
		if (tp->t_lflag & XCASE)
			ldmode |= O_LCASE;
		if (tp->t_lflag & ECHO)
			ldmode |= O_ECHO;
		if (tp->t_cflag & PARODD)
			ldmode |= O_ODDP;
		else
			if (tp->t_iflag & INPCK)
				ldmode |= O_EVENP;
			else
				ldmode |= O_EVENP | O_ODDP;
		if (tp->t_oflag & ONLCR){
			ldmode |= O_CRMOD;
			if (tp->t_oflag & CR1)
				ldmode |= O_CR1;
			if (tp->t_oflag & CR2)
				ldmode |= O_CR2;
		}
		else {
			if (tp->t_oflag & CR1)
				ldmode |= O_NL1;
			if (tp->t_oflag & CR2)
				ldmode |= O_NL2;
		}
		if (((tp->t_oflag &TABDLY) == TAB3) || (tp->t_oflag & TAB1))
			ldmode |= O_TBDELAY;
		if (tp->t_oflag & FF1)
			ldmode |= O_VTDELAY;
		if (tp->t_oflag & BSDLY)
			ldmode |= O_BSDELAY;
		if (tp->t_iflag & IXOFF)
			ldmode |= O_TANDEM;
		told.sg_flags = ldmode;
		if (copyout((caddr_t) &told, addr.l, sizeof(told)))
			u.u_error = EFAULT;
		break;
	case TIOCHPCL:
		tp->t_state |= XCLUSE;
		break;
	case TCNXCLUSE:
		tp->t_state &= ~XCLUSE;
		break;
	case TCSETAW:
		if (copyin( addr.l, (caddr_t) &toc, sizeof(toc))){
			tp->t_cc[VINTR] = toc.t_intrc;
			tp->t_cc[VQUIT] = toc.t_quitc;
			if (tp->t_lflag & ICANON)
				tp->t_cc[VEOF] = toc.t_eofc;
		}
		else
			u.u_error = EFAULT;
		break;
	case TCSETA:
		toc.t_intrc = tp->t_cc[VINTR];
		toc.t_quitc = tp->t_cc[VQUIT];
		toc.t_eofc = tp->t_cc[VEOF];
		toc.t_startc = CSTART;
		toc.t_stopc = CSTOP;
		toc.t_brkc = 0;
		if (copyout((caddr_t)&toc, addr.l, sizeof(toc)))
			u.u_error =EFAULT;
		break;
	default:
		if ((com & IOCTYPE) == LDIOC)
			(*linesw[tp->t_line].l_ioctl)(tp, com, addr.l, flag);
		else
			u.u_error = EINVAL;
	}
	return(0);
}

ttinit(tp)
register struct tty *tp;
{
	tp->t_line = 0;
	tp->t_iflag = IXON | ICRNL | ISTRIP | IGNBRK;
	tp->t_oflag = TAB3 | ONLCR | OPOST;
	tp->t_cflag = HUPCL | (unsigned char) ~B75;
	tp->t_lflag = (unsigned char) (~(NOFLSH | XCASE));
	bcopy((caddr_t) ttcchar, tp->t_cc, sizeof(ttcchar));

}

ttywait(tp)
register struct tty *tp;
{

	dvi();
	while ((tp->t_outq.c_cc) || (tp->t_state & (BUSY | TIMEOUT))){
		tp->t_state |= TTIOW;
		sleep( &tp->t_oflag, TTOPRI);
	}
	evi();
}

ttyflush(tp, com)
register struct tty *tp;
{
	register struct cblock *p;
	register unsigned s;

	if (com & T_SUSPEND){
		while (p = getcb ( &tp->t_outq))
			putcf(p);
		(*tp->t_proc)(tp, T_WFLUSH);
		if (tp->t_state & OASLP){
			tp->t_state &= ~OASLP;
			wakeup(&tp->t_outq);
		}
		if (tp->t_state & TTIOW){
			tp->t_state &= ~TTIOW;
			wakeup(&tp->t_oflag);
		}
	}
	if (com & T_TIME){
		while (p = (getcb(&tp->t_canq)))
			putcf(p);
		s = dvi();
		while (p = (getcb(&tp->t_rawq)))
			putcf(p);
		tp->t_delct = 0;
		rvi(s);
		(*tp->t_proc)(tp, T_RFLUSH);
		if (tp->t_state & IASLP){
			tp->t_state &= ~IASLP;
			wakeup(&tp->t_rawq);
		}
	}
}

canon(tp)
register struct tty *tp;
{

	register char *cbp;
	register int c;
	register int nc;

	dvi(tp);
	if ((tp->t_rawq.c_cc) == 0)
		tp->t_delct = 0;
	while (tp->t_delct == 0){
		if ((tp->t_state & CARR_ON) == 0){
			evi();
			return;
		}
		if (!(tp->t_lflag & ICANON) && !(tp->t_cc[VEOF])){
			if (tp->t_cc[VEOL] == 0)
				break;
			tp->t_state &= ~RTO;
			if ((tp->t_state & TACT) == 0)
				tttimeout(tp);
		}
		tp->t_state |= IASLP;
		sleep(tp, TTIPRI);
	}
	if (!(tp->t_lflag & ICANON)){
		tp->t_canq = tp->t_rawq;
		tp->t_rawq = ttnulq;
		tp->t_delct=0;
		evi();
		return;
	}
	evi();
	cbp = canonb;
	nc = 0;
	if (tp->t_state & IEXTPROC){
		do {
		    if (((c = getc(&tp->t_rawq)) < 0) || (c == tp->t_cc[VEOF]))
			break;
		    putc(c, &tp->t_canq);
		    sysinfo.canch++;
		} 	
		while ((c != '\n') && (c != tp->t_cc[VEOL]));
mret: 
		tp->t_delct--;
		return;
	}
	while ((c = getc(&tp->t_rawq)) >= 0){
		if (nc == 0)
			if (c == '\\')
				nc++;
			else {
				if (c == tp->t_cc[VERASE]){
					if (cbp > canonb)
						cbp--;
					continue; 
				}
				if (c == tp->t_cc[VKILL]){
					cbp = canonb;
					continue;
				}
				else{
					if (c == tp->t_cc[VEOF])
						break;
				}
			}
		else {
			nc = 0;
			if ((c == tp->t_cc[VERASE]) ||
			    (c == tp->t_cc[VKILL]) ||
			    (c == tp->t_cc[VEOF]))
				cbp--;
			else
				if (tp->t_lflag & XCASE){
					if ((c < 0x80) && (maptab[c])){
						cbp--;
						c = maptab[c];
					} 
					else
						if (c =='\\')
							continue;
				}
				else /* von Y */
					if (c == '\\')
						nc++;
		}
		*cbp++ = c;
		if ((c == '\n') || (c == tp->t_cc[VEOL]))
			break;
		if (cbp >= &canonb[Canbsiz])
			cbp--;
	}
	c = cbp - canonb;
	sysinfo.canch += (long) c;
	cbp = canonb;
	while ((int)c-- != 0)
		putc((int) *cbp++, &tp->t_canq);
	goto mret;
}

ttrstrt(tp)
register struct tty *tp;
{

	(*tp->t_proc)(tp, T_TIME);
}

ttread(tp)
register struct tty *tp;
{

	register struct clist *tc;
	register count;
	register c;
	union {
		struct cblock *cf;
		char rc[CLSIZE];
	} 
	ur;

	tc = &tp->t_canq;
	if (tc->c_cc == 0){
		if (u.u_fmode & FPIPE)
			return;
		canon(tp);
	}
	while ((u.u_count) && (u.u_error == 0)){
		if (u.u_count >= CLSIZE){
			if ((ur.cf = getcb(tc)) == 0)
				break;
			count = min(u.u_count, ur.cf->c_last - ur.cf->c_first);
			if (copyout(&ur.cf->c_data[ur.cf->c_first], u.u_base, count))
				u.u_error = EFAULT;
			putcf(ur.cf);
		}
		else {
			count = 0;
			while (count < u.u_count){
				if ((c = getc(tc)) < 0)
					break;
				ur.rc[count] = c;
				count++;
			}
			if (count == 0)
				break;
			if (copyout(ur.rc, u.u_base, count))
				u.u_error = EFAULT;
		}
		u.u_base.half.right += count;
		u.u_count -= count;
	}
	if ((tp->t_state & TBLOCK) && (tp->t_rawq.c_cc < TTXOLO))
		(*tp->t_proc)(tp, T_UNBLOCK);
}

ttwrite(tp)
register struct tty *tp;
{
	register count;
	register struct cblock *cf;

	if (tp->t_state & CARR_ON){
		while (u.u_count){
			dvi();
			while (tp->t_outq.c_cc > tthiwat[tp->t_cflag & CBAUD]) {
				(*tp->t_proc)(tp, T_OUTPUT);
				tp->t_state |= OASLP;
				sleep(&tp->t_outq, TTOPRI);
			}
			evi();
			if (u.u_count >= 8){
				if ((cf = getcf()) == 0)
					break;
				count = min(u.u_count, cf->c_last);
				if (copyin(u.u_base, cf->c_data, count)){
					u.u_error = EFAULT;
					putcf(cf);
					break;
				}
				u.u_base.half.right += count;
				u.u_count -= count;
				cf->c_last = count;
				ttxput(tp, cf, count);
			}
			else {  
				char wc;
				if (fubyte(u.u_base.l++, &wc)){
					u.u_error = EFAULT;
					break;
				}
				u.u_count--;
				ttxput(tp, (long) wc, 0);
			}
		}
		dvi();
		(*tp->t_proc)(tp, T_OUTPUT);
		evi();
	}
}

ttchkopen()
{
	if (noterm < numterm){
		noterm++;
		return(0);
	}
	return(1);
}

ttchkclose()
{
	noterm--;
}
