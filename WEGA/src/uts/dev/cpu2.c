/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	KERN 3.2	Modul : cpu2.c
 
	Bearbeiter	: P. Hoge
	Datum		: 2.8.88
	Version		: 1.4
 
***************************************************************************
**************************************************************************/

/*-----------------------------------------------------------------------*/
/*
 * Koppelprogramm fuer die Erweiterungskarten U80601
 */
/*-----------------------------------------------------------------------*/

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/iobuf.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/conf.h>
#include <sys/s.out.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>

#define CPU2PORT	0xff1f	/* PORT-ADRESSE */
#define RESET		0xfeff
#define RUN		0xffff
# define RAMSTART	0x7000	/* obere 16-Bit der 24-Bit Adresse */
# define RAMEND		0x8000 	/* 1 MByte */
#define CPU2RADR	0x7fd0	/* physische RAM-Adresse */
#define CPU2ADDR	(struct cpu2_connect *)(CPU2SEG0<<24)

#define CPU2PRI		30
#define NDEV		4	/* Anzahl der Minor-Geraete */

extern last_int_serv;
extern cpu2_flag;
extern unsigned mmut;
extern console;

unsigned cpu2_openflag	= 0;
unsigned cpu2_error	= 0;
unsigned cpu2_clock	= 0;
unsigned cpu2_nwaitint	= 0;
struct proc *cpu2_pproc;
unsigned cpu2_sleepflag[NDEV];
char cpu2_waitint[NDEV];

struct cpu2_inchar {
	char	vector;
	char	instop;
	char	inbyte;
};
struct cpu2_outchar {
	char	outstop;
	int	noutb;
};
struct cpu2_date {
	char	sec;
	char	min;
	char	hour;
	char	day;
	char	mon;
	char	year;
};
struct cpu2_connect {
	char	reset;		/* Reset */
	char	intin;		/* Richtung zu Erweiterungskarten */
	struct cpu2_inchar inchar[NDEV];
	char	intout;		/* Richtung von Erweiterungskarten */
	struct cpu2_outchar outchar[NDEV];
	char	hd0_typ;
	char	hd1_typ;
	struct cpu2_date date;
	char	ttybuf[0x800];
	char	lpbuf[0x100];
	char	diskrq[0x10];
};

struct segd cpu2mmuout = {0, 0xff, 0};

/*
 * Open-Routine
 */
cpu2open(dev, flag)
dev_t dev;
unsigned flag;
{
	static run = 0;
	static raminit = 0;

	if (raminit == 0) {
		raminit++;
		cpu2initram(1);		/* cpu2 RAM-Test */
	}
	if (cpu2_error) {
		u.u_error = ENXIO;
		return;
	}
	if (cpu2_openflag++ == 0) {
		cpu2_pproc = u.u_procp;
		if ((flag & 4) == 0 && run) {
			out(CPU2PORT, RESET);	/* Reset Prozessor */
			run = 0;
			cpu2initram(0);		/* cpu2 RAM mit 0 init */
		}
		return;
	}
	if (cpu2_pproc != u.u_procp) {
		u.u_error = ENXIO;
		return;
	}
	if (cpu2_openflag == 4) {
		/* Programmierung MMU-Segment fuer Kopplung */
		cpu2mmuout.sg_base = CPU2RADR;
		cpu2mmuout.sg_limit = 0x11;
		loadsd(mmut, CPU2SEG0, &cpu2mmuout);
		cpu2mmuout.sg_limit = 0xff;
		if (run == 0) {
			out(CPU2PORT, RUN);	/* Start Prozessor */
			run = 1;
		}
	}
}

/*
 * Close-Routine
 */
cpu2close()
{
	cpu2_openflag = 0;
	cpu2_pproc = NULL;
}

/*
 * Read Routine
 */
cpu2read(dev)
register dev_t dev;
{
	register unsigned mdev;
	register struct cpu2_outchar *op;
	register struct cpu2_connect *cp;

	if ((mdev = minor(dev)) >= NDEV) {
		u.u_error = ENODEV;
		return;
	}
	if (mdev == 0) {			/* RAM-Raw */
		cpu2ramcp(B_READ);
		return;
	}
	cp = CPU2ADDR;
	op = &cp->outchar[mdev];
	dvi();
	while (op->noutb == 0) {
		/* Sleep bis Zeichen eingelaufen */
		cpu2_sleepflag[mdev] = 1;
		sleep(&cpu2_sleepflag[mdev], CPU2PRI);
	}
	op->outstop = 1;	/* CPU2 darf keine weiteren Zeichen eintragen */
	evi();
	/* Uebergabe der Zeichen zum User */
	if (mdev == 1)
		copyout(cp->ttybuf, u.u_base, op->noutb);
	else if (mdev == 2)
		copyout(cp->lpbuf, u.u_base, op->noutb);
	else if (mdev == 3)
		copyout(cp->diskrq, u.u_base, op->noutb);
	u.u_count -= op->noutb;
	op->noutb = 0;
	op->outstop = 0;	/* CPU2 darf wieder Zeichen eintragen */
}

/* 
 * Interrupt Routine
 */
cpu2int()
{
	register unsigned mdev;
	struct cpu2_connect *cp = CPU2ADDR;

	last_int_serv = CPU2INT;
	mdev = cp->intout;
	cp->intout = 0;			/* CPU2 kann weiteren Int geben */
	if (mdev == 0 || mdev >= NDEV)
		return;			/* invalid int or out instruction */

	/* Prozesse wecken, die auf Eingabezeichen warten */
	if (cpu2_sleepflag[mdev]) {
		cpu2_sleepflag[mdev] = 0;
		wakeup(&cpu2_sleepflag[mdev]); 
	}
}

/*
 * Write Routine
 */
cpu2write(dev)
register dev_t dev;
{
	register c;
	register unsigned mdev;

	switch (mdev = minor(dev)) {
	case 0:				/* RAM-Raw */
		cpu2ramcp(B_WRITE);
		break;
	case 1:				/* Terminal */
	case 3:				/* Disk */
		while (u.u_count) {
			/* Zeichen vom User-Prozess holen */
			if ((c = cpass()) < 0)
				break;
			cpu2out(c, mdev);
		}
		break;
			
	default:
		u.u_error = ENODEV;
	}
}

/*
 * Clock fuer Erweiterungskarten
 */
cpu2clk()
{
	struct cpu2_connect *cp;

	if (cpu2_openflag > 3) {		/* Clock durchschalten */
		if (cpu2_nwaitint) {
			cp = CPU2ADDR;
			if (cp->intin == 0) {
				cp->intin = 1;
				cpu2_nwaitint--;
				out(CPU2PORT, cpu2_waitint[cpu2_nwaitint] |0xff00);
			}
		}
		if (++cpu2_clock == 6) {	/* 10Hz Clock an CPU2 */
			cpu2_clock = 0;
			cpu2out(1, 2);	/* Minor-Nummer 2 */
		}
	}
}

/*
 * Ausgabe an CPU2-Karte
 */
cpu2out(c, mdev)
register unsigned c;
register unsigned mdev;
{
	register struct cpu2_connect *cp = CPU2ADDR;
	register struct cpu2_inchar *ip;
	unsigned s;

	ip = &cp->inchar[mdev];
	if (ip->instop == 0) {	/* wenn != 0 gehen die Zeichen verloren */
		ip->inbyte = c;
		ip->instop = 1;	/* 1 Zeichen da */
		if (ip->vector) {/* wenn Vektor != 0 Interrupt ausloesen */
			s = dvi();
			if (cp->intin == 0) {
				cp->intin = 1;
				out(CPU2PORT, ip->vector | 0xff00);
			} else {
				cpu2_waitint[cpu2_nwaitint] = ip->vector;
				cpu2_nwaitint++;
			}
			evi(s);
		}
	}
}


/*
 * Initialisierung des RAM
 */
cpu2initram(flag)
int flag;
{
	register unsigned raddr;

	for (raddr = RAMSTART; raddr < RAMEND; raddr += 0x0100) {
		cpu2mmuout.sg_base = raddr;
		loadsd(mmut, CPU2SEG1, &cpu2mmuout);
		if ((cpu2_error = ramtest(CPU2SEG1<<8, flag)) != 0)
			return;
	}
}

/*
 * RAM-Block-Treiber
 */
cpu2strategy(bp)
register struct buf *bp;
{
	register unsigned raddr;

	/* RAM-Adresse ermitteln */
	raddr = (int)(bp->b_blkno) * (BSIZE/CPAS);

	/* Ueberpruefung der Parameter */
	if (raddr + ((bp->b_bcount + CPAS-1) / CPAS) >= RAMEND) {
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		iodone(bp);
		return;
	}
	/* Programmierung MMU-Segmente fuer Uebertragung */
	/* RAM-Disk */
	dvi();
	cpu2mmuout.sg_base = RAMSTART + raddr;
	loadsd(mmut, CPU2SEG1, &cpu2mmuout);
	/* Puffer / User-Adresse */
	cpu2mmuout.sg_base = (int)(bp->b_paddr >>8);
	loadsd(mmut, CPU2SEG2, &cpu2mmuout);

	if (bp->b_flags & B_READ)
	  /* RAM-Block zur physischen Adresse kopieren */
	  bcopy(0,CPU2SEG1<<8,((int)bp->b_paddr)&0xff,CPU2SEG2<<8,bp->b_bcount);
	else
	  /* von physischer Adresse zum RAM-Block kopieren */
	  bcopy(((int)bp->b_paddr)&0xff,CPU2SEG2<<8,0,CPU2SEG1<<8,bp->b_bcount);

	evi();
	bp->b_resid = 0;
	bp->b_error = 0;
	iodone(bp);
}

/*
 * Raw-Treiber fuer RAM der Erweiterungskarten
 */
cpu2ramcp(flag)
register flag;
{
	register unsigned raddr;

	/* RAM-Adresse ermitteln */
	raddr = u.u_offset / CPAS;

	/* Ueberpruefung der Parameter */
	if (raddr + ((u.u_count + CPAS-1) / CPAS) >= RAMEND) {
		u.u_error = ENXIO;
		return;
	}
	/* Programmierung MMU-Segment fuer Uebertragung */
	/* RAM-Disk */
	dvi();
	cpu2mmuout.sg_base = RAMSTART + raddr;
	loadsd(mmut, CPU2SEG1, &cpu2mmuout);

	if (flag & B_READ)
	  /* RAM-Block zur logischen Adresse kopieren */
	  copyout((int)(u.u_offset) & 0xff, CPU2SEG1<<8, u.u_base, u.u_count);
	else
	  /* von logischer Adresse zum RAM-Block kopieren */
	  copyin(u.u_base, (int)(u.u_offset) & 0xff, CPU2SEG1<<8, u.u_count);

	evi();
	u.u_count = 0;
}
