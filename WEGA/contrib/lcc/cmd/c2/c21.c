 /*
 * C object code improver-- second part
 */

#include "c2.h"
#define NIL (char *)0

unsigned hexa ();
rmove()
{
	register struct node *p;
	register char *p1, *p2p;
	static char p2[100];
	register int r;
	register  r1, i;

	if(debug > 1)
		printf("rmove works\n");
	for (p=first.forw; p!=0; p = p->forw) {

	if(debug > 2){
		printf("rmove works on:");
		dbprint(p);
	}
	switch (p->op) {

	case LD:
	case LDK:
		dualop(p);
		if(equstr(regs[RT1],regs[RT2])) {
			if(debug > 2)
				puts("elim 1");
			elim(p);
			++redunm;
			break;
		}
ld1:
		if (( r = findrand(regs[RT2],regt[RT2])) >= 0 && regt[r]==regt[RT2])
			if( r == isreg(regs[RT1]) && regt[r]==regt[RT1]) {
				elim(p);
			if(debug > 2)
				puts("elim 2");
				++redunm;
				break;
			}

		if((r = findrand(regs[RT1],regt[RT1])) >= 0 && regt[r]==regt[RT1])
			if(r==isreg(regs[RT2]) && regt[r]==regt[RT2]) {
				if(debug > 2)
					puts("elim 3");
				elim(p);
				++redunm;
				break;
			}

clr:
		if((r=isreg(regs[RT1]))>=0 && regp[r]!=0 && regt[r]==regt[RT1]){
			if(debug > 2)
				puts("elim 4");
			if(!teil(regs[RT1],regs[RT2]))
				elim(regp[r]);
			clrr(r,regt[RT1]);
			if(isreg(ccloc)==(r))
				clrcc();
		}

		if ((p->op==CLR||equstr(regs[RT2],"#0")) && p->subop != LONG
			&& isreg(regs[RT1])>=0) {
			p->op = XOR;
			regs[RT2][0] = ',';
			i = 0;
			do
				regs[RT2][i+1] = regs[RT1][i];
			while(regs[RT1][i++]);
			p->code = copy(2,regs[RT1],regs[RT2]);
			nchange++;
			break;
		}

		if(p->op==LD && p->subop==LONG && r >= 2 && r < 7 &&
		   p->forw && (p->forw->op==ADD || p->forw->op==INC)
		   && p->forw->subop==0 &&
		   p->forw->forw && p->forw->forw->op==LD) {
			/* ldl rri,FP; add r(i+1),#ddd; ldx r(x)i,@rri */
			/* ldl rri,FP; inc r(i+1),#ddd; ldx r(x)i,@rri */
			i = isreg(regs[RT2]);
			if(i>=0 && i<=14 && regc=='r') {
				dualop(p->forw);
				r1 = isreg(regs[RT1]);
				if(r1 == (r+1) && regs[RT2][0] == '#') {
					p2p = p2;
					*p2p++ = ',';
					if(i==12)
						p1 = "FP(";
					else if(i==14)
						p1 = "SP(";
					else if(i==10)
						p1 = "rr10(";
					else
						p1 = "rrx(";
					while(*p2p++ = *p1++);
					if(i < 10)
						*(p2p-3) = i + '0';
					p2p--;
					p1 = regs[RT2];
					while(*p2p++ = *p1++);
					*(p2p-1) = ')';
					*p2p = 0;
					dualop(p->forw->forw);
					r1 = isreg(regs[RT1]);
					if(r1==r || r1 == (r+1)){
						elim(p);
						elim(p->forw);
						p = p->forw->forw;
						p->code = copy(2,regs[RT1],p2);
						if(debug>2) {
							printf("neu erzeugt: ");
							dbprint(p);
						}
						nchange++;
						p = p->back;
						break;
					}
				}
			}
		}
		dualop(p);
		if(p->op != LDK) {
			repladdr(p, 0);
			dualop(p);
		}
mfldil:
		r = isreg(ccloc);
		r1 = isreg(regs[RT1]);
		if(r<0 && r1<0 || r==r1 && ccloct==regt[RT1] ||
		   ccloct=='r' && r==(r1 & ~01) ||
		   regt[RT1]=='r' && r1==(r & ~01))
			clrcc();
		r = isreg(regs[RT2]);
		dest(regs[RT1],regt[RT1]);
		if(r1 < 0)
			source(regs[RT1],regt[RT1]);
		source(regs[RT2], regt[RT2]);
		if (r >= 0)
			if (r1 >= 0)
				if(r1 < r)
					savereg(r1,regs[RT2],regt[RT2],p);
				else
					savereg(r1, regs[r],regt[r], p);
			else { if(regs[r][0]==0)
			       	savereg(r, regs[RT1],regt[RT1], NIL);
			       else 
				savereg(r+NREG, regs[RT1],regt[RT1], NIL);
			}
		else
			if (r1 >= 0)
				savereg(r1, regs[RT2], regt[RT2], p);
			else
				setcon(regs[RT2], regs[RT1], regt[RT1]);
		break;

	case INC:
	case DEC:
	case SET:
	case RES:
	case SRL:
	case SLL:
	case ASR:
	case ASL:
		dualop(p);
		if(p->op==INC && equstr(regs[RT1],"SP") && p->forw
		&& p->forw->op==PUSH && regs[RT2][0]=='#') {
			dualop(p->forw);
			if(regs[RT2][0]!='r' && regs[RT2][0]!='#')
				goto ende;
			dualop(p);
			if(regs[RT2][1]=='%')
				r1 = hexa(&(regs[RT2][2]));
			else
				r1 = hexa(&(regs[RT2][1]));
			if(p->forw->subop==LONG && r1>3)
				r1 -= 4;
			else if(r1>1 && p->forw->subop != LONG)
				r1 -= 2;
			else goto ende;
			if(!r1){
				if(debug > 2)
					puts("elim 5");
				elim(p);
			}else
				sprintf(regs[RT2],",#%%%x",r1);
			p->forw->op = LD;
			p->code = copy(2,regs[RT1],regs[RT2]);
			redunm++;
			break;
		}
ende:
		dualop(p);
		dest(regs[RT1],regt[RT1]);
		source(regs[RT2], regt[RT2]);
		source(regs[RT1], regt[RT1]);
		if(p->op==SET||p->op==RES||p->op==PUSH||p->op==POP)
			clrcc();
		else
			setcc(regs[RT1],regt[RT1]);
		break;

	case ADD:
	case SUB:
	case OR:
	case XOR:
	case MUL:
	case DIV:
	case SDA:
	case SDL:
	case PUSH:
	case POP:
	dble:
		dualop(p);
		if(p->op==POP && (r=isreg(regs[RT1]))>=0 && regp[r]!=0) {
			if(regt[r]==regt[RT1]) {
				if(debug > 2)
					puts("elim 6");
				elim(regp[r]);
				clrr(r,regt[RT1]);
				if(isreg(ccloc)==r)
					clrcc();
			}
		}
		if(p->op==OR && regs[RT2][0]=='#' && p->subop==0) {
			p1 = regs[RT2] + 1;
			if(*p1 == '%')
				++p1;
			if((i = ispow2a(hexa(p1)))>=0) {
				p->op = SET;
				setadr(p,i);
				clrcc();
				++nchange;
				break;
			}
		}
		if(p->op==XOR &&(r=isreg(regs[RT1]))>=0 &&
		   equstr(regs[RT1],regs[RT2])) {
			if(regp[r] && regt[r]==regt[RT1]) {
				if(debug > 2)
					puts("elim 7");
				elim(regp[r]);
				clrr(r,regt[RT1]);
				if(isreg(ccloc)==r)
					clrcc();
			}
			if(regt[r]==regt[RT1] &&
				(regt[r]!='h' && equstr(regs[r],"#0") ||
				 regt[r]=='h' && equstr(regh[r],"#0"))
			||regt[r]=='l'&&regt[RT1]=='h'&&equstr(regh[r],"#0")
			||regt[r]=='h'&&regt[RT1]=='l'&&equstr(regs[r],"#0")){
				if(debug > 2)
					puts("elim 8");
				elim(p);
				++redunm;
				break;
			} else if(regt[RT1]=='h') {
				if(regt[r]!='l')
					clrr(r,'h');
				regh[r][0] = '#';
				regh[r][1] = '0';
				regh[r][2] = 0;
				regp[r] = p;
			} else {
				savereg(r,"#0",regt[RT1], p);
			}
		} else {
			repladdr(p, 0);
			dualop(p);
			dest(regs[RT1],regt[RT1]);
			source(regs[RT2], regt[RT2]);
			source(regs[RT1], regt[RT1]);
		}
		setcc(regs[RT1],regt[RT1]);
		break;

	case NEG:
	case CLR:
	case COM:
	case SXT:
		singop(p);
		if (p->op==CLR)
			if ((r = isreg(regs[RT1])) >= 0)
				goto clr;
			else
				setcon("#0", regs[RT1], regt[RT1]);
		dest(regs[RT1],regt[RT1]);
		source(regs[RT1],regt[RT1]);
		if(p->op!=CLR && p->op!=SXT)
			setcc(regs[RT1], regt[RT1]);
		break;

	case TST:
		singop(p);
tst:
		repladdr(p, 1);
		singop(p);
		if ((ccloct==regt[RT1] && equstr(regs[RT1],ccloc)) ||
			p->forw && p->forw->op > CBR) {
			if(debug > 2)
				puts("elim 9");
			elim(p);
			nrtst++;
		}
		else
			setcc(regs[RT1], regt[RT1]);
		source(regs[RT1], regt[RT1]);
		break;

	case CMP:
		if(p->forw->op==CBR && (r = p->forw->subop) < JLO) {
			dualop(p);
			if(equstr(regs[RT2],"#0")) {
				if(r == JLT)
					p->forw->subop = MI;
				if(r == JGE)
					p->forw->subop = PL;
				if(p->forw->subop < JLE) {
					p->op = TST;
					regs[RT2][0] = 0;
					p->code = copy(1,regs[RT1]);
					goto tst;
				}
			}
		}

	case BIT:
		if(p->forw->op > CBR) {
			if(debug > 2)
				puts("elim 10");
			elim(p);
			break;
		}

	case AND:
		dualop(p);
		if(p->op==AND && regs[RT2][0]=='#' && p->subop==0) {
			p1 = regs[RT2] + 1;
			if(*p1 == '%')
				++p1;
			if ((i = ispow2a(~hexa(p1)))>=0) {
				p->op = RES;
				setadr(p,i);
				clrcc();
				++nchange;
				break;
			}
		}

		if(p->op==AND) {
		     if(equstr(regs[RT2],"#%ffff")||equstr(regs[RT2],"#-%1")){
				p->op = TST;
				regs[RT2][0] = 0;
				regt[RT2] = 0;
				p->code = copy(1, regs[RT1]);
				nchange++;
				nsaddr++;
				goto tst;
			} else
			if(equstr(regs[RT2],"#%0")) {
				p->op = CLR;
				regs[RT2][0] = 0;
				regt[RT2] = 0;
				p->code = copy(1, regs[RT1]);
				nchange++;
				nsaddr++;
				goto clr;
			}
		dest(regs[RT1],regt[RT1]);
		}
		if(p->op!=BIT &&(p->op!=CMP || regs[RT2][0]!='#')) {
			repladdr(p, 1);
			dualop(p);
		}
		if(p->op!=AND)
			clrcc();
		else 
			setcc(regs[RT1], regt[RT1]);
		source(regs[RT1], regt[RT1]);
		source(regs[RT2], regt[RT2]);
		break;

	case CBR:

		if (p->back->op==TST || p->back->op==CMP) {
			if (p->back->op==TST) {
				singop(p->back);
				savereg(RT2, "#0", regt[RT1], p->back);
			} else
				dualop(p->back);
			r = compare(p->subop, findcon(RT1), findcon(RT2));
			if (r==0) {
				if(p->forw->op == CBR ) {
					if(debug > 2)
						puts("elim 11");
					elim(p);
					clrcc();
				} else {
					p->back->back->forw = p->forw;
					p->forw->back = p->back->back;
					clrcc();
				}
				decref(p->ref);
				p = p->back->back;
				nchange++;
			} else if (r>0) {
				p->op += 2;
				p->subop = 0;
				p->back->back->forw = p;
				p->back = p->back->back;
				p = p->back;
				nchange++;
			}
		}

	case JMP:
		redunbr(p);
		clrp();
		break;

	case END:
	case EROU:
	case CALL:
		for(r = 0; r<8; ++r)
			if (regp[r]!=0 && regs[r][0]=='r') {
				if(debug > 2)
					puts("elim 12");
				elim(regp[r]);
				if(isreg(ccloc)==r)
					clrcc();
		}

	default:
		clearreg();
	}
	}
}

jumpsw()
{
	register struct node *p, *p1;
	register t;
	register struct node *tp;
	register nj;

	t = 0;
	nj = 0;
	for (p=first.forw; p!=0; p = p->forw)
		p->refc = ++t;
	for (p=first.forw; p!=0; p = p1) {
		if(debug > 2){
			printf("jumpsw works on:");
			dbprint(p);
		}
		p1 = p->forw;
		if ((p->op == CBR || p1->op == JMP) && p->ref && p1->ref
		 && Abs(p->refc - p->ref->refc) > Abs(p1->refc - p1->ref->refc)) {
			if (p->ref==p1->ref ||p->labno==0 || p1->labno==0)
				continue;
			p->subop = revbr[p->subop];
			tp = p1->ref;
			p1->ref = p->ref;
			p->ref = tp;
			t = p1->labno;
			p1->labno = p->labno;
			p->labno = t;
			nrevbr++;
			nj++;
		}
	}
	return(nj);
}

addsob()
{
	register struct node *p, *p1;

	for (p = &first; (p1 = p->forw)!=0; p = p1) {
		if(debug > 2){
			printf("addsob works on:");
			dbprint(p);
		}
		switch(p->op) {
			case CLR:
			case AND:
			case OR:
			case BIT:
			case RES:
			case SET:
			case CMP:
			case TST:
			case LD:
			case LDK:
				p1 = nonlab(p->forw);
		           	if(p1 && p1->op==p->op
					 && equstr(p->code,p1->code)){
					if(debug > 2)
						puts("elim 13");
					elim(p);
					p = p->back;
					if(!(p1 = p->forw))
						return;
				}
		}

		if(p1 && p1->op == TST) {
			switch(p->op) {
			case CLR:
				if(p1->forw && p1->forw->op == CBR 
					&& equstr(p->code,p1->code))
					switch (p1->subop) {
						case JNE:
						case JGT:
						case JHI:
							if(debug > 2)
								puts("elim 14");
							elim(p1->forw);
							elim(p1);
							if(!(p1=p->forw))
								return;
						case JEQ:
						case JLOS:
						case JLE:
							p=p1->forw;
							if(debug > 2)
								puts("elim 15");
							elim(p1);
							p->op += 2;
							p->subop=0;
							if(!p)
								return;
					}
				else break;

			case COM:
			case INC:
			case DEC:
			case NEG:
				if(equstr(p->code,p1->code)) {
					if(debug > 2)
						puts("elim 16");
					elim(p1);
					nrtst++;
					if((p1 = p1->forw) == 0)
						return;
				}
			}
		}
		if (p->op==TST && p1->op > CBR) {
			if(debug > 2)
				puts("elim 17");
			elim(p);
			nrtst++;
			continue;
		}

		if (p->op==DEC && isreg(p->code)>=0
		 && p1->op==CBR && p1->subop==JNE) {
			if (p->refc < p1->ref->refc)
				continue;
			if (toofar(p1))
				continue;
			dualop(p);
			p->code = copy(1,regs[RT1]);
			p->labno = p1->labno;
			p->op = DJNZ;
			p->subop = 0;
			p1->forw->back = p;
			p->forw = p1->forw;
			nsob++;
		}
	}
}

toofar(ap)
struct node *ap;
{
	register struct node *p, *p1;
	register len;

	p = ap;
	for(p1 = first.forw; p1 != p ; p1 = p1->forw)
		if (p1 == p->ref)
			break;

	if(p1 == p->ref) {
		p = p1;
		p1 = ap;
	} else
		p1 = p->ref;
	len = 0;
	for (p = p->forw; p && p!=p1; p = p->forw)
		len += ilen(p);
	if (len < 256)
		return(0);
	return(1);
}

ilen(p)
register struct node *p;
{
	if (p->op == 0)
		return(0);

	switch (p->op) {
	case LABEL:
	case DLABEL:
	case CP:
	case EROU:
	case EVEN:
	case CODE:
	case BSS:
	case CD:
	case CMT:
		return(0);


	case SRL:
	case SLL:
	case ASR:
	case ASL:
	case SDA:
	case LDI:
	case LDD:
		return(4);

	case LDK:
	case DJNZ:
		return(2);

	default:
		dualop(p);
		switch(p->op) {
		case BIT:
		case RES:
		case SET:
		case INC:
		case DEC:
			return(2 + adrlen(regs[RT1]));

		case LD:
		case CMP:
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			if(p->subop==LONG && regs[RT2][0]=='#')
				return(6);

		default:
			return(2 + adrlen(regs[RT1]) + adrlen(regs[RT2]));
		}
	}
}

adrlen(s)
register char *s;
{
	if (*s == 0)
		return(0);
	if (*s=='r' || *s=='S' && *(s+1)=='P')
		return(0);
	if (*s=='r' || *s=='F' && *(s+1)=='P')
		return(0);
	if (*s=='r' || *s=='s' && *(s+1)=='p')
		return(0);
	if (*s=='r' || *s=='f' && *(s+1)=='p')
		return(0);
	if (*s=='(' && (*(s+1)=='r' || *(s+1)=='F' || *(s+1)=='f'))
		return(0);
	if (*s=='@')
		return(0);
	if (*s=='|')
		return(2);
	return(4);
}

Abs(x)
{
	return(x<0? -x: x);
}

equop(ap1, p2)
struct node *ap1;
register struct node *p2;
{
	register char *cp1, *cp2;
	register struct node *p1;

	p1 = ap1;
	if (p1->op!=p2->op || p1->subop!=p2->subop)
		return(0);
	if (p1->op!=p2->op || p1->subop!=p2->subop)
		return(0);
	if (p1->op>0 && p1->op<LD)
		return(0);
	cp1 = p1->code;
	cp2 = p2->code;
	if (cp1==0 && cp2==0)
		return(1);
	if (cp1==0 || cp2==0)
		return(0);
	while (*cp1 == *cp2++)
		if (*cp1++ == 0)
			return(1);
	return(0);
}

decref(p)
register struct node *p;
{
	if(debug > 2){
		printf("decref works on:");
		dbprint(p);
	}
	if (--p->refc <= 0) {
		if(debug > 2)
			puts("elim 18");
		nrlab++;
		elim(p);
	}
}

struct node *
nonlab(p)
register struct node *p;
{
	while (p && p->op==LABEL)
		p = p->forw;
	return(p);
}

char *
alloc(n)
register n;
{
	register char *p;

	n++;
	n &= ~01;
	if (lasta+n >= lastr) {
		if (addrseg == 9) {
			fprintf(stderr, "lpopt: out of space\n");
			exit(1);
		}
		addrseg++;
		if(address[addrseg] == (char  *)0) {
			lasta = (char *)malloc(0xfff8);
			address[addrseg] = lasta;
			lastr = lasta +  0xfff8;
		}
	}
	p = lasta;
	lasta += n;
	return(p);
}

clearreg()
{
	register int i;

	for (i=0; i<FREG; i++) {
		regs[i][0] = '\0';
		regt[i] = 0;
		regp[i] = 0;
	}
	conloc[0] = 0;
	conloct = 0;
	conval[0] = 0;
	convalt = 0;
	clrcc();
	for(i = 0; i < 8; i++)
		regh[i][0] = 0;
}

savereg(ai, as, at, ap)
register ai;
register struct node *ap;
char *as;
char at;
{
	register char *p, *s, *sp;

	sp = p = regs[ai];
	s = as;
	clrr(ai,at);
	regt[ai] = at;
	regp[ai] = ap;

	while (*p++ = *s) {
		if (s[0]=='(' && regnum(s+1) != 14 ) {
			*sp = 0;
			return;
		}
		if (*s++ == ',')
			break;
	}
	*--p = '\0';
}

dest(as, at)
char *as;
char at;
{
	register char *s;
	register int i, j, j1;
	char *s1 = "rr10";

	s = as;
	if ((j = isreg(s)) >= 0) {
		clregrr(j,at);
		clrr(j,at);
		clrr(j+NREG,at);
		j1 = j & ~1;
		if(j1 < 10) {
			s1[2] = j1 + '0';
			s1[3] = 0;
		} else {
			s1[2] = '1';
			s1[3] = j1 - 10 + '0';
		}
	} else
		s1[3] = '4';
	for (i = 0; i < FREG; i++) {
		if(j>=0 && j==isreg(regs[i]))
			clrr(i,at);
		else if(equstr(s,regs[i])||equstr(s1,regs[i]))
			clrr(i,at);
		else if(teil(s,regs[i])|| teil(s1,regs[i]))
			clrr(i,at);
		else if(teil(regs[i],s)|| teil(regs[i],s1))
			clrr(i,at);

	}
	if ((i = findrand(s,at)) >= 0)
		if(regt[i]==at)
			clrr(i,at);
	while (*s) {
		if ((*s=='(' && (*(s+1)!='F' || *(s+2)!='P' || *(s+1)!='f'
			|| *(s+2)!='p' || *(s+1)!='s')) || *s++=='@') {
			for (i=0; i<FREG; i++) {
				if (regs[i][0] != '#' && regs[i][0] != 'r')
					clrr(i,at);
				conloc[0] = 0;
				conloct = 0;
			}
			return;
		}
	}
}

source(as, at)
char *as;
char at;
{
	register char *s;
	register int i;

	s = as;
	if ((i = isreg(s)) >= 0) {
		regp[i] = 0;
		regp[i+NREG] = 0;
		if((i & 1) && regt[i-1]=='r') {
			regp[i-1] = 0;
			regp[i+NREG-1] = 0;
		}
		if(at=='r' || at=='q') {
			regp[i+1] = 0;
			regp[i+1 + NREG] = 0;
		}
		if(at=='q') {
			regp[i+2] = 0;
			regp[i+3] = 0;
			regp[i+2 + NREG] = 0;
			regp[i+3 + NREG] = 0;
		}
	} else
	if(*s=='@' && (i = isreg(++s))>=0) {
		regp[i] = 0;
		regp[i+NREG] = 0;
	}
	else  {
		while(*s && *s!='(')
			++s;
		if(*s && *s=='(' && (i = regnum(++s))>=0) {
			regp[i] = 0;
			regp[i+NREG] = 0;
		}
	}
}

singop(ap)
struct node *ap;
{
	register char *p1, *p2;

	p1 = ap->code;
	p2 = regs[RT1];
	while (*p2++ = *p1++);
	settyp(RT1,ap);
	regs[RT2][0] = 0;
	regt[RT2] = 0;
}


dualop(ap)
struct node *ap;
{
	register char *p1, *p2;
	register struct node *p;

	p = ap;
	p1 = p->code;
	p2 = regs[RT1];
	while (*p1 && *p1!=',')
		*p2++ = *p1++;
	settyp(RT1,ap);
	*p2++ = 0;
	p2 = regs[RT2];
	*p2 = 0;
	if (*p1++ !=',') {
		regt[RT2] = 0;
		return;
	}
	while (*p2++ = *p1++);
	settyp(RT2,ap);
}

findrand(as, at)
char *as;
char at;
{
	register int i, j;

	if(*as==0)
		return(-1);
/*
	if((i = isreg(as))>=0 && (j = isreg(regs[i]))>=0){
		if(j>i && regt[j]==regt[i])
			return ( j );
		else
			return(-1);
	}
*/
	for (i = 0; i < FREG; i++) {
		if (equstr(regs[i], as)) {
			if(regt[i] != at)
				continue;
			if((regt[i & ~01] == 'r' && at != 'r') ||
				(regt[i & ~03] == 'q' && at != 'q'))
				continue;
			else
				return(i);
		}
	}
	return(-1);
}

isreg(as)
char *as;
{
	register char *s;

	s = as;
	if(s==0 || (*s)==0)
		return(-1);
	return(regnum(s));
}

check()
{
	register struct node *p, *lp;

	lp = &first;
	for (p=first.forw; p!=0; p = p->forw) {
		if (p->back != lp)
			abort();
		lp = p;
	}
}


repladdr(p, f)
register f;
struct node *p;
{
	register r, i;
	register r1, r2;
	register char *p1, *p2;
	static char rt1[50], rt2[50];

	if(debug > 2){
		printf("repladdr works on:");
		dbprint(p);
	}
	if (f)
		r = findrand(regs[RT1], regt[RT1]);
	else
		r = -1;
	r1 = findrand(regs[RT2], regt[RT2]);
	if (r1 >= NREG)
		r1 -= NREG;
	if (r >= NREG)
		r -= NREG;
	if (r>=0 || r1>=0) {
		p2 = regs[RT1];
		for (p1 = rt1; *p1++ = *p2++;);
		if (regs[RT2][0]) {
			p1 = rt2;
			*p1++ = ',';
			for (p2 = regs[RT2]; *p1++ = *p2++;);
		} else
			rt2[0] = 0;
		if (r>=0 && (regs[r][0]==0 || regt[r]==regt[RT1]) && 
		((r2 = isreg(regs[RT1]))<0 || r2<r && regt[r2]==regt[RT1])) {
			rt1[0] = 'r';
			i = 1;
			if(regt[RT1] != 0)
				rt1[i++] = regt[RT1];
			if( r > 9 ) {
				rt1[i++] = '1';
				rt1[i++] = r - 10 + '0';
			}else
				rt1[i++] = r + '0';
			rt1[i] = 0;
			nsaddr++;
			nchange++;
		}
		if (r1>=0 && (regs[r1][0]==0 || regt[r1]==regt[RT2]) && 
		((r2 = isreg(regs[RT2]))<0 ||r2<r1 && regt[r2]==regt[RT2])) {
			rt2[1] = 'r';
			i = 2;
			if (regt[RT2] != 0)
				rt2[i++] = regt[RT2];
			if ( r1 > 9 ) {
				rt2[i++] = '1';
				rt2[i++] = r1 - 10 + '0';
			} else
				rt2[i++] = r1 + '0';
			rt2[i] = 0;
			nsaddr++;
			nchange++;
		}
		p->code = copy(2, rt1, rt2);
		if(debug > 2)
			printf("repladdr liefert '%s'\n",p->code);
	}
}

redunbr(p)
register struct node *p;
{
	register struct node *p1;
	register char *ap1;
	register char *ap2;

	if ((p1 = p->ref) == 0)
		return;
	p1 = nonlab(p1);
	if (p1->op==TST) {
		singop(p1);
		savereg(RT2, "#0", regt[RT1], p1);
	} else if (p1->op==CMP)
		dualop(p1);
	else
		return;
	if (p1->forw->op!=CBR)
		return;
	ap1 = findcon(RT1);
	ap2 = findcon(RT2);
	p1 = p1->forw;
	if (compare(p1->subop, ap1, ap2)>0 && p1->labno) {
		nredunj++;
		nchange++;
		decref(p->ref);
		p->ref = p1->ref;
		p->labno = p1->labno;
		p->ref->refc++;
	}
}

char *
findcon(i)
register i;
{
	register char *p;
	register r;

	p = regs[i];
	if (*p=='#')
		return(p);
	if ((r = isreg(p)) >= 0 && regt[r]==regt[i])
		return(regs[r]);
	if (equstr(p, conloc) && conloct==regt[i])
		return(conval);
	return(p);
}

compare(oper, cp1, cp2)
register oper;
char *cp1, *cp2;
{
	register unsigned n1, n2;

	if (*cp1++ != '#' || *cp2++ != '#')
		return(-1);
	n2 = hexa(cp2);
	n1 = hexa(cp1);
	do {
		if (*cp1++ != *cp2)
			return(-1);
	} while (*cp2++);
	switch(oper) {

	case JEQ:
		return(n1 == n2);
	case JNE:
		return(n1 != n2);
	case JLE:
		return((int)n1 <= (int)n2);
	case JGE:
		return((int)n1 >= (int)n2);
	case JLT:
		return((int)n1 < (int)n2);
	case JGT:
		return((int)n1 > (int)n2);
	case JLO:
		return(n1 < n2);
	case JHI:
		return(n1 > n2);
	case JLOS:
		return(n1 <= n2);
	case JHIS:
		return(n1 >= n2);
	case PL:
		return((int)n1 >= 0);
	case MI:
		return((int)n1 < 0);
	}
	return(-1);
}

setcon(ar1, ar2, t)
char *ar1, *ar2;
char t;
{
	register char *cl, *cv, *p;

	cl = ar2;
	cv = ar1;
	if (*cv != '#')
		return;
	if (!natural(cl))
		return;
	p = conloc;
	while (*p++ = *cl++);
	p = conval;
	while (*p++ = *cv++);
	conloct = t;
	convalt = t;
}

equstr(ap1, ap2)
char *ap1, *ap2;
{
	register char *p1, *p2;

	p1 = ap1;
	p2 = ap2;
	if(!p1) {
		if(!p2)
			return 1;
		else
			return 0;
	}
	if(*p1 == '#' && *p2 == '#') {
		if(*++p1 =='%')
			++p1;
		if(*++p2 =='%')
			++p2;
	}
	do {
		if (*p1++ != *p2)
			return(0);
	} while (*p2++);
	return(1);
}

setcc(ap, t)
char *ap;
char t;
{
	register char *p, *p1;

	p = ap;
	ccloct = t;
	if (!natural(p)) {
		clrcc();
		return;
	}
	p1 = ccloc;
	while (*p1++ = *p++);
}

clrcc()
{
	ccloc[0] = 0;
	ccloct = 0;
}

natural(ap)
char *ap;
{
	register char *p;

	p = ap;
	if (*p == '@' || *p == '(')
		return(0);
	while (*p++);
	p--;
	if ( *--p ==')' && *--p != 'P')
		return(0);
	return(1);
}

unsigned
hexa(cp)
char *cp;
{
	unsigned register n;
	n = 0;
	while(*cp >= '0' && *cp <= '9' || *cp >= 'a' && *cp <= 'f') {
		n <<= 4;
		if(*cp < 'a')
			n += *cp++ - '0';
		else
			n += *cp++ - 'W';
	}
	return(n);
}

elim(p)
register struct node *p;
{
	if(debug){
		printf("elim works on:");
		dbprint(p);
	}
	p->back->forw = p->forw;
	p->forw->back = p->back;
	nchange++;
}

regnum(r)
char *r;
{
	if (*r != 'r' && *r != 'F' && *r != 'f' && *r != 'S' && *r!='s')
		return(-1);
	regc = 0;

	if (*r =='S' && *(r+1) == 'P' && *(r+2) == 0) {
		regc = 'r';
		return(14);
	}

	if (*r =='s' && *(r+1) == 'p' && *(r+2) == 0)
		return(15);

	if (*r =='F' && *(r+1) == 'P' && *(r+2) == 0) {
		regc = 'r';
		return(12);
	}

	if (*r =='f' && *(r+1) == 'p' && *(r+2) == 0)
		return(13);

	if(*r++ != 'r')
		return(-1);
	if(*r == 'l' || *r== 'r' || *r == 'h' || *r == 'q')
		regc = *r++;

	if(*(r+1) >= '0' && *(r+1) <= '5' && *r == '1') {
		r++;
		if(*(r+1) == 0)
			return(10 + *r++ - '0');
		else
			return(-1);

		}
	else if(*r >= '0' && *r <= '9' && *(r+1) == 0)
			return(*r++ - '0');
	return(-1);

}

settyp(a,ap)
register a;
register struct node *ap;
{
	if(regs[a][0] == 'r' && regs[a][1] > '9')
		regt[a] = regs[a][1];
	else if (ap->subop==LONG)
		regt[a] = 'r';
	else if (ap->subop==BYTE)
		regt[a] = 'l';
	else regt[a] = 0;
}

clrr(i,at)
register i;
char at;
{
	register j,k;
	k = i&~01;
	j = i&~03;

	if((i & 1) && regt[i-1]=='r') {
		regs[i-1][0] = 0; 
		regt[i-1]=0;
		regp[i-1]=0;
	}
	if(i < 8 && at!='l')
		regh[i][0] = 0;
	if(regt[j]=='q' || at=='q') {
		regs[j][0] = 0; regt[j] = 0; regp[j] = 0;
		regs[++j][0] = 0; regt[j] = 0; regp[j] = 0;
		regs[++j][0] = 0; regt[j] = 0; regp[j] = 0;
		regs[++j][0] = 0; regt[j] = 0; regp[j] = 0;
	}
	else if (regt[k]=='r' || at=='r') {
		regs[k][0] = 0; regt[k] = 0; regp[k] = 0;
		regs[++k][0] = 0; regt[k] = 0; regp[k] = 0;
	}
	else {
		regs[i][0] = 0;
		regt[i] = 0;
		regp[i] = 0;
	}
	regp[i] = 0;
}

clrp()
{
	register i;
	for( i=0; i<30; i++)
	regp[i] = 0;
}

ispow2a(a)
register unsigned a;
{
	register i;
	if(a>0 && a<=0x8000 && (a&(a-1))==0) {
		for(i = 0; a >>= 1; i++);
		return(i);
	}
	else
		return(-1);
}

setadr(p, i)
register struct node *p;
register i;
{
	static char rt[5];
	regt[RT2] = 0;
	rt[0] = ',';
	rt[1] = '#';
	rt[2] = '%';
	if(i < 10)
		rt[3] = '0' + i;
	else
		rt[3] = 'a' + i - 10;
	rt[4] = 0;
	p->code = copy(2, regs[RT1], rt);
}

/* ist r teilstring in s */
teil(r,s)
char *r,*s;
{
	static char f[5];
	char *fp, *sp, *osp = s;
	int len,i;
	if(equstr(r,s))
		return(0);
	len = strlen(r);
	while((sp = strchr(osp,*r))!=NULL) {
		osp = sp + 1;
		fp = f;
		for(i=0; i < len; i++)
			*fp++ = *sp++;
		*fp = 0;
		if(equstr(f,r))
			return(1);
	}
	return(0);
}

clregrr(r, t)
register r;
char t;
{
	char *s1 = "rr10";
	int j1, j;

again:
	j1 = r & ~1;
	if(j1 < 10) {
		s1[2] = j1 + '0';
		s1[3] = 0;
	} else {
		s1[2] = '1';
		s1[3] = j1 - 10 + '0';
	}
	for(j = 0; j < NREG; j++)
		if(teil(s1, regs[j]))
			clrr(j, regt[j]);
	if(t == 'q') {
		t = 0;
		r += 2;
		goto again;
	}
}
