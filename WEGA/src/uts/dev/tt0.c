/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : tt0.c
 
	Bearbeiter	: O. Lehmann
	Datum		: 03.05.08
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
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/inode.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>

/*
 * Line discipline 0
 */

char tt0wstr[] = "@[$]tt0.c		Rev : 4.1 	09/29/83 01:02:58";


extern putcb();
extern char partab[];

/*
 * Place character(s) on raw TTY input queue, putting in delimiters
 * and waking up top half as needed.
 * Also echo if required.
 */
ttin(tp, ucp, ncode)
register struct tty *tp;
union { 
	long ch;
	struct cblock *ptr;
	struct
	    {
		char l_l;
		char l_r;
		char r_l;
		char r_r;
	} 
	byte;
} ucp;
int ncode;
{
	register c;
	register flg;
	register unsigned char *cp;
	char *csp;
	extern  tttimeo();

/* irgendwie fehlt noch was - siehe TODO

gebraucht wird

31c2  002c a12a  35c2  002e  0702  0004  bd30  9c28  ee05  a1a2  8da4
|     |     |     |     |     |     |     |     |     |     |     |  jp      ne,$8000+$0x46e (sprung vor die while(ncode--) schleife)
|     |     |     |     |     |     |     |     |     |     |     +- test    r10
|     |     |     |     |     |     |     |     |     |     +------- ld      r2,r10              zurueckladen von flg nach tp->t_lflag.
|     |     |     |     |     |     |     |     |     +------------- jpr     ne,#$00b8
|     |     |     |     |     |     |     |     +------------------- testl   rr2		teste register 2 auf 0
|     |     |     |     |     |     |     +------------------------- ldk     r3,0		lade ins register 3 die konstante 0
|     |     |     |     |     |     |
|     |     |     |     |     +-----+------------------------------- and     r2,#$0004          tp->t_lflag wird mit IEXTPROC binaer verknuepft  --- tp->t_lflag&IEXTROC
|     |     |     |     |
|     |     |     +-----+------------------------------------------- ldl     rr2,rr12(#46)
|     |     +------------------------------------------------------- ld      r10,r2		flg wird mit r2 definiert	                 -+
|     |                                                                                                                                           |
+-----+------------------------------------------------------------- ld      r2,rr12(#44)	tp->t_lflag wird in r2 geladen                    +- flg = tp->t_lflag


//      flg = tp->t_lflag;
        .line   186     "tt0.c" 
        ld      r2,rr12(#44)
        ld      r10,r2


*/
	flg = tp->t_iflag;
	switch (ncode) {
	case 0:
		ncode++;
		c = ucp.ch;
		if (c&PERROR && !(flg&INPCK))
			c &= ~PERROR;
		 if (c&(FRERROR|PERROR|OVERRUN)) {
			if ((c&0377) == 0) {
				if (flg&IGNBRK)
					return;
				if (flg&BRKINT) {
					signal(tp->t_pgrp, SIGINT);
					ttyflush(tp, (FREAD|FWRITE));
					return;
				}
			} else {
				if (flg&IGNPAR)
					return;
			}			
			if (flg&PARMRK) {
				ttin(tp, (long) 0377, 1);
				ttin(tp, (long) 0, 1);
			} else
				c = 0;
			c |= 0400;
		} else { 
			if (flg&ISTRIP)
				c &= 0177;
			else {
				c &= 0377;
				if (c == 0377 && flg&PARMRK)
				    if (putc(0377, &tp->t_rawq))
				     return;
			}
		}
		if (flg&IXON) {
			if (tp->t_state&TTSTOP) {
				if (c == CSTART || flg&IXANY)
					(*tp->t_proc)(tp, T_RESUME);
			} else {
				if (c == CSTOP)
					(*tp->t_proc)(tp, T_SUSPEND);
			}
			if (c == CSTART || c == CSTOP)
				return;
		}
		if (c == '\n' && flg&INLCR)
			c = '\r';
		else if (c == '\r')
			if (flg&IGNCR)
				return;
			else if (flg&ICRNL)
				c = '\n';
		if (flg&IUCLC && 'A' <= c && c <= 'Z')
			c += 'a' - 'A';
		ucp.ch = c;
	case 1:
		if (putc(ucp.r_r, &tp->t_rawq))
			return;
		sysinfo.rawch++;
		cp = (unsigned char *)&ucp.r_r;
		break;
	default:
		putcb(ucp.ch, &tp->t_rawq);
		sysinfo.rawch += ncode;
		cp = (unsigned char *)&ucp.ptr->c_data[ucp.ptr->c_first];
		break;
	}
	if (tp->t_rawq.c_cc > TTXOHI) {
		if (flg&IXOFF && !(tp->t_state&TBLOCK))
			(*tp->t_proc)(tp, T_BLOCK);
		if (tp->t_rawq.c_cc > TTYHOG) {
			ttyflush(tp, FREAD);
			return;
		}
	}

/* TODO - das passt noch nicht 100%ig */
/*

soll:
0060:     ea29  31c2  002c  
                            a12a  35c2  002e  0702  0004
0070:     bd30  9c28  ee05  a1a2  8da4
                                        5e0e  8000  046e
0080:     a7a1  5e0e  8000  04cc  94c2  0103  002e  1424



try #1
0060:     ea30  31c2  002c  
                            a12a  35c2  002e  0702  0004
0070:     bd30  9c28  e602  070a  0001  94c2  0103  002c
0080:     2f2a  31c2  002c  8d24  
                                  5e0e  8000  0478  a7a1
	flg = tp->t_lflag;
	if (flg&IEXTPROC)
		flg &= OPOST;
	tp->t_lflag = flg;

try #2
0060:     ea2f  31c2  002c  bd40  0704  0004  bd50  9c48
0070:     e607  94c2  0103  002c  2124  0704  0001  2f24
0080:     31c2  002c  8d24  5e0e  8000  0476  a7a1  5e0e

	if (tp->t_lflag&IEXTPROC)
		tp->t_lflag &= OPOST;

try #3
0060:     ea26  31c2  002c  a12a  a123  b12a  0702  0004
0070:     bd30  9c28  5e0e  8000  0464  a7a1  5e0e  8000
0080:     04bc  94c2  0103  002e  1424  0705  efff  1d24

	flg = tp->t_lflag;
	if (flg&IEXTPROC) while (ncode--) {


try #4
0060:     ea2a  31c2  002c  a12a  a123  b12a  0702  0004
0070:     bd30  9c28  e602  070a  0001  8da4  5e0e  8000
0080:     046c  a7a1  5e0e  8000  04c4  94c2  0103  002e

	flg = tp->t_lflag;
	if (flg&IEXTPROC)
		flg &= OPOST;
	if (flg) while (ncode--) {
		c = *cp++;

try #5
0060:     ea29  31c2  002c  a12a  a123  b12a  0702  0004
0070:     bd30  9c28  ee05  a1a2  8da4  5e0e  8000  046a
0080:     a7a1  5e0e  8000  04c8  94c2  0103  002e  1424

	flg = tp->t_lflag;
	if (!(flg&IEXTPROC)) {
	if (tp->t_lflag) while (ncode--) {
        [...]
	}}
	if (!(flg&ICANON)) {
	
try #6
0060:     ea27  31c2  002c  8d24  e609  35c2  002e  0702
0070:     0004  bd30  9c28  5e06  8000  0466  a7a1  5e0e
0080:     8000  04c4  94c2  0103  002e  1424  0705  efff

	if (tp->t_lflag) {
		if(!(tp->t_state&IEXTPROC)) { 
             while (ncode--) {

#looks like that is what we want but this looks strange...
0060:     ea29  31c2  002c  a12a  35c2  002e  0702  0004
0070:     bd30  9c28  ee05  a1a2  8da4  5e0e  8000  046a
0080:     a7a1  5e0e  8000  04c8  94c2  0103  002e  1424
	flg = tp->t_lflag;
	if (!(tp->t_state&IEXTPROC)) 
	if (tp->t_lflag) { 
*/
	flg = tp->t_lflag;
	if (!(tp->t_state&IEXTPROC)) 
	if (tp->t_lflag) { 
             while (ncode--) {
		c = *cp++;
		flg = tp->t_lflag;

		if (flg&ISIG) {
			if (c == tp->t_cc[VINTR]) {
				signal(tp->t_pgrp, SIGINT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				if ((flg&ECHO) &&
				    (flg&ISPCI)) { /* muss bleiben */
					for ( csp = "(intr)"; (*csp); csp++)
						ttxput(tp, (long) *csp, 0);
					(*tp->t_proc)(tp, T_OUTPUT);
				}
				continue;
			} 
			if (c == tp->t_cc[VQUIT]) {
				signal(tp->t_pgrp, SIGQUIT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				if ((flg&ECHO) ||  
				    (flg&ISPCI)) { /* muss bleiben */
					for ( csp = "(quit)"; (*csp); csp++)
						ttxput(tp,(long) *csp, 0);
					(*tp->t_proc)(tp, T_OUTPUT);
				}
				continue;
			}
		}
		if (flg&ICANON) {
			if (c == '\n') {
				if (flg&ECHONL)
					flg |= ECHO;
				tp->t_delct++;
			} else if (c == tp->t_cc[VEOL])
				tp->t_delct++;
			if (!(tp->t_state&ESC)) {
				if (c == '\\')
					tp->t_state |= ESC;
				if (c == tp->t_cc[VERASE] && flg&ECHOE) {
					if (flg&ECHO)
						ttxput(tp, (long) '\b', 0);
					flg |= ECHO;
					ttxput(tp, (long) ' ', 0);
					c = '\b';
				} else if (c == tp->t_cc[VKILL] && flg&ECHOK) {
					if ((flg & ECHO) &&
					    (flg & ISPCI)) {
						for ( csp = "XXX"; (*csp);)
							ttxput(tp, (long) *csp++, 0);
						(*tp->t_proc)(tp, T_OUTPUT);
					}
					ttxput(tp, (long) c, 0);
					flg |= ECHO;
					c = '\n';
				} else if (c == tp->t_cc[VEOF]) {
					if ((flg & ECHO) &&
					    (flg & ISPCI)) {  /* muss bleiben */
						for ( csp = "(eof)\n"; (*csp);)
							ttxput(tp, (long) *csp++, 0);
						(*tp->t_proc)(tp, T_OUTPUT);
					}
					flg &= ~ECHO;
					tp->t_delct++;
				}
			} else {
				if (c != '\\' || (flg&XCASE))
					tp->t_state  &= ~ESC;
			}
		}
		if (flg&ECHO) {
			ttxput(tp, (long) c, 0);
			(*tp->t_proc)(tp, T_OUTPUT);
		}
	}}
	if (!(flg&ICANON)) {
		tp->t_state &= ~RTO;
		if (tp->t_rawq.c_cc >= tp->t_cc[VMIN])
			tp->t_delct = 1;
		else if (tp->t_cc[VTIME]) {
			if (!(tp->t_state&TACT))
				tttimeo(tp);
		}
	}
	if (tp->t_delct && (tp->t_state&IASLP)) {
		tp->t_state &= ~IASLP;
		wakeup(tp);
	}
}

/*
 * Put character(s) on TTY output queue, adding delays,
 * expanding tabs, and handling the CR/NL bit.
 * It is called both from the top half for output, and from
 * interrupt level for echoing.
 */
ttxput(tp, ucp, ncode)
register struct tty *tp;
union { 
	long ch;
	struct cblock *ptr;
	struct
	    {
		char l_l;
		char l_r;
		char r_l;
		char r_r;
	} 
	byte;
} ucp;
int ncode;
{
	register c;
	register flg;
	register unsigned char *cp;
	register char *colp;
	int ctype;
	register int cs;
	struct cblock *scf;

	flg = tp->t_oflag;
	if (tp->t_state&OEXTPROC)
		flg &= ~OPOST;
	if (ncode == 0) {
		if (tp->t_outq.c_cc >= TTYHOG)
			return;
		ncode++;
		if (!(flg&OPOST)) {
			sysinfo.outch++;
			putc(ucp.r_r, &tp->t_outq);
			return;
		}
		cp = (unsigned char *)&ucp.r_r;
		scf = NULL;
	} else {
		if (!(flg&OPOST)) {
			sysinfo.outch += ncode;
			putcb(ucp.ch, &tp->t_outq);
			return;
		}
		cp = (unsigned char *)&ucp.ptr->c_data[ucp.ptr->c_first];
		scf = ucp.ptr;
	}
	while (ncode--) {
		c = *cp++;
		if (c >= DELAYDEL){
			if (c == DELAYDEL)
				putc(DELAYDEL, &tp->t_outq);
			sysinfo.outch++;
			putc(c, &tp->t_outq);
			continue;
		} 
		/*
		 * Generate escapes for upper-case-only terminals.
		 */
		if (tp->t_lflag&XCASE)  {
			colp = "({)}!|^~'`\\";
			while(*colp++)
				if (c == *colp++) {
					tp->t_lflag &= ~XCASE;
					ttxput(tp, (long)'\\', 0);
					tp->t_lflag |= XCASE;
					c = colp[-2];
					break;
				}
			if ('A' <= c && c <= 'Z'){
				tp->t_lflag &= ~XCASE;
				ttxput(tp, (long)'\\', 0);
				tp->t_lflag |= XCASE;
			}
		}
		if (flg&OLCUC && 'a' <= c && c <= 'z')
			c += 'A' - 'a';
		cs = c;
		/*
		 * Calculate delays.
		 * The numbers here represent clock ticks
		 * and are not necessarily optimal for all terminals.
		 * The delays are indicated by characters above 0200.
		 */
		ctype = partab[c];
		colp = &tp->t_col;
		c = 0;
		switch (ctype&077) {
		case 0: /* ordinary */
			(*colp)++;

		case 1: /* non-printing */
			break;

		case 2: /* backspace */
			if (flg&BSDLY)
				c = 2;
			if (*colp)
				(*colp)--;
			break;
		case 3: /* line feed */
			if (flg&ONLRET)
				goto cr;
			if (flg&ONLCR) {
				if (!(flg&ONOCR && *colp==0)) {
					sysinfo.outch++;
					putc('\r', &tp->t_outq);
				}
				goto cr;
			}
		nl:
			if (flg&NLDLY)
				c = 5;
			break;
		case 4:	/* tab */
			c = 8 - ((*colp)&07);
			*colp += c;
			ctype = flg&TABDLY;
			if (ctype == TAB0) {
				c = 0;
			} else if (ctype == TAB1) {
				if (c < 5)
					c = 0;
			} else if (ctype == TAB2) {
				c = 2;
			} else if (ctype == TAB3) {
				sysinfo.outch += c;
				do
					putc(' ', &tp->t_outq);
				while (--c);
				continue;
			}
			break;

		case 5: /* vertical tab */
			if (flg&VTDLY)
				c = 0177;
			break;

		case 6: /* carriage return */
			if (flg&OCRNL) {
				cs = '\n';
				goto nl;
			}
			if (flg&ONOCR && *colp == 0)
				continue;
		cr:				
			ctype = flg&CRDLY;
			if (ctype == CR1) {
				if (*colp)
					c = max((*colp>>4) + 3, 6);
			} else if (ctype == CR2) {
				c = 5;
			} else if (ctype == CR3) {
				c = 9;
			}
			*colp = 0;
			break;
		case 7: /* form feed */
			if (flg&FFDLY)
				c = 0177;
			break;
		}
		sysinfo.outch++;
		putc(cs, &tp->t_outq);
		if (c) {
			if ((c < 32) && flg&OFILL) {
				if (flg&OFDEL)
					cs = 0177;
				else
					cs = 0;
				putc(cs, &tp->t_outq);
				if (c > 3)
					putc(cs, &tp->t_outq);
			} else {
				putc(DELAYDEL, &tp->t_outq);
				putc(c|DELAYDEL, &tp->t_outq);
			}
		}
	}
	if (scf != NULL)
		putcf(scf);
}


/*
 * Get next function from output queue.
 * Called from xmit interrupt complete.
 */
ttout(tp)
register struct tty *tp;
{
	return;
}

extern timeout();
tttimeo(tp)
register struct tty *tp;
{

	tp->t_state &= ~TACT;		/* timeout waiting on rawq */
	if (tp->t_lflag&ICANON)
		return;			/* enable canonicalization */
	if (tp->t_cc[VTIME] == 0)
		return;			/* time out */
	if (( tp->t_rawq.c_cc == 0) && (tp->t_cc[VMIN]))
		return;
	if (tp->t_state&RTO) {		/* timeout waiting on rawq */
		tp->t_delct = 1;
		if (tp->t_state&IASLP) {/* wakeup when input done */
			tp->t_state &= ~IASLP;
			wakeup(tp);
		}
	} else {
		tp->t_state |= RTO|TACT;
		timeout(tttimeo, tp, tp->t_cc[VTIME]*(HZ/10));
	}
}

ttioctl(tp, cmd, arg)
register struct tty *tp;
int cmd;
long arg;
{
	ushort chg;

	switch(cmd) {
	case LDCHG:
		chg = tp->t_lflag^arg;
		if (!(chg&ICANON))
			break;
		dvi();
		if (tp->t_canq.c_cc) {
			if (tp->t_rawq.c_cc) {
				tp->t_canq.c_cc += tp->t_rawq.c_cc;
				tp->t_canq.c_cl->c_next = tp->t_rawq.c_cf;
				tp->t_canq.c_cl = tp->t_rawq.c_cl;
			}
			tp->t_rawq = tp->t_canq;
			tp->t_canq = ttnulq;
		}
		tp->t_delct = tp->t_rawq.c_cc;
		evi();
		break;

	default:
		break;
	}
}
