static char sccsid[] = "@[$]format.c\t3.2  08/25/83 09:48:50 - 87wega3.2";

#include "defs.h"
#include <ctype.h>

INT		maxoff;
STRING		sigint;
INT		sigqit;


MSG		BADMOD;
MSG		NOFORK;
MSG		ADWRAP;


scanform(icount,ifp,itype,ptype)
L_INT		icount;
STRING		ifp;
{
	STRING		fp;
	CHAR		modifier;
	INT		fcount, init=1;
	ADDR		savdot;

	WHILE icount
	DO  fp=ifp;
	    IF init==0 ANDF findsym(&dot,ptype)==0 ANDF maxoff
	    THEN printf("\n%s:%16t",symname);
	    FI
	    savdot=dot; init=0;

	    /*now loop over format*/
	    WHILE *fp ANDF errflg==0
	    DO  IF isdigit(modifier = *fp)
		THEN fcount=0;
		     WHILE isdigit(modifier = *fp++)
		     DO fcount *= 10;
			fcount += modifier-'0';
		     OD
		     fp--;
		ELSE fcount=1;
		FI

		IF *fp==0 THEN break; FI
		fp=exform(fcount,fp,itype,ptype);
	    OD
	    dotinc=dot.a_offset-savdot.a_offset;
	    dot=savdot;

	    IF errflg
	    THEN IF icount<0
		 THEN errflg=0; break;
		 ELSE error(errflg);
		 FI
	    FI
	    IF --icount
	    THEN dot.a_offset=inkdot(dotinc)->a_offset;
	    FI
	    IF mkfault THEN error(0); FI
	OD
}

STRING	exform(fcount,ifp,itype,ptype)
INT		fcount;
STRING		ifp;
{
	/* execute single format item `fcount' times
	 * sets `dotinc' and moves `dot'
	 * returns address of next format item
	 */
	POS		w;
	ADDR		savdot;
	L_INT		wx;
	STRING		fp;
	CHAR		c, modifier, longpr;
	INT		w1, w2;
	L_REAL		fw = 0;
	struct{
		L_INT	sa;
		INT	sb,sc;
	};
	struct{
		REAL	fa;
		L_INT	fb;
	};

	WHILE fcount>0
	DO	fp = ifp; c = *fp;
		longpr=((BOOL)(c>='A')&(BOOL)(c<='Z'))|(BOOL)(c=='f');
		IF itype==NSP ORF *fp=='a'
		THEN	{
			UNION{L_INT l; CHAR c[4];} u;

			u.l = dot.a_offset;
			u.c[1] = 0;
			u.c[0] = dot.a_segno;
			wx = u.l;
			}
			w = shorten(dot.a_offset);
		ELSE w=get(&dot,itype);
		     IF longpr
		     THEN wx=itol(w,get(inkdot(2),itype)); wx &= 0x7fffffff;
		     ELSE wx=w;
		     FI
		FI
		IF c=='F'
		THEN w1=get(inkdot(4),itype);
		     w2=get(inkdot(6),itype);
		FI
		IF errflg THEN return(fp); FI
		IF mkfault THEN error(0); FI
		svar[0].a_segno=dot.a_segno;
		svar[0].a_offset=wx;
		modifier = *fp++;
		dotinc=(longpr?4:2);

		IF charpos()==0 ANDF modifier!='a' THEN printf("%16m"); FI

		switch(modifier) {

		    case SP: case TB:
			break;

		    case 't': case 'T':
			printf("%T",fcount); return(fp);

		    case 'r': case 'R':
			printf("%M",fcount); return(fp);

		    case 'a':
			psymoff(&dot,ptype,":%16t"); dotinc=0; break;

		    case 'p':
			psymoff(&svar[0],ptype,"%16t"); break;

		    case 'u':
			printf("%04u  ",w); break;

		    case 'U':
			printf("%08U  ",wx); break;

		    case 'c': case 'C':
			c=(w>>8)&LOBYTE;
			IF modifier=='C'
			THEN printesc(c);
			ELSE printc2(c, 0);
			FI
			dotinc=1; break;

		    case 'b': case 'B':
			printf("%%%04x  ", (w>>8)&LOBYTE); dotinc=1; break;

		    case 's': case 'S':
			savdot=dot; dotinc=1;
			WHILE (c=(get(&dot,itype)>>8)&LOBYTE) ANDF errflg==0
			DO dot.a_offset=inkdot(1)->a_offset;
			   IF modifier == 'S'
			   THEN printesc(c);
			   ELSE printc2(c, 0);
			   FI
			   endline();
			OD
			dotinc=dot.a_offset-savdot.a_offset+1; dot=savdot; break;

		    case 'x':
		    case 'w':
			printf("%%%04x  ",w); break;

		    case 'X':
		    case 'W':
			printf("%%%08X  ", wx); break;

		    case 'Y':
			printf("%-24Y", wx); break;

		    case 'o':
			printf("%06o ", w); break;

		    case 'O':
			printf("%012O  ", wx); break;

		    case 'i':
			printins(&dot,itype); printc2(EOR, 0); break;

		    case 'd':
			printf("%6d ", w); break;

		    case 'D':
			printf("%11D ", wx); break;

		    case 'f':
			fw.sa = wx;
			fw = fw.fa;
			printf("%-16.9f", fw);
			dotinc=4; break;

		    case 'F':
			fw.sa = wx;
			fw = fw.fa;
			fw.sb = w1;
			fw.sc = w2;
			printf("%-32.18F", fw);
			dotinc=8; break;

		    case 'n': case 'N':
			printc2('\n', 0); dotinc=0; break;

		    case '"':
			dotinc=0;
			WHILE *fp != '"' ANDF *fp
			DO printc2(*fp++, 0); OD
			IF *fp THEN fp++; FI
			break;

		    case '^':
			dot.a_offset=inkdot(-dotinc*fcount)->a_offset; return(fp);

		    case '+':
			dot.a_offset=inkdot(fcount)->a_offset; return(fp);

		    case '-':
			dot.a_offset=inkdot(-fcount)->a_offset; return(fp);

		    default: error(BADMOD);
		}
		IF itype!=NSP
		THEN	dot.a_offset=inkdot(dotinc)->a_offset;
		FI
		fcount--; endline();
	OD
	return(fp);
}

unox()
{
	INT		rc, status, unixpid;
	STRING		argp = lp;

	WHILE lastc!=EOR DO rdc(); OD
	IF (unixpid=fork())==0
	THEN	signal(SIGINT,sigint); signal(SIGQUIT,sigqit);
		*lp=0; execl("/bin/sh", "sh", "-c", argp, 0);
		exit(16);
	ELIF unixpid == -1
	THEN	error(NOFORK);
	ELSE	signal(SIGINT,1);
		WHILE (rc = wait(&status)) != unixpid ANDF rc != -1 DONE
		signal(SIGINT,sigint);
		prints("!"); lp--;
	FI
}

printesc(c)
{
	c &= STRIP;
	IF c<SP ORF c>'~' ORF c=='@'
	THEN printf("@%c",(c=='@' ? '@' : c^0140));
	ELSE printc2(c, 0);
	FI
}

ADDR	*inkdot(incr)
{
	static ADDR	newdot = { 0 };

	newdot=dot;
	newdot.a_offset=incr+dot.a_offset;
	IF (dot.a_offset NEQ newdot.a_offset) >> 24 THEN error(ADWRAP); FI
	return(&newdot);
}

nullfn()
{
}
