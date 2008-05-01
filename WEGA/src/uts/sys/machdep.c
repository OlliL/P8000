/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	machdep.c
 
 
	Bearbeiter:	P. Hoge
	Datum:		29.9.88
	Version:	1.2
 
*******************************************************************************
 
	Dieser Modul ist fuer alle U8000 Wega Implementationen gleich.
 
	Einbinden in LIB1:
	chkout machdep.c
	scc -O -c machdep.c
	ar r LIB1 machdep.o
	(cp LIB1 /usr/sys/sys/LIB1)
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/state.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/acct.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/map.h>
#include <sys/buf.h>
 
#define sreg state->s_reg
#define stp state->s_sp
#define eventid state->s_eventid
#define ps state->s_ps
#define pcseg state->s_pcseg
#define pc state->s_pc
 

/*---------------------------------------------------------------------------*/
/*
 *icode
 * hexadezimales Bootstrap Programm fuer das Hochfahren des
 * Wega Systems nachdem der Wega Kern geladen ist. Prozess 1
 * wird aktiviert (etc/init wird gestartet), icode ist das erste
 * Programm, das im User Mode abgearbeitet wird.
 * C Aequivalent fuer icode:
 *	main() {
 *	 execle ("/etc/INIT", "init  -s", 0);
 *	 while (1);
 *	}
 */
/*---------------------------------------------------------------------------*/
 
char	icode[] =
{
	0x76, 0x00,	/* lda r0, %0016 ; Filename Pointer */
	0x00, 0x16,
	0x76, 0x01,	/* lda r1, %0010 ; Pointer zu Argument Pointern */
	0x00, 0x10,
	0x76, 0x02,	/* lda r2, %0014  */
	0x00, 0x14,
	0x7f, 0x3b,	/* sc %3b ; exec syscall */
	0xe8, 0xff,	/* loop: jr loop ; while (1) */
	0x00, 0x20,	/* Pointer zu 1. Argument ab Adresse %0020 */
	0x00, 0x00,
	0x00, 0x00,	/* Adresse %0014 */
	'/','e','t','c','/','I','N','I','T',0,
	'i','n','i','t',' ',' ',' ',' ',0, 0,
};
int	bfoff = 32; /* Adresse in icode */
int	szicode = sizeof(icode);

/*---------------------------------------------------------------------------*/
/*
 *setscr
 * System-Configuration-Register (SCR) fuer segmentierten oder
 * nichtsegmentierten User setzen
 */
/*---------------------------------------------------------------------------*/
setscr()
{
	if (u.u_segmented)
		outb(SCR, SUSRON);
	else
		outb(SCR, SUSROFF);
}

/*---------------------------------------------------------------------------*/
/*
 *sendsig
 * Signal an Prozess senden durch Simulation eines Interrupts
 * PC und FCW (ps) werden im Userstack gerettet (analog Interrupt)
 * Stackpointer wird dercrementiert, PC wird auf Adresse
 * der Signal (Interrupt) Routine im Userprogramm (in Userstruktur enthalten)
 * gesetzt
 */
/*---------------------------------------------------------------------------*/
sendsig(p)
register long p;
{
	register struct state *u_state;
	register unsigned stseg;
	register unsigned stoff;

	u_state = u.u_state;
	stoff = u_state->s_sp - 6;
	grow(stoff);
	if (u.u_segmented)
		stseg = u.u_stakseg <<8;
	else
		stseg = nsseg(u_state->s_sp);

	if (suword(stoff+4, stseg, u_state->s_ps) == 0)
	   if (suword(stoff+2, stseg, u_state->s_pc) == 0)
	      if (suword(stoff, stseg, u_state->s_pcseg) == 0)
			goto noerror;
	u.u_error = EFAULT;
noerror:

	u_state->s_sp = stoff;
	u_state->s_pcseg = (int)(p>>16);
	u_state->s_pc = (int)p;
}
 
/*---------------------------------------------------------------------------*/
/*
 *sysiret
 * System Call 0
 * Exit von User Signal Service Routine
 * (reti von simuliertem Interrupt)
 * PC und FCW (ps) auf alten Wert (im Stack abgelegt) setzen
 */
/*---------------------------------------------------------------------------*/
sysiret()
{
	unsigned fcw;
	register struct state *u_state;
	register unsigned stseg;
	register unsigned stoff;

	u_state = u.u_state;
	if (u.u_segmented)
		stseg = u.u_stakseg <<8;
	else
		stseg = nsseg(u_state->s_sp);

	stoff = u_state->s_sp;

	if (fuword(stoff, stseg, &u_state->s_pcseg) == 0)
	   if (fuword(stoff+2, stseg, &u_state->s_pc) == 0)
	      if (fuword(stoff+4, stseg, &fcw) == 0)
			goto noerror;
	u.u_error = EFAULT;
noerror:
	u_state->s_ps = (u_state->s_ps & 0xff00) | (fcw & 0x00ff);
	u_state->s_sp += 6;
}
 
/*---------------------------------------------------------------------------*/
/*
 *scfunc
 * Pointer zur Systemfunktion laden
 * Die Nummer der Systemfunktion wird aus der Struktur state (beim
 * Trap im Stack abgelegt) ermittelt und aus der Tabelle sysent
 * (sysent.c) die Adresse der Systemfunktion entnommen
 */
/*---------------------------------------------------------------------------*/
 
typedef int (*sy_call)();
 
sy_call
scfunc(state)
register struct state *state;
{
	register struct sysent *sy;
	register func;
	register n;
 
	func = eventid & 0x00ff;
	if (func > MAXSCID )
		func = BADSCID;
	sy = &sysent[func];
	if (u.u_segmented) {
		for (n = 0; n < sy->sy_nreg; n++)
			u.u_arg[n] = sreg[n];
		switch(func) {
		case OPENS:
		case CREAT:
		case UNLINK:
		case CHDIR:
		case MKNOD:
		case CHMOD:
		case CHOWN:
		case BRK:
		case UMOUNT:
		case ACCESS:
		case FTIME:
		case TIMES:
		case PROFIL:
		case SYSACCT:
		case UTSSYS:
		case CHROOT:
			u.u_arg[0] &= 0x7f00;
			break;
		case READS:
		case WRITES:
		case FSTAT:
		case STTY:
		case GTTY:
		case SIGNAL:
		case MDMCTL:
			u.u_arg[1] &= 0x7f00;
			break;
		case LINK:
		case EXEC:
		case STATS:
		case MOUNT:
		case UTIME:
			u.u_arg[0] &= 0x7f00;
			u.u_arg[2] &= 0x7f00;
			break;
		case EXECE:
			u.u_arg[0] &= 0x7f00;
			u.u_arg[2] &= 0x7f00;
			u.u_arg[4] &= 0x7f00;
			break;
		case LKDATA:
		case UNLK:
		case IOCTL:
		case SPTRACE:
			u.u_arg[2] &= 0x7f00;
			break;
		case PTRACE:
			u.u_arg[2] = 0;
			u.u_arg[3] = sreg[2];
			u.u_arg[4] = sreg[3];
			break;
		}
	}
	else {
		switch(func) {
		case OPENS:
		case CREAT:
		case MKNOD:
		case CHMOD:
		case CHOWN:
		case BRK:
		case ACCESS:
		case PROFIL:
		case UTSSYS:
			u.u_arg[0] = nsseg(sreg[0]);
			for (n = 1; n < sy->sy_nreg; n++)
				u.u_arg[n] = sreg[n-1];
			break;
		case READS:
		case WRITES:
		case MDMCTL:
			u.u_arg[0] = sreg[0];
			u.u_arg[1] = nsseg(u.u_arg[2] = sreg[1]);
			u.u_arg[3] = sreg[2];
			break;
		case UNLINK:
		case CHDIR:
		case UMOUNT:
		case FTIME:
		case TIMES:
		case SYSACCT:
		case CHROOT:
			u.u_arg[0] = nsseg(u.u_arg[1] = sreg[0]);
			break;
		case MOUNT:
			u.u_arg[0] = nsseg(u.u_arg[1] = sreg[0]);
			u.u_arg[2] = nsseg(u.u_arg[3] = sreg[1]);
			u.u_arg[4] = sreg[2];
			break;
		case LINK:
		case EXEC:
		case STATS:
		case UTIME:
			u.u_arg[0] = nsseg(u.u_arg[1] = sreg[0]);
			u.u_arg[2] = nsseg(u.u_arg[3] = sreg[1]);
			break;
		case EXECE:
			u.u_arg[0] = nsseg(u.u_arg[1] = sreg[0]);
			u.u_arg[2] = nsseg(u.u_arg[3] = sreg[1]);
			u.u_arg[4] = nsseg(u.u_arg[5] = sreg[2]);
			break;
		case LKDATA:
		case UNLK:
		case IOCTL:
			u.u_arg[0] = sreg[0];
			u.u_arg[1] = sreg[1];
			u.u_arg[2] = nsseg(u.u_arg[3] = sreg[2]);
			break;
		case PTRACE:
			u.u_arg[0] = sreg[0];
			u.u_arg[1] = sreg[1];
			u.u_arg[2] = 0;
			u.u_arg[3] = sreg[2];
			u.u_arg[4] = sreg[3];
			break;
		case FSTAT:
		case STTY:
		case GTTY:
			u.u_arg[0] = sreg[0];
			u.u_arg[1] = nsseg(u.u_arg[2] = sreg[1]);
			break;
		case SIGNAL:
			u.u_arg[0] = sreg[0];
			if ((u.u_arg[2] = sreg[1]) == 0 || sreg[1] & 0x0001)
				u.u_arg[1] = 0;
			else
				u.u_arg[1] = 0x3f00;
			break;
		default:
			for (n = 0; n < sy->sy_nreg; n++)
				u.u_arg[n] = sreg[n];
			if (func == SPTRACE)
				u.u_arg[2] &= 0x7f00;
			break;
		}
	}
	return(sy->sy_call);
}
 
/*---------------------------------------------------------------------------*/
/*
 *backup
 * wird von segvio (in trap.c) aufgerufen, wenn ein MMU Trap
 * ausgeloest wurde.
 * backup macht die (letzte) Instruction, wenn sie zum Stack Trap gefuehrt
 * hat, rueckgaengig (call, calr, push ...).
 * Return 0 = backup war erfolgreich
	 -1 = backup nicht moeglich
 */
/*---------------------------------------------------------------------------*/
backup(state)
register struct state *state;
{
	register i;
	register unsigned roff;
	register instr;
	unsigned instr1;
	unsigned instr2;
	unsigned *addr;
	long Cbupc();
	long Dbupc();
	long Sbupc();
 
	i = eventid;
	if (SYSMODE(ps)) {
		if ((i & 0x0e00) == 0x0200) { /* trap von text mmu ? */
			if (pwwarn1())
				return(0);
		}
	}
	else {
		if ((i & 0x0e00) == 0x0800) { /* trap von stack mmu ? */
			if (pwwarn3())
				return(0);
		}
	}
	if (u.u_segmented == 0) {
		if ((i & 0x0e00) == 0x0200) { /* text mmu */
			pc = Cbupc();
			if (SYSMODE(ps) == 0) {
				stp += 1;
				return(0);
			}
		}
		else
			pc = Sbupc();
	}
	else {
		if ((i & 0x0e00) == 0x0200) { /* text mmu */
			i = 0;
			pc = Cbupc();
		}
		else {
		if ((i & 0x0e00) == 0x0400) { /* data mmu */
			i = getdbcs() & 0x000f;
			pc = Dbupc();
		}
		else {
		if ((i & 0x0e00) == 0x0800) { /* stack mmu */
			i = getsbcs() & 0x000f;
			pc = Sbupc();
		}
		}}
		if (i == 0x000c || i == 0x000d) {
			stp += 1;
			return(0);
		}
	}

	if (ps & SEGFCW) /* segmentierter Mode ? */
		roff = 1;
	else
		roff = 0;

	if (SYSMODE(ps)) {
		addr = (unsigned *)((((long)pcseg<<16) + (long)pc) & 0x7f00ffff);
		instr1 = *addr;
		instr2 = *(addr+1);
	}
	else {
		if (u.u_segmented) {
			if (fuword(pc, pcseg, &instr1) == 0)
			   if (fuword(pc+2, pcseg, &instr2) == 0)
				goto noerror;
		}
		else {
			if (fuiword(pc, &instr1) == 0)
			   if (fuiword(pc+2, &instr2) == 0)
				goto noerror;
		}
		u.u_error = EFAULT;
noerror:	;
	}

	instr = instr1;

/* Test violation bei call oder calr */
 
	if (((instr & 0xbf0f) == 0x1f00) || ((instr & 0xF000) == 0xd000)) {
		stp += 2;
		if (u.u_segmented || SYSMODE(ps))
			stp += 2;
		return(0);
	}
 
/* Test violation bei push immidate */
 
	if ((instr & 0xff0f) == 0x0d09) {
		sreg[((instr >> 4) & 0x000f) + roff] += 2;
		return(0);
	}
 
/* Test violation bei sonstigem push oder pop */
 
	if ((instr & 0x3900) == 0x1100) {
		i = 4;			/* long */
		if (instr & 0x0200)	/* word push pop */
			i = 2;		/* word */
		if (instr & 0x400)	/* pop ? */
			i = -i;
		sreg[((instr >> 4) & 0x000f) + roff] += i;
		return(0);
	}
 
/* Test violation bei Translate oder Compare */
 
	if ((instr & 0xfc00) == 0xb800) {
		i = -2;			/* byte Zugriff */
		if (instr & 0x0100)
			i = -4;		/* word Zugriff */
		if (instr & 0x0008)	/* auto decrement ? */
			i = -i;
 
		sreg[((instr >> 4) & 0x000f) + roff] += i;
		sreg[((instr2 >> 8) & 0x000f)] += 2;
 
/* Test Compare mit Register */
 
		if ((instr & 0x0203) > 0x0200)
			sreg[((instr2 >> 4) & 0x000f) + roff] += i;
	}
	return(0);
}
