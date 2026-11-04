static char sccsid[] = "@[$]dc2.c	1.1  07/23/82 19:17:38 - 87wega3.2";

#include <stdio.h>
#include <signal.h>
#include "dc.h"
struct blk *
add0(p,ct)
int ct;
struct blk *p;
{
		/* returns pointer to struct with ct 0's & p */
	register struct blk *q,*t;

	q = salloc(length(p)+(ct+1)/2);
	while(ct>1){
		sputc(q,0);
		ct -= 2;
	}
	rewind(p);
	while(sfeof(p) == 0){
		sputc(q,sgetc(p));
	}
	release(p);
	if(ct == 1){
		t = mult(tenptr,q);
		release(q);
		return(t);
	}
	return(q);
}
struct blk *
mult(p,q)
struct blk *p,*q;
{
	register struct blk *mp,*mq,*mr;
	int sign,offset,carry;
	int cq,cp,mt,mcr;

	offset = sign = 0;
	fsfile(p);
	mp = p;
	if(sfbeg(p) == 0){
		if(sbackc(p)<0){
			mp = copy(p,length(p));
			chsign(mp);
			sign = ~sign;
		}
	}
	fsfile(q);
	mq = q;
	if(sfbeg(q) == 0){
		if(sbackc(q)<0){
			mq = copy(q,length(q));
			chsign(mq);
			sign = ~sign;
		}
	}
	mr = salloc(length(mp)+length(mq));
	zero(mr);
	rewind(mq);
	while(sfeof(mq) == 0){
		cq = sgetc(mq);
		rewind(mp);
		rewind(mr);
		mr->rd += offset;
		carry=0;
		while(sfeof(mp) == 0){
			cp = sgetc(mp);
			mcr = sfeof(mr)?0:slookc(mr);
			mt = cp*cq + carry + mcr;
			carry = mt/100;
			salterc(mr,mt%100);
		}
		offset++;
		if(carry != 0){
			mcr = sfeof(mr)?0:slookc(mr);
			salterc(mr,mcr+carry);
		}
	}
	if(sign < 0){
		chsign(mr);
	}
	if(mp != p)release(mp);
	if(mq != q)release(mq);
	return(mr);
}
chsign(p)
struct blk *p;
{
	register int carry;
	register char ct;

	carry=0;
	rewind(p);
	while(sfeof(p) == 0){
		ct=100-slookc(p)-carry;
		carry=1;
		if(ct>=100){
			ct -= 100;
			carry=0;
		}
		salterc(p,ct);
	}
	if(carry != 0){
		sputc(p,-1);
		fsfile(p);
		sbackc(p);
		ct = sbackc(p);
		if(ct == 99){
			truncate(p);
			sputc(p,-1);
		}
	}
	else{
		fsfile(p);
		ct = sbackc(p);
		if(ct == 0)truncate(p);
	}
	return;
}
readc(){
loop:
	if((readptr != &readstk[0]) && (*readptr != 0)){
		if(sfeof(*readptr) == 0)return(lastchar = sgetc(*readptr));
		release(*readptr);
		readptr--;
		goto loop;
	}
	lastchar = getc(curfile);
	if(lastchar != EOF)return(lastchar);
	if(readptr != &readptr[0]){
		readptr--;
		if(*readptr == 0)curfile = stdin;
		goto loop;
	}
	if(curfile != stdin){
		fclose(curfile);
		curfile = stdin;
		goto loop;
	}
	exit(0);
}
unreadc(c)
char c;
{

	if((readptr != &readstk[0]) && (*readptr != 0)){
		sungetc(*readptr,c);
	}
	else ungetc(c,curfile);
	return;
}
binop(c)
char c;
{
	register struct blk *r;

	switch(c){
	case '+':
		r = add(arg1,arg2);
		break;
	case '*':
		r = mult(arg1,arg2);
		break;
	case '/':
		r = div(arg1,arg2);
		break;
	}
	release(arg1);
	release(arg2);
	sputc(r,savk);
	pushp(r);
	return;
}
print(hptr)
struct blk *hptr;
{
	int sc;
	register struct blk *p,*q,*dec;
	int dig,dout,ct;

	rewind(hptr);
	while(sfeof(hptr) == 0){
		if(sgetc(hptr)>99){
			rewind(hptr);
			while(sfeof(hptr) == 0){
				printf("%c",sgetc(hptr));
			}
			printf("\n");
			return;
		}
	}
	fsfile(hptr);
	sc = sbackc(hptr);
	if(sfbeg(hptr) != 0){
		printf("0\n");
		return;
	}
	count = ll;
	p = copy(hptr,length(hptr));
	sunputc(p);
	fsfile(p);
	if(sbackc(p)<0){
		chsign(p);
		OUTC('-');
	}
	if((obase == 0) || (obase == -1)){
		oneot(p,sc,'d');
		return;
	}
	if(obase == 1){
		oneot(p,sc,'1');
		return;
	}
	if(obase == 10){
		tenot(p,sc);
		return;
	}
	create(strptr);
	dig = log10*sc;
	dout = ((dig/10) + dig) /logo;
	dec = getdec(p,sc);
	p = removc(p,sc);
	while(length(p) != 0){
		q = div(p,basptr);
		release(p);
		p = q;
		(*outdit)(rem,0);
	}
	release(p);
	fsfile(strptr);
	while(sfbeg(strptr) == 0)OUTC(sbackc(strptr));
	if(sc == 0){
		release(dec);
		printf("\n");
		return;
	}
	create(strptr);
	OUTC('.');
	ct=0;
	do{
		q = mult(basptr,dec);
		release(dec);
		dec = getdec(q,sc);
		p = removc(q,sc);
		(*outdit)(p,1);
	}while(++ct < dout);
	release(dec);
	rewind(strptr);
	while(sfeof(strptr) == 0)OUTC(sgetc(strptr));
	printf("\n");
	return;
}

struct blk *
getdec(p,sc)
struct blk *p;
{
	int cc;
	register struct blk *q,*t,*s;

	rewind(p);
	if(length(p)*2 < sc){
		q = copy(p,length(p));
		return(q);
	}
	q = salloc(length(p));
	while(sc >= 1){
		sputc(q,sgetc(p));
		sc -= 2;
	}
	if(sc != 0){
		t = mult(q,tenptr);
		s = salloc(cc = length(q));
		release(q);
		rewind(t);
		while(cc-- > 0)sputc(s,sgetc(t));
		sputc(s,0);
		release(t);
		t = div(s,tenptr);
		release(s);
		release(rem);
		return(t);
	}
	return(q);
}
tenot(p,sc)
struct blk *p;
{
	register int c,f;

	fsfile(p);
	f=0;
	while((sfbeg(p) == 0) && ((p->rd-p->beg-1)*2 >= sc)){
		c = sbackc(p);
		if((c<10) && (f == 1))printf("0%d",c);
		else printf("%d",c);
		f=1;
		TEST2;
	}
	if(sc == 0){
		printf("\n");
		release(p);
		return;
	}
	if((p->rd-p->beg)*2 > sc){
		c = sbackc(p);
		printf("%d.",c/10);
		TEST2;
		OUTC(c%10 +'0');
		sc--;
	}
	else {
		OUTC('.');
	}
	if(sc > (p->rd-p->beg)*2){
		while(sc>(p->rd-p->beg)*2){
			OUTC('0');
			sc--;
		}
	}
	while(sc > 1){
		c = sbackc(p);
		if(c<10)printf("0%d",c);
		else printf("%d",c);
		sc -= 2;
		TEST2;
	}
	if(sc == 1){
		OUTC(sbackc(p)/10 +'0');
	}
	printf("\n");
	release(p);
	return;
}
oneot(p,sc,ch)
struct blk *p;
char ch;
{
	register struct blk *q;

	q = removc(p,sc);
	create(strptr);
	sputc(strptr,-1);
	while(length(q)>0){
		p = add(strptr,q);
		release(q);
		q = p;
		OUTC(ch);
	}
	release(q);
	printf("\n");
	return;
}
hexot(p,flg)
struct blk *p;
{
	register int c;
	rewind(p);
	if(sfeof(p) != 0){
		sputc(strptr,'0');
		release(p);
		return;
	}
	c = sgetc(p);
	release(p);
	if(c >= 16){
		printf("hex digit > 16");
		return;
	}
	sputc(strptr,c<10?c+'0':c-10+'A');
	return;
}
bigot(p,flg)
struct blk *p;
{
	register struct blk *t,*q;
	register int l;
	int neg;

	if(flg == 1)t = salloc(0);
	else{
		t = strptr;
		l = length(strptr)+fw-1;
	}
	neg=0;
	if(length(p) != 0){
		fsfile(p);
		if(sbackc(p)<0){
			neg=1;
			chsign(p);
		}
		while(length(p) != 0){
			q = div(p,tenptr);
			release(p);
			p = q;
			rewind(rem);
			sputc(t,sfeof(rem)?'0':sgetc(rem)+'0');
			release(rem);
		}
	}
	release(p);
	if(flg == 1){
		l = fw1-length(t);
		if(neg != 0){
			l--;
			sputc(strptr,'-');
		}
		fsfile(t);
		while(l-- > 0)sputc(strptr,'0');
		while(sfbeg(t) == 0)sputc(strptr,sbackc(t));
		release(t);
	}
	else{
		l -= length(strptr);
		while(l-- > 0)sputc(strptr,'0');
		if(neg != 0){
			sunputc(strptr);
			sputc(strptr,'-');
		}
	}
	sputc(strptr,' ');
	return;
}
struct blk *
add(a1,a2)
struct blk *a1,*a2;
{
	register struct blk *p;
	register int carry,n;
	int size;
	int c,n1,n2;

	size = length(a1)>length(a2)?length(a1):length(a2);
	p = salloc(size);
	rewind(a1);
	rewind(a2);
	carry=0;
	while(--size >= 0){
		n1 = sfeof(a1)?0:sgetc(a1);
		n2 = sfeof(a2)?0:sgetc(a2);
		n = n1 + n2 + carry;
		if(n>=100){
			carry=1;
			n -= 100;
		}
		else if(n<0){
			carry = -1;
			n += 100;
		}
		else carry = 0;
		sputc(p,n);
	}
	if(carry != 0)sputc(p,carry);
	fsfile(p);
	if(sfbeg(p) == 0){
		while(sfbeg(p) == 0 && (c = sbackc(p)) == 0);
		if(c != 0)salterc(p,c);
		truncate(p);
	}
	fsfile(p);
	if(sfbeg(p) == 0 && sbackc(p) == -1){
		while((c = sbackc(p)) == 99){
			if(c == EOF)break;
		}
		sgetc(p);
		salterc(p,-1);
		truncate(p);
	}
	return(p);
}
eqk(){
	register struct blk *p,*q;
	register int skp;
	int skq;

	p = pop();
	EMPTYS;
	q = pop();
	EMPTYSR(p);
	skp = sunputc(p);
	skq = sunputc(q);
	if(skp == skq){
		arg1=p;
		arg2=q;
		savk = skp;
		return(0);
	}
	else if(skp < skq){
		savk = skq;
		p = add0(p,skq-skp);
	}
	else {
		savk = skp;
		q = add0(q,skp-skq);
	}
	arg1=p;
	arg2=q;
	return(0);
}
struct blk *
removc(p,n)
struct blk *p;
{
	register struct blk *q,*r;

	rewind(p);
	while(n>1){
		sgetc(p);
		n -= 2;
	}
	q = salloc(2);
	while(sfeof(p) == 0)sputc(q,sgetc(p));
	if(n == 1){
		r = div(q,tenptr);
		release(q);
		release(rem);
		q = r;
	}
	release(p);
	return(q);
}
struct blk *
scalint(p)
struct blk *p;
{
	register int n;
	n = sunputc(p);
	p = removc(p,n);
	return(p);
}
struct blk *
scale(p,n)
struct blk *p;
{
	register struct blk *q,*s,*t;

	t = add0(p,n);
	q = salloc(1);
	sputc(q,n);
	s = exp(inbas,q);
	release(q);
	q = div(t,s);
	release(t);
	release(s);
	release(rem);
	sputc(q,n);
	return(q);
}
subt(){
	arg1=pop();
	EMPTYS;
	savk = sunputc(arg1);
	chsign(arg1);
	sputc(arg1,savk);
	pushp(arg1);
	if(eqk() != 0)return(1);
	binop('+');
	return(0);
}
command(){
	int c;
	char line[100],*sl;
	register (*savint)(),pid,rpid;
	int retcode;

	switch(c = readc()){
	case '<':
		return(cond(NL));
	case '>':
		return(cond(NG));
	case '=':
		return(cond(NE));
	default:
		sl = line;
		*sl++ = c;
		while((c = readc()) != '\n')*sl++ = c;
		*sl = 0;
		if((pid = fork()) == 0){
			execl("/bin/sh","sh","-c",line,0);
			exit(0100);
		}
		savint = signal(SIGINT, SIG_IGN);
		while((rpid = wait(&retcode)) != pid && rpid != -1);
		signal(SIGINT,savint);
		printf("!\n");
		return(0);
	}
}
cond(c)
char c;
{
	register struct blk *p;
	register char cc;

	if(subt() != 0)return(1);
	p = pop();
	sunputc(p);
	if(length(p) == 0){
		release(p);
		if(c == '<' || c == '>' || c == NE){
			readc();
			return(0);
		}
		load();
		return(1);
	}
	else {
		if(c == '='){
			release(p);
			readc();
			return(0);
		}
	}
	if(c == NE){
		release(p);
		load();
		return(1);
	}
	fsfile(p);
	cc = sbackc(p);
	release(p);
	if((cc<0 && (c == '<' || c == NG)) ||
		(cc >0) && (c == '>' || c == NL)){
		readc();
		return(0);
	}
	load();
	return(1);
}
load(){
	register int c;
	register struct blk *p,*q;
	struct blk *t,*s;
	c = readc() & 0377;
	sptr = stable[c];
	if(sptr != 0){
		p = sptr->val;
		if(c >= ARRAYST){
			q = salloc(length(p));
			rewind(p);
			while(sfeof(p) == 0){
				s = getwd(p);
				if(s == 0){putwd(q, (struct blk *)NULL);}
				else{
					t = copy(s,length(s));
					putwd(q,t);
				}
			}
			pushp(q);
		}
		else{
			q = copy(p,length(p));
			pushp(q);
		}
	}
	else{
		q = salloc(1);
		sputc(q,0);
		pushp(q);
	}
	return;
}
log2(n)
long n;
{
	register int i;

	if(n == 0)return(0);
	i=31;
	if(n<0)return(i);
	while((n= n<<1) >0)i--;
	return(--i);
}

struct blk *
salloc(size)
int size;
{
	register struct blk *hdr;
	register char *ptr;
	all++;
	nbytes += size;
	ptr = malloc((unsigned)size);
	if(ptr == 0){
		garbage("salloc");
		if((ptr = malloc((unsigned)size)) == 0)
			ospace("salloc");
	}
	if((hdr = hfree) == 0)hdr = morehd();
	hfree = (struct blk *)hdr->rd;
	hdr->rd = hdr->wt = hdr->beg = ptr;
	hdr->last = ptr+size;
	return(hdr);
}
struct blk *
morehd(){
	register struct blk *h,*kk;
	headmor++;
	nbytes += HEADSZ;
	hfree = h = (struct blk *)malloc(HEADSZ);
	if(hfree == 0){
		garbage("morehd");
		if((hfree = h = (struct blk *)malloc(HEADSZ)) == 0)
			ospace("headers");
	}
	kk = h;
	while(h<hfree+(HEADSZ/BLK))(h++)->rd = (char *)++kk;
	(--h)->rd=0;
	return(hfree);
}
/*
sunputc(hptr)
struct blk *hptr;
{
	hptr->wt--;
	hptr->rd = hptr->wt;
	return(*hptr->wt);
}
*/
struct blk *
copy(hptr,size)
struct blk *hptr;
int size;
{
	register struct blk *hdr;
	register unsigned sz;
	register char *ptr;

	all++;
	nbytes += size;
	sz = length(hptr);
	ptr = nalloc(hptr->beg, (unsigned)size);
	if(ptr == 0){
		garbage("copy");
		if((ptr = nalloc(hptr->beg, (unsigned)size)) == NULL){
			printf("copy size %d\n",size);
			ospace("copy");
		}
	}
	if((hdr = hfree) == 0)hdr = morehd();
	hfree = (struct blk *)hdr->rd;
	hdr->rd = hdr->beg = ptr;
	hdr->last = ptr+size;
	hdr->wt = ptr+sz;
	ptr = hdr->wt;
	while(ptr<hdr->last)*ptr++ = '\0';
	return(hdr);
}
sdump(s1,hptr)
char *s1;
struct blk *hptr;
{
	char *p;
	printf("%s %o rd %o wt %o beg %o last %o\n",s1,hptr,hptr->rd,hptr->wt,hptr->beg,hptr->last);
	p = hptr->beg;
	while(p < hptr->wt)printf("%d ",*p++);
	printf("\n");
}
seekc(hptr,n)
struct blk *hptr;
{
	register char *nn,*p;

	nn = hptr->beg+n;
	if(nn > hptr->last){
		nbytes += nn - hptr->last;
		free(hptr->beg);
		p = realloc(hptr->beg, (unsigned)n);
		if(p == 0){
			hptr->beg = realloc(hptr->beg, (unsigned)(hptr->last-hptr->beg));
			garbage("seekc");
			if((p = realloc(hptr->beg, (unsigned)n)) == 0)
				ospace("seekc");
		}
		hptr->beg = p;
		hptr->wt = hptr->last = hptr->rd = p+n;
		return;
	}
	hptr->rd = nn;
	if(nn>hptr->wt)hptr->wt = nn;
	return;
}
salterwd(hptr,n)
struct wblk *hptr;
struct blk *n;
{
	if(hptr->rdw == hptr->lastw)more(hptr);
	*hptr->rdw++ = n;
	if(hptr->rdw > hptr->wtw)hptr->wtw = hptr->rdw;
	return;
}
more(hptr)
struct blk *hptr;
{
	register unsigned size;
	register char *p;

	if((size=(hptr->last-hptr->beg)*2) == 0)size=1;
	nbytes += size/2;
	free(hptr->beg);
	p = realloc(hptr->beg, (unsigned)size);
	if(p == 0){
		hptr->beg = realloc(hptr->beg, (unsigned)(hptr->last-hptr->beg));
		garbage("more");
		if((p = realloc(hptr->beg,size)) == 0)
			ospace("more");
	}
	hptr->rd = hptr->rd-hptr->beg+p;
	hptr->wt = hptr->wt-hptr->beg+p;
	hptr->beg = p;
	hptr->last = p+size;
	return;
}
ospace(s)
char *s;
{
	printf("out of space: %s\n",s);
	printf("all %ld rel %ld headmor %ld\n",all,rel,headmor);
	printf("nbytes %ld\n",nbytes);
	sdump("stk",*stkptr);
	abort();
}
garbage(s)
char *s;
{
	int i;
	struct blk *p, *q;
	struct sym *tmps;
	int ct;

/*	printf("got to garbage %s\n",s);	*/
	for(i=0;i<TBLSZ;i++){
		tmps = stable[i];
		if(tmps != 0){
			if(i < ARRAYST){
				do {
					p = tmps->val;
					if(((int)p->beg & 01)  != 0){
						printf("string %o\n",i);
						sdump("odd beg",p);
					}
					redef(p);
					tmps = tmps->next;
				} while(tmps != 0);
				continue;
			}
			else {
				do {
					p = tmps->val;
					rewind(p);
					ct = 0;
					while((q = getwd(p)) != NULL){
						ct++;
						if(q != 0){
							if(((int)q->beg & 01) != 0){
								printf("array %o elt %d odd\n",i-ARRAYST,ct);
printf("tmps %o p %o\n",tmps,p);
								sdump("elt",q);
							}
							redef(q);
						}
					}
					tmps = tmps->next;
				} while(tmps != 0);
			}
		}
	}
}
redef(p)
struct blk *p;
{
	register offset;
	register char *newp;

	if ((int)p->beg&01) {
		printf("odd ptr %o hdr %o\n",p->beg,p);
		ospace("redef-bad");
	}
	free(p->beg);
	free(dummy);
	dummy = malloc(1);
	if(dummy == NULL)ospace("dummy");
	newp = realloc(p->beg, (unsigned)(p->last-p->beg));
	if(newp == NULL)ospace("redef");
	offset = newp - p->beg;
	p->beg = newp;
	p->rd += offset;
	p->wt += offset;
	p->last += offset;
}

release(p)
register struct blk *p;
{
	rel++;
	nbytes -= p->last - p->beg;
	p->rd = (char *)hfree;
	hfree = p;
	free(p->beg);
}

struct blk *
getwd(p)
struct blk *p;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->rdw == wp->wtw)
		return(NULL);
	return(*wp->rdw++);
}

putwd(p, c)
struct blk *p, *c;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->wtw == wp->lastw)
		more(p);
	*wp->wtw++ = c;
}

struct blk *
lookwd(p)
struct blk *p;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->rdw == wp->wtw)
		return(NULL);
	return(*wp->rdw);
}
char *
nalloc(p,nbytes)
register char *p;
unsigned nbytes;
{
	char *malloc();
	register char *q, *r;
	q = r = malloc(nbytes);
	if(q==0)
		return(0);
	while(nbytes--)
		*q++ = *p++;
	return(r);
}
