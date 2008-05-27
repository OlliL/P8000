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
		panic("sureg: user is segmented");

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
	/*FIXME: the 3rd test should be done in r5, not r2 */
	if(((nt > 0x100 || (nd+ns) > 0x00ff)) || (maxmem < nt+nd+ns+USIZE)) { 
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

sestabur(nt, nd, ns, sep)
register unsigned nt;
char nd;
register unsigned ns;
register unsigned sep;
{
	int stakseg;
	
	if(nt > 256) {
		u.u_error = ENOMEM;
		return(-1);
	}
	
	stakseg = nd&LONGMASK;
	
	if(sep > 128) {
		u.u_error = ENOEXEC;
		return(-1);
	}
	
	if(stakseg == u.u_stakseg) {
		u.u_segmts[NUSEGS-1].sg_limit = (!nt?0:-nt+256);
		u.u_segmts[NUSEGS-1].sg_attr  = (!nt?32:-nt+20);
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
		segno = u.u_segno[i]&0x7f;
		j = u.u_segmts[i].sg_attr&0x14;
		if (u.u_segno[i] & 0x80){
			u.u_segmts[i].sg_base = caddr;
			if(u.u_segmts[i].sg_limit > 0 || !j)
				caddr += u.u_segmts[i].sg_limit-1; /*FIXME: r2 gets used here, r3 has to be used here */
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
	u.u_segmts[NUSEGS-1].sg_limit = addr - u.u_segmts[NUSEGS-1].sg_limit;
	loadsd((u.u_stakseg<0x40?mmud:mmus),0x7f,&u.u_segmts[NUSEGS-1]);
}

ldsdr()
{
}

prmmu()
{
}
