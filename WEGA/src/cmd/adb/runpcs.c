static char sccsid[] = "@[$]runpcs.c\t3.2  08/25/83 09:49:03 - 87wega3.2";

#include "defs.h"


MSG		NOFORK;
MSG		ENDPCS;
MSG		BADWAIT;


/* breakpoints */






/* service routines for sub process control */

getsig(sig)
{	return(expr(0) ? shorten(expv.a_offset) : sig);
}

ADDR		userpc = { 0, 1L };

runpcs(runmode, execsig)
{
	INT		samepc;
	INT		rc;
	REG BKPTR	bkpt;
	ADDR		a;

	IF adrflg
	THEN userpc=dot;
	FI
	printf("%s: running\n", symfil);
	IF runmode != SINGLE THEN setbp(); FI

	WHILE (loopcnt--)>0
	DO
		stty(0,&usrtty);
		IF shorten(userpc.a_offset)==regs[19]
		THEN samepc=1;
		ELSE samepc=0;
		FI
		IF segflg==1
		THEN sptrace(runmode,pid,itol(userpc.a_segno<<8,shorten(userpc.a_offset)),execsig);
		ELSE ptrace(runmode,pid,shorten(userpc.a_offset),execsig);
		FI
		bpwait(SIGILL,SIGTRAP); chkerr();
		readregs();

		/*look for bkpt*/
		a.a_segno = (regs[18]>>8)&0177;
		a.a_offset = regs[19]-2;
		IF signo==0 ANDF (bkpt=scanbkpt(&a,runmode))
		THEN /*stopped at bkpt*/
		     IF runmode != SINGLE
		     THEN userpc=bkpt->loc;
			  regs[19]=shorten(bkpt->loc.a_offset);
			  regs[18]=bkpt->loc.a_segno<<8;
			  bkpt->flag=BKPTEXEC;
			  IF --bkpt->count
			  THEN IF samepc==0 ANDF bkpt->comm[0]
			       THEN command(bkpt->comm,'.');
			       ELSE bkpt->count++;
			       FI
			       execbkpt(bkpt);
			       loopcnt++;
			       execsig=0;
			       userpc.a_segno=0; userpc.a_offset=1;
			  ELIF samepc
			  THEN bkpt->count++;
			       execbkpt(bkpt);
			       loopcnt++;
			       execsig=0;
			       userpc.a_segno=0; userpc.a_offset=1;
			  ELSE IF bkpt->comm[0] THEN command(bkpt->comm,'.'); FI
			       bkpt->count=bkpt->initcnt;
			  FI
		     ELSE
			  IF bkpt->comm[0] THEN command(bkpt->comm,'.'); FI
			  bkpt->count--;
			  IF bkpt->count == 0 THEN bkpt->count=bkpt->initcnt; FI
			  rc=1;
		     FI
		ELSE execsig=signo; userpc.a_offset=1; rc=0;
		FI
	OD
	IF runmode != SINGLE THEN delbp(); FI
	return(rc);
}

endpcs()
{
	REG BKPTR	bkptr;

	IF pid
	THEN IF segflg==1
	     THEN sptrace(EXIT,pid,0L,0);
	     ELSE ptrace(EXIT,pid,0,0);
	     FI
	     pid=0; userpc.a_segno=0; userpc.a_offset=1;
	     FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	     DO IF bkptr->flag
		THEN bkptr->flag=BKPTSET;
		FI
	     OD
	FI
}

setup()
{
	REG BKPTR	bkptr;

	close(fsym); fsym = -1;
	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO bkptr->count=bkptr->initcnt; OD
	IF (pid = fork()) == 0
	THEN IF segflg==1
	     THEN sptrace(SETTRC,0,0L,0);
	     ELSE ptrace(SETTRC,0,0,0);
	     FI
	     signal(SIGINT,sigint); signal(SIGQUIT,sigqit);
	     doexec(); exit(0);
	ELIF pid == -1
	THEN error(NOFORK);
	ELSE bpwait(SIGTRAP,SIGTRAP);
	     IF segflg==1
	     THEN uptr->u_state = (POS)sptrace(RUREGS,pid,(long)((INT *)&((struct suser *)0)->u_state + 1),0);
	     ELSE uptr->u_state = (POS)ptrace(RUREGS,pid,(INT *)&((struct suser *)0)->u_state + 1,2);
	     FI
	     readregs(); lp[0]=EOR; lp[1]=0;
	     fsym=open(symfil,wtflag);
	     IF errflg
	     THEN printf("%s: cannot execute\n",symfil);
		  endpcs(); error(0);
	     FI
	FI
}

execbkpt(bkptr)
BKPTR	bkptr;
{
	ADDR		bkptloc;

	bkptloc = bkptr->loc;
	IF segflg==1
	THEN sptrace(WIUSER,pid,itol(bkptloc.a_segno<<8,shorten(bkptloc.a_offset)),bkptr->ins);
	ELSE ptrace(WIUSER,pid,shorten(bkptloc.a_offset),bkptr->ins);
	FI
	stty(0,&usrtty);
	IF segflg==1
	THEN sptrace(SINGLE,pid,itol(bkptloc.a_segno<<8,shorten(bkptloc.a_offset)),0);
	ELSE ptrace(SINGLE,pid,shorten(bkptloc.a_offset),0);
	FI
	bpwait(SIGTRAP,SIGTRAP); readregs(); chkerr();
	IF segflg==1
	THEN sptrace(WIUSER,pid,itol(bkptloc.a_segno<<8,shorten(bkptloc.a_offset)),BPT);
	ELSE ptrace(WIUSER,pid,shorten(bkptloc.a_offset),BPT);
	FI
	bkptr->flag=BKPTSET;
}


doexec()
{
	STRING		argl[MAXARG];
	CHAR		args[LINSIZ];
	STRING		p, *ap, filnam;

	ap=argl; p=args;
	*ap++=symfil;
	REP	IF rdc()==EOR THEN break; FI
		*ap = p;
		WHILE lastc!=EOR ANDF lastc!=SP ANDF lastc!=TB DO *p++=lastc; readchar(); OD
		*p++=0; filnam = *ap+1;
		IF **ap=='<'
		THEN	close(0);
			IF open(filnam,0) == -1
			THEN	printf("%s: cannot open\n",filnam); exit(0);
			FI
		ELIF **ap=='>'
		THEN	close(1);
			IF creat(filnam,0666) == -1
			THEN	printf("%s: cannot create\n",filnam); exit(0);
			FI
		ELSE	ap++;
		FI
	PER lastc!=EOR DONE
	*ap++=0;
	execv(symfil, argl);
}

BKPTR	scanbkpt(adr, runmode)
ADDR	*adr;
{
	REG BKPTR	bkptr;

	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF runmode != SINGLE
	   THEN IF bkptr->flag ANDF adr->a_segno==bkptr->loc.a_segno ANDF bkptr->loc.a_offset==adr->a_offset
		THEN break;
		FI
	   ELIF bkptr->flag ANDF adr->a_segno==bkptr->loc.a_segno ANDF adr->a_offset+2==bkptr->loc.a_offset
	   THEN break;
	   FI
	OD
	return(bkptr);
}

delbp()
{
	INT		a;
	REG BKPTR	bkptr;

	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF bkptr->flag
	   THEN IF segflg==1
		THEN sptrace(WIUSER,pid,itol(bkptr->loc.a_segno<<8,shorten(bkptr->loc.a_offset)),bkptr->ins);
		ELSE ptrace(WIUSER,pid,shorten(bkptr->loc.a_offset),bkptr->ins);
		FI
	   FI
	OD
}

setbp()
{
	INT		a;
	REG BKPTR	bkptr;

	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF bkptr->flag
	   THEN IF segflg==1
		THEN bkptr->ins = sptrace(RIUSER,pid,itol(bkptr->loc.a_segno<<8,shorten(bkptr->loc.a_offset)),0);
		     sptrace(WIUSER,pid,itol(bkptr->loc.a_segno<<8,shorten(bkptr->loc.a_offset)),BPT);
		ELSE bkptr->ins = ptrace(RIUSER,pid,shorten(bkptr->loc.a_offset),0);
		     ptrace(WIUSER,pid,shorten(bkptr->loc.a_offset),BPT);
		FI
		IF errno
		THEN prints("cannot set breakpoint: ");
		     psymoff(&bkptr->loc,ISYM,"\n");
		FI
	   FI
	OD
}

bpwait(sig1, sig2)
{
	REG INT w;
	INT stat;

	signal(SIGINT, 1);
	WHILE (w = wait(&stat))!=pid ANDF w != -1 DONE
	signal(SIGINT,sigint);
	gtty(0,&usrtty);
	stty(0,&adbtty);
	IF w == -1
	THEN pid=0;
	     errflg=BADWAIT;
	ELIF (stat & 0177) != 0177
	THEN IF signo = stat&0177
	     THEN sigprint();
	     FI
	     IF stat&0200
	     THEN prints(" - core dumped");
		  close(fcor);
		  setcor();
	     FI
	     pid=0;
	     errflg=ENDPCS;
	ELSE signo = stat>>8;
	     IF signo!=sig1 ANDF signo!=sig2
	     THEN sigprint();
	     ELSE signo=0;
	     FI
	     flushbuf();
	FI
}

readregs()
{
	/*get REG values from pcs*/
	INT		*uaddr;
	REG REGPTR	p;
	REG INT		i;
	REG INT		j;

	uaddr = uptr->u_state;
	uaddr -= 0x7B00;
	FOR p=reglist; p->rname; p++
	DO IF segflg==1
	   THEN regs[p->roffs] = sptrace(RUREGS,pid,(long)(uaddr+p->roffs),0);
	   ELSE regs[p->roffs] = ptrace(RUREGS,pid,uaddr+p->roffs,0);
	   FI
	OD
	IF segflg==1
	THEN regs[18] = ptrace(RUREGS,pid,(long)(uaddr+18),0);
	ELSE regs[18] = 0;
	FI
	uaddr = (INT *)&((struct suser *)0)->u_fpstate;
	FOR i=0; i<8; i++
	DO FOR j=0; j<5; j++
	   DO IF segflg==1
	      THEN uptr->u_fpstate.uf_reg[i][j] = sptrace(RUREGS,pid,(long)&((INT (*)[5])uaddr)[i][j],0);
	      ELSE uptr->u_fpstate.uf_reg[i][j] = ptrace(RUREGS,pid,&((INT (*)[5])uaddr)[i][j],0);
	      FI
	   OD
	OD
}
