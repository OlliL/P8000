/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle

	KERN 3.2	Modul: ureg.c
 
 
	Bearbeiter:	O. Lehmann
	Datum:		25.05.08
	Version:	1.0
 
*******************************************************************************
******************************************************************************/
 
char uregwstr[] = "@[$]ureg.c		Rev : 4.1 	08/27/83 12:01:05";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/text.h>

extern int	mmut;	/* STACK MMU */
extern int	mmud;	/* DATA MMU */
extern int	mmus;	/* CODE MMU */

/*
 * Load the user hardware segmentation
 * registers from the software prototype.
 * The software registers must have
 * been setup prior by estabur.
 */
sureg()
{
	register struct segd *segmnt;
	register struct text *textp;

	if(u.u_segmented || inb(SCR)&0x0004)
		panic("sureg: user is segmented\n");

	u.u_segmts[USEG].sg_base = u.u_procp->p_addr+USIZE;
	u.u_segmts[NUSEGS-1].sg_base = (u.u_segmts[USEG].sg_limit + u.u_segmts[USEG].sg_base + 1)
	                                - u.u_segmts[NUSEGS-1].sg_limit;	
	textp = u.u_procp->p_textp;
	if(textp) {
		u.u_nsucode.sg_base = textp->x_caddr;
		segmnt = &u.u_nsucode;
	} else {
		segmnt = &u.u_segmts[USEG];
	}
	loadsd(mmut,USEG,segmnt);
	loadsd(mmud,USEG,&u.u_segmts[USEG]);
	loadsd(mmus,USEG,&u.u_segmts[NUSEGS-1]);
	u.u_break = u.u_segmts[USEG].sg_limit + 1;
	outb(0xffd1,u.u_break);
}

/*
 * Set up software prototype segmentation
 * registers to implement the 3 pseudo
 * text,data,stack segment sizes passed
 * as arguments.
 * The argument sep specifies if the
 * text and data+stack segments are to
 * be separated.
 * The last argument determines whether the text
 * segment is read-write or read-only.
 */
estabur(nt, nd, ns, sep)
register unsigned nt, nd, ns;
{
	if(nt > 0x100 || ((nd+ns) > 0x00ff || (nt+nd+ns+USIZE) > maxmem)) { 
		u.u_error = ENOMEM;
		return(-1);
	}

	u.u_nsucode.sg_limit = (!nt?0:nt-1);
	u.u_segmts[USEG].sg_limit = (!nd?0:nd-1);
	u.u_segmts[NUSEGS-1].sg_limit = (!ns?0:-ns+0x100);
	
	u.u_nsucode.sg_attr = (!nt?0x14:sep);    /*FIXME: use r3 here instead of r2 */
	u.u_segmts[USEG].sg_attr = (!nd?0x14:0); /*FIXME: use r2 here instead of r3 */
	u.u_segmts[NUSEGS-1].sg_attr = (!ns?0x14:0x20);
	sureg();
	return(0);
}

/*FIXME: The function works from the functional point of view, but in the original
 *       object the stkseg stuff at the beginning shows that at least one int is
 *       used - which cannot be, because if stakseg is an int the work with stackseg
 *       is no longer compatible - same goes with nd. but if for example stakseg is
 *       declared as int, nt, ns and sep are assigned correctly so that r12 get used
 *       otherwise r3 gets used instead of r12
 */
sestabur(nt, nd, ns, sep)
register unsigned nt, ns, sep;
char nd;
{
	char stakseg;
	
	if(nt > 256) {
		u.u_error = ENOMEM;
		return(-1);
	}
	
	stakseg = nd&STAK;
	
	if(ns > 128) {
		u.u_error = ENOEXEC;
		return(-1);
	}
	
	if(stakseg == u.u_stakseg) {
		u.u_segmts[NUSEGS-1].sg_limit = (!nt?0:-nt+256);
		u.u_segmts[NUSEGS-1].sg_attr  = (!nt?20:32);
		return(0);
	}
	
	u.u_segmts[ns].sg_limit = (!nt?0:nt-1);

	if(nd&128)
		u.u_segmts[ns].sg_attr = (!nt?20:sep);
	else
		u.u_segmts[ns].sg_attr = (!nt?20:0);

	return(0);
}

segureg()
{
	register short caddr;
	register short addr;
	register int segno;
	register struct text *textp;
	char i, j;

	addr = u.u_procp->p_addr+USIZE;
	textp = u.u_procp->p_textp;

	if(textp)
		caddr = textp->x_caddr;
	else
		caddr = 0;
	
	for(i=0;i<u.u_nsegs;i++) {
		segno = u.u_segno[i]&STAK;
		j = u.u_segmts[i].sg_attr&0x14;
		if (u.u_segno[i] & 0x80){
			u.u_segmts[i].sg_base = caddr;
			if(u.u_segmts[i].sg_limit > 0 || !j)
				caddr += u.u_segmts[i].sg_limit+1; /*FIXME: r2 gets used here, r3 has to be used here */
		} else {
			u.u_segmts[i].sg_base = addr;
			if(u.u_segmts[i].sg_limit || !j)
				addr += u.u_segmts[i].sg_limit+1;
		}
		if((unsigned)segno < 0x0040)
			loadsd(mmud,segno,&u.u_segmts[i]);
		else
			loadsd(mmus,segno,&u.u_segmts[i]);
	}
	u.u_segmts[NUSEGS-1].sg_base = addr - u.u_segmts[NUSEGS-1].sg_limit;
	loadsd((u.u_stakseg<0x40?mmud:mmus),STAK,&u.u_segmts[NUSEGS-1]);
}

ldsdr(segno, sbase, slimit, sattr)
{
	register struct segd *segmnt;
	register int i;

	segno &= STAK;
	if(segno != u.u_stakseg) {
		for(i=0;i<u.u_nsegs;i++) {
			if((u.u_segno[i]&STAK) == segno)
				break;
		}
	
		if(i == u.u_nsegs) {
			u.u_error = ENOSEG;
			return;
		}
		segmnt = &u.u_segmts[i];
	} else {
		segmnt = &u.u_segmts[NUSEGS-1];
	}
	
	getsd(segno<0x40?mmud:mmus,segno,segmnt);
	
	if(sbase != 0xffff)
		segmnt->sg_base = sbase;

	if(slimit != 0xffff)
		segmnt->sg_limit = ((segno == u.u_stakseg)?0x100-slimit:slimit-1);

	if(sattr != 0xffff)
		segmnt->sg_attr = sattr;

	loadsd(segno<0x40?mmud:mmus,segno,segmnt);
}

prmmu()
{
	int segno;
	int i;
	struct segd segmnt;

	putchar(0x0a);	/*newline*/
	if(!u.u_segmented) {
		getsd(mmut,0x3f,&segmnt);
		printf("code: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
		getsd(mmud,0x3f,&segmnt);
		printf("data: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
	} else {
		for(i=0;i<u.u_nsegs;i++) {
			segno = u.u_segno[i];
			if(segno < 0x40) {
				getsd(mmud,segno,&segmnt);
			} else {
				getsd(mmus,segno,&segmnt);
			}
			printf("seg/slot %x/%x: %x %x %x\n",segno,i,segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
		}
	}
	getsd(mmus,0x3f,&segmnt);
	printf("stak: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
	getsd(mmut,0x3b,&segmnt);
	printf("MAPU: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
	getsd(mmut,0x3c,&segmnt);
	printf("DMA1: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
	getsd(mmut,0x3d,&segmnt);
	printf("DMA2: %x %x %x\n",segmnt.sg_base,hibyte(segmnt.sg_limit),hibyte(segmnt.sg_attr));
}
