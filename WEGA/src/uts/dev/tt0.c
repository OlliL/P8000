


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

extern putcb();

ttioctl(tp, com, arg)

register struct tty *tp;
int com;
long arg;

{

	ushort ldmode;

	if (com !=(short) LDCHG) return;
	ldmode = tp->t_lflag ^ arg;
	if (!(ldmode & ICANON)) return;
	dvi();
	if (tp->t_canq.c_cc){
		if (tp->t_rawq.c_cc){
			tp->t_canq.c_cc += tp->t_rawq.c_cc;
			tp->t_canq.c_cl->c_next = tp->t_rawq.c_cf;
			tp->t_outq.c_cf = tp->t_rawq.c_cl;
		}
		tp->t_rawq = tp->t_canq;
		tp->t_canq = ttnulq;
	}
	tp->t_delct = tp->t_rawq.c_cc;
	evi();
}
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


ttout(tp)

register struct tty *tp;
{
	return;
}


extern timeout();
tttimeout(tp)

register struct tty *tp;

{

	tp->t_state &= ~TACT; /* timeout waiting on rawq */
	if (tp->t_lflag & ICANON) return; /* enable canonicalization */
	if (tp->t_cc[VTIME] == 0) return; /* time out */
	if (( tp->t_rawq.c_cc == 0) && (tp->t_cc[VMIN])) return;
	if (tp->t_state & RTO){ /* timeout waiting on rawq */
		tp->t_delct = 1;
		if (!(tp->t_state & IASLP)) return; /* wakeup when input done */
		tp->t_state &= ~IASLP;
		wakeup(tp);
		return;
	}
	tp->t_state |= (RTO | TACT);
	timeout(tttimeout, tp, tp->t_cc[VTIME] * 6);
}

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

extern char partab[];

ttxput(tp, arg, count)

register struct tty *tp;
union { 
	long c;
	struct cblock *cb;
	struct
	    {
		char l_l;
		char l_r;
		char r_l;
		char r_r;
	} 
	byte;
} 
arg;
int count;

{

	register ch;
	register ushort mode;
	register char *cp;
	char *colp;
	int pch, vch;
	struct cblock *cbp;

	mode = tp->t_oflag;
	if (tp->t_state & OEXTPROC)
		mode &= ~OPOST;
	if (!(count)) {
		if (tp->t_outq.c_cc >= TTYHOG) return;
		count++;
		if (!(mode & OPOST)){
			sysinfo.outch++;
			putc(arg.r_r, &tp->t_outq);
			return;
		}
		else {
			cp = &arg.r_r;
			cbp = 0;
		}
	}
	else {
		if (!(mode & OPOST)){
			sysinfo.outch += count;
			putcb(arg.c, &tp->t_outq);
			return;
		}
		else {
			cp = &arg.cb->c_data[arg.cb->c_first];
			cbp = arg.cb;
		}
	}
	while(count--){
		ch = (unsigned char) (*cp++);
		if (ch >= DELAYDEL){ /* b7 */
			if (ch == DELAYDEL)
				putc(DELAYDEL, &tp->t_outq);
			sysinfo.outch++;
			putc(ch, &tp->t_outq);
			continue;
		} 
		if (tp->t_lflag  & XCASE){
			for ( colp = "({)}!|^~'`\\"; (*colp++); )
				if (ch  == *colp++){
					tp->t_lflag &= ~XCASE;
					ttxput(tp, (long)'\\', 0);
					tp->t_lflag |= XCASE;
					ch = *(colp - 2);
					break;
				}
			if ((ch >= 'A') && (ch <= 'Z')){
				tp->t_lflag &= ~XCASE;
				ttxput(tp, (long)'\\', 0);
				tp->t_lflag |= XCASE;
			}
		}
		if (mode & OLCUC)
			if ((ch >= 'a') &&
			    (ch <= 'z'))
				ch += 'A'-'a';
		vch = ch;
		pch = partab[ch];
		colp = &tp->t_col;
		ch = 0;
		switch (pch & 077){
		case 0:
			(*colp)++;
			break;
		case 2:
			if (mode & BS1)
				ch = 2;
			if (*colp)
				(*colp)--;
			break;
		case 3:
			if (!(mode & ONLRET)){
				if (mode & ONLCR){
					if ((!(mode & ONOCR)) ||
					    (*colp)){
						sysinfo.outch++;
						putc('\r', &tp->t_outq);
					}
				}
				else 
					goto nldly;
			}
			goto crdly;
		case 4:
			ch = 8 - (*colp & 7);
			*colp += ch;
			pch = mode & TABDLY;
			if (pch == 0){
				ch = 0;
				break;
			}
			if (pch == TAB1){
				if (ch < 5) 
					ch = 0;
				break;
			}
			if (pch == TAB2){
				ch = 2;
				break;
			}
			if (pch == TAB3){
				sysinfo.outch += ch;
				do
				    putc(' ', &tp->t_outq);
				while (--ch);
				continue;
			}
			break;
		case 5:
			if (mode & VTDLY)
				ch = 0x7f;
			break;
		case 6:
			if (mode & OCRNL){
				vch = '\n';
nldly:					
				if (mode & NLDLY)
					ch = 5;
				break;
			}
			if ((mode & ONOCR) &&
			    (*colp == 0))
				continue;
crdly:				
			pch = mode & CRDLY;
			if (pch == CR1){
				if (*colp)
					ch  = max((*colp>>4)+3, 6);
			}
			else
				if (pch == CR2)
					ch = 5;
				else
					if (pch == CR3)
						ch = 9;
			*colp = 0;
		case 1:
		default:
			break;
		case 7:
			if (mode & FFDLY)
				ch = 0x7f;
		}
		sysinfo.outch++;
		putc(vch, &tp->t_outq);
		if (ch){
			if ((ch < ' ') &&
			    (mode & OFILL)){
				if (mode & OFDEL)
					vch = 0x7f;
				else
					vch = 0;
				putc(vch, &tp->t_outq);
				if (ch > 3)
					putc(vch, &tp->t_outq);

			}
			else {
				putc(DELAYDEL, &tp->t_outq);
				putc(ch | DELAYDEL, &tp->t_outq);
			}
		}
		/* }
												 else {
													putc(DELAYDEL, &tp->t_outq);
													sysinfo.outch++;
													putc(ch, &tp->t_outq);
													continue;
												} */
	} /* von while */
	if  (cbp)
		putcf(cbp);
}


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

extern putcb();

ttin(tp, c, count)

register struct tty *tp;
long c;
int count;

{

	register ushort ch;
	register ushort mode;
	register char *cp;
	char *csp;

	mode = tp->t_iflag;
	if (count){
		if (count == 1)
			goto L214;
		putcb(c, tp);
		sysinfo.rawch += count;
		cp = &c->c_data[c->c_first];
L5a:			
		if (tp->t_rawq.c_cc <= TTXOHI) { /* 3 */
L62:			
			mode = tp->t_lflag;
			if ((tp->t_state & IEXTPROC) ||
			    (mode == 0)){ /* 4,5 */
				if (!(mode & ICANON)){/* 6 */
					tp->t_state &= ~RTO;
					if (tp->t_cc[VMIN] <= tp->t_rawq.c_cc) /* 7 */
						tp->t_delct = 1;
					else
						if ((tp->t_cc[VTIME]) &&
						    ((tp->t_state & TACT) == 0)) /* 72,73 */
							ttimeout(tp);
					if ((tp->t_delct) == 0) return; /* 74 */
				}
				if (tp->t_state & IASLP) { /* 75 */
					tp->t_state &= ~IASLP;
					wakeup(tp);
				}
				return;
			}
			if (count--){ /* 67 */
				ch = *cp++;
				mode = tp->t_lflag;
				if (!(mode & ISIG)){ /* 68 */
L494:						
					if (mode & ICANON){ /* 69 */
						if (ch == '\n'){ /* 70 */
							if (mode & ECHONL)
								mode |= ECHO;
							tp->t_delct++;
						} 
						else
							if (ch == tp->t_cc[VEOL])
								tp->t_delct++;
						if ((tp->t_state & ESC) == 0){ /* 50 */
							if (ch == '\\')
								tp->t_state |= ESC;
							if ((ch == tp->t_cc[VERASE]) &&
							    (mode & INPCK)){ /* 52,53 */
								if (mode & ECHO) /* 54 */
									ttxput(tp, (long) CERASE, 0);
								mode |= ECHO;
								ttxput(tp, (long) ' ', 0);
								ch = CERASE;
							} /* von 52,53 */
							else if ((ch == tp->t_cc[VKILL]) &&
							    (mode & ISTRIP)){ /* 55,56 */
								if ((mode & ECHO) &&
								    (mode & ISPCI)){ /* 57,58 */
									for ( csp = &"???"; (*csp);)
										ttxput(tp, (long) *csp++, 0);
									(*tp->t_proc)(tp, T_OUTPUT);
								}
								ttxput(tp, (long) ch, 0);
								mode |= ECHO;
								ch = '\n';
							}
							else if (ch != tp->t_cc[VEOF]) /* 60 */
								if ((mode & ECHO) &&
								    (mode & ISPCI)){ /* 61,62 */
									for ( csp = &"(eof)"; (*csp);)
										ttxput(tp, (long) *csp++, 0);
									(*tp->t_proc)(tp, T_OUTPUT);
								}
							mode &= ~ECHO;
							tp->t_delct++;
						}
						else if ((ch == '\\') ||
						    (mode & XCASE)) /* 64,65 */
							tp->t_state  &= ~RTO;
					}
					if (mode & ECHO) /* 66 */
						ttxput(tp, (long) ch, 0);
					(*tp->t_proc)(tp, T_OUTPUT);
				} /* von 68 */
				else {
					if (ch != tp->t_cc[VINTR]){ /* 43 */
						if (ch == tp->t_cc[VQUIT]) /* 44 */
							goto L494;
						if (!(mode & NOFLSH))
							ttyflush(tp, T_RESUME);
						if ((mode & ECHO) ||
						    (mode & ISPCI)){ /* 46,47 */
							for ( csp = &"(quit)"; (*csp); csp++)
								ttxput(tp,(long) *csp, 0);
							(*tp->t_proc)(tp, T_OUTPUT);
						} /* von 43 */
					}
					else {
						if (!(mode & NOFLSH))
							ttyflush(tp, T_RESUME);
						if ((mode & ECHO) &&
						    (mode & ISPCI)) /* 40,41 */{
							for ( csp = &"(intr)"; (*csp); csp++)
								ttxput(tp, (long) *csp, 0);
							(*tp->t_proc)(tp, T_OUTPUT);
						} /* von 40,41 */
					}
				} /* von 68 else */
			} /* von 67 */
			if (!(mode & ICANON)){ /* 6 */
				if (tp->t_cc[VMIN] <= tp->t_rawq.c_cc)
					tp->t_delct = 1;
				if ((tp->t_cc[VTIME]) &&
				    ((tp->t_state & TACT) == 0))
					tttimeout(tp);
			} /* von 6 */
			if ((tp->t_delct) &&
			    (tp->t_state & IASLP)){ /* 74,75 */
				tp->t_state &= ~IASLP;
				wakeup(tp);
			}
			return;
		} /* von 3 */
		else {
			if ((mode & IXOFF)&&
			    (tp->t_state & TBLOCK)) /* 8,9 */
				(*tp->t_proc)(tp, T_BLOCK);
			if (tp->t_rawq.c_cc <= TTYHOG) /* 10 */
				goto L62;
			else
				ttyflush(tp, T_TIME);
			return;
		} /* 3 von else */
	} /* von 1 */
	else {
		count++;
		ch = c.half.left;
		if ((ch & IXOFF) &&
		    (!(mode & INPCK))) /* 13,14 */
			ch &= ~PERROR;
		if (ch & (OVERRUN | FRERROR | PERROR)){ /* 15 */
			if (ch & 0xff){ /* 16 */
				if (mode & IGNPAR) return;
				if (mode & IGNBRK) return;
				if (mode & BRKINT){ /* 12 */
					signal(tp->t_pgrp, SIGINT);
					ttyflush(tp, T_RESUME);
					return;
				}
			} /* von 16 */
			if (mode & PARMRK){ /* 18 */
				ttin(tp, (long) 0xff, 1);
				ttin(tp, (long) 0, 1);
			}
			else
				ch = 0;
			ch |= ICRNL;
		} /* von 15 */
		else { 
			if (mode & ISTRIP) /* 23 */
				ch &= 0x7f;
			else
				if (((ch & 0xff)== 0xff) &&
				    (mode & PARMRK) &&
				    (putc( 0xff, tp))) /* 24,25,26 */ return;
		}
		if (mode & IXON){ /* 19 */
			if (tp->t_state & TTSTOP)
				if ((ch == CSTART) ||
				    (mode & IXANY)) /* 21,22 */
					(*tp->t_proc)(tp, T_RESUME);
			if (ch  == CSTART) return;
			if (ch == CSTOP) return;
		}
		if ((ch == '\n')&& 
		    (mode & INLCR)) /* 30,31 */
			c = '\r';
		else {
			if (c == 0xd){ /* 32 */
				if (mode & IGNCR) return;
				if (mode & ICRNL)
					c = '\n';
			}
		}
	}
	if (mode & IUCLC)  /* 35 */
		c = toupper(c);
L214:			
	if (putc(c, tp)){/* 38 */
		sysinfo.rawch++;
		cp = &lobyte(c.half.right);
		goto L5a;
	}
}
