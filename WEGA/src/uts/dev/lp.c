/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1987
 
	KERN 3.2	Modul:	lp.c
 
	Bearbeiter:	P. Hoge
	Datum:		13.2.89
	Version:	1.3
 
*******************************************************************************
******************************************************************************/
 
 
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/tty.h>
#include <sys/ioctl.h>
#include <sys/user.h>

#define LPOUT ESC	/* ESC-Bit siehe tty.h */

extern struct tty sio[];
extern struct tty * sioopen();
 
struct lprint
{
	struct lparms max;
	int curline;
	int curcol;
	int raw;
	int indflg;
};
 
struct lprint lp1_dt = {66, 128, 0, 0, 0, 0, 0};
struct lprint lp2_dt = {66, 128, 0, 0, 0, 0, 0};
 
/*---------------------------------------------------------------------------*/
/*
 *lpopen
 */
/*---------------------------------------------------------------------------*/
lpopen(dev)
register dev_t dev;
{
	register struct tty *tp;

	if ((tp = sioopen(dev & 0xff1f, -1)) == 0)
		return;

/* Verriegeln des Treibers fuer weiteres Open */
/*	tp->t_state |= XCLUSE; */

	if ((tp->t_state & LP) == 0) {
		tp->t_state |= LP;
		tp->t_cflag &= ~HUPCL;
		tp->t_lflag = 0;
		tp->t_iflag = IGNPAR|ISTRIP|IXON;
	}
	tp->t_oflag = 0;
	sioparam(dev & 0x1f);
}
 
/*---------------------------------------------------------------------------*/
/*
 *lpclose
 */
/*---------------------------------------------------------------------------*/
lpclose(dev)
register dev_t dev;
{
	register struct tty *tp;

	tp = &sio[dev & 0x1f];
	if (tp->t_state & LPOUT) {
		if ((dev & 0x40) == 0)
			lpoutput(0x80, tp, dev);
		lpoutput(0x0c, tp, dev);
	}
	sioclose(dev & 0x1f);
	dvi();
	tp->t_state &= ~(XCLUSE | LPOUT);
	evi();
}
 
/*---------------------------------------------------------------------------*/
/*
 *lpwrite
 */
/*---------------------------------------------------------------------------*/
lpwrite(dev)
register dev_t dev;
{
	register struct tty *tp;
	register c;

	tp = &sio[dev & 0x1f];
	while (u.u_count) {
		dvi();
		tp->t_state |= LPOUT;
		while (tp->t_outq.c_cc > tthiwat[tp->t_cflag & CBAUD]) {
			sioproc(tp, T_OUTPUT);
			tp->t_state |= OASLP;
			sleep(&tp->t_outq, TTOPRI);
		}
		evi();
		if ((c = cpass()) < 0)
			break;
		lpoutput(c, tp, dev);
	}
	dvi();
	sioproc(tp, T_OUTPUT);
	evi();
}
 
/*---------------------------------------------------------------------------*/
/*
 *lpoutput
 * 1B hex Einschalten Raw-Mode
 * 80 hex Ausschalten Raw-Mode
 */
/*---------------------------------------------------------------------------*/
lpoutput(c, tp, dev)
register c;
struct tty *tp;
dev_t dev;
{
	register n;
	register struct lprint *plp;
	register struct clist *poutq = &tp->t_outq;
 
	if (dev & 0x40) {	/* permanenter raw-Mode */
		putc(c, poutq);
		return;
	}
	if (dev & 0x0080)
		plp = &lp2_dt;
	else
		plp = &lp1_dt;

	if (c==0x80) {		/* reset lp raw mode */
		plp->raw = 0;
		return;
	}
	if (plp->raw) {
		putc(c, poutq);
		return;
	}
	if (dev & 0x20)		/* keine autom. Umschaltung auf Raw-Mode */
		goto noraw;

	if (c==0x1b) {		/* set lp raw mode */ 
		plp->raw = 1;
		putc(c, poutq);
		return;
	}
noraw: 
	if (c == '\n') {
		lpoutput('\r', tp, dev);
		plp->curline++;
		if (plp->max.lines && plp->curline >= plp->max.lines)
			c = 0x0c;
	}
	if (c == 0x0c) {
		if (plp->curline == 0 && plp->curcol == 0)
			return;
		plp->curline = 0;
		putc(c, poutq);
		c = '\r';
	}
	if (c == '\r') {
		plp->indflg = 0;
		plp->curcol = 0;
		putc(c, poutq);
		return;
	}
	if (c == '\t') {
		do
			lpoutput(' ', tp, dev);
		while (plp->curcol & 07);
		return;
	}
	if (c == 0x08) {
		if (plp->curcol) {
			plp->curcol--;
			putc(c, poutq);
		}
		return;
	}
	if (c == '\n' || c == 0x07) {
		putc(c, poutq);
		return;
	}
	if (plp->indflg == 0) {
		plp->indflg++;
		for (n = plp->max.indent; n > 0; n--)
			putc(' ', poutq);
	}
	plp->curcol++;
	if (plp->max.cols == 0 || plp->curcol <= plp->max.cols+plp->max.indent)
		putc(c, poutq);
}
 
/*---------------------------------------------------------------------------*/
/*
 *lpioctl
 * Lesen und Setzen der lparms Struktur
 * und Aufruf ttiocco (tty.c)
 */
/*---------------------------------------------------------------------------*/
lpioctl(dev, cmd, addr, flag)
register dev_t dev;
register cmd;
register caddr_t addr;
int flag;
{
	register struct lparms *plp;
 
	if ((dev & 0x0080) == 0)
		plp = &lp1_dt.max;
	else
		plp = &lp2_dt.max;
 
	if (cmd == SHOWLPR) {
		if ((copyout(plp, addr, sizeof(struct lparms))) < 0)
			u.u_error = EFAULT;
		return;
	}
 
	if (cmd == SETLPR) {
		if ((copyin(addr, plp, sizeof(struct lparms))) < 0)
			u.u_error = EFAULT;
		if (plp->cols <= plp->indent)
			plp->indent = 0;
		return;
	}
	sioioctl((dev & 0xff1f), cmd, addr, flag);
}
