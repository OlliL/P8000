/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	KERN 3.2	Modul:	trap.c
 
	Bearbeiter:	P. Hoge
	Datum:		26.9.88		
	Version:	1.3	
 
*******************************************************************************
 
	Die nmint Routine ist abhaengig von der Hardware
 
	Einbinden in LIB1:
	chkout trap.c
	scc -O -c trap.c
	ar r LIB1 trap.o
	(cp LIB1 /usr/sys/sys/LIB1)
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/state.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/callo.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/text.h>

#define	SCHMAG	8/10
 

#define sreg state->s_reg
#define eventid state->s_eventid
#define ps state->s_ps
#define pcseg state->s_pcseg
#define pc state->s_pc
 
extern long scfunc();
extern int Nproc;
extern long nofault;
extern int last_int_serv;
extern int console;
extern long lifetime;
extern struct proc *cpu2_pproc;
 
/*---------------------------------------------------------------------------*/
/*
 *evntentry
 * Eintritt in die Interrupt oder Trap Rountine. Diese Routine muss
 * bei allen Traps und Interrupts am Anfang durchlaufen werden
 * (realisiert in event.s). Falls User Mode: State Pointer in User Struktur
 * eintragen und Prozess Systemzeit merken
 */
/*---------------------------------------------------------------------------*/
evntentry(state)
register struct state *state;
{
	if (SYSMODE(ps))
		return;
	else {
		u.u_syst = u.u_stime;
		u.u_state = state;
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *uniminstr
 * Trap unimlementierte Instruction
 * Dieser Trap tritt auf, wenn eine EPU Instruction auftriit und keine
 * EPU vorhanden oder im FCW zugelassen ist.
 * In den User Programmen koennen Instruction's fuer die Floating Point EPU
 * enthalten sein, die zum Aufruf der Floating Point Emulator Software
 * fuehren. EPU Instruction's im Kern sind nicht zugelassen und bewirken
 * Panic
 */
/*---------------------------------------------------------------------------*/
uniminstr(state)
register struct state *state;
{
	register rc;
 
	ps &= FCWBITS;
	ps |= EPUBIT;
	if (SYSMODE(ps)) {
		prstate(state);
		panic("EPU inst.");
	}
	evi();
	rc = fpepu(state, &u.u_fpstate);
	if (rc & 0x00ff)
		psignal (u.u_procp, SIGSEG);	/* segment violation */
	if (rc & 0xFF00)
		psignal (u.u_procp, SIGFPE);	/* floating point exception */
	if (u.u_fpflag & FPSTEP)
		psignal (u.u_procp, SIGTRAP);	/* trace */
 
	u.u_fpflag = 0;
}
 
/*---------------------------------------------------------------------------*/
/*
 *privinstr
 * Trap priviligierte Instruction
 * tritt auf, wenn eine priviligierte U8000 Instruction im User Mode
 * abgearbeitet werden soll.
 * Im System Mode tritt dieser Trap nicht auf, das Programm muss auf
 * anderen unzulaessigen Wegen in diese Routine gelaufen sein, es
 * erfolgt Panic
 */
/*---------------------------------------------------------------------------*/
privinstr(state)
register struct state *state;
{
	ps &= FCWBITS;
	ps |= TRPBIT;
	if (SYSMODE(ps)) {
		prstate(state);
		panic ("Privileged inst.");
	}
	evi();
	psignal (u.u_procp, SIGILL);	/* illegal instruction */
}
 
/*---------------------------------------------------------------------------*/
/*
 *syscall
 * Trap System Call
 * Ueber diesen Trap wird der gewuenschte System Call aufgerufen und
 * ausgefuehrt. Die CPU geht in den System Mode
 * Die Tabelle der Pointer zu den System Call Funktionen befindet sich
 * in sysent.c. Der Pointer zur gewuenschten Funktion wird in
 * scfunc (machdep.c) gesetzt.
 */
/*---------------------------------------------------------------------------*/
syscall(state)
register struct state *state;
{
typedef int (*call)();
	call sy_call;
 
	ps &= FCWBITS;
	ps |= TRPBIT;
	sysinfo.syscall++;
	evi();
	if (SYSMODE(ps)) {
		if (debug(state))
			return;
		panic ("Kernel syscall");
	}
	u.u_deverr = 0;
	u.u_error = 0;
	ps &= ~EBIT;
	sy_call = (call)scfunc(state);
	u.u_dirp.left = u.u_arg[0];
	u.u_dirp.right = u.u_arg[1];
	u.u_r.r_val1 = sreg[RETRLO];
	u.u_r.r_val2 = sreg[RETRHI];
	u.u_ap = u.u_arg;
	if (save(u.u_qsav)) {
		setscr();
		if (u.u_error == 0)
			u.u_error = EINTR;
	}
	else
		sy_call();
 
	if (u.u_error) {
		ps |= EBIT;
		sreg[RETRLO] = u.u_error;
		sreg[RETRHI] = u.u_deverr;
	}
	else {
		sreg[RETRLO] = u.u_r.r_val1;
		sreg[RETRHI] = u.u_r.r_val2;
	}
}

/*---------------------------------------------------------------------------*/
/*
 *segvio
 * Segment Trap von einer MMU
 * tritt der Trap im System Mode auf, erfolgt Panic. Ausnahme: Zugriff auf
 * physische Adresse mit fuiword, fu...., suiword, su...., copyiin,
 * copy.... in mch.s und im EPU Mode.
 * Liegt User Mode und Stackueberlauf vor, wird der letzte Befehl
 * rueckgaengig gemacht (in backup, machdep.c) und der Stack fuer den
 * Prozess vergroessert (in grow).
 * Liegt User Mode und keine Stackverletzung vor, wird das Signal
 * Segmentation violation an den Prozess gesendet
 */
/*---------------------------------------------------------------------------*/
segvio(state)
register struct state *state;
{
	ps &= FCWBITS;
	ps |= TRPBIT;
	if (SYSMODE(ps)) {
		if ((u.u_state->s_ps & EPUBIT))
			;
		else {
			if (nofault == 0) {
				prstate(state);
				prtmmu(state);
				resmmu();
				panic ("Kernel seg. violation");
			}
			else {
				resmmu();
				pcseg = hiword(nofault);
				pc = loword(nofault);
				return;
			}
		}
	}
	if (backup(state) == 0) {
		resmmu();
		if (grow (u.u_state->s_sp)) {
			ps &= ~TRPBIT;
			if (SYSMODE(ps)) {
				mapustk();
				return;
			}
			evi();
			return;
		}
	}
	if (SYSMODE(ps)) {
		if (nofault == 0) {
			prstate(state);
			prtmmu(state);
			resmmu();
			prstack(state);
			panic ("Kernel/fp seg. violation");
		}
		else {
			resmmu();
			pcseg = hiword(nofault);
			pc = loword(nofault);
			return;
		}
	} else {
		evi();
		if (u.u_ttyd == console) {
			putchar('\n');
			prstate(state);
			prtmmu(state);
			printf("User seg. violation\n");
		}
		resmmu();
		psignal(u.u_procp, SIGSEG);
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *nmint
 * Nichtmaskierter Interrupt
 * Je nach Implementation koennen ein oder mehrere NMI Quellen vorhanden
 * sein:
 *		- manuell (Taste)
 *		- Paritaetsfehler RAM
 *		- Spannungsausfall
 *		- Paritaetsfehler RAM 2. CPU
 * Die Unterscheidung erfolgt ueber Event Identifier.
 * Bei manuellem NMI werden Register und Stackinhalt auf der Konsole
 * ausgegeben und das unterbrochene Programm fortgesetzt. Bei Paritaets-
 * fehler erfolgt Panic, bei Spannungsausfall gibt sich der Rechner per
 * Software selbst ein Reset um noch groesseren Schaden zu vermeiden.
 * Bei Paritaetsfehler und Spannungsausfall erfolgt kein Update der Platte,
 * Platte kann inkonsistent werden !
 */
/*---------------------------------------------------------------------------*/
nmint(state)
register struct state *state;
{
	register unsigned scr;
 
	printf ("NMI ");
	switch (eventid & 0x0007) {
 
		case 0x0001:
			printf ("(manual)\n");
			prstate (state);
			debug(state);
			break;
 
		case 0x0002:
			printf ("(power fail)\n");
			in(0xffe9);	/* Soft-Reset */
			break;
 
		case 0x0004:
			printf ("(memory)\n");
			if (SYSMODE(ps))
				panic ("parity");
			else {
				psignal (u.u_procp, SIGBUS);
				scr = inb(SCR);
				outb(SCR, scr&0xf7);
				outb(SCR, scr);
			}
			break;
/*
		case 0x0007:
*/
		default:
			out(0xff1f, 0xfeff); /* cpu2 reset */
			printf ("(memory cpu2)\n");
			if (cpu2_pproc != NULL)
				psignal (cpu2_pproc, SIGINT);
			scr = inb(SCR);
			outb(SCR, scr&0xf7);
			outb(SCR, scr);
			break;
/*
		default:
			printf ("(unknown source)\n");
*/
	}
	evi();
}
 
/*---------------------------------------------------------------------------*/
/*
 *nvi
 * Nichtvektorisierter Interrupt
 * Eine NVI Quelle ist nicht vorgesehen, es erfolgt Ausgabe der
 * U8000 Register und Panic
 */
/*---------------------------------------------------------------------------*/
nvint(state)
register struct state *state;
{
	prstate (state);
	panic ("NVI");
}
 
/*---------------------------------------------------------------------------*/
/*
 *unexint
 * nichtvorhandene vektorisierte Interrupts
 * Tritt ein VI mit einem nicht im System vorhandenen Vektor auf,
 * werden die Register ausgegeben und es erfolgt Panic
 */
/*---------------------------------------------------------------------------*/
unexint(state)
register struct state *state;
{
	last_int_serv = UNEXINT;
	prstate (state);
	panic ("Unexpected VI");
}

/*--------------------------------------------------------------------------*/
/*spurint
 * nichtvorhandene vektorisierte Interrupts
 */
/*--------------------------------------------------------------------------*/
spurint()
{
	printf("WARNING: spurious int\n");
}

/*---------------------------------------------------------------------------*/
/*
 *stepint
 * VI fuer die Step (Trace) Funktion
 * Dieser Interrupt dient der schrittweisen Abarbeitung von zu testenden
 * Programmen. Dieser Interrupt wird in der Procedure GO_INT in event.s
 * aktiviert.
 */
/*---------------------------------------------------------------------------*/
stepint(state)
register struct state *state;
{
	register struct state *u_state;
 
	last_int_serv = STEPINT;
	resstep();
	evi();
	u_state = u.u_state;
	if (SYSMODE(ps)) {
		if (u_state->s_ps & TRPBIT) {
			u_state->s_ps &= ~TBIT;
			psignal (u.u_procp, SIGTRAP);
		}
		else {
			if (u_state->s_ps & EPUBIT)
				u.u_fpflag |= FPSTEP;
			else {
				if (SYSMODE(ps))
					debug(state);
				u_state->s_ps |= TBIT;
			}
		}
	}
	else {
		u_state->s_ps &= ~TBIT;
		psignal (u.u_procp, SIGTRAP);
	}
}

/*---------------------------------------------------------------------------*/
/*
 *profint
 */
/*---------------------------------------------------------------------------*/
profint()
{ 
	last_int_serv = PROFINT;
}

/*---------------------------------------------------------------------------*/
/*
 *clokint
 * Clock Interrupt Service Routine
 * Aufgaben bei jedem Int:
 *	reti an Interrupt Quelle
 *	Realisierung von Callouts
 *	 Timeout Zeiten decrementieren
 *	 bei abgelaufenen Zeiten Callout Routinen ausfuehren
 *	Laufzeit des aktuellen Prozesses unterteilt in User
 *	 und System Zeit incrementieren
 *	Profile incrementieren
 *	Systemzeit (und Datum) incrementieren
 *
 * jede Sekunde:
 *	Prozesswechsel anfordern
 *	Sekundenprozesse wecken
 *	Alarmzeiten der Prozesse decrementieren, bei Ablauf Alarm
 *	 Signalsenden
 *	Prozess Prioritaeten neu berechnen
 *	Scheduler aktivieren, falls erforderlich
 */
/*---------------------------------------------------------------------------*/
clokint(state)
struct state *state;
{
	register struct callo *p1, *p2;
	register a;
	extern unsigned waitloc;

	last_int_serv = CLOCKINT;
	clkreti();
	lifetime++;
 
	/*
	 * callouts (timeouts)
	 * wenn keine vorhanden, schnell weiter
	 * sonst erste Zeit aktualisieren
	 */
 
	if(callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=NULL)
		p2++;
	p2->c_time--;

	/*
	 * callout Routinen bei abgelaufener Zeit ausfuehren
	 * und aus Callout (Timeout) Tabelle streichen
	 */

	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

	/*
	 * Laufzeit aktueller Prozess incrementieren
	 * Profile
	 * Systemzeit und Datum
	 */
 
out:
	if (SYSMODE(ps)) {
		u.u_stime++;
		if ((((long)((pcseg&MASKSEG)) <<16) | (long)pc) != (long)&waitloc) {
			a = CPU_IDLE;
			if (syswait.iowait)
				sysinfo.wait[W_IO]++;
			if (syswait.swap)
				sysinfo.wait[W_SWAP]++;
			if (syswait.physio)
				sysinfo.wait[W_PIO]++;
		}
		else
			a = CPU_KERNAL;
	} else {
		u.u_utime++;
		a = CPU_USER;
		if(u.u_prof[0].pr_scale)
			aprof(state, 1);
	}
	sysinfo.cpu[a] += 1;
	clock2();		/* Fortsetzung */
}

/* Fortsetzung von clokint */

clock2()
{
	register struct proc *pp;
	register a;

	pp = u.u_procp;
	if(pp->p_stat == SRUN) {
		u.u_mem += (unsigned)pp->p_size;
		if (pp->p_textp) {
			a = pp->p_textp->x_ccount;
			if (a == 0)
				a++;
			u.u_mem += pp->p_textp->x_size / a;
		}
	}
	if(++pp->p_cpu == 0)
		pp->p_cpu--;
	if(++lbolt >= HZ) {
		lbolt -= HZ;
		time++;
		evi();
 
	/*
	 * Prozesswechsel fordern
	 * Sekundenprozesse wecken (lightning bolt time-out)
	 */
 
		runrun++;
		wakeup((caddr_t)&lbolt);
 
	/*
	 * Prozesszeiten incrementieren
	 * Alarm Funktion
	 * Prioritaeten neu festlegen
	 */
 
		for(pp = &proc[0]; pp < &proc[Nproc]; pp++)
		if (pp->p_stat && pp->p_stat<SZOMB) {
			if(pp->p_time != 127)
				pp->p_time++;
			if(pp->p_clktim)
				if(--pp->p_clktim == 0)
					psignal(pp, SIGALRM);
			a = (pp->p_cpu & 0377)*SCHMAG + pp->p_nice - NZERO;
			if(a < 0)
				a = 0;
			if(a > 255)
				a = 255;
			pp->p_cpu = a;
			if(pp->p_pri >= PUSER)
				setpri(pp);
		}
 
	/* Scheduler aktivieren */
 
		if(runin!=0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *evntret
 * Austritt aus allen Interrupts und Traps
 * wenn Signal an aktuellen Prozess vorhanden, an Prozess senden
 * Prioritaet des aktuellen Prozesses neu festlegen
 * wenn Prozesswechsel gefordert, Prozesswechsel ausfuehren
 * Profile des aktuellen Prozesses (korrigieren)
 */
/*---------------------------------------------------------------------------*/
evntret(state)
register struct state *state;
{
	if (SYSMODE(ps))
		return;
	else {
		evi();
		if (u.u_procp->p_sig) {
			if (issig())
				psig();
		}
		if (runrun) {
			curpri = setpri(u.u_procp);
			if (runrun) {
				sysinfo.preempt++;
				qswtch();
			}
		}
		setscr();
		dvi();
		if (u.u_segmented)
			ps |= SEGFCW;
		else
			ps &= ~SEGFCW;

		if (u.u_prof[0].pr_scale)
			aprof(state, (int)(u.u_stime-u.u_syst));
	}
}

/*---------------------------------------------------------------------------*/
/*
 *aprof
 * fuer profiling
 * Die Routine war ursprueglich in clokint und evntret enthalten
 */
/*--------------------------------------------------------------------------*/
aprof(state, time)
register struct state *state;
register time;
{
	register unsigned seg;
	register unsigned i;

	i = 0;
	if (u.u_segmented) {
	   seg = (pcseg & MASKSEG) >> 8;
	      for (;; i++) {
		if (i >= u.u_nprof)
			return;
		if ((unsigned char)u.u_pr_segno[i] == seg)
			break;
	      }
 	}
	addupc (pc, pcseg, &u.u_prof[i], time);
}
