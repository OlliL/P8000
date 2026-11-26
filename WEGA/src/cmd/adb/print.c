static char sccsid[] = "@[$]print.c\t3.2  08/25/83 09:49:00 - 87wega3.2";

#include "defs.h"


MSG		LONGFIL;
MSG		NOTOPEN;
MSG		BADMOD;



/* breakpoints */




L_REAL		fpcvt();

/* general printing routines ($) */

printtrace(modif)
{
	INT		narg, i, stat, name, limit, inst;
	POS		dynam;
	REG BKPTR	bkptr;
	CHAR		hi;
	INT		word;
	STRING		comptr;
	L_INT		argp, frame;
	SYMPTR		symp;

	IF cntflg==0 THEN cntval = -1; FI

	switch (modif) {

	    case '<':
	    case '>':
		{CHAR		file[64];
		INT		index;

		index=0;
		IF modif=='<'
		THEN	iclose();
		ELSE	oclose();
		FI
		IF rdc()!=EOR
		THEN	REP file[index++]=lastc;
			    IF index>=63 THEN error(LONGFIL); FI
			PER readchar()!=EOR DONE
			file[index]=0;
			IF modif=='<'
			THEN	infile=open(file,0);
				IF infile<0
				THEN	infile=0; error(NOTOPEN);
				FI
			ELSE	outfile=open(file,1);
				IF outfile<0
				THEN	outfile=creat(file,0644);
				ELSE	lseek(outfile,0L,2);
				FI
			FI

		FI
		lp--;
		}
		break;

	    case 'd':
		ibase = 2; break;

	    case 'x':
		ibase = 4; break;

	    case 'q': case 'Q': case '%':
		done();

	    case 'w': case 'W':
		maxpos=(adrflg?adrval.a_offset:MAXPOS);
		break;

	    case 's': case 'S':
		maxoff=(adrflg?adrval.a_offset:MAXOFF);
		break;

	    case 'v': case 'V':
		prints("variables\n");
		printf("%  address\n");
		FOR i=0;i<4;i++
		DO IF svar[i].a_offset
		   THEN printc2((i==3 ? 'e' : (i<=9 ? '0' : 'a'-10) + i), 0);
			IF segflg
			THEN printf(" = <<%%%x>>%%%-10X\n",svar[i].a_segno,svar[i].a_offset);
			ELSE printf(" = %%%-10X\n",svar[i].a_offset);
			FI
		   FI
		OD
		printf("%  other\n");
		FOR i=0;i<42;i++
		DO IF var[i]
		   THEN switch (i) {
			    case VARB: printc2('b', 0); break;
			    case VARD: printc2('d', 0); break;
			    case VARM: printc2('m', 0); break;
			    case VARS: printc2('s', 0); break;
			    case VART: printc2('t', 0); break;
			    default: printc2((i<=9 ? '0' : 'a'-10) + i, 0);
			}
			printf(" = %%%-10X\n",var[i]);
		   FI
		OD
		break;

	    case 'm': case 'M':
		printmap("? map",&txtmap);
		printmap("/ map",&datmap);
		break;

	    case 0: case '?':
		IF pid
		THEN printf("pcs id = %d\n",pid);
		ELSE prints("no process\n");
		FI
		sigprint(); flushbuf();

	    case 'r': case 'R':
		printregs();
		return;

	    case 'f': case 'F':
		printfregs(modif=='F');
		break;

	    case 'c': case 'C':
		IF modif=='C' THEN printf("Local variables not implemented\n"); FI
		findroutine(&dot, modif=='C');
		break;

	    /*print externals*/
	    case 'e': case 'E':
		symset();
		WHILE (symp=symget())
		DO chkerr();
		   IF (symp->symf)==043 ORF (symp->symf)==044
		   THEN {
			ADDR	a;

			itolws.L = symp->symv;
			a.a_segno = (itolws.I[0]>>8)&0177;
			a.a_offset = (POS)symp->symv;
			printf("%.8s:%12t%%%04x\n", symname, get(&a,DSP));
			}
		   FI
		OD
		break;

	    /*print breakpoints*/
	    case 'b': case 'B':
		printf("breakpoints\ncount%8tbkpt%24tcommand\n");
		FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
		DO IF bkptr->flag
		   THEN printf("%-8.8d",bkptr->count);
			psymoff(&bkptr->loc,ISYM,"%24t");
			comptr=bkptr->comm;
			WHILE *comptr DO printc2(*comptr++, 0); OD
		   FI
		OD
		break;

	    default: error(BADMOD);
	}

}

printmap(s,amap)
STRING	s; MAP *amap;
{
	int file;

	file=amap->ufd;
	printf("%s%12t`%s'\n",s,
	     (file<0 ? "-" : (file==fcor?corfil : symfil)));
	IF segflg THEN printf("segment = <<%%%x>>\n",amap->seg&0xff); FI
	printf("              b1 = %%%-10X ",amap->b1);
	printf("e1 = %%%-10X ",amap->e1);
	printf("f1 = %%%-10X\n",amap->f1);
	printf("              b2 = %%%-10X ",amap->b2);
	printf("e2 = %%%-10X ",amap->e2);
	printf("f2 = %%%-10X",amap->f2);
	printc2(EOR, 0);
}

printfregs(longpr)
{
	REG INT		i;
	REG INT		*p;
	REG INT		j;
	INT		k;
	L_REAL		f;

	p = (INT *)&uptr->u_fpstate;
	FOR i=0; i<8; i++
	DO	f = fpcvt(p);
		printf("fr%d:%5t", i);
		FOR j=0; j<5; j++
		DO printf("%%%04x  ", *p++); OD
		printf("%20.16F\n", f);
	OD
}

printregs()
{
	REG REGPTR	p;
	ADDR		a;
	INT		i;

	FOR p=reglist; p->rname; p += 3
	DO	FOR i=0; i<3; i++
		DO	printf("%s%8t", p[i].rname);
			IF p[i].roffs==19 ANDF segflg
			THEN printf("<<%%%x%>>%%%04x%8t", (regs[18]>>8)&0177, regs[p[i].roffs]);
			ELSE IF p[i].roffs==15 ANDF segflg
			     THEN printf("<<%%%x%>>", curseg);
			     FI
			     printf("%%%04x", regs[p[i].roffs]);
			FI
			printf("%12t");
		OD
		printc2(EOR, 0);
	OD
	printpc();
}

getreg(regnam)
CHAR	regnam;
{
	REG REGPTR	p;
	CHAR		name[4];

	name[0] = regnam;
	IF regnam=='r' ORF regnam=='f' ORF regnam=='s' ORF regnam=='p'
	THEN	IF (name[1]=readchar())==EOR
		THEN	name[1]=0;
		ELSE	name[2]=readchar(); name[3]=0;
			IF name[2]==EOR ORF (!eqstr(name,"fcw") ANDF (name[2]<'0' ORF name[2]>'9'))
			THEN	lp--;
				name[2]=0;
			FI
		FI
		FOR p=reglist; p->rname; p++
		DO	IF eqstr(p->rname, name)
			THEN return(p->roffs);
			FI
		OD
		lp--;
	FI
	return(-1);
}

printpc()
{
	dot.a_segno=(regs[18]>>8)&0177;
	dot.a_offset=(POS)regs[19];
	psymoff(&dot,ISYM,":%16t"); printins(&dot,ISP);
	printc2(EOR, 0);
}

sigprint()
{
	prints(signals[signo]);
}
