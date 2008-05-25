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

	u.u_segmts[USEG].sg_base = u.u_procp->p_addr+10;
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
estabur(nt, nd, ns, sep, xrw)
unsigned nt, nd, ns;
{
}

sestabur()
{
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
