static char sccsid[] = "@[$]access.c\t3.3  09/28/83 11:36:41 - 87wega3.2";

#include "defs.h"

INT		kflag;
INT		kcorf;
INT		kdump;
INT		kmemf;
INT		segflg;
INT		pid;
INT		wtflag;
MAP		datmap;
MAP		txtmap;
STRING		errflg;
CHAR		curseg;
L_INT		stksiz;
L_INT		maxstor;


MSG		ODDADR;
MSG		BADDAT;
MSG		BADTXT;

L_INT		kvseek();
L_INT		kpseek();

/* file handling and access routines */

put(adr,space,value)
ADDR	*adr;
{
	access(WT,adr,space,value);
}

POS	get(adr, space)
ADDR	*adr;
{
	chkerr();
	return(access(RD,adr,space,0));
}

POS	chkget(n, space)
ADDR	*n;
{
	REG INT		w;
	INT		t;

	w = get(n, space);
	chkerr();
	return(w);
}

access(mode,ap,space,value)
ADDR	*ap;
{
	INT		w, w1, pmode, rd, file;
	ADDR		adr;

	adr = *ap;
	rd = mode==RD;

	IF space == NSP THEN return(0); FI
	errno = 0;

	IF kflag ANDF space==DSP
	THEN {
	     L_INT	koff;

	     file = kcorf;
	     IF ap->a_offset>>16
	     THEN IF ap->a_offset>>24
		  THEN	file = kcorf;
			koff = kdump ? kvseek(file, ap->a_offset) : lseek(file, ap->a_offset, 0);
		  ELSE	file = kmemf;
			koff = kdump ? kpseek(file, ap->a_offset) : lseek(file, ap->a_offset, 0);
		  FI
	     ELSE file = kcorf;
		  koff = kdump ? kvseek(file, ((long)ap->a_segno<<24)|ap->a_offset)
			       : lseek(file, ((long)ap->a_segno<<24)|ap->a_offset, 0);
	     FI
	     IF koff != -1 ANDF (rd ? read(file,&w,2) : write(file,&value,2)) == 2
	     THEN return(w);
	     FI
	     errflg = (space&DSP ? BADDAT : BADTXT);
	     return(w);
	     }
	FI

	IF pid		/* tracing on? */
	THEN IF (adr.a_offset&01) ANDF !rd THEN error(ODDADR); FI
	     pmode = (space&DSP?(rd?RDUSER:WDUSER):(rd?RIUSER:WIUSER));
	     IF segflg==1
	     THEN w = sptrace(pmode, pid, itol(adr.a_segno<<8, shorten(adr.a_offset)&~01), value);
	     ELSE w = ptrace(pmode, pid, shorten(adr.a_offset)&~01, value);
	     FI
	     IF adr.a_offset&01
	     THEN IF segflg==1
		  THEN w1 = sptrace(pmode, pid, itol(adr.a_segno<<8, shorten(adr.a_offset)+1), value);
		  ELSE w1 = ptrace(pmode, pid, shorten(adr.a_offset)+1, value);
		  FI
		  w = (w<<8) | (w1>>8)&LOBYTE;
	     FI
	     IF errno
	     THEN errflg = (space&DSP ? BADDAT : BADTXT);
	     FI
	     return(w);
	FI
	w = 0;
	IF mode==WT ANDF wtflag==0
	THEN	error("not in write mode");
	FI
	IF !chkmap(&adr,space)
	THEN return(0);
	FI
	file=(space&DSP?datmap.ufd:txtmap.ufd);
	IF longseek(file,adr.a_offset)==0 ORF
	   (rd ? read(file,&w,2) : write(file,&value,2)) < 1
	THEN	errflg=(space&DSP?BADDAT:BADTXT);
	FI
	return(w);
}

chkmap(adr,space)
	REG ADDR	*adr;
	REG INT		space;
{
	REG MAPPTR amap;
	INT		i;

	amap=((space&DSP?&datmap:&txtmap));
	IF segflg ANDF adr->a_segno!=curseg ANDF adr->a_segno!=amap->seg
	THEN IF !(space&DSP)
	     THEN printf("'?' map changing to segment <<%x>>\n", adr->a_segno&0xff);
		  chgtmap(adr->a_segno);
	     ELSE printf("'/' map changing to segment <<%x>>\n", adr->a_segno&0xff);
		  chgdmap(adr->a_segno);
	     FI
	FI
	IF space&STAR ORF adr->a_segno==curseg ORF !within(*adr,amap->b1,amap->e1,amap->seg)
	THEN IF segflg ANDF adr->a_segno==curseg THEN adr->a_segno = amap->seg; FI
	     IF within(*adr,amap->b2,amap->e2,amap->seg)
	     THEN adr->a_offset = amap->f2+stksiz-(maxstor-adr->a_offset);
	     ELSE errflg=(space&DSP?BADDAT:BADTXT);  return(0);
	     FI
	ELSE adr->a_offset += (amap->f1)-(amap->b1);
	FI
	return(1);
}

within(adr,lbd,ubd,seg)
ADDR	adr;
unsigned long	lbd, ubd;
CHAR	seg;
{
	IF adr.a_segno!=seg THEN return(0); FI
	return(adr.a_offset>=lbd && adr.a_offset<ubd);
}
