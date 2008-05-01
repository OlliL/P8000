/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1987
	KERN 3.2       Modul: prf.c

	Bearbeiter:	N. Taege, P. Hoge
	Datum:		9.8.88
	Version:	1.2

*******************************************************************************
******************************************************************************/


#include <sys/param.h>
#include <sys/state.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/buf.h>

#define sreg state->s_reg
#define eventid state->s_eventid
#define ps state->s_ps
#define pcseg state->s_pcseg
#define pc state->s_pc

char *panicstr;


/*---------------------------------------------------------------------------*/
/*
 *lswap
 * Vertauschen low und high von long integer
 */
/*---------------------------------------------------------------------------*/
unsigned long lswap(l)
unsigned long l;
{
	return(l<<16 | l>>16);
}

/*---------------------------------------------------------------------------*/
/*
 *printfv
 * Abgeruestete Version des C-Library printf.
 * Realisiert nur die Ausgaben %s %u %d (==%u) %o %x %D %X
 * Ausgabe ist zur Konsole gerichtet.
 * Sollte nicht fuer "chit-chat" benutzt werden.
 *
 * Der eigentliche Entry Point muss in Assembler geschrieben sein
 * und befindet sich in mch.s
 */
/*---------------------------------------------------------------------------*/
printfv(fmt, x1)
register char *fmt;
register unsigned *x1;
{
	register c;
	register unsigned *adx;
	register char *s;

	adx = x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		for(; adx-x1 < 4; adx++) {
			if((adx-x1)%2 == 1)
				continue;
			s=(char *)(lswap(*(long *)adx) & 0x7F00FFFF);
			goto end1;
		}
		s=(char *)(*(long *)adx & 0x7F00FFFF);
		end1:
		adx++;
		while(c = *s++)
			putchar(c);
	} else if(c == 'D' || c == 'X' ) {
		for(; adx-x1 < 4; adx++) {
			if((adx-x1)%2 == 1) continue;
			printn(lswap(*(long *)adx), c=='D' ? 10 : 16);
			goto end2;
		}
		printn(*(long *)adx, c=='D' ? 10 : 16 );
		end2:
		adx += (sizeof(long) / sizeof(int)) - 1;
	} else if(c == 'C' || c == 'c')
		putchar(*(s=(char *)*adx));
	adx++;
	goto loop;
}


/*---------------------------------------------------------------------------*/
/*
 *printn
 * gibt ein long zur Basis b an die Konsole aus
 */
/*---------------------------------------------------------------------------*/
printn(n, b)
register long n;
register b;
{
	register long a;

	if (n < 0 && b != 16) {
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}


/*---------------------------------------------------------------------------*/
/*
 * panic
 * Realisiert die Reaktion auf fatale Fehler
 */
/*---------------------------------------------------------------------------*/
panic(s)
char *s;
{
	panicstr = s;
	evi();
	update();
	printf("\007panic: %s\n", s);
	sc(1);
	for(;;)
		idle();
}


/*---------------------------------------------------------------------------*/
/*
 * prdev
 * Gibt auf die Konsole die Nachricht "s on dev x/y"
 * Wobei s eine Zeichenkette (i.A. eine Warnung) und
 * x und y die major- und minor-Nummer des Devices sind.
 */
/*---------------------------------------------------------------------------*/
prdev(str, dev)
char *str;
dev_t dev;
{

	printf("%s on dev %u/%u\n", str, major(dev), minor(dev));
}


/*---------------------------------------------------------------------------*/
/*
 * deverror
 * Gibt eine Diagnose eines fehlerhaft angesteuerten Device-Treibers aus
 * "err on dev x/y
 *  bn=d er=o1,o2"
 * Wobei d die dezimale Blocknummer (long) des Devices
 * und o1 und o2 die hexadezimalen Werte zweier
 * Fehlerstatus-Register sind.
 */
/*---------------------------------------------------------------------------*/
deverror(bp, o1, o2)
struct buf *bp;
{

	prdev("\007err", bp->b_dev);
	printf("bn=%D er=%x,%x\n", bp->b_blkno, o1, o2);
}


/*---------------------------------------------------------------------------*/
/*
 *prsave
 * wird nicht benutzt !!!
 */
/*---------------------------------------------------------------------------*/
/*
prsave(plow, phigh, adr)
int	*adr;
{
	int i;

	printf("\n Resume p = %x, @%X\n", plow, adr);
	i=0;
	do {
	    printf("\t%x:\t%x\n", 2*i, adr[i]);
	    i++; }
	while (i < 11);
}
*/

 
/*---------------------------------------------------------------------------*/
/*
 *prstate
 * Print U8000 Status
 * U8000 PC, FCW und alle Register an Konsole ausgeben
 */
/*---------------------------------------------------------------------------*/
prstate(state)
register struct state *state;
{
	register i;

	printf ("\nSTATE @%X ", state);
	printf ("eventid=%x, fcw=%x, pcseg=%x, pcoff=%x",
			          eventid, ps, pcseg, pc);
	for (i = 0; i < NUMREG+1; i++) {
		if ((i & 7) == 0)
			putchar('\n');
		printf("r%d:%x ", i, sreg[i]);
	}
	printf("\nSystem sp= ");
	prsp();
	putchar('\n');
	proc_disp();
}


/*---------------------------------------------------------------------------*/
/*
 *proc_display
 */
/*---------------------------------------------------------------------------*/

proc_disp()
{
	static char *msg[] = {
		"not ints yet",
		"clock",
		"step",
		"profiling",
		"unexpected",
		"SIO transmit",
		"SIO external/status",
		"SIO receive",
		"SIO special receive",
		"lp",
		"fd",
		"md",
		"ud",
		"koppel",
		"mt",
		"u1",
		"u2",
		"u3",
		"u4",
		"u5",
		"u6",
		"cpu2",
	};

	extern last_int_serv;
	extern sioaddrs[];
	extern md_flag, fd_flag, ud_flag, mt_flag, cpu2_flag;

	register i;

	printf("Last VI serviced: %s", msg[last_int_serv]);

	printf("\nPending VIs: ");
	if (inb(sioaddrs[4*2] | 0x0004) & 0x0002)
		printf("SIO-tty4/5 ");
	if (inb(sioaddrs[6*2] | 0x0004) & 0x0002)
		printf("SIO-tty6/7 ");
	if (md_flag)
		printf("md ");
	if (fd_flag)
		printf("fd ");
	if (ud_flag)
		printf("ud ");
	if (mt_flag)
		printf("mt ");
	if (cpu2_flag)
		printf("cpu2 ");

	printf("\nScheduler- st: %x fl: %x wchan: %x\n",
			proc[0].p_stat, proc[0].p_flag, proc[0].p_wchan);

	if (u.u_procp == &proc[0]) {
		printf("Scheduling or idle\n");
		return;
	}
	if (*u.u_comm)
		printf("Command: %s, ", u.u_comm);
	if (u.u_segmented == 0)
		printf("Non-");
	printf("Segmented programm, Real user id= %d\n", u.u_ruid);

	printf("Proc ptr: %X  Break addr: %x  tsize: %x  dsize: %x",
		u.u_procp, u.u_break, u.u_tsize, u.u_dsize); 
	if (u.u_segmented) {
		printf("\nSegs: ");
		for (i = 0; i <= 8 && i < u.u_nsegs; i++) {
			prseg(i);
			if (i & 1)
				printf("\n      ");
			else
				printf(" | ");
		}
	}
	else {
		printf("\nCode seg  ");
		prseg(TEXT);
		printf("\nData seg  ");
		prseg(DATA);
		printf("\nStack seg ");
		prseg(STAK);
	}
	putchar('\n');
}


/*---------------------------------------------------------------------------*/
/*
 *prtmmu
 * Print MMU, die Trap ausloeste mit Status-Register
 */
/*---------------------------------------------------------------------------*/
prtmmu(state)
register struct state *state;
{
	long Cbupc(), Dbupc(), Sbupc();
	long Cvaddr(), Dvaddr(), Svaddr();

	if (eventid & 0x0200)
		printf("Trap Code-MMU PC: %X, Vaddr: %X, Vtyp/BCS: %x\n",
			Cbupc(), Cvaddr(), getcbcs());
	if (eventid & 0x0400)
		printf("Trap Data-MMU PC: %X, Vaddr: %X, Vtyp/BCS: %x\n",
			Dbupc(), Dvaddr(), getdbcs());
	if (eventid & 0x0800)
		printf("Trap Stack-MMU PC: %X, Vaddr: %X, Vtyp/BCS: %x\n",
			Sbupc(), Svaddr(), getsbcs());
}


/*---------------------------------------------------------------------------*/
/*
 *prseg
 * Print ein Segment der MMU's
 */
/*---------------------------------------------------------------------------*/
prseg(seg)
register seg;
{
	printf("base:%x", u.u_segmts[seg].sg_base);
	printf(" size:%x", u.u_segmts[seg].sg_limit);
	printf(" attr:%x", u.u_segmts[seg].sg_attr);
}


/*---------------------------------------------------------------------------*/
/*
 *prstack
 * Print Stack
 * Ausgabe des Stacks ab 0xFFFE an die Konsole
 */
/*---------------------------------------------------------------------------*/
prstack(adr)
int *adr;
{
	register *addr;
	register *ad;
	register i, j;

	j = 0;
	ad = adr+(int)0x14;
	if ((unsigned long)ad < (unsigned long)adr)
		printf("Empty stack before interrupt");
	else {
		addr = ad;
		i = ((0xfffe-(int)ad)>>1)+1;
		if (i > 16)
			i = 16;
		for (; i >= 1; addr++, j++, i--) {
			if ((j & 7) == 0)
				printf("\n%x: ", (int)addr);
			printf ("%x   ", *(int *)addr);
		}
	}
	putchar('\n');
}


/*---------------------------------------------------------------------------*/
/*
 *prpsa
 * Print PSA
 * nicht benutzt
 */
/*---------------------------------------------------------------------------*/
/*
prpsa()
{
	register i;
	register long *a;

	a = 0;
	for (i = 0; i < 100; i++)
		printf("%X ", a[i]);
	putchar('\n');
}
*/
