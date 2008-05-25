/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle

	KERN 3.2	Modul: ureg.c
 
 
	Bearbeiter:	O. Lehmann
	Datum:		25.05.08
	Version:	1.0
 
*******************************************************************************
******************************************************************************/
 
char uregwstr[] = @[$]ureg.c		Rev : 4.1 	08/27/83 12:01:05";

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
register unsigned ns;
register unsigned sep;
{
	int foo2;
	
	if(nt > 0x100) {
		u.u_error = ENOMEM;
		return(-1);
	}
	
	foo2 = nd&LONGMASK;
	
	if(sep>0x0080) {
		u.u_error = ENOEXEC;
		return(-1);
	}
	
	if(foo2 == u.u_stakseg) {
		u.u_segmts[NUSEGS-1].sg_limit = (!nt?0:-nt+0x100);
		u.u_segmts[NUSEGS-1].sg_attr = (!nt?0x20:-nt+0x14);
		return(0);
	}

/*FIXME: and now the whole thing is missing.... */


	return(0);
}

segureg()
{
}

ldsdr()
{
}

prmmu()
{
}
