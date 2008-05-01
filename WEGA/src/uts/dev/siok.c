/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
	KERN 3.2      Modul: siok.c

	Bearbeiter:	U. Wiesner, P. Hoge
	Datum:		27.10.89
	Version:	1.4

*******************************************************************************

	Terminal - Treiber fuer Schaltkreis Z80-SIO und Koppelschnittstelle

*******************************************************************************
******************************************************************************/

#include "sys/param.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/state.h"
#include "sys/mmu.h"
#include "sys/s.out.h"
#include "sys/user.h"
#include "sys/ioctl.h"
#include "sys/tty.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
#include "sys/file.h"

/*----------------------------------------------------------------*/
/* alle Variablen- und Konfiguriervereinbarungen befinden sich in der
 * Datei sio.h
 */
#include "sys/sioconst.h"
#include "sys/sio.h"

# define mca(addr)   (addr > 0xff00 ? addr | ctl : addr + 1)

extern ttrstrt();
extern int last_int_serv;
extern char *panicstr;
extern udos;

sioproc();
mdmchk();

/*----------------------------------------------------------------*/
/* sioopen eroeffnet einen seriellen Kanal entweder fuer normalen
 * Terminalbetrieb oder fuer Druckerbetrieb
 */
struct tty *
sioopen(dev, flag)
dev_t dev;
{
	register struct tty *tp;
	register unsigned d, caddr, baud, s;

	tp = &sio[(d = minor(dev))];
	caddr = sioaddrs[d].sio;
	caddr = mca(caddr);
	if ((tp->t_state & (ISOPEN|WOPEN|LP)) == 0) {
		if (ttchkopen()) {
			u.u_error = ENXIO;
			return(0);
		}
		tp->t_proc = sioproc;
		ttinit(tp);
		tp->t_cflag = (tp->t_cflag & ~CBAUD) | B9600|CLOCAL;
		if (sioset & ((long)1<<d))
			tp->t_cflag &= ~CLOCAL;
		sioparam(d);
	}
	s = dvi();
	tp->t_state &= ~CARR_ON;
	if ((tp->t_cflag & CLOCAL) || (sioinb(caddr) & dcd))
			tp->t_state |= CARR_ON;
	if ((flag & FNDELAY) == 0) {
		while ((tp->t_state & CARR_ON) == 0) {
			tp->t_state |= WOPEN;
			sleep(&tp->t_canq, TTIPRI);
		}
	}
	rvi(s);
	(*linesw[tp->t_line].l_open)(tp);
	return(tp);
}

/*----------------------------------------------------------------*/
sioclose(dev, flag)
dev_t dev;
{
	static char ditab[] = {res_xi|WR5,xmt8|enb_xmt,res_xi|WR1,stat_v};
	register struct tty *tp;
	register unsigned d, caddr;

	tp = &sio[(d = minor(dev))];
	caddr = sioaddrs[d].sio;
	caddr = mca(caddr);
	ttchkclose();
	(*linesw[tp->t_line].l_close)(tp);
	if (tp->t_cflag & HUPCL)
		siootirb(caddr, ditab, sizeof(ditab));
}

/*----------------------------------------------------------------*/
/* sioread und siowrite rufen die entsprechenden Handlerroutinen zur
 * Einzelzeichen-Ein- und -Ausgabe aus Anwenderprogrammen auf
 */
sioread(dev)
dev_t dev;
{
	struct tty *tp;

	tp = &sio[minor(dev)];
	(*linesw[tp->t_line].l_read)(tp);
}

/*----------------------------------------------------------------*/
siowrite(dev)
dev_t dev;
{
	struct tty *tp;

	tp = &sio[minor(dev)];
	(*linesw[tp->t_line].l_write)(tp);
}

/*----------------------------------------------------------------*/
/* Uebergabe des Kommandos zur Veraenderung der Terminalbehandlung
 * an den Handler (tty.c)
 * ueberprueft Initialisierung des SIO
 * ermoeglicht Veraenderung der Baudrate
 */
sioioctl(dev, cmd, addr, flag)
dev_t dev;
register cmd;
caddr_t addr;
{
	register tret;
	register struct tty *tp;
	long rsioset;

	tp = &sio[minor(dev)];
	if (cmd == TIOCMCG) {
		if (copyout(&sioset, addr, 4))
			u.u_error = EFAULT;
		return;
	}
	if (cmd == TIOCMCS) {
		if (copyin(addr, &rsioset, 4)) {
			u.u_error = EFAULT;
			return;
		}
		siomset(rsioset);
		return;
	}
	tret = ttiocom(tp, cmd, addr, flag);
	tp->t_cflag |= CLOCAL;
	if (sioset & (1L<<(tp-sio)))
		tp->t_cflag &= ~CLOCAL;
	if (tret)
		sioparam(minor(dev));
}

/*----------------------------------------------------------------*/
/* ISR Empfaengerinterrupt fuer SIO auf 16-Bit-Teil
 * Es werden alle Zeichen gelesen, die inzwischen eingelaufen sind
 */
siorint(state)
struct state *state;
{
	register struct tty *tp;
	register unsigned d, addr, caddr;
	register c;

	last_int_serv = SIORINT;
	d = (((minor(state->s_eventid) - siovec) / 8) ^1) + 4;
	tp = &sio[d];
	addr = sioaddrs[d].sio;
	caddr = addr | ctl;
	if (panicstr == 0) {
		while (inb(caddr) & rcv_rdy) {
			c = inb(addr);
			if (tp->t_state & (WOPEN|ISOPEN))
				(*linesw[tp->t_line].l_input)(tp, (long)c, 0);
		}
	}
	outb(caddr & ca, sreti);
}

/*----------------------------------------------------------------*/
/* rint: ISR fuer SIO auf 8-Bit-Seite
 * gepufferte Uebernahme der eingelaufenen Zeichen von einem Kanal
 */
rint(d, c)
dev_t d;
unsigned c;
{
	register struct tty *tp;

	tp = &sio[d];
	if (panicstr == 0 && (tp->t_state & (WOPEN|ISOPEN)))
		(*linesw[tp->t_line].l_input)(tp, (long)c, 0);
}

/*----------------------------------------------------------------*/
/* ISR Senderinterrupt
 */
sioxint(state)
register struct state *state;
{
	register unsigned d, caddr;

	last_int_serv = SIOXINT;
	d = (((minor(state->s_eventid) - siovec) / 8) ^1) + 4;
	caddr = sioaddrs[d].sio | ctl;
	outb(caddr, res_tx);
	xint(d);
}

/*----------------------------------------------------------------*/
/* xint: globale ISR fuer alle SIO
 * Hier wird die Ausgabe des naechsten Zeichens vorbereitet;
 * Ausgabe CSTART, CSTOP
 * Ausgabe des naechsten Zeichens in sioproc()
 */
xint(d)
register dev_t d;
{
	register unsigned addr, caddr;
	register struct tty *tp;

	tp = &sio[d];
	addr = sioaddrs[d].sio;
	caddr = mca(addr);
	if (caddr > 0xff00)
		outb(caddr & ca, sreti);
	if (panicstr)
		return;
	tp->t_state &= ~BUSY;
	sioxout(tp, addr);
}

/*----------------------------------------------------------------*/
/* ISR Spezielle Empfangsbedingungen fuer SIO auf 16-Bit-Teil
 */
siosint(state)
register struct state *state;
{
	register unsigned d, caddr;

	last_int_serv = SIOSINT;
	d = (((minor(state->s_eventid) - siovec) / 8) ^1) + 4;
	caddr = sioaddrs[d].sio | ctl;
	outb(caddr, res_xi|WR1);
	sint(d, inb(caddr));
}

/*----------------------------------------------------------------*/
/* sint: globale ISR Spezielle Empfangsbedingungen
 * Tritt auf bei Empfaengerueberlauf, Parity-Error oder
 * Framing-Error
 */
sint(d, rdr1)
register dev_t d;
register unsigned rdr1;
{
	register struct tty *tp;
	register unsigned addr;
	register unsigned c;

	tp = &sio[d];
	addr = sioaddrs[d].sio;
	c = sioinb(addr);
	if (rdr1 & ovr_err)
		c |= OVERRUN;
	if (rdr1 & par_err)
		c |= PERROR;
	if (rdr1 & fram_err)
		c |= FRERROR;
	(*linesw[tp->t_line].l_input)(tp, (long)c, 0);
	if (addr > 0xff00) {
		addr = mca(addr);
		outb(addr & ca, sreti);
		outb(addr, res_err);
	}
}

/*----------------------------------------------------------------*/
/* ISR External/Status Interrupt fuer SIO auf 16-Bit-Teil
 * muendet in eint (globale Routine fuer alle Kanaele)
 */
sioeint(state)
register struct state *state;
{
	register unsigned d, caddr;

	last_int_serv = SIOEINT;
	d = (((minor(state->s_eventid) - siovec) / 8) ^1) + 4;
	caddr = sioaddrs[d].sio | ctl;
	outb(caddr, res_xi);
	eint(d, inb(caddr));
}

/*----------------------------------------------------------------*/
/* eint: globale ISR fuer External/Status-Interrupt
 * Tritt bei einer Veraenderung des Pegels am DCD-Pin des SIO auf.
 * Signalisiert das Ein- bzw. Ausschalten eines Terminals.
 * (BREAK wird behandelt)
 */
eint(d, rdr0)
register dev_t d;
register unsigned rdr0;
{
	static char tabeint[] = {WR1,enb_rxp|enb_rx1|stat_v|enb_tx};
	register struct tty *tp;
	register unsigned addr, caddr;

	tp = &sio[d];
	addr = sioaddrs[d].sio;
	caddr = mca(addr);
	if (rdr0 & siobreak)				/* BREAK-Anfang */
		brkflag |= (long)1<<d;
	else {						/* BREAK-Ende */
		if (brkflag & ((long)1<<d)) {
			sioinb(addr);			/* Blindeingabe */
			brkflag &= ~((long)1<<d);
			/* FRERROR - framimg error */
			(*linesw[tp->t_line].l_input)(tp, 0L|FRERROR, 0);
		}
	}
	if ((tp->t_cflag & CLOCAL) == 0) {
		if ((rdr0 & dcd) ^ (tp->t_state & CARR_ON)) {
			siootirb(caddr, tabeint, sizeof(tabeint));
			if (addr > 0xff00)
				outb(caddr & ca, sreti);
			timeout(mdmchk, tp, 3);
			return;
		}
		if (rdr0 & cts)
			tp->t_state |= CLR_TS;
		else
			tp->t_state &= ~CLR_TS;
	}
	if ((tp->t_state & LP) && (rdr0 & ifss) == 0) {
		if (rdr0 & dcd)
			sioproc(tp, T_RESUME);	/* restart output */
		else
			sioproc(tp, T_SUSPEND);	/* stop output */
	}
	if (addr > 0xff00)
		outb(caddr & ca, sreti);
}

/*----------------------------------------------------------------*/
/* sioproc: Allgemeine SIO-Behandlungsroutine
 */
sioproc(tp, cmd)
register struct tty *tp;
{
	static char ptab1[] = {res_xi|WR5,0};
	static char ptab2[] = {res_xi|WR5,dtr|xmt_break|enb_xmt|rts|xmt8};
	register unsigned addr, caddr, s;

	addr = sioaddrs[tp-sio].sio;
	caddr = mca(addr);
	switch (cmd) {
	case T_TIME:
		switch (tp->t_cflag & CSIZE) {
			case CS5: s = xmt5|dtr|enb_xmt|rts; break;
			case CS6: s = xmt6|dtr|enb_xmt|rts; break;
			case CS7: s = xmt7|dtr|enb_xmt|rts; break;
			case CS8: s = xmt8|dtr|enb_xmt|rts; break;
		}
		tp->t_state &= ~TIMEOUT;
		ptab1[1] = s;
		siootirb(caddr, ptab1, sizeof(ptab1));
		goto start;

	case T_RESUME: 
	case T_WFLUSH:
		if ((tp->t_state & LP) && (sioinb(caddr)&(dcd|ifss))==0)
			return;
		tp->t_state &= ~TTSTOP;

	case T_OUTPUT:
start:
		if (tp->t_state & BUSY)
			return;
		s = dvi();
		sioxout(tp, addr);
		rvi(s);
		return;

	case T_SUSPEND:
		tp->t_state |= TTSTOP;
		return;

	case T_BLOCK:
		tp->t_state &= ~TTXON;
		tp->t_state |= (TTXOFF | TBLOCK);
		sioproc(tp, T_OUTPUT);
		return;

	case T_RFLUSH:
		if ((tp->t_state & TBLOCK) == 0)
			return;

	case T_UNBLOCK:
		tp->t_state &= ~(TTXOFF | TBLOCK);
		tp->t_state |= TTXON;
		sioproc(tp, T_OUTPUT);
		return;

	case T_BREAK:
		siootirb(caddr, ptab2, sizeof(ptab2));
		tp->t_state |= TIMEOUT;
		timeout(ttrstrt, tp, 15);
		return;
	}
}

/* wurde aus sioproc herausgeloest */
sioxout(tp, addr)
register struct tty *tp;
register unsigned addr;
{
	register c;

	if (tp->t_state & TTXOFF) {
		tp->t_state |= BUSY;
		siooutb(addr, CSTOP);
		tp->t_state &= ~TTXOFF;
		return;
	}
	if (tp->t_state & TTXON) {
		tp->t_state |= BUSY;
		siooutb(addr, CSTART);
		tp->t_state &= ~TTXON;
		return;
	}
	if (tp->t_state & (TTSTOP|TIMEOUT))
		return;
	if ((tp->t_state & TTIOW) && (tp->t_outq.c_cc == 0)) {
			tp->t_state &= ~TTIOW;
			wakeup(&tp->t_oflag);
	}
	if ((c = getc(&tp->t_outq)) >= 0) {
		if ((tp->t_oflag & OPOST) && c==DELAYDEL) {
			if ((c = getc(&tp->t_outq)) < 0)
				return;
			if (c > DELAYDEL) {
				tp->t_state |= TIMEOUT;
				timeout(ttrstrt, tp, c&0x7f);
				return;
			}
		}
		/* Ausgabe des Zeichens */
		tp->t_state |= BUSY;
		siooutb(addr, c);
	}
	if ((tp->t_state & OASLP) && tp->t_outq.c_cc <=
			ttlowat[tp->t_cflag & CBAUD]) {
		tp->t_state &= ~OASLP;
		wakeup(&tp->t_outq);
	}
}

/*----------------------------------------------------------------*/
/* sioparam: Initialisierung eines Terminalkanals
 */
sioparam(d)
register dev_t d;		/* minor dev number */
{
	static char itab1[] = {res_xi|WR5,xmt8|enb_xmt,res_xi|WR1,stat_v};
	register unsigned caddr;
	register struct tty *tp;
	register unsigned s, cflag;
	char rsiotab[11];

	tp = &sio[d];
	caddr = sioaddrs[d].sio;
	caddr = mca(caddr);
	cflag = tp->t_cflag;
	if ((cflag & CBAUD) == 0) {	/* hang up */
		siootirb(caddr, itab1, sizeof(itab1));
		if ((tp->t_state & CARR_ON) == 0)
			return;
		if (tp->t_state & ISOPEN) {
			signal (tp->t_pgrp, SIGHUP);
			ttyflush(tp, T_RESUME);
		}
		tp->t_state &= ~CARR_ON;
		return;
	}
	bcopy(siotab, rsiotab, 11);
	s = rsiotab[1];
	if (cflag & PARENB)
		s |= parity;
	if ((cflag & PARODD) == 0)
		s |= even;
	if (cflag & CSTOPB)
		s |= stop2;
	else
		s |= stop1;
	rsiotab[1] = s;
	s = rsiotab[3];
	if (cflag & CREAD)
		s |= enb_rcv;
	switch (cflag & CSIZE) {
	case CS5: break;
	case CS6:
		s |= rcv6;
		rsiotab[5] |= xmt6;
		break;

	case CS7:
		s |= rcv7;
		rsiotab[5] |= xmt7;
		break;

	case CS8:
		s |= rcv8;
		rsiotab[5] |= xmt8;
		break;

	}
	rsiotab[3] = s;
	/*
	 * Interruptvektor eintragen
	 */
	if (caddr > 0xff00) {
		s = (d-4) >> 1;
		rsiotab[7] = siovec + (s << 4);
	} else {
		/* Int.vector nur Kanal B lesbar  */
		rsiotab[7] = koutinb(caddr|3, WR2) & 0xf0;
	}
	if (cflag & CLOCAL)
		sioset &= ~(1L<<d);
	else
		sioset |= (1L<<d);
	if ((cflag & CLOCAL) && (tp->t_iflag & IGNBRK))
		rsiotab[9] |= enb_rxp|enb_rx1|stat_v|enb_tx;
	else
		rsiotab[9] |= enb_rxp|enb_rx1|stat_v|enb_tx|enb_xi;
	s = dvi();
	tp->t_state &= ~BUSY;
	siootirb(caddr, rsiotab, 11);
	rvi(s);
	if ((d & 1) == 0) {
		tp++;
		if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
			rsiotab[9] = stat_v;
			rsiotab[5] &= ~(dtr|rts);
			caddr |= 2;	/* Kanal B fuer 8+16 Bit-SIO */
			s = dvi();
			siooutb(caddr, res_ch);
			siootirb(caddr, rsiotab, 11);
			rvi(s);
		}
	}
	sioctcinit(d);
}

/*----------------------------------------------------------------*/
/* sioctcinit: Einstellen der Baudrate
 */
sioctcinit(d)
register dev_t d;	/* minor dev number */
{
	static char btab[] = {BAUD, 2};
	register unsigned addr;
	register struct tty *tp;

	addr = sioaddrs[d].ctc;
	tp = &sio[d];
	btab[1] = siospeed[tp->t_cflag & CBAUD];
	siootirb(addr, btab, sizeof(btab));
}

/*----------------------------------------------------------------*/
/* siomset: Modem-Mode setzen und aufheben
 */
siomset(set)
long set;
{
	static char stab[] = {WR1,enb_rxp|enb_rx1|stat_v|enb_tx};
	register struct tty *tp;
	register unsigned d, caddr, modem;
	register long devmask;		/* dev-mask in sioset */

	for (d = 0; d < nsiochan; d++) {
		tp = &sio[d];
		caddr = sioaddrs[d].sio;
		caddr = mca(caddr);
		devmask = 1L << d;
		if (((sioset ^ set) & devmask) == 0)
			continue;		/* keine Aenderungen */
		modem = (set & devmask) ? 1 : 0 ;
		if (modem) {
			sioset |= devmask;
			tp->t_cflag &= ~CLOCAL;
		} else {
			sioset &= ~devmask;
			tp->t_cflag |= CLOCAL;
		}
		if ((tp->t_state & (ISOPEN|WOPEN)) == 0)
			continue;		/* kein OPEN gewuenscht */
		siooutb(caddr, res_xi);
		if (modem == 0 && (tp->t_iflag & IGNBRK))
			stab[1] = enb_rxp|enb_rx1|stat_v|enb_tx;
		else
			stab[1] = enb_rxp|enb_rx1|stat_v|enb_tx|enb_xi;
		siootirb(caddr, stab, sizeof(stab));
		if ((tp->t_state & WOPEN) && modem == 0) {
			tp->t_state |= CARR_ON;
			wakeup(&tp->t_canq);
		} else {
			if ((tp->t_state & ISOPEN) && modem)
				mdmchk(tp);
		}
	}
}

/*----------------------------------------------------------------*/
/* putchar: Einzelzeichenausgabe aus dem Kern heraus
 *	    Nur auf die Systemconsole!
 */
char pctab[] = {res_xi|WR1,stat_v};

putchar(c)
register c;
{
	static char pctab2[] = {res_xi|WR1,enb_rxp|enb_rx1|stat_v|enb_tx};
	register unsigned addr, caddr;
	int s;
	register timo;
	register struct tty *tp;

	if (udos == 0)		/* kein 8-bit-Teil */
		console = 5;	/* alternative System-Console */
	s = dvi();
	tp = &sio[console];
	addr = sioaddrs[console].sio;	
	caddr = mca(addr);
	siootirb(caddr, pctab, sizeof(pctab));

	/*
	 * warten, bis die Console frei ist (falls nicht, wird jedoch
	 * nach einer definierten Zeit weitergemacht)
	 */
	timo = 30000;
	while ((sioinb(caddr) & xmt_rdy) == 0)
		if (timo-- == 0)
			break;

	if (c == '\0') {
	  if (tp->t_state & (WOPEN|ISOPEN)) {
		if ((tp->t_cflag & CLOCAL) && (tp->t_iflag & IGNBRK))
			pctab2[1] = enb_rxp|enb_rx1|stat_v|enb_tx;
		else
			pctab2[1] = enb_rxp|enb_rx1|stat_v|enb_tx|enb_xi;
		siootirb(caddr, pctab2, sizeof(pctab2));
	  }
	  return;
	}

	*msgbufp++ = c;
	if (msgbufp >= &msgbuf[MSGBUFS])
		msgbufp = msgbuf;
	siooutb(addr, c);
	if (c == '\n')
		putchar('\r');
	putchar('\0');
	rvi(s);
}

/*----------------------------------------------------------------*/
/* getchar: Eingabe eines Zeichens von der Systemconsole
 */
getchar()
{
	register struct tty *tp;
	register unsigned addr, caddr, s;
	register c;

	s = dvi();
	tp = &sio[console];
	addr = sioaddrs[console].sio;
	caddr = mca(addr);
	putchar('?');
	putchar('\b');
	siootirb(caddr, pctab, sizeof(pctab));
	while ((sioinb(caddr)&rcv_rdy) == 0)
		;
	c = sioinb(addr);
	putchar(c);
	rvi(s);
	return(c & 0x7f);
}

/*----------------------------------------------------------------*/
/* mdmchk:
 */
mdmchk(tp)
register struct tty *tp;
{
	static char mstab[] = {res_xi|WR1,enb_rxp|enb_rx1|stat_v|enb_tx|enb_xi};
	register unsigned caddr;

	caddr = sioaddrs[tp-sio].sio;
	caddr = mca(caddr);
	if (sioinb(caddr) & dcd) {
		if ((tp->t_state & CARR_ON) == 0) {
			tp->t_state |= CARR_ON;
			wakeup(&tp->t_canq);
		}
	} else {
		if (tp->t_state & CARR_ON) {
			if (tp->t_state & ISOPEN) {
				signal(tp->t_pgrp, SIGHUP);
				ttyflush(tp, T_RESUME);
			}
			tp->t_state &= ~CARR_ON;
		}
	}
	siootirb(caddr, mstab, sizeof(mstab));
	if (caddr > 0xff00)
		outb(caddr & ca, sreti);
}
