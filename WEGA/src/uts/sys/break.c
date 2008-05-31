/******************************************************************************
*******************************************************************************

	W E G A - Quelle

	KERN 3.2	Modul: break.c


	Bearbeiter:	O. Lehmann
	Datum:		30.05.08
	Version:	1.0

*******************************************************************************
******************************************************************************/

char breakwstr[] = "@[$]break.c		Rev : 4.1	08/27/83 11:54:02";

#include <sys/param.h>
#include <sys/sysparm.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/map.h>

sbreak()
{
	register int l,r;
	register unsigned i;
	struct a {
		saddr_t segaddr;
	} *uap;

	uap = (struct a *)u.u_ap;
	r = lbtoc((long)uap->segaddr.right);
	if(u.u_segmented) {
		if(!uap->segaddr.l) {
			for(i=0;i<u.u_nsegs;i++) {
				if (((int)u.u_segno[i] & 0x80) == 0){
					u.u_r.r_val1 = u.u_segno[i]<<8;
					u.u_r.r_val2 = (u.u_segmts[i].sg_limit+1)<<8;
				}
			}
			return;
		} else {
			l = uap->segaddr.left >> 8;
			for(i=0;i<u.u_nsegs;i++) {
				if(u.u_segno[i] == l)
					break;
			}
			if(i==u.u_nsegs) {
				u.u_error = ENOMEM;
				return;
			}
		}
	}
	break_handler(l,r,i);
}

ssgbrk()
{
	register int a,j,b,i;
	struct a {
		int par1;
		unsigned par2;
	} *uap;

	uap = (struct a *)u.u_ap;
	j = uap->par1;

	for(i=0;i<u.u_nsegs;i++) {
		if(u.u_segno[i] == j)
			break;
	}
	if(i==u.u_nsegs) {
		u.u_error = ENOMEM;
		return;
	}
	if(!u.u_segmts[i].sg_limit && u.u_segmts[i].sg_attr&0x14)
		b=0;
	else
		b=(u.u_segmts[i].sg_limit&0xff)+1;
	a=lbtoc((long)uap->par2);
	if(a) {
		break_handler(j,b+a,i);
		if(u.u_error)
			return;
	}
	u.u_r.r_val1 = uap->par1<<8;
	u.u_r.r_val2 = b<<8;
}

break_handler(l,r,i)
register int l;
register unsigned r;
register int i;
{
	register int foo1,foo2,b;
	int size;
	struct proc *p;
	
	p = u.u_procp;
	if(r > 0x100) {
		u.u_error = ENOMEM;
		return;
	}
	if(u.u_segmented) {
		if(!u.u_segmts[i].sg_limit && u.u_segmts[i].sg_attr&0x14)
			b=r;
		else
			b=r-(u.u_segmts[i].sg_limit+1);
		if((u.u_tsize + USIZE + u.u_dsize + u.u_ssize + b) > umemory) {
			u.u_error = ENOMEM;
			return;
		}
		size = p->p_addr + p->p_size;
		if(!u.u_segmts[i].sg_limit && u.u_segmts[i].sg_attr&0x14)
			size -= u.u_segmts[i].sg_base;
		else
			size -= u.u_segmts[i].sg_base+u.u_segmts[i].sg_limit+1;
		sestabur(r,u.u_segno[i],i,0);
		if(u.u_error)
			return;

		u.u_dsize += b;
		shufflecore(p->p_size+b,p->p_size,size);
	} else {
		b = r - u.u_dsize;
		if(estabur(u.u_tsize,r,u.u_ssize,1))
			return;
		u.u_dsize = r;
		shufflecore(p->p_size+b,p->p_size,u.u_ssize);
	}
}

shufflecore(a,b,c)
register a,b,c;
{
	register d,e;
	
	e=a-b;
	if(e>0) {
		expand(a);
		d=u.u_procp->p_addr+a;
		while(c--) {
			d--;
			copyseg(d-e,d);
		}
		while(e--){
			d--;
			clearseg(d);
		}
	} else {
		if(e<0) {
			while(c--) {
				copyseg((u.u_procp->p_addr+b-1)-c,(u.u_procp->p_addr+a-1)-c);
			}
			expand(a);
		}
	}

	if(!u.u_segmented)
		return;
	invsdrs();
	segureg();
}
