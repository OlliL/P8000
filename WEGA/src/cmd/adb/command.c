static char sccsid[] = "@[$]command.c\t3.2  08/25/83 09:48:38 - 87wega3.2";

#include "defs.h"

CHAR		*lp;
INT		symtype;
INT		adrflg;
ADDR		expv;
ADDR		dot;
ADDR		ditto;
ADDR		adrval;
INT		cntflg;
L_INT		cntval;
INT		dotinc;
INT		fsym;
INT		fcor;
L_INT		locval;
L_INT		locmsk;
INT		mkfault;
struct suser	*uptr;
POS		regs[20];
ADDR		svar[4];
L_INT		var[42];
INT		executing;


MSG		BADEQ;
MSG		NOMATCH;
MSG		BADVAR;
MSG		BADCOM;







/* command decoding */

command(buf,defcom)
STRING		buf;
CHAR		defcom;
{
	ADDR		savdot;
	INT		itype, ptype, modifier, regptr;
	BOOL		longpr, eqcom;
	CHAR		wformat[1];
	CHAR		savc;
	L_INT		w;
	STRING		savlp=lp;
	INT		*uaddr;
	IF buf
	THEN IF *buf==EOR
	     THEN return(FALSE);
	     ELSE lp=buf;
	     FI
	FI

	REP
	symtype = ESYM;
	IF adrflg=expr(0)
	THEN	dot=expv;
		ditto=dot;
	FI
	adrval=dot;
	IF rdc()==',' ANDF expr(0)
	THEN cntflg=TRUE; cntval=expv.a_offset;
	ELSE cntflg=FALSE; cntval=1; lp--;
	FI

	IF !eol(rdc())
	THEN lastcom=lastc;
	ELSE IF adrflg==0 THEN dot.a_offset=inkdot(dotinc)->a_offset; FI
	     lp--; lastcom=defcom;
	FI

	switch(lastcom&STRIP) {

	    case '/':
		itype=DSP; ptype=DSYM;
		goto trystar;

	    case '=':
		itype=NSP; ptype=ASYM;
		goto trypr;

	    case '?':
		itype=ISP; ptype=ISYM;
		goto trystar;

	    trystar:
		IF rdc()=='*' THEN lastcom |= QUOTE; ELSE lp--; FI
		IF lastcom&QUOTE
		THEN IF pid
		     THEN error("'/* (?*)' is not a valid file location when program is executing");
		     FI
		     IF itype==ISP ANDF symtype==ISYM
		     THEN error("'?*' is not a valid file location for an instruction symbol");
		     FI
		     itype |= STAR; ptype = (DSYM+ISYM)-ptype;
		ELSE IF symtype==DSYM ANDF itype==ISP
		     THEN error("'?' is not a valid file location for a data symbol");
		     FI
		FI

	    trypr:
		longpr=FALSE; eqcom=lastcom=='=';
		switch (rdc()) {

			case 'm':
			    {/*reset map data*/
			    INT		fcount;
			    MAP		*smap;
			    UNION{MAP *m; L_INT *mp;} amap;

			    IF eqcom THEN error(BADEQ); FI
			    smap=(itype&DSP?&datmap:&txtmap);
			    amap.m=smap; fcount=3;
			    IF itype&STAR
			    THEN amap.mp += 3;
			    FI
			    WHILE fcount-- ANDF expr(0)
			    DO *(amap.mp)++ = expv.a_offset; OD
			    IF rdc()=='?' THEN smap->ufd=fsym;
			    ELIF lastc == '/' THEN smap->ufd=fcor;
			    ELSE lp--;
			    FI
			    }
			    break;

			case 'L':
			    longpr=TRUE;
			case 'l':
			    /*search for exp*/
			    IF eqcom THEN error(BADEQ); FI
			    dotinc=2; savdot=dot;
			    expr(1); locval=expv.a_offset;
			    IF expr(0) THEN locmsk=expv.a_offset; ELSE locmsk = -1L; FI
			    LOOP w=leng(get(&dot,itype));
				 IF longpr
				 THEN w=itol(w,get(inkdot(2),itype));
				 FI
				 IF errflg ORF mkfault ORF (w&locmsk)==locval THEN break; FI
				 dot.a_offset=inkdot(dotinc)->a_offset;
			    POOL
			    IF errflg
			    THEN dot=savdot; errflg=NOMATCH;
			    FI
			    psymoff(&dot,ptype,"");
			    break;

			case 'W':
			    longpr=TRUE;
			case 'w':
			    IF eqcom THEN error(BADEQ); FI
			    wformat[0]=lastc; expr(1);
			    REP  savdot=dot; psymoff(&dot,ptype,":%16t"); exform(1,wformat,itype,ptype);
				 errflg=0; dot=savdot;
				 IF longpr
				 THEN put(&dot,itype,(expv.a_offset>>16)&0xffff);
				 FI
				 put((longpr?inkdot(2):&dot),itype,shorten(expv.a_offset));
				 savdot=dot;
				 printf("=%8t"); exform(1,wformat,itype,ptype);
				 newline();
			    PER  expr(0) ANDF errflg==0 DONE
			    dot=savdot;
			    chkerr();
			    break;

			default:
			    lp--;
			    getformat(eqcom ? eqformat : stformat);
			    IF !eqcom
			    THEN psymoff(&dot,ptype,":%16t");
			    FI
			    scanform(cntval,(eqcom?eqformat:stformat),itype,ptype);
		}
		break;

	    case '>':
		lastcom=0; savc=rdc();
		IF (regptr=getreg(savc)) != -1
		THEN {
		     L_INT	laddr;

		     uaddr = uptr->u_state;
		     uaddr -= 0x7B00;
		     IF regptr==19
		     THEN regs[18] = dot.a_segno<<8;
			  regs[19] = shorten(dot.a_offset);
			  IF segflg==1
			  THEN sptrace(WUREGS,pid,itol(0,uaddr+18),regs[regptr-1]);
			       sptrace(WUREGS,pid,itol(0,uaddr+19),regs[19]);
			  ELSE ptrace(WUREGS,pid,uaddr+18,regs[regptr-1]);
			       ptrace(WUREGS,pid,uaddr+19,regs[19]);
			  FI
		     ELSE regs[regptr] = shorten(dot.a_offset);
			  IF segflg==1
			  THEN IF regptr==15
			       THEN laddr = itol(curseg<<8,uaddr+regptr);
			       ELSE laddr = itol(0,uaddr+regptr);
			       FI
			       sptrace(WUREGS,pid,laddr,regs[regptr]);
			  ELSE ptrace(WUREGS,pid,uaddr+regptr,regs[regptr]);
			  FI
		     FI
		     }
		ELIF (modifier=varchk(savc)) != -1
		THEN	IF modifier&0x8000
			THEN svar[modifier&0x7fff]=dot;
			ELSE var[modifier]=dot.a_offset;
			FI
		ELSE	error(BADVAR);
		FI
		break;

	    case '!':
		lastcom=0;
		unox(); break;

	    case '$':
		lastcom=0;
		printtrace(nextchar());
		break;

	    case ':':
		IF !executing
		THEN executing=TRUE;
		     subpcs(nextchar());
		     executing=FALSE;
		     lastcom=0;
		FI
		break;

	    case 0:
		prints(DBNAME);
		break;

	    default: error(BADCOM);
	}

	flushbuf();
	PER rdc()==';' DONE
	IF buf THEN lp=savlp; ELSE lp--; FI
	return(adrflg ANDF dot.a_offset!=0);
}
