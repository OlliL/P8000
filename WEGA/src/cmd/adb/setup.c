static char sccsid[] = "@[$]setup.c\t3.3  09/28/83 11:57:06 - 87wega3.2";

#include "defs.h"


MSG		BADNAM;
MSG		BADMAG;
MSG		BADSEGT;
MSG		BADTSEG;
MSG		BADDSEG;






#define NSEGSYM	300

setsym()
{
	INT		i;
	INT		type;
	struct segt	segt;
	struct segsym	*sp;
	SYMPTR		symp;
	struct s_exec	hdr;

	fsym=getfile(symfil,1);
	txtmap.ufd=fsym;
	findsym(&dot,ISYM);
	IF read(fsym, &hdr, sizeof hdr)==sizeof hdr
	THEN	curseg = (hdr.s_flag&SF_7FSTK ? 0x7f : 0x3f);
		magic=hdr.s_magic;
		IF magic!=S_MAGIC1 ANDF magic!=S_MAGIC3 ANDF magic!=S_MAGIC4 ANDF
		   magic!=N_MAGIC1 ANDF magic!=N_MAGIC3 ANDF magic!=N_MAGIC4
		THEN	magic=0;
		ELSE	segflg = (magic==S_MAGIC1 ORF magic==S_MAGIC3 ORF magic==S_MAGIC4 ? 1 : 0);
			IF (segtab=sbrk(hdr.s_segt)) == -1
			THEN	nsegs=1;
				segtab=sbrk(sizeof (struct segt));
				printf("%s\n",BADSEGT);
			ELSE	nsegs=hdr.s_segt/sizeof (struct segt);
				longseek(fsym,(long)sizeof hdr);
				FOR i=0; i<nsegs; i++
				DO	IF read(fsym, &segt, sizeof segt) != sizeof segt
					THEN	nsegs=1;
						printf("%s\n",BADSEGT);
						break;
					FI
					segtab[i].st_seg = segt.sg_segno;
					segtab[i].st_code = segt.sg_code;
					segtab[i].st_data = segt.sg_data;
					segtab[i].st_bss = segt.sg_bss;
					segtab[i].st_atr = segt.sg_atr;
				OD
			FI
			symoff = hdr.s_imsize;
			IF !(hdr.s_flag&SF_STRIP) THEN symoff <<= 1; FI
			symoff += hdr.s_segt+sizeof hdr;

			/* set up the segment symbol table */
			nsyms = (hdr.s_syms==0 ? 0 : NSEGSYM);
			sp = segsyms = sbrk(nsyms==0 ? sizeof (struct segsym) : (NSEGSYM+1)*sizeof (struct segsym));
			IF segsyms == -1
			THEN	printf("%s\n",BADNAM);
				sp = segsyms = sbrk(sizeof (struct segsym));
			ELSE	segsyms[nsyms].ss_type = -1;
				symset();
				WHILE (symp=symget()) ANDF errflg==0
				DO	IF sp->ss_type == -1
					THEN	IF sbrk((NSEGSYM/3+1)*sizeof (struct segsym)) == -1
						THEN	printf("%s\n",BADNAM);
							break;
						FI
						sp[NSEGSYM/3].ss_type = -1;
					FI
					type = symp->symf;
					sp->ss_type = SYMTYPE(type);
					itolws.L = symp->symv;
					sp->ss_seg = (itolws.I[0]>>8)&0177;
					sp->ss_base = leng(symp->symv&0xffff);
					sp->ss_segt = symp->symseg;
					IF symp->symc[0]&0200
					THEN	i = (symp->symc[0]&0177)-8;
						sp->ss_count = ((i+13)/14+1)*14;
					ELSE	sp->ss_count = 14;
					FI
					sp++;
				OD
			FI
			sp->ss_type = -1;
			nsyms = (sp-segsyms)/sizeof (struct segsym);
			itolws.L = entrypt = hdr.s_entry;
			chgtmap((CHAR)((itolws.I[0]>>8)&0177));
		FI
	FI
	IF magic==0 THEN txtmap.e1=maxfile; FI
}

setcor(argc)
{
	fcor=getfile(corfil,2);
	datmap.ufd=fcor;
	IF kflag
	THEN	kcorf=fcor;
		IF argc<=2
		THEN	kmemf=getfile(memfil,2);
			return;
		FI
		kdump++;
		kmemf=kcorf;
		read(fcor, regs, 40);
	ELSE	IF (uptr=sbrk(ctob(USIZE))) == -1
		THEN	error("not enough memory for user area\n");
		FI
		IF read(fcor, uptr, ctob(USIZE))==ctob(USIZE)
		THEN	txtsiz = 0;
			txtsiz = uptr->u_tsize;
			txtsiz <<= 8;
			datsiz = 0;
			datsiz = uptr->u_dsize;
			datsiz <<= 8;
			stksiz = 0;
			stksiz = uptr->u_ssize;
			stksiz <<= 8;
			datbas = 0;
			datmap.b1 = 0;
			datmap.e1 = datsiz;
			IF magic==S_MAGIC1 ORF magic==N_MAGIC1
			THEN datmap.e1 += txtsiz;
			ELSE datmap.e1 += datmap.b1;
			FI
			datmap.f1 = ctob(USIZE);
			datmap.b2 = maxstor-stksiz;
			datmap.e2 = maxstor;
			datmap.f2 = datmap.e1+ctob(USIZE);
			longseek(fcor, (shorten(uptr->u_state)-0x3e00f600L)&0xffff);
			read(fcor, regs, 40);
			IF segflg==0 THEN regs[18] = 0; FI
			IF magic ANDF magic!=uptr->u_exdata.s_exc.s_magic
			THEN	printf("%s\n",BADMAG);
			FI
		ELSE	datmap.f1 = txtmap.f2;
			datmap.e1 = txtmap.e2;
			datmap.ufd = txtmap.ufd;
		FI
	FI
	dot.a_segno = (regs[18]>>8)&0177;
	dot.a_offset = (POS)regs[19];
	findsym(&dot,ISYM);
}

create(f)
STRING	f;
{	INT fd;
	IF (fd=creat(f,0644))>=0
	THEN close(fd); return(open(f,wtflag));
	ELSE return(-1);
	FI
}

getfile(filnam,cnt)
STRING	filnam;
{
	REG INT		fsym;

	IF !eqstr("-",filnam)
	THEN	fsym=open(filnam,wtflag);
		IF fsym<0 ANDF argcount>cnt
		THEN	IF wtflag
			THEN	fsym=create(filnam);
			FI
			IF fsym<0
			THEN printf("cannot open `%s'\n", filnam);
			FI
		FI
	ELSE	fsym = -1;
	FI
	return(fsym);
}

chgtmap(seg)
CHAR	seg;
{
	REG INT		i;
	REG INT		j;

	FOR i=0; i<nsegs; i++
	DO IF segtab[i].st_seg == seg THEN break; FI OD
	IF i==nsegs THEN error(BADTSEG); FI
	txtsiz = segtab[i].st_code;
	datsiz = segtab[i].st_data;
	txtmap.seg = segtab[i].st_seg;
	txtmap.b1 = 0;
	txtmap.f1 = nsegs*16+(long)sizeof (struct s_exec);
	IF segflg==1
	THEN	FOR j=0; j<i; j++
		DO txtmap.f1 += segtab[j].st_code+segtab[j].st_data; OD
	FI
	IF magic==S_MAGIC1 ORF magic==N_MAGIC1
	THEN	txtmap.e1 = txtsiz+datsiz;
		txtmap.e2 = txtsiz+datsiz;
		txtmap.f2 = nsegs*16+(long)sizeof (struct s_exec);
	ELSE	txtmap.e1 = txtsiz;
		txtmap.e2 = datsiz;
		txtmap.f2 = nsegs*16+(long)sizeof (struct s_exec);
		IF segflg==1
		THEN	FOR j=0; j<i; j++
			DO txtmap.f2 += segtab[j].st_code+segtab[j].st_data; OD
		ELSE	txtmap.f2 += txtmap.e1;
		FI
	FI
	datbas = 0;
	txtmap.b2 = 0;
}

chgdmap(seg)
CHAR	seg;
{
	REG INT		i;
	REG INT		j;

	FOR i=0; i<nsegs; i++
	DO IF segtab[i].st_seg == seg THEN break; FI OD
	IF i==nsegs THEN error(BADDSEG); FI
	txtsiz = segtab[i].st_code;
	datsiz = segtab[i].st_data;
	datmap.seg = segtab[i].st_seg;
	datmap.b1 = 0;
	IF datmap.ufd == txtmap.ufd
	THEN	datmap.f1 = nsegs*16+(long)sizeof (struct s_exec);
		IF segflg==1
		THEN	FOR j=0; j<i; j++
			DO datmap.f1 += segtab[j].st_code+segtab[j].st_data; OD
		FI
		IF magic==S_MAGIC1 ORF magic==N_MAGIC1
		THEN datmap.e1 = txtsiz+datsiz;
		ELSE datmap.e1 = datsiz;
		FI
	ELSE	datmap.f1 = ctob(USIZE);
		IF segflg==1
		THEN	FOR j=0; j<i; j++
			DO	IF magic==S_MAGIC1
				THEN datmap.f1 += segtab[j].st_code+segtab[j].st_data;
				ELSE datmap.f1 += segtab[j].st_data;
				FI
			OD
		FI
		IF magic==S_MAGIC1 ORF magic==N_MAGIC1
		THEN datmap.e1 = txtsiz+datsiz;
		ELSE datmap.e1 = segtab[j].st_bss+datsiz;
		FI
	FI
}
