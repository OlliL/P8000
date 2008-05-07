/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : tty.c
 
	Bearbeiter	: O. Lehmann
	Datum		: 01.05.08
	Version		: 1.0
 
***************************************************************************
**************************************************************************/

/*
 * general TTY subroutines
 */

#include <sys/param.h>
#include <sys/state.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/ttold.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/sysparm.h>
#include <sys/proc.h>
#include <sys/inode.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>

#define CDEF	(CLOCAL | PARODD | PARENB | CREAD | CSTOPB)
#define CDEF1	(CLOCAL | PARENB | CREAD | CSTOPB)

char ttywstr[] = "@[$]tty.c		Rev : 4.2+ 	02/07/84 13:50:59";

extern int Canbsiz;
extern int numterm;

/*
 * tty low and high water marks
 * high < TTYHOG
 */
int	tthiwat[16] = {
	0, 60, 60, 60,
	60, 60, 60, 120,
	120, 180, 180, 240,
	240, 240, 100, 100,
};
int	ttlowat[16] = {
	0, 20, 20, 20,
	20, 20, 20, 40,
	40, 60, 60, 80,
	80, 80, 50, 50,
};

char	ttcchar[NCC] = {
	CINTR,
	CQUIT,
	CERASE,
	CKILL,
	CEOF,
	0,
	0,
	0
};
static int noterm;
/* canon buffer */
char	canonb[256];
/*
 * Input mapping table-- if an entry is non-zero, when the
 * corresponding character is typed preceded by "\" the escape
 * sequence is replaced by the table value.  Mostly used for
 * upper-case only terminals.
 */
char	maptab[] ={
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,'|',000,000,000,000,000,'`',
	'{','}',000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,'~',000,
	000,'A','B','C','D','E','F','G',
	'H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W',
	'X','Y','Z',000,000,000,000,000,
};

/*
 * routine called on first teletype open.
 * establishes a process group for distribution
 * of quits and interrupts from the tty.
 */
ttopen(tp)
register struct tty *tp;
{
	register struct proc *pp;

	pp = u.u_procp;
	if (tp->t_state&XCLUSE) {
		u.u_error = EACCES;
		return;
	}
	if ((pp->p_pid == pp->p_pgrp)
	&&(u.u_ttyp == NULL)
	&&(tp->t_pgrp == 0)) {
		u.u_ttyp = tp;
		tp->t_pgrp = pp->p_pgrp;
	}
	tp->t_state &= ~WOPEN;
	tp->t_state |= ISOPEN;
}

ttclose(tp)
register struct tty *tp;
{
	dvi();
	(*tp->t_proc)(tp, T_RESUME);
	evi();
	ttyflush(tp, FREAD);
	ttywait(tp);
	tp->t_state &= ~(ISOPEN|WOPEN);
}

/*
 * common ioctl tty code
 */
ttiocmd(tp, cmd, arg, mode)
register struct tty *tp;
caddr_t arg;
int cmd;
{
	register ushort cflg;
	register int tmod;
	register flag;
	ushort iflg;
	struct termio cb;
	struct sgttyb tb;
	struct tchars toc;

	switch(cmd) {
	case IOCTYPE:
		u.u_r.r_val1 = TIOC;
		break;
	case TCSETAW:
	case TCSETAF:
		ttywait(tp);
		if (cmd == TCSETAF)
			ttyflush(tp, (FREAD|FWRITE));
	case TCSETA:
		if (copyin( arg, (caddr_t)&cb, sizeof(cb))==0) {
			if (tp->t_line != cb.c_line) {
				if ((cb.c_line < 0) || (cb.c_line >= linecnt)) {
					u.u_error = EINVAL;
					break;
				}
				(*linesw[tp->t_line].l_ioctl)(tp, LDCLOSE,  (caddr_t) 0, mode);
			}
			flag = tp->t_lflag;
			tmod = 0;
			if ((tp->t_cflag&CBAUD) != (cb.c_cflag&CBAUD))
				tmod++;
			if ((tp->t_cflag&CSIZE) != (cb.c_cflag&CSIZE))
				tmod++;
			if ((tp->t_cflag ^ cb.c_cflag )&CDEF)
				tmod++;
			if ((tp->t_iflag ^ cb.c_iflag )&IGNBRK)
				tmod++;
			tp->t_iflag = cb.c_iflag;
			tp->t_oflag = cb.c_oflag;
			tp->t_cflag = cb.c_cflag;
			tp->t_lflag = cb.c_lflag;
			bcopy(cb.c_tcc, tp->t_cc, sizeof(tp->t_cc));
			if (tp->t_line != cb.c_line) {
				tp->t_line = cb.c_line;
				(*linesw[tp->t_line].l_ioctl)(tp, LDOPEN, (caddr_t) 0, mode);
			} else if (tp->t_lflag != flag) {
				(*linesw[tp->t_line].l_ioctl)(tp, LDCHG, (long) flag, mode);
			}
			if (tmod)
				return(1);
		} else
			u.u_error = EFAULT;
		break;
	case TCGETA:
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;
		cb.c_cflag = tp->t_cflag;
		cb.c_lflag = tp->t_lflag;
		cb.c_line = tp->t_line;
		bcopy (tp->t_cc, cb.c_tcc,NCC);
		if (copyout((caddr_t)&cb, arg, sizeof(cb)))
			u.u_error = EFAULT;
		break;
	case TCXONC:
		switch (arg) {
		case 0:
			(*tp->t_proc)(tp, T_SUSPEND);
			break;
		case 1:
			(*tp->t_proc)(tp, T_RESUME);
			break;
		case 2:
			(*tp->t_proc)(tp, T_BLOCK);
			break;
		case 3:
			(*tp->t_proc)(tp, T_UNBLOCK);
			break;
		default:
			u.u_error = EINVAL;
		}
		break;
	case TCSBRK:
		ttywait(tp);
		if(arg == 0)
			(*tp->t_proc)(tp, T_BREAK);
		break;

	case TIOCFLUSH:
		ttyflush(tp,  (FREAD|FWRITE));
		break;
	case TCFLSH:
		switch (arg) {
		case 0:
		case 1:
		case 2:
			ttyflush(tp, arg);
			break;

		default:
			u.u_error  = EINVAL;
		}
		break;
	case TIOCHPCL:
		tp->t_cflag |= HUPCL;
		break;
	case TIOCSETP:
		ttywait(tp);
		ttyflush(tp, (FREAD|FWRITE));
	case TIOCSETN:
		if (copyin( arg, (caddr_t)&tb, sizeof(tb))) {
			cflg = tp->t_cflag;
			iflg = tp->t_iflag;
			tp->t_iflag = 0;
			tp->t_oflag = 0;
			tp->t_lflag = 0;
			tp->t_cflag = (tb.sg_ispeed&CBAUD)|CREAD;
			tp->t_cflag |= CLOCAL|CSTOPB;
			tp->t_cc[VERASE] = tb.sg_erase;
			tp->t_cc[VKILL] = tb.sg_kill;
			flag = tb.sg_flags;
			if (flag&O_TBDELAY) {
				if (flag&O_TAB1)
					tp->t_oflag |= TAB1;
				if (flag&O_TAB2)
					tp->t_oflag |= TAB2;
			}
			if (flag&O_LCASE) {
				tp->t_iflag |= IUCLC;
				tp->t_oflag |= OLCUC;
				tp->t_lflag |= XCASE;
			}
			if (flag&O_ECHO)
				tp->t_lflag |= ECHO;
			if (flag&O_CRMOD) {
				tp->t_iflag |= ICRNL;
				tp->t_oflag |= ONLCR;
			}
			if (flag&O_CR1)
				tp->t_oflag |= CR1;
			if (flag&O_CR2)
				tp->t_oflag |= CR2;
			if (flag&O_NL1)
				tp->t_oflag |= NLDLY;
			if (flag&O_NL2)
				tp->t_oflag |= NLDLY;
			if (flag&O_RAW) {
				tp->t_cc[VTIME] = 6;
				tp->t_cc[VMIN] = 1;
				tp->t_iflag &= ~(ICRNL|IUCLC);
				tp->t_cflag |= CS8;
			}
			else
				if (flag&O_CBREAK) {
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
			if (flag&O_ODDP)
				if (flag&O_EVENP) {
					tp->t_iflag &= ~INPCK;
					tp->t_cflag &= ~PARENB;
				}
				else
					tp->t_cflag |= PARODD;
			if (flag&O_VTDELAY)
				tp->t_oflag |= FF1;
			if (flag&O_BSDELAY)
				tp->t_oflag |= BS1;
			if (flag&O_TANDEM)
				tp->t_iflag |= IXOFF;
			if (iflg&ISPCI)
				tp->t_iflag |= ISPCI;
			tmod = 0;
			if ((tp->t_cflag&CBAUD) != (cflg&CBAUD))
				tmod++;
			if ((tp->t_cflag&CSIZE) != (cflg&CSIZE))
				tmod++;
			if ((tp->t_cflag ^ cflg)&CDEF1)
				tmod++;
			if ((tp->t_iflag ^ iflg)&IGNBRK)
				tmod++;
			if (tmod)
				return(1);
		}
		else
			u.u_error = EFAULT;
		break;
	case TIOCGETP:
		tb.sg_ospeed = tb.sg_ispeed = tp->t_cflag&CBAUD;
		tb.sg_erase = tp->t_cc[VERASE];
		tb.sg_kill = (long)tp->t_cc[VKILL];
		flag = 0;
		if ((tp->t_lflag&ICANON) || (tp->t_lflag&ISIG) )
			flag |= O_RAW;
		if ((tp->t_lflag&ICANON) || (tp->t_lflag&ISIG))
			flag |= O_CBREAK;
		if (tp->t_lflag&XCASE)
			flag |= O_LCASE;
		if (tp->t_lflag&ECHO)
			flag |= O_ECHO;
		if (tp->t_cflag&PARODD)
			flag |= O_ODDP;
		else
			if (tp->t_iflag&INPCK)
				flag |= O_EVENP;
			else
				flag |= O_EVENP | O_ODDP;
		if (tp->t_oflag&ONLCR) {
			flag |= O_CRMOD;
			if (tp->t_oflag&CR1)
				flag |= O_CR1;
			if (tp->t_oflag&CR2)
				flag |= O_CR2;
		}
		else {
			if (tp->t_oflag&CR1)
				flag |= O_NL1;
			if (tp->t_oflag&CR2)
				flag |= O_NL2;
		}
		if (((tp->t_oflag&TABDLY) == TAB3) || (tp->t_oflag&TAB1))
			flag |= O_TBDELAY;
		if (tp->t_oflag&FF1)
			flag |= O_VTDELAY;
		if (tp->t_oflag&BSDLY)
			flag |= O_BSDELAY;
		if (tp->t_iflag&IXOFF)
			flag |= O_TANDEM;
		tb.sg_flags = flag;
		if (copyout((caddr_t)&tb, arg, sizeof(tb)))
			u.u_error = EFAULT;
		break;
	case TCXCLUSE:
		tp->t_state |= XCLUSE;
		break;
	case TCNXCLUSE:
		tp->t_state &= ~XCLUSE;
		break;
	case TIOCSETC:
		if (copyin( arg, (caddr_t)&toc, sizeof(toc))) {
			tp->t_cc[VINTR] = toc.t_intrc;
			tp->t_cc[VQUIT] = toc.t_quitc;
			if (tp->t_lflag&ICANON)
				tp->t_cc[VEOF] = toc.t_eofc;
		}
		else
			u.u_error = EFAULT;
		break;
	case TIOCGETC:
		toc.t_intrc = tp->t_cc[VINTR];
		toc.t_quitc = tp->t_cc[VQUIT];
		toc.t_eofc = tp->t_cc[VEOF];
		toc.t_startc = CSTART;
		toc.t_stopc = CSTOP;
		toc.t_brkc = 0;
		if (copyout((caddr_t)&toc, arg, sizeof(toc)))
			u.u_error =EFAULT;
		break;
	default:
		if ((cmd&IOCTYPE) == LDIOC)
			(*linesw[tp->t_line].l_ioctl)(tp, cmd, arg, mode);
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
	while (tp->t_outq.c_cc || (tp->t_state&(BUSY|TIMEOUT))) {
		tp->t_state |= TTIOW;
		sleep(&tp->t_oflag, TTOPRI);
	}
	evi();
}

/*
 * flush TTY queues
 */
ttyflush(tp, cmd)
register struct tty *tp;
{
	register struct cblock *cp;
	register s;

	if (cmd&FWRITE) {
		while ((cp = getcb(&tp->t_outq)) != NULL)
			putcf(cp);
		(*tp->t_proc)(tp, T_WFLUSH);
		if (tp->t_state&OASLP) {
			tp->t_state &= ~OASLP;
			wakeup(&tp->t_outq);
		}
		if (tp->t_state&TTIOW) {
			tp->t_state &= ~TTIOW;
			wakeup(&tp->t_oflag);
		}
	}
	if (cmd&FREAD) {
		while ((cp = getcb(&tp->t_canq)) != NULL)
			putcf(cp);
		s = dvi();
		while ((cp = getcb(&tp->t_rawq)) != NULL)
			putcf(cp);
		tp->t_delct = 0;
		rvi(s);
		(*tp->t_proc)(tp, T_RFLUSH);
		if (tp->t_state&IASLP) {
			tp->t_state &= ~IASLP;
			wakeup(&tp->t_rawq);
		}
	}
}

/*
 * Transfer raw input list to canonical list,
 * doing erase-kill processing and handling escapes.
 */
canon(tp)
register struct tty *tp;
{

	register char *cbp;
	register int c;
	register int nc;

	dvi(tp);
	if ((tp->t_rawq.c_cc) == 0)
		tp->t_delct = 0;
	while (tp->t_delct == 0) {
		if ((tp->t_state&CARR_ON) == 0) {
			evi();
			return;
		}
		if (!(tp->t_lflag&ICANON)&&!(tp->t_cc[VEOF])) {
			if (tp->t_cc[VEOL] == 0)
				break;
			tp->t_state &= ~RTO;
			if ((tp->t_state&TACT) == 0)
				tttimeout(tp);
		}
		tp->t_state |= IASLP;
		sleep(tp, TTIPRI);
	}
	if (!(tp->t_lflag&ICANON)) {
		tp->t_canq = tp->t_rawq;
		tp->t_rawq = ttnulq;
		tp->t_delct=0;
		evi();
		return;
	}
	evi();
	cbp = canonb;
	nc = 0;
	if (tp->t_state&IEXTPROC) {
		do {
		    if (((c = getc(&tp->t_rawq)) < 0) || (c == tp->t_cc[VEOF]))
			break;
		    putc(c, &tp->t_canq);
		    sysinfo.canch++;
		} 	
		while ((c != '\n')&&(c != tp->t_cc[VEOL]));
mret: 
		tp->t_delct--;
		return;
	}
	while ((c = getc(&tp->t_rawq)) >= 0) {
		if (nc == 0)
			if (c == '\\')
				nc++;
			else {
				if (c == tp->t_cc[VERASE]) {
					if (cbp > canonb)
						cbp--;
					continue; 
				}
				if (c == tp->t_cc[VKILL]) {
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
				if (tp->t_lflag&XCASE) {
					if ((c < 0x80)&&(maptab[c])) {
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
		if (cbp >=&canonb[Canbsiz])
			cbp--;
	}
	c = cbp - canonb;
	sysinfo.canch += (long) c;
	cbp = canonb;
	while ((int)c-- != 0)
		putc((int) *cbp++, &tp->t_canq);
	goto mret;
}

/*
 * Restart typewriter output following a delay timeout.
 * The name of the routine is passed to the timeout
 * subroutine and it is called during a clock interrupt.
 */
ttrstrt(tp)
register struct tty *tp;
{

	(*tp->t_proc)(tp, T_TIME);
}

/*
 * Called from device's read routine after it has
 * calculated the tty-structure given as argument.
 */
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
	if (tc->c_cc == 0) {
		if (u.u_fmode&FPIPE)
			return;
		canon(tp);
	}
	while ((u.u_count)&&(u.u_error == 0)) {
		if (u.u_count >= CLSIZE) {
			if ((ur.cf = getcb(tc)) == 0)
				break;
			count = min(u.u_count, ur.cf->c_last - ur.cf->c_first);
			if (copyout(&ur.cf->c_data[ur.cf->c_first], u.u_base, count))
				u.u_error = EFAULT;
			putcf(ur.cf);
		}
		else {
			count = 0;
			while (count < u.u_count) {
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
	if ((tp->t_state&TBLOCK)&&(tp->t_rawq.c_cc < TTXOLO))
		(*tp->t_proc)(tp, T_UNBLOCK);
}

/*
 * Called from device's write routine after it has
 * calculated the tty-structure given as argument.
 */
ttwrite(tp)
register struct tty *tp;
{
	register count;
	register struct cblock *cf;

	if (tp->t_state&CARR_ON) {
		while (u.u_count) {
			dvi();
			while (tp->t_outq.c_cc > tthiwat[tp->t_cflag&CBAUD]) {
				(*tp->t_proc)(tp, T_OUTPUT);
				tp->t_state |= OASLP;
				sleep(&tp->t_outq, TTOPRI);
			}
			evi();
			if (u.u_count >= 8) {
				if ((cf = getcf()) == 0)
					break;
				count = min(u.u_count, cf->c_last);
				if (copyin(u.u_base, cf->c_data, count)) {
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
				if (fubyte(u.u_base.l++, &wc)) {
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
	if (noterm < numterm) {
		noterm++;
		return(0);
	}
	return(1);
}

ttchkclose()
{
	noterm--;
}
