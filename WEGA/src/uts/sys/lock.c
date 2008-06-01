/******************************************************************************
*******************************************************************************

	W E G A - Quelle

	KERN 3.2	Modul: lock.c


	Bearbeiter:	O. Lehmann
	Datum:		01.06.08
	Version:	1.0

*******************************************************************************
******************************************************************************/

char lockwstr[] = "@[$]lock.c		Rev : 4.1 	08/27/83 11:55:14";

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
#include <sys/lockblk.h>

long lkfront();

#ifdef 0
long
lkdata()
{
	register long lkfrontret;
	register long i; /*rr10*/
	long foo,lkfront1,lkfront2;
	struct a {
		int    fildes;
		int    flag;
		struct lockblk *lkblk;
	} *uap;
	int j;

	uap = (struct a *)u.u_ap;
	lkfrontret = lkfront(uap,&lkfront1,&lkfront2);
	if(lkfrontret) {
		foo = lkfrontret;
		if(locked(uap->flag&4?0x80:0,lkfrontret,lkfront1))
			return;
		i=lkfrontret+0x4a; /*FIXME: this is wrong */
		for(j=(uap->flag&1?0:0x100);i;) {
		j++;
		}
		
	}
}

long
unlk()
{
	register struct a {
		int    fildes;
		int    flag;
		struct lockblk *lkblk;
	} *uap;

	uap = (struct a *)u.u_ap;
}

locked()
{
}

deadlock()
{
}

unlock()
{
}

lockalloc()
{
}

#endif
lockfree(lkblk)
register struct lockblk *lkblk;
{
	register long *foo;

	foo=0;

}

#ifdef 0
lockadd()
{
}

long
lkfront(arg,par1,par2)
{
}
#endif
